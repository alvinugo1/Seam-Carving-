# Seam Carving (Content-Aware Image Resizing)

This project implements **content-aware image resizing** using the **seam carving algorithm** in **C++**.  
Instead of uniformly scaling an image, seam carving removes low-energy paths of pixels (seams), preserving important visual structures such as edges and objects.

The implementation supports **vertical and horizontal seam removal** using **dynamic programming** and operates on **PPM (P3)** images.

---

## Overview

Given an image of size `W × H`, seam carving works by repeatedly:

1. Computing an **energy value** for each pixel  
2. Finding a **minimum-energy seam** using dynamic programming  
3. Removing that seam from the image  
4. Repeating until the target dimensions are reached  

This approach avoids the distortions caused by traditional scaling methods.

---

## Coordinate System

Pixels are indexed using `(x, y)` coordinates where:

- `(0, 0)` is the **top-left** corner  
- `x` increases left to right  
- `y` increases top to bottom  

Example for a `3 × 4` image:

| (0,0) | (1,0) | (2,0) |
|-------|-------|-------|
| (0,1) | (1,1) | (2,1) |
| (0,2) | (1,2) | (2,2) |
| (0,3) | (1,3) | (2,3) |

Each pixel is represented in **RGB space**, with each channel ranging from `0` to `255`.

---

## Energy Function

Each pixel is assigned an **energy value** representing its visual importance.

This project uses the **dual-gradient energy function**, which measures the squared color gradient in both the horizontal and vertical directions:

- Horizontal gradient: difference between left and right neighbors  
- Vertical gradient: difference between top and bottom neighbors  

Pixels with strong color changes (edges, object boundaries) have **high energy**, making them less likely to be removed.

Boundary pixels are handled using **clamped coordinates** to prevent out-of-bounds access.

---

## Seam Identification (Dynamic Programming)

A **seam** is a connected path of pixels:

- **Vertical seam**: one pixel per row, top to bottom  
- **Horizontal seam**: one pixel per column, left to right  

The objective is to find the seam with the **minimum total energy**.

This is solved using **dynamic programming**:

- A cost table stores the minimum cumulative energy to reach each pixel  
- A parent table stores the predecessor pixel that produced the minimum cost  
- Vertical seams are computed top-to-bottom  
- Horizontal seams are computed left-to-right  

Once the DP table is filled, the seam is recovered by backtracking from the minimum-energy endpoint.

This guarantees a **globally optimal seam**, unlike greedy approaches.

---

## Seam Removal

After identifying a seam:

### Vertical Seam Removal
- One pixel is removed from each row  
- Remaining pixels shift left  
- Image width decreases by one  

### Horizontal Seam Removal
- One pixel is removed from each column  
- Remaining pixels shift upward  
- Image height decreases by one  

The image is stored as a dynamically allocated `Pixel**` array and is modified in place.

---

## File Format

The program operates on **PPM (P3)** images:

- ASCII-based format  
- Header:
  - `P3`
  - `width height`
  - `255`
- Followed by `width × height` RGB triplets  

The program validates:
- Correct PPM format  
- Matching dimensions  
- Maximum color value of 255  
- Proper pixel data length  

---

## Features

- Dynamic programming seam carving  
- Vertical and horizontal seam support  
- Explicit heap memory management  
- Robust input validation  
- In-place image modification  
- Command-line interface  

---

## Build Instructions

Compile using a C++17-compatible compiler:

```bash
g++ -std=c++17 -O2 main.cpp functions.cpp -o seamcarve
```


## Usage

Run the program:
```bash
./seamcarve
```

You will be prompted for:
1. Input image filename  
2. Image width and height  
3. Target width and height  

The program iteratively removes seams until the target dimensions are reached.

The output file is named:

```bash
carved<width>X<height>.<original_filename>
```

---

## Error Handling

The program reports errors for:
- Invalid numeric input  
- Non-positive dimensions  
- Target dimensions larger than the original image  
- File I/O failures  
- Invalid PPM headers or pixel data  

Execution terminates safely on error.

---

## Applications

Seam carving is useful for:
- Content-aware image resizing  
- Aspect ratio adaptation  
- Preserving visual structure during resizing  
- Image preprocessing for UI and responsive layouts  

---

## Author

**Alvin Ugo-Mgbemene**  
C++ implementation of seam carving using dynamic programming.
