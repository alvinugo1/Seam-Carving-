#include <iostream>
#include <fstream>
#include <cstring>
#include <cmath>
#include <limits>
#include "functions.h"

using std::cin, std::cout, std::endl;
using std::numeric_limits, std::streamsize;
using std::ofstream, std::ifstream;

/***************************************************************************
 * 
 * This provided code uses C-style strings.
 * Later this semester the students will be learning and using C++ Strings 
 * instead.
 * It is important to know about C-style strings because they are used
 * in a lot of code. Unless you have a specific, compelling reason to use C-style
 * strings, once you learn C++ Strings, use std::string (defined in the <string> header) instead. 
 * std::string is easier, safer, and more flexible. 
 *
 ***************************************************************************/

/*  Function processImage
 *  choice: char representing which type of image processing to perform
 *  image: 2d-array of Pixels (structs)
 *  Return value: none
 */
 // You should not modify this function unless you add another processing option. //
void processImage(char choice, Pixel image[MAX_WIDTH][MAX_HEIGHT]) {
  const int maxFilenameSize = 100;
  char originalImageFilename[maxFilenameSize];
  int width = 0, height = 0;
  
  // get filename, width and height from user
  cout << "Image filename: ";
  cin >> originalImageFilename;
  width = getInteger("width", 1, MAX_WIDTH); // ensure user does not input value greater than the MAX_WIDTH for our array
  height = getInteger("height", 1, MAX_HEIGHT); // ensure user does not input value greater than the MAX_HEIGHT for our array
  
  // load image
  if (loadImage(originalImageFilename, image, width, height)) {
    // only get in here if image loaded successfully
  
    char outputImageFilename[maxFilenameSize+7]; // maxFilenameSize plus 6 for adding on "sepia." ".copy" is smaller so works as well
    
    // modify image
    switch (toupper(choice)) {
      case 'G':
        grayscaleImage(image, width, height);
        strncpy(outputImageFilename, "grey.", 6);
        strncat(outputImageFilename, originalImageFilename, maxFilenameSize);
        break;
      case 'S':
        sepiaImage(image, width, height);
        strncpy(outputImageFilename,"sepia.", 7);
        strncat(outputImageFilename, originalImageFilename, maxFilenameSize);
        break;
      case 'H':
      {
        int frequency = getInteger("Enter frequency of lines to remove.", 1, 25);
        cout << "got the number" << endl;
        height = removeHorizontalLines(image, width, height, frequency);
        strncpy(outputImageFilename,"h_removed.", 11);
        strncat(outputImageFilename, originalImageFilename, maxFilenameSize);
        break;
      }
      case 'V':
      {
        int frequency = getInteger("Enter frequency of lines to remove.", 1, 25);
        width = removeVerticalLines(image, width, height, frequency);
        strncpy(outputImageFilename,"v_removed.", 11);
        strncat(outputImageFilename, originalImageFilename, maxFilenameSize);
        break;
      }
    }
    
    // output image
    outputImage(outputImageFilename, image, width, height);
  }
}

/*  Function loadImage
 *  filename: c-string which is the ppm file to read
 *  image: 2d-array of Pixels (structs)
 *  width: int for the width of the image array
 *  height: int for height of the image array
 *  Return value: bool true if the image loads, bool false if the image fails to load
 */
bool loadImage(const char filename[], Pixel image[MAX_WIDTH][MAX_HEIGHT], int width, int height) {
  cout << "Loading image..." << endl;
  // declare/define and open input file stream
  ifstream ifs (filename);
  
  // check if input stream opened successfully
  if (!ifs.is_open()) {
    cout << "Error: failed to open input file " << filename << endl;
    return false;
  }
  
  // get type from preamble
  char type[3];
  ifs >> type; // should be P3
  if ((toupper(type[0]) != 'P') || (type[1] != '3')) { // check that type is correct
    cout << "Error: type is " << type << "instead of P3" << endl;
    return false;
  }
  
  // get width (w) and height (h) from preamble
  int w = 0, h = 0;
  ifs >> w >> h;
  if (w != width) { // check that width matches what was passed into the function
    cout << "Error: file width does not match input" << endl;
    cout << " - input width: " << width << endl;
    cout << " -  file width: " << w << endl;
    return false;
  }
  if (h != height) { // check that height matches what was passed into the function
    cout << "Error: file height does not match input" << endl;
    cout << " - input height: " << height << endl;
    cout << " -  file height: " << h << endl;
    return false;
  }
  
  // get maximum value from preamble
  int colorMax = 0;
  ifs >> colorMax;
  if (colorMax != 255) {
    cout << "Error: file is not using RGB color values." << endl;
    return false;
  }

  // get RGB pixel values
  while (!ifs.eof()) {
    for (int row = 0; row < height; ++row) {
      for (int col = 0; col < width; ++col) {
        ifs >> image[col][row].r;
        ifs >> image[col][row].g;
        ifs >> image[col][row].b;
      }
    }
  }
  
  return true;
}


/*  Function removeHorizontalLine
 *  index: int for row to remove
 *  image: 2d-array of Pixels (structs)
 *  width: int for the width of the image array
 *  height: int for height of the image array
 *  Return value: nothing, but image should have lines removed
 *  Do nothing if row is not valid.
 */
void removeHorizontalLine(unsigned int index, Pixel image[MAX_WIDTH][MAX_HEIGHT], int width, int height) {
    // TODO(student): remove a horizontal line
	for (int i = index; i < height-1; i++){
		for (int j = 0; j < width; j++){
			image[j][i] = image[j][i+1];
		}
	}
}


/*  Function removeHorizontalLines
 *  image: 2d-array of Pixels (structs)
 *  width: int for the width of the image array
 *  height: int for height of the image array
 *  frequency: int for number of lines to skip before 
 *    removing a line. A zero means remove no lines.
 *  Return value: new value for height, but image should be modified to be grayscale colors
 */
int removeHorizontalLines(Pixel image[MAX_WIDTH][MAX_HEIGHT], int width, int height, unsigned int frequency) {
  int row = 1; // start at 1 since first row will not be removed
  unsigned int skippedLines = 1; // already skipped the first line
  while (row < height) {
    if (skippedLines == frequency) { // remove this line
      removeHorizontalLine(row, image, width, height);
      height--;
      skippedLines = 0;
    }
    else {
      row++;
      skippedLines++;
    }
  }
  return height;
}

/*  Function removeVerticalLine
 *  index: int for col to remove
 *  image: 2d-array of Pixels (structs)
 *  width: int for the width of the image array
 *  height: int for height of the image array
 *  Return value: nothing, but image should have lines removed
 *  Do nothing if col is not valid.
 */
void removeVerticalLine(unsigned int index, Pixel image[MAX_WIDTH][MAX_HEIGHT], int width, int height) {
    // TODO(student): remove a vertical line
	for (int i = index; i < height-1; i++){
		for (int j = 0; j < width; j++){
			image[j][i] = image[j+1][i];
		}
	}
}


/*  Function removeVertialLines
 *  image: 2d-array of Pixels (structs)
 *  width: int for the width of the image array
 *  height: int for height of the image array
 *  frequency: int for number of lines to skip before 
 *    removing a line. A zero means remove no lines.
 *  Return value: new value for height, but image should be modified to be grayscale colors
 */
int removeVerticalLines(Pixel image[MAX_WIDTH][MAX_HEIGHT], int width, int height, unsigned int frequency) {
    // TODO(student): remove vertical lines
	int row = 1; // start at 1 since first row will not be removed
  unsigned int skippedLines = 1; // already skipped the first line
  while (row < width) {
    if (skippedLines == frequency) { // remove this line
      removeVerticalLineLine(row, image, width, height);
      width--;
      skippedLines = 0;
    }
    else {
      row++;
      skippedLines++;
    }
  }
  return width;
}

/*  Function grayscale
 *  image: 2d-array of Pixels (structs)
 *  width: int for the width of the image array
 *  height: int for height of the image array
 *  Return value: none, but image should be modified to be grayscale colors
 */
void grayscaleImage(Pixel image[MAX_WIDTH][MAX_HEIGHT], int width, int height) {
  cout << "Making grayscale image... " << endl;
  // iterate through 2d image of Pixels and convert them to grayscale
  for (int row = 0; row < height; ++row) {
    for (int col = 0; col < width; ++col) {
      int greyColor = round((image[col][row].r + image[col][row].g + image[col][row].b) / 3.0);
      image[col][row] = { greyColor, greyColor, greyColor };
    }
  }  
}



/*  Function sepiaImage
 *  image: 2d-array of Pixels (structs)
 *  width: int for the width of the image array
 *  height: int for height of the image array
 *  Return value: none, but image should be modified to be sepia colors
 */
void sepiaImage(Pixel image[MAX_WIDTH][MAX_HEIGHT], int width, int height) {
  cout << "Making sepia image... " << endl;
  // iterate through 2d image of Pixels and convert them to sepia
  for (int row = 0; row < height; ++row) {
    for (int col = 0; col < width; ++col) {
      int newRed = round(0.393*image[col][row].r + 0.769*image[col][row].g + 0.189*image[col][row].b);
      if (newRed > 255) newRed = 255;
      int newGreen = round((0.349*image[col][row].r + 0.686*image[col][row].g + 0.168*image[col][row].b));
      if (newGreen > 255) newGreen = 255;
      int newBlue = round((0.272*image[col][row].r + 0.534*image[col][row].g + 0.131*image[col][row].b));
      if (newBlue > 255) newBlue = 255;
      
      image[col][row] = { newRed, newGreen, newBlue };
    }
  }  
}



/*  Function outputImage
 *  filename: c-string which is the ppm file to write
 *  image: 2d-array of Pixels (structs)
 *  width: int for the width of the image array
 *  height: int for height of the image array
 *  Return value: none, but ppm file should be created
 */
void outputImage(const char filename[], const Pixel image[MAX_WIDTH][MAX_HEIGHT], int width, int height) {
  cout << "Outputting image..." << endl;
  // declare/define and open output file stream
  ofstream ofs (filename);
  
  // check if output stream opened successfully
  if (!ofs.is_open()) {
    cout << "Error: failed to open output file " << filename << endl;
    return;
  }
  
  // output preamble
  ofs << "P3" << endl;
  ofs << width << " " << height << endl;
  ofs << 255 << endl;
  
  // output pixels
  for (int row = 0; row < height; ++row) {
    for (int col = 0; col < width; ++col) {
      ofs << image[col][row].r << " ";
      ofs << image[col][row].g << " ";
      ofs << image[col][row].b << " ";
    }
    ofs << endl;
  }
}



/*  Function printMenu
 *  Return value: none
 */
 // You should not modify this function. //
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



/*  Function getInteger
 *  label: label for outputing what the user is inputting, it should work when used as "Please enter <label>"
 *  min: int value indicating the smallest value the user should provide
 *  max: int value indicating the largest value the user should provide
 *  Return value: int value within the range min <= value <= max
 */
 // You should not modify this function. //
int getInteger(const char label[], int min, int max) {
  // get value from user repeatedly until input matches requirements
  int num = 0;
  do {
    cin.clear(); // reset stream states
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // clear buffer
    
    cout << "Please enter " << label << " (" << min << " - " << max << "): ";
    cin >> num;
  } while (!cin.good() || num < min || num > max); // while input does not match requirements
  return num;
}