#include <cctype>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>

#include "functions.h"

using std::cin;
using std::cout;
using std::endl;
using std::ifstream;
using std::numeric_limits;
using std::ofstream;
using std::streamsize;

void processImage(char choice, Pixel image[MAX_WIDTH][MAX_HEIGHT]) {
  constexpr int maxFilenameSize = 100;

  char originalImageFilename[maxFilenameSize]{};
  int width = 0, height = 0;

  cout << "Image filename: ";
  cin >> originalImageFilename;

  width = getInteger("width", 1, MAX_WIDTH);
  height = getInteger("height", 1, MAX_HEIGHT);

  if (!loadImage(originalImageFilename, image, width, height)) {
    return;
  }

  char outputImageFilename[maxFilenameSize + 7]{};

  switch (std::toupper(static_cast<unsigned char>(choice))) {
    case 'G':
      grayscaleImage(image, width, height);
      std::strncpy(outputImageFilename, "grey.", sizeof(outputImageFilename) - 1);
      std::strncat(outputImageFilename, originalImageFilename,
                   sizeof(outputImageFilename) - std::strlen(outputImageFilename) - 1);
      break;

    case 'S':
      sepiaImage(image, width, height);
      std::strncpy(outputImageFilename, "sepia.", sizeof(outputImageFilename) - 1);
      std::strncat(outputImageFilename, originalImageFilename,
                   sizeof(outputImageFilename) - std::strlen(outputImageFilename) - 1);
      break;

    case 'H': {
      int frequency = getInteger("Enter frequency of lines to remove.", 1, 25);
      height = removeHorizontalLines(image, width, height, static_cast<unsigned>(frequency));
      std::strncpy(outputImageFilename, "h_removed.", sizeof(outputImageFilename) - 1);
      std::strncat(outputImageFilename, originalImageFilename,
                   sizeof(outputImageFilename) - std::strlen(outputImageFilename) - 1);
      break;
    }

    case 'V': {
      int frequency = getInteger("Enter frequency of lines to remove.", 1, 25);
      width = removeVerticalLines(image, width, height, static_cast<unsigned>(frequency));
      std::strncpy(outputImageFilename, "v_removed.", sizeof(outputImageFilename) - 1);
      std::strncat(outputImageFilename, originalImageFilename,
                   sizeof(outputImageFilename) - std::strlen(outputImageFilename) - 1);
      break;
    }

    default:
      cout << "Error: unknown option '" << choice << "'.\n";
      return;
  }

  outputImage(outputImageFilename, image, width, height);
}

bool loadImage(const char filename[], Pixel image[MAX_WIDTH][MAX_HEIGHT], int width, int height) {
  cout << "Loading image..." << endl;

  ifstream ifs(filename);
  if (!ifs.is_open()) {
    cout << "Error: failed to open input file " << filename << endl;
    return false;
  }

  char type[3] = {0, 0, 0};
  if (!(ifs >> type)) {
    cout << "Error: failed to read file type.\n";
    return false;
  }
  if (type[0] != 'P' || type[1] != '3') {
    cout << "Error: expected P3 PPM, got " << type << endl;
    return false;
  }

  int w = 0, h = 0;
  if (!(ifs >> w >> h)) {
    cout << "Error: failed to read width/height from header.\n";
    return false;
  }

  if (w != width) {
    cout << "Error: file width does not match input\n";
    cout << " - input width: " << width << "\n";
    cout << " -  file width: " << w << "\n";
    return false;
  }
  if (h != height) {
    cout << "Error: file height does not match input\n";
    cout << " - input height: " << height << "\n";
    cout << " -  file height: " << h << "\n";
    return false;
  }

  int colorMax = 0;
  if (!(ifs >> colorMax)) {
    cout << "Error: failed to read max color value.\n";
    return false;
  }
  if (colorMax != 255) {
    cout << "Error: expected max color value 255, got " << colorMax << endl;
    return false;
  }

  for (int row = 0; row < height; ++row) {
    for (int col = 0; col < width; ++col) {
      if (!(ifs >> image[col][row].r >> image[col][row].g >> image[col][row].b)) {
        cout << "Error: unexpected EOF or invalid pixel data while reading.\n";
        return false;
      }
    }
  }

  return true;
}

void removeHorizontalLine(unsigned int index,
                          Pixel image[MAX_WIDTH][MAX_HEIGHT],
                          int width,
                          int height) {
  if (index >= static_cast<unsigned>(height)) {
    return;
  }

  for (int row = static_cast<int>(index); row < height - 1; ++row) {
    for (int col = 0; col < width; ++col) {
      image[col][row] = image[col][row + 1];
    }
  }
}

int removeHorizontalLines(Pixel image[MAX_WIDTH][MAX_HEIGHT],
                          int width,
                          int height,
                          unsigned int frequency) {
  if (frequency == 0) {
    return height;
  }

  int row = 1;
  unsigned skipped = 1;

  while (row < height) {
    if (skipped == frequency) {
      removeHorizontalLine(static_cast<unsigned>(row), image, width, height);
      --height;
      skipped = 0;
    } else {
      ++row;
      ++skipped;
    }
  }

  return height;
}

void removeVerticalLine(unsigned int index,
                        Pixel image[MAX_WIDTH][MAX_HEIGHT],
                        int width,
                        int height) {
  if (index >= static_cast<unsigned>(width)) {
    return;
  }

  for (int row = 0; row < height; ++row) {
    for (int col = static_cast<int>(index); col < width - 1; ++col) {
      image[col][row] = image[col + 1][row];
    }
  }
}

int removeVerticalLines(Pixel image[MAX_WIDTH][MAX_HEIGHT],
                        int width,
                        int height,
                        unsigned int frequency) {
  if (frequency == 0) {
    return width;
  }

  int col = 1;
  unsigned skipped = 1;

  while (col < width) {
    if (skipped == frequency) {
      removeVerticalLine(static_cast<unsigned>(col), image, width, height);
      --width;
      skipped = 0;
    } else {
      ++col;
      ++skipped;
    }
  }

  return width;
}

void grayscaleImage(Pixel image[MAX_WIDTH][MAX_HEIGHT], int width, int height) {
  cout << "Making grayscale image... " << endl;

  for (int row = 0; row < height; ++row) {
    for (int col = 0; col < width; ++col) {
      const int grey = static_cast<int>(
          std::lround((image[col][row].r + image[col][row].g + image[col][row].b) / 3.0));
      image[col][row] = {grey, grey, grey};
    }
  }
}

void sepiaImage(Pixel image[MAX_WIDTH][MAX_HEIGHT], int width, int height) {
  cout << "Making sepia image... " << endl;

  for (int row = 0; row < height; ++row) {
    for (int col = 0; col < width; ++col) {
      int newRed = static_cast<int>(std::lround(0.393 * image[col][row].r +
                                               0.769 * image[col][row].g +
                                               0.189 * image[col][row].b));
      int newGreen = static_cast<int>(std::lround(0.349 * image[col][row].r +
                                                 0.686 * image[col][row].g +
                                                 0.168 * image[col][row].b));
      int newBlue = static_cast<int>(std::lround(0.272 * image[col][row].r +
                                                0.534 * image[col][row].g +
                                                0.131 * image[col][row].b));

      if (newRed > 255) newRed = 255;
      if (newGreen > 255) newGreen = 255;
      if (newBlue > 255) newBlue = 255;

      image[col][row] = {newRed, newGreen, newBlue};
    }
  }
}

void outputImage(const char filename[],
                 const Pixel image[MAX_WIDTH][MAX_HEIGHT],
                 int width,
                 int height) {
  cout << "Outputting image..." << endl;

  ofstream ofs(filename);
  if (!ofs.is_open()) {
    cout << "Error: failed to open output file " << filename << endl;
    return;
  }

  ofs << "P3\n";
  ofs << width << " " << height << "\n";
  ofs << "255\n";

  for (int row = 0; row < height; ++row) {
    for (int col = 0; col < width; ++col) {
      ofs << image[col][row].r << ' '
          << image[col][row].g << ' '
          << image[col][row].b << ' ';
    }
    ofs << '\n';
  }
}

void printMenu() {
  cout << "--------------------------" << endl;
  cout << " 'H': Remove horizontal lines" << endl;
  cout << " 'V': Remove vertical lines" << endl;
  cout << " 'G': Make grayscale image" << endl;
  cout << " 'S': Make sepia image" << endl;
  cout << " 'Q': Quit" << endl;
  cout << "--------------------------" << endl;
  cout << endl << "Please enter your choice: ";
}

int getInteger(const char label[], int min, int max) {
  int num = 0;
  do {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    cout << "Please enter " << label << " (" << min << " - " << max << "): ";
    cin >> num;
  } while (!cin.good() || num < min || num > max);

  return num;
}
