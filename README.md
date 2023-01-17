
# Seams Carving 

Seam Carving for Content Aware Image Resizing and Object removal. 


## Introduction 

The goal of this project is to perform content-aware image resizing for both reduction and expansion and image object removal with seam carving operator. This allows image to be resized without losing or distorting meaningful content from scaling. In this project, we create a data type that resizes a W-by-H image using the seam-carving technique. 


## Algorithmn Overview 
Finding and removing a seam involves three parts and a tiny bit of notation:

In image processing, pixel (x, y) refers to the pixel in column x and row y, with pixel (0, 0) at the upper left corner and pixel (W − 1, H − 1) at the bottom right corner.  

Note: This is the opposite of the standard mathematical notation used in linear algebra where (i, j) refers to row i and column j and with Cartesian coordinates where (0, 0) is at the lower left corner.
a 3-by-4 image

      (0, 0)  	  (1, 0)  	  (2, 0)  
      (0, 1)  	  (1, 1)  	  (2, 1)  
      (0, 2)  	  (1, 2)  	  (2, 2)  
      (0, 3)  	  (1, 3)  	  (2, 3)  

We also assume that the color of a pixel is represented in RGB space, using three integers between 0 and 255. This is consistent with the [ajva.awt.Color](https://docs.oracle.com/javase/7/docs/api/java/awt/Color.html) data type.

### Energy Calculation
The first step is to calculate the energy of each pixel, which is a measure of the importance of each pixel—the higher the energy, the less likely that the pixel will be included as part of a seam (as we'll see in the next step). In this Project, I implement the dual-gradient energy function, which is described below.

![image](https://user-images.githubusercontent.com/93239793/212788687-f54b41d2-6fe6-4382-a8f4-e713a2428333.png)

The energy is high (white) for pixels in the image where there is a rapid color gradient (such as the boundary between the sea and sky and the boundary between the surfer on the left and the ocean behind him). The seam-carving technique avoids removing such high-energy pixels.

### Seam Identification 
The next step is to find a vertical seam of minimum total energy. This is similar to the classic shortest path problem in an edge-weighted digraph except for the following:

1. The weights are on the vertices instead of the edges.
 
2. We want to find the shortest path from any of the W pixels in the top row to any of the W pixels in the bottom row.
 
3. The digraph is acyclic, where there is a downward edge from pixel (x, y) to pixels (x − 1,   y + 1), (x, y + 1), and (x + 1, y + 1), assuming that the coordinates are in the    prescribed range.

![image](https://user-images.githubusercontent.com/93239793/212789158-93dd400d-5731-4c2a-9dd8-0fc90990fd37.png)

### Seam removal
The final step is to remove from the image all of the pixels along the seam.

### The SeamCarver API.

public class SeamCarver {

   // create a seam carver object based on the given picture
   public SeamCarver(Picture picture)

   // current picture
   public Picture picture()

   // width of current picture
   public int width()

   // height of current picture
   public int height()

   // energy of pixel at column x and row y
   public double energy(int x, int y)

   // sequence of indices for horizontal seam
   public int[] findHorizontalSeam()

   // sequence of indices for vertical seam
   public int[] findVerticalSeam()

   // remove horizontal seam from current picture
   public void removeHorizontalSeam(int[] seam)

   // remove vertical seam from current picture
   public void removeVerticalSeam(int[] seam)

   //  unit testing (optional)
   public static void main(String[] args)

}


