#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#ifndef MONO_BIT_PAINTER_H
#define MONO_BIT_PAINTER_H

// Default grid dimensions and output file name
#define DEFAULT_COLUMNS 16
#define DEFAULT_ROWS 16
#define DEFAULT_OUTPUT_FILENAME "bit_painter_output.txt"

// Upper limits for grid and related buffers
#define MAX_ROWS 32
#define MAX_COLUMNS 32

// Number of bytes needed to store MAX_ROWS * MAX_COLUMNS bits (+7 for ceiling division)
#define MAX_OUTPUT_BYTES ((MAX_ROWS * MAX_COLUMNS + 7) / BYTE_SIZE)

// File parsing limits for loading saved drawings
#define MAX_LINE_LENGTH 1024

// Each byte is represented as "XX, " (3 chars) except possibly the last one; +1 for string terminator
#define MAX_HEX_STRING_LENGTH (MAX_OUTPUT_BYTES * 3 + 1)

// UI text visibility duration (in seconds) for status messages (e.g., "Saved")
#define TEXT_LIFETIME_SECONDS 3

#define BYTE_SIZE 8

#define MIN(a, b) ((a < b) ? (a) : (b))

// Application state container for rendering, input, and file I/O
typedef struct {
    uint8_t output_bits[MAX_OUTPUT_BYTES];

    char data[MAX_ROWS][MAX_COLUMNS];

    int rows;
    int columns;

    int cell_size;

    int screen_width;
    int screen_height;

    // Pixel offsets to center the grid on screen
    int offset_x;
    int offset_y;

    // Timestamp when status text started showing
    double text_init_time;

    // Output file name for saving/loading
    char *filename;

    bool load;
    bool output_saved_text;
} AppContext_t;

int get_cell_size(const AppContext_t *app);
void get_offset(AppContext_t *app);
void draw_grid(const AppContext_t *app);
void handle_mouse_input(AppContext_t *app);
int handle_keyboard(AppContext_t *app);
int get_total_bytes(const AppContext_t *app);
void pack_bits(AppContext_t *app);
void unpack_bits(AppContext_t *app);
int save_results(AppContext_t *app);
void output_text(const char *text, int fontSize, AppContext_t *app);
void print_usage();
int handle_flags(int argc, char *argv[], AppContext_t *app);
int load_drawing_from_file(AppContext_t *app);
int read_data_from_file(AppContext_t *app, FILE *file);

#endif
