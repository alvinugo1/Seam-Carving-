#include <iostream>
#include <string>
#include <sstream>
#include "functions.h"

using namespace std;

int main() {
  string filename;
  int width = 0;
  int height = 0;
  int originalWidth = 0;
  int targetWidth = 0;
  int targetHeight = 0;

  cout << "Input filename: ";
  cin >> filename;

  cout << "Input width and height: ";
  cin >> width;
  if (cin.fail()) {
    cout << "Error: width is a non-integer value" << endl;
    return 1;
  }
  if (width <= 0) {
    cout << "Error: width must be greater than 0. You entered " << width << endl;
    return 1;
  }

  cin >> height;
  if (cin.fail()) {
    cout << "Error: height is a non-integer value" << endl;
    return 1;
  }
  if (height <= 0) {
    cout << "Error: height must be greater than 0. You entered " << height << endl;
    return 1;
  }

  cout << "Input target width and height: ";
  cin >> targetWidth;
  if (cin.fail()) {
    cout << "Error: target width is a non-integer value" << endl;
    return 1;
  }
  if (targetWidth <= 0) {
    cout << "Error: target width must be greater than 0. You entered " << targetWidth << endl;
    return 1;
  }

  cin >> targetHeight;
  if (cin.fail()) {
    cout << "Error: target height is a non-integer value" << endl;
    return 1;
  }
  if (targetHeight <= 0) {
    cout << "Error: target height must be greater than 0. You entered " << targetHeight << endl;
    return 1;
  }

  if (targetWidth > width) {
    cout << "Error: target width must be less than width, " << targetWidth << " is greater than " << width << endl;
    return 1;
  }

  if (targetHeight > height) {
    cout << "Error: target height must be less than height, " << targetHeight << " is greater than " << height << endl;
    return 1;
  }

  originalWidth = width;

  Pixel** image1 = createImage(width, height);
  if (!image1) {
    cout << "Error: failed to allocate image." << endl;
    return 1;
  }

  if (!loadImage(filename, image1, width, height)) {
    deleteImage(image1, originalWidth);
    return 1;
  }

  while ((width - targetWidth > 0) || (height - targetHeight > 0)) {
    if (width - targetWidth > 0) {
      int* verticalSeam = findMinVerticalSeam(image1, width, height);
      if (!verticalSeam) {
        cout << "Error: failed to allocate vertical seam." << endl;
        break;
      }
      removeVerticalSeam(image1, width, height, verticalSeam);
      deleteSeam(verticalSeam);
      width--;
    }

    if (height - targetHeight > 0) {
      int* horizontalSeam = findMinHorizontalSeam(image1, width, height);
      if (!horizontalSeam) {
        cout << "Error: failed to allocate horizontal seam." << endl;
        break;
      }
      removeHorizontalSeam(image1, width, height, horizontalSeam);
      deleteSeam(horizontalSeam);
      height--;
    }
  }

  stringstream ss;
  ss << "carved" << width << "X" << height << "." << filename;
  outputImage(ss.str(), image1, width, height);

  deleteImage(image1, originalWidth);
  return 0;
}
