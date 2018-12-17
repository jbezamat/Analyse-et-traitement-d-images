#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>

using namespace cv;

/** @function main */
int main( int argc, char** argv )
{

  Mat src, src_gray;
  Mat grad;
  char* window_name = "Sobel Demo - Simple Edge Detector";
  int scale = 1;
  int delta = 0;
  int ddepth = CV_16S;

  int c;

  /// Load an image
  src = imread( argv[1] );

  if( !src.data )
  { return -1; }

  GaussianBlur( src, src, Size(3,3), 0, 0, BORDER_DEFAULT );

  /// Convert it to gray
  cvtColor( src, src_gray, CV_BGR2GRAY );

  /// Create window
  namedWindow( window_name, CV_WINDOW_AUTOSIZE );

  /// Generate grad_x and grad_y
  Mat grad_x, grad_y;
  Mat abs_grad_x, abs_grad_y;

  /// Gradient X
  //Scharr( src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
  Sobel( src_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT );
  convertScaleAbs( grad_x, abs_grad_x );

  /// Gradient Y
  //Scharr( src_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
  Sobel( src_gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT );
  convertScaleAbs( grad_y, abs_grad_y );

  /// Total Gradient (approximate)
  addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad );

  imshow( window_name, grad );

  int kernel_size = 5;

  Mat blur_gray;
  GaussianBlur(grad, blur_gray, Size(kernel_size, kernel_size), 0);
  namedWindow("blur", WINDOW_AUTOSIZE);
  imshow("blur", blur_gray);

  int low_threshold = 50;
  int high_threshold = 150;
  Mat edges;
  Canny(blur_gray, edges, low_threshold, high_threshold);
  namedWindow("canny", WINDOW_AUTOSIZE);
  imshow("canny", edges);


  double rho = 1;  // distance resolution in pixels of the Hough grid
  double theta = M_PI / 180; // angular resolution in radians of the Hough grid
  int threshold = 15; // minimum number of votes (intersections in Hough grid cell)
  double min_line_length = 50; // minimum number of pixels making up a line
  double max_line_gap = 20; //maximum gap in pixels between connectable line segments

  // Run Hough on edge detected image
  // Output "lines" is an array containing endpoints of detected line segments
  vector<Vec4i> lines;
  HoughLinesP(edges, lines, rho, theta, threshold, min_line_length, max_line_gap);
  for (size_t i = 0; i < lines.size(); i++){
    line(src, Point(lines[i][0], lines[i][1]),
         Point(lines[i][2], lines[i][3]), Scalar(0, 0, 255), 3, 8);
  }

  namedWindow("Detected Lines", 1);
  imshow("Detected Lines", src);

  waitKey(0);

  return 0;
  }