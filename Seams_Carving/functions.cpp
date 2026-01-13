#include <fstream>
#include <iostream>
#include <string>
#include <new>

#include "functions.h"



static int clampInt(int v, int lo, int hi) {
  if (v < lo) return lo;
  if (v > hi) return hi;
  return v;
}

static int sq(int x) { return x * x; }

Pixel** createImage(int width, int height) {
  if (width <= 0 || height <= 0) return nullptr;

  Pixel** image = new (std::nothrow) Pixel*[width];
  if (!image) return nullptr;

  for (int x = 0; x < width; ++x) {
    image[x] = new (std::nothrow) Pixel[height];
    if (!image[x]) {
      for (int k = 0; k < x; ++k) delete[] image[k];
      delete[] image;
      return nullptr;
    }
  }
  return image;
}

void deleteImage(Pixel** image, int width) {
  if (!image) return;
  for (int x = 0; x < width; ++x) delete[] image[x];
  delete[] image;
}

int* createSeam(int length) {
  if (length <= 0) return nullptr;
  int* seam = new (std::nothrow) int[length];
  return seam;
}

void deleteSeam(int* seam) { delete[] seam; }

bool loadImage(const std::string& filename, Pixel** image, int width, int height) {
  std::ifstream in(filename);
  if (!in.is_open()) {
    std::cout << "Error: failed to open input file " << filename << "\n";
    return false;
  }

  std::string magic;
  in >> magic;
  if (!in.good() || magic != "P3") {
    std::cout << "Error: expected P3 PPM format.\n";
    return false;
  }

  int w = 0, h = 0;
  in >> w >> h;
  if (!in.good() || w != width || h != height) {
    std::cout << "Error: file dimensions do not match inputs.\n";
    return false;
  }

  int maxVal = 0;
  in >> maxVal;
  if (!in.good() || maxVal != 255) {
    std::cout << "Error: expected max color value 255.\n";
    return false;
  }

  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      int r, g, b;
      if (!(in >> r >> g >> b)) {
        std::cout << "Error: unexpected EOF while reading pixels.\n";
        return false;
      }
      image[x][y].r = r;
      image[x][y].g = g;
      image[x][y].b = b;
    }
  }

  return true;
}

bool outputImage(const std::string& filename, Pixel** image, int width, int height) {
  std::ofstream out(filename);
  if (!out.is_open()) {
    std::cout << "Error: failed to open output file " << filename << "\n";
    return false;
  }

  out << "P3\n";
  out << width << " " << height << "\n";
  out << "255\n";

  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      out << image[x][y].r << ' '
          << image[x][y].g << ' '
          << image[x][y].b << ' ';
    }
    out << '\n';
  }

  return true;
}

int energy(Pixel** image, int x, int y, int width, int height) {
  if (!image) return 0;
  x = clampInt(x, 0, width - 1);
  y = clampInt(y, 0, height - 1);

  const int xl = clampInt(x - 1, 0, width - 1);
  const int xr = clampInt(x + 1, 0, width - 1);
  const int yu = clampInt(y - 1, 0, height - 1);
  const int yd = clampInt(y + 1, 0, height - 1);

  const Pixel& L = image[xl][y];
  const Pixel& R = image[xr][y];
  const Pixel& U = image[x][yu];
  const Pixel& D = image[x][yd];

  const int dx = sq(R.r - L.r) + sq(R.g - L.g) + sq(R.b - L.b);
  const int dy = sq(D.r - U.r) + sq(D.g - U.g) + sq(D.b - U.b);

  return dx + dy;
}

int loadVerticalSeam(Pixel** image, int start_col, int width, int height, int* seam) {
  if (!seam || width <= 0 || height <= 0) return 0;
  start_col = clampInt(start_col, 0, width - 1);

  int total = 0;
  int col = start_col;

  for (int row = 0; row < height; ++row) {
    seam[row] = col;
    total += energy(image, col, row, width, height);

    if (row == height - 1) break;

    const int left = clampInt(col - 1, 0, width - 1);
    const int mid  = col;
    const int right= clampInt(col + 1, 0, width - 1);

    const int eL = energy(image, left,  row + 1, width, height);
    const int eM = energy(image, mid,   row + 1, width, height);
    const int eR = energy(image, right, row + 1, width, height);


    if (eL <= eM && eL <= eR) col = left;
    else if (eM <= eR)        col = mid;
    else                      col = right;
  }

  return total;
}

int loadHorizontalSeam(Pixel** image, int start_row, int width, int height, int* seam) {
  if (!seam || width <= 0 || height <= 0) return 0;
  start_row = clampInt(start_row, 0, height - 1);

  int total = 0;
  int row = start_row;

  for (int col = 0; col < width; ++col) {
    seam[col] = row;
    total += energy(image, col, row, width, height);

    if (col == width - 1) break;

    const int up   = clampInt(row - 1, 0, height - 1);
    const int mid  = row;
    const int down = clampInt(row + 1, 0, height - 1);

    const int eU = energy(image, col + 1, up,   width, height);
    const int eM = energy(image, col + 1, mid,  width, height);
    const int eD = energy(image, col + 1, down, width, height);


    if (eU <= eM && eU <= eD) row = up;
    else if (eM <= eD)        row = mid;
    else                      row = down;
  }

  return total;
}

int* findMinVerticalSeam(Pixel** image, int width, int height) {
  if (!image || width <= 0 || height <= 0) return nullptr;

  int** cost = new (std::nothrow) int*[width];
  int** parent = new (std::nothrow) int*[width];
  if (!cost || !parent) {
    delete[] cost;
    delete[] parent;
    return nullptr;
  }

  for (int x = 0; x < width; ++x) {
    cost[x] = new (std::nothrow) int[height];
    parent[x] = new (std::nothrow) int[height];
    if (!cost[x] || !parent[x]) {
      for (int k = 0; k <= x; ++k) {
        delete[] cost[k];
        delete[] parent[k];
      }
      delete[] cost;
      delete[] parent;
      return nullptr;
    }
  }

  for (int x = 0; x < width; ++x) {
    cost[x][0] = energy(image, x, 0, width, height);
    parent[x][0] = x;
  }

  for (int y = 1; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      int bestPrevX = x;
      int bestPrevCost = cost[x][y - 1];

      if (x - 1 >= 0 && cost[x - 1][y - 1] <= bestPrevCost) {
        bestPrevCost = cost[x - 1][y - 1];
        bestPrevX = x - 1;
      }
      if (x + 1 < width && cost[x + 1][y - 1] < bestPrevCost) {
        bestPrevCost = cost[x + 1][y - 1];
        bestPrevX = x + 1;
      }

      cost[x][y] = energy(image, x, y, width, height) + bestPrevCost;
      parent[x][y] = bestPrevX;
    }
  }

  int bestEndX = 0;
  int bestEnergy = cost[0][height - 1];
  for (int x = 1; x < width; ++x) {
    if (cost[x][height - 1] < bestEnergy) {
      bestEnergy = cost[x][height - 1];
      bestEndX = x;
    }
  }

  int* seam = createSeam(height);
  if (!seam) {
    for (int x = 0; x < width; ++x) { delete[] cost[x]; delete[] parent[x]; }
    delete[] cost;
    delete[] parent;
    return nullptr;
  }

  int x = bestEndX;
  for (int y = height - 1; y >= 0; --y) {
    seam[y] = x;
    x = parent[x][y];
  }

  for (int i = 0; i < width; ++i) {
    delete[] cost[i];
    delete[] parent[i];
  }
  delete[] cost;
  delete[] parent;

  return seam;
}

int* findMinHorizontalSeam(Pixel** image, int width, int height) {
  if (!image || width <= 0 || height <= 0) return nullptr;

  int** cost = new (std::nothrow) int*[width];
  int** parent = new (std::nothrow) int*[width];
  if (!cost || !parent) {
    delete[] cost;
    delete[] parent;
    return nullptr;
  }

  for (int x = 0; x < width; ++x) {
    cost[x] = new (std::nothrow) int[height];
    parent[x] = new (std::nothrow) int[height];
    if (!cost[x] || !parent[x]) {
      for (int k = 0; k <= x; ++k) {
        delete[] cost[k];
        delete[] parent[k];
      }
      delete[] cost;
      delete[] parent;
      return nullptr;
    }
  }

  for (int y = 0; y < height; ++y) {
    cost[0][y] = energy(image, 0, y, width, height);
    parent[0][y] = y;
  }

  for (int x = 1; x < width; ++x) {
    for (int y = 0; y < height; ++y) {
      int bestPrevY = y;
      int bestPrevCost = cost[x - 1][y];

      if (y - 1 >= 0 && cost[x - 1][y - 1] <= bestPrevCost) {
        bestPrevCost = cost[x - 1][y - 1];
        bestPrevY = y - 1;
      }
      if (y + 1 < height && cost[x - 1][y + 1] < bestPrevCost) {
        bestPrevCost = cost[x - 1][y + 1];
        bestPrevY = y + 1;
      }

      cost[x][y] = energy(image, x, y, width, height) + bestPrevCost;
      parent[x][y] = bestPrevY;
    }
  }

  int bestEndY = 0;
  int bestEnergy = cost[width - 1][0];
  for (int y = 1; y < height; ++y) {
    if (cost[width - 1][y] < bestEnergy) {
      bestEnergy = cost[width - 1][y];
      bestEndY = y;
    }
  }

  int* seam = createSeam(width);
  if (!seam) {
    for (int x = 0; x < width; ++x) { delete[] cost[x]; delete[] parent[x]; }
    delete[] cost;
    delete[] parent;
    return nullptr;
  }

  int y = bestEndY;
  for (int x = width - 1; x >= 0; --x) {
    seam[x] = y;
    y = parent[x][y];
  }

  for (int i = 0; i < width; ++i) {
    delete[] cost[i];
    delete[] parent[i];
  }
  delete[] cost;
  delete[] parent;

  return seam;
}

void removeVerticalSeam(Pixel** image, int width, int height, int* verticalSeam) {
  if (!image || !verticalSeam || width <= 1) return;

  for (int row = 0; row < height; ++row) {
    int colToRemove = clampInt(verticalSeam[row], 0, width - 1);
    for (int col = colToRemove; col < width - 1; ++col) {
      image[col][row] = image[col + 1][row];
    }
  }
}

void removeHorizontalSeam(Pixel** image, int width, int height, int* horizontalSeam) {
  if (!image || !horizontalSeam || height <= 1) return;

  for (int col = 0; col < width; ++col) {
    int rowToRemove = clampInt(horizontalSeam[col], 0, height - 1);
    for (int row = rowToRemove; row < height - 1; ++row) {
      image[col][row] = image[col][row + 1];
    }
  }
}
