#include <raylib.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "mono_bitpainter.h"

int main(int argc, char *argv[])
{
    AppContext_t app = {0};

    if (handle_flags(argc, argv, &app) == EXIT_FAILURE)
        return EXIT_FAILURE;
    if (app.load) {
        if (load_drawing_from_file(&app) == EXIT_FAILURE)
            return EXIT_FAILURE;
    }

    SetTraceLogLevel(LOG_ERROR);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 800, "BitPainter");

    // NOTE: Fonts MUST be loaded after Window initialization (OpenGL context is required)
    app.fontData = LoadFontEx(FONT_PATH, 100, NULL, 250);
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (handle_keyboard(&app) == 1)
            break;

        BeginDrawing();
            ClearBackground(RAYWHITE);

            app.screen_width = GetScreenWidth();
            app.screen_height = GetScreenHeight();
            app.cell_size = get_cell_size(&app);

            get_offset(&app);
            draw_grid(&app);

            if (app.output_saved_text &&
                (GetTime() - app.text_init_time) < TEXT_LIFETIME_SECONDS) {
                output_text("Saved", 100, &app);
            }

            handle_mouse_input(&app);
        EndDrawing();
    }
    CloseWindow();
    return EXIT_SUCCESS;
}

int get_cell_size(const AppContext_t *app)
{
    return MIN(app->screen_width / app->columns, app->screen_height / app->rows);
}

void get_offset(AppContext_t *app)
{
    app->offset_x = (app->screen_width - app->columns * app->cell_size) / 2;
    app->offset_y = (app->screen_height - app->rows * app->cell_size) / 2;
}

// Renders the grid and fills cells
void draw_grid(const AppContext_t *app)
{
    int x, y;
    for (int c = 0; c < app->columns; c++) {
	for (int r = 0; r < app->rows; r++) {
            x = app->offset_x + c * app->cell_size;
            y = app->offset_y + r * app->cell_size;
            DrawRectangleLines(x, y, app->cell_size, app->cell_size, BLACK);
            if (app->data[c][r])
                DrawRectangle(x, y, app->cell_size, app->cell_size, BLACK);
        }
   }
}

// Handles mouse input to set/clear cells
void handle_mouse_input(AppContext_t *app)
{
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) || IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
    int row, column;
    int relative_x = GetMouseX() - app->offset_x;
    int relative_y = GetMouseY() - app->offset_y;
    if (relative_x > 0 && relative_x < (app->columns * app->cell_size) &&
        relative_y > 0 && relative_y < (app->rows * app->cell_size)) {
            row = relative_y / app->cell_size;
            column = relative_x / app->cell_size;
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
                app->data[column][row] = 1;
            if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
                app->data[column][row] = 0;
        }
    }
}

// Handles keyboard input for save and resizing grid
int handle_keyboard(AppContext_t *app)
{
    if (IsKeyReleased(KEY_ENTER)) {
        save_results(app);
        app->output_saved_text = true;
        app->text_init_time = GetTime();
    } else if (IsKeyReleased(KEY_LEFT_BRACKET)) {
        if (app->rows > 1)
            app->rows--;
    } else if (IsKeyReleased(KEY_RIGHT_BRACKET)) {
        if (app->rows < MAX_ROWS)
            app->rows++;
    } else if (IsKeyReleased(KEY_O)) {
        if (app->columns > 1)
            app->columns--;
    } else if (IsKeyReleased(KEY_P)) {
        if (app->columns < MAX_COLUMNS)
            app->columns++;
    }
    return 0;
}

// Packs grid bits into output byte array (Vertical packing)
void pack_bits(AppContext_t *app) {
    int bytes_per_row = BYTES_PER_ROW(app);
    memset(app->output_bits, 0, TOTAL_BYTES(app));

    for (int r = 0; r < app->rows; r++) {
        for (int c = 0; c < app->columns; c++) {
            if (app->data[c][r]) {
                int byte_idx = (r * bytes_per_row) + (c / 8);
                int bit_pos = 7 - (c % 8);
                app->output_bits[byte_idx] |= (1 << bit_pos);
            }
        }
    }
}

// packing logic:
// byte 1 will contain the first 8 pixels of row 0
// byte 2 will contain the next 8 pixels of row 0, and so on

// Unpacks bits from output byte array back into grid
void unpack_bits(AppContext_t *app) {
    int bytes_per_row = BYTES_PER_ROW(app);
    for (int r = 0; r < app->rows; r++) {
	for (int c = 0; c < app->columns; c++) {
	    int byte_idx = (r * bytes_per_row) + (c / 8);
	    int bit_pos = 7 - (c % 8);
	    uint8_t byte = app->output_bits[byte_idx];
	    app->data[c][r] = (byte & (1 << bit_pos)) ? 1 : 0;
	}
    }
}

// Saves packed grid data to a file in a simple text format
int save_results(AppContext_t *app)
{
    pack_bits(app);

    FILE *f = fopen(app->filename, "w");
    if (f == NULL) {
        printf("Couldn't open output file\n");
        return EXIT_FAILURE;
    }
    fprintf(f, "ROWS= %d\n", app->rows);
    fprintf(f, "COLUMNS= %d\n", app->columns);
    fprintf(f, "DATA=\n");

    int i;
    for (i = 0; i < TOTAL_BYTES(app) - 1; i++)
        fprintf(f, "0x%02X, ", app->output_bits[i]);

    fprintf(f, "0x%02X", app->output_bits[i]);
    fprintf(f, "\n");

    fclose(f);

    printf("Output saved to %s\n", app->filename);

    return EXIT_SUCCESS;
}

// Draws a short status text centered on the screen
void output_text(const char *text, int fontSize, AppContext_t *app)
{
    int x = app->screen_width / 2 - fontSize;
    int y = app->screen_height / 2 - fontSize;
    DrawTextEx(app->fontData, text, (Vector2){ (float)x, (float)y }, fontSize, 2, GREEN);
}

void print_usage()
{
    printf("BitPainter - Monochrome grid editor\n\n"

"Usage:\nmono_bitpainter [options]\n\n"

"Options:\n"
  "-rows <N>    Rows (default: 16, max: 32)\n"
  "-cols <N>    Columns (default: 16, max: 32)\n"
  "-file <path> File for save/load (default: bit_painter_output.txt)\n"
  "-l           Load drawing at start\n"
  "-help        Show help\n");
}

// Parses command line flags and sets initial app state
int handle_flags(int argc, char *argv[], AppContext_t *app)
{
    app->rows = DEFAULT_ROWS;
    app->columns = DEFAULT_COLUMNS;
    app->filename = DEFAULT_OUTPUT_FILENAME;

    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-rows") == 0 && i + 1 < argc) {
            app->rows = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-cols") == 0 && i + 1 < argc) {
            app->columns = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-file") == 0 && i + 1 < argc) {
            app->filename = argv[++i];
        } else if (strcmp(argv[i], "-l") == 0) {
            app->load = true;
        } else if (strcmp(argv[i], "-help") == 0) {
            print_usage();
	    exit(0);
        }
    }
    return EXIT_SUCCESS;
}

// Loads a saved drawing from file and populates grid
int load_drawing_from_file(AppContext_t *app)
{
    FILE *f = fopen(app->filename, "r");
    if (f == NULL) {
        perror("Error openning file");
        return EXIT_FAILURE;
    }
    if (read_data_from_file(app, f) == EXIT_FAILURE) {
        fclose(f);
        return EXIT_FAILURE;
    }
    fclose(f);
    unpack_bits(app);
    return EXIT_SUCCESS;
}

// Reads rows, columns, and hex byte data from file
int read_data_from_file(AppContext_t *app, FILE *file)
{
    char line[MAX_LINE_LENGTH];
    int byte_index = 0;
    int line_num = 0;
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        line_num++;
        size_t len = strlen(line);

        if (len > 0 && line[len-1] == '\n')
            line[len-1] = '\0';
        if (line_num == 1 && sscanf(line, "ROWS=%d", &app->rows) != 1) {
            perror("Error reading rows");
            return EXIT_FAILURE;
        } else if (line_num == 2 && sscanf(line, "COLUMNS=%d", &app->columns) != 1) {
            perror("Error reading columns");
            return EXIT_FAILURE;
        } else if (line_num == 4) {
            char *token = strtok(line, ", ");
            while (token != NULL) {
                if (*token != '\0') {
                    char *endptr;
                    unsigned long byte_value = strtoul(token, &endptr, 16);
                    if (endptr == token || *endptr != '\0') {
                        fprintf(stderr, "Error converting '%s'\n", token);
                        return EXIT_FAILURE;
                    }
                    if (byte_index > MAX_OUTPUT_BYTES) {
                        perror("Data exceeds maximum buffer size");
                        return EXIT_FAILURE;
                    }
                    if (byte_value > 0xFF) {
                        perror("Byte value exceeds 0xFF; truncating");
                    }
                    app->output_bits[byte_index++] = (uint8_t)byte_value;
                }
            token = strtok(NULL, ", ");
            }
        }
    }
    if (app->rows <= 0 || app->columns <= 0) {
        perror("Rows or columns not succesfully read");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
