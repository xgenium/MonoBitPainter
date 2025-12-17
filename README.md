# MonoBitPainter

MonoBitPainter is a simple monochrome grid editor built with raylib. It lets you paint cells on a resizable grid, then saves the result to a compact hex-encoded bit format. You can also load previously saved drawings.

I made this program to help me draw sprites for my retro, monochrome game. Don't expect anything from the code because it's my one of the first projects in C.

(it looks like I messed up rows with collumns. If bitmap looks weird, try changing rows and collumns with each other)

## Features
- Paint cells with the mouse:
  - Left click: fill cell
  - Right click: clear cell
- Resize grid during runtime:
- Save and load drawings
- Resizable window with centered grid

## Demo
![](https://github.com/xgenium/MonoBitPainter/blob/main/demo.gif)

## Flags
- `-help`: Prints program usage
- `-rows <N>`: Set initial number of rows (default: 16)
- `-cols <N>`: Set initial number of columns (default: 16)
- `-file <path>`: Set output/input file path (default: `bit_painter_output.txt`)
- `-l`: Load an existing drawing from the file at startup

## Save Format
Text file with:
- `ROWS= <n>`
- `COLUMNS= <n>`
- `DATA=` followed by new line and comma-separated hex bytes representing packed bits (Big-Endian order)

## Build
Ensure raylib is installed, then compile:
```bash
cc -o mono_bitpainter mono_bitpainter.c -lraylib
```

## Run
Start with defaults:
```bash
./mono_bitpainter
```

Start with custom grid and file:
```bash
./mono_bitpainter -rows 24 -cols 24 -file my_drawing.txt
```

Load an existing drawing:
```bash
./mono_bitpainter -file my_drawing.txt -l
```

## Controls
- Mouse: paint/erase cells
- Enter: save to file and show "Saved" message
- `[` / `]`: decrease/increase rows
- `O` / `P`: decrease/increase columns
