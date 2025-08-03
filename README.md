# CLI Image Editor

**Author:** PANA ROBERT IONUT

A C-based image editor that supports manipulation of PGM (grayscale) and PPM (color) images.

## Features

- Load and save PGM/PPM images (ASCII and binary formats)
- Image selection (regions or entire image)
- Histogram display and equalization
- Image rotation (multiples of 90 degrees)
- Cropping functionality
- Apply filters: SHARPEN, BLUR, EDGE, GAUSSIAN_BLUR
- Memory management with proper cleanup

## Building

To compile the project:

```bash
make build
```

To clean build files:

```bash
make clean
```

## Usage

Run the program:

```bash
./image_editor
```

The program accepts commands via standard input. See [sources/README](sources/README) for a complete list of available commands.

## Project Structure

```
├── sources/                 # Source code files
│   ├── image_editor.c      # Main program entry point
│   ├── def.h              # Header file with declarations
│   ├── load_functions.c   # Image loading functionality
│   ├── save_functions.c   # Image saving functionality
│   ├── apply_functions.c  # Filter application functions
│   ├── crop_functions.c   # Selection and cropping functions
│   ├── rotate_functions.c # Image rotation functions
│   ├── histogram_functions.c  # Histogram operations
│   ├── equalize_functions.c   # Histogram equalization
│   ├── memory_handling_functions.c  # Memory management
│   ├── exit_function.c    # Program exit handling
│   └── README            # Detailed command reference
├── Makefile              # Build configuration
├── README               # Original project documentation
└── .gitignore          # Git ignore rules

```

## Commands

- `LOAD <filename>` - Load an image
- `SAVE <filename>` - Save current image
- `SELECT ALL` or `SELECT <x1> <y1> <x2> <y2>` - Select region
- `HISTOGRAM <x> <bins>` - Display histogram (grayscale only)
- `EQUALIZE` - Apply histogram equalization (grayscale only)
- `ROTATE <degrees>` - Rotate image (multiples of 90°)
- `CROP` - Crop to current selection
- `APPLY <filter>` - Apply filter (color images only)
- `EXIT` - Close program

See [sources/README](sources/README) for detailed command documentation.

## Requirements

- GCC compiler
- Standard C libraries
- Math library (-lm)

## Testing

The Makefile includes several testing targets:

```bash
make test      # Run with test input
make memcheck  # Check for memory leaks with valgrind
make checker   # Run automated tests
```
