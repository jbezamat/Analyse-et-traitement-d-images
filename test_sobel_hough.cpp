#include <iostream>
#include <cstdlib>
#include <math.h>

#include <opencv2/opencv.hpp>

using namespace cv; 
using namespace std;

void process(const char *imsname){
  Mat image;
  image = imread(imsname, CV_LOAD_IMAGE_COLOR);
  if (!image.data)
  {
    cout << "Could not open or find the image" << '\n';
    return;
  }
  namedWindow(imsname, WINDOW_AUTOSIZE);
  imshow(imsname, image);

  Mat gray;
  cvtColor(image, gray, COLOR_BGR2GRAY);
  int kernel_size = 5;

  Mat blur_gray;
  GaussianBlur(gray, blur_gray, Size(kernel_size, kernel_size), 0);
  namedWindow("blur", WINDOW_AUTOSIZE);
  imshow("blur", blur_gray);

  int low_threshold = 50;
  int high_threshold = 150;
  Mat edges;
  Canny(blur_gray, edges, low_threshold, high_threshold);
  namedWindow("canny", WINDOW_AUTOSIZE);
  imshow("canny", edges);


  int scale = 1;
  int delta = 0;
  int ddepth = CV_16S;
  /// Generate grad_x and grad_y
  Mat grad_x, grad_y;
  Mat abs_grad_x, abs_grad_y;

  /// Gradient X
  Scharr( edges, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
  Sobel( edges, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT );
  convertScaleAbs( grad_x, abs_grad_x );

  /// Gradient Y
  Scharr( edges, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
  Sobel( edges, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT );
  convertScaleAbs( grad_y, abs_grad_y );

  /// Total Gradient (approximate)
  Mat grad;
  addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad );

  imshow( "sobel", grad );


  double rho = 1;  // distance resolution in pixels of the Hough grid
  double theta = M_PI / 180; // angular resolution in radians of the Hough grid
  int threshold = 15; // minimum number of votes (intersections in Hough grid cell)
  double min_line_length = 50; // minimum number of pixels making up a line
  double max_line_gap = 20; //maximum gap in pixels between connectable line segments

  // Run Hough on edge detected image
  // Output "lines" is an array containing endpoints of detected line segments
  vector<Vec4i> lines;
  HoughLinesP(grad, lines, rho, theta, threshold, min_line_length, max_line_gap);
  for (size_t i = 0; i < lines.size(); i++){
    line(image, Point(lines[i][0], lines[i][1]),
         Point(lines[i][2], lines[i][3]), Scalar(0, 0, 255), 3, 8);
  }

  namedWindow("Detected Lines", 1);
  imshow("Detected Lines", image);
  waitKey(0);
}

void usage (const char *s){
  std::cerr<<"Usage: "<<s<<" imsname\n"<<std::endl;
  exit(EXIT_FAILURE);
}

#define param 1
int main( int argc, char* argv[] ){
  if(argc != (param+1)){
    usage(argv[0]);
  }
  process(argv[1]);
  return EXIT_SUCCESS;
}