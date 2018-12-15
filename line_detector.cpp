#include <iostream>
#include <cstdlib>
#include <math.h>

#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int low_H = 0, low_S = 180, low_V = 160;
int high_H = 48, high_S = 255, high_V = 253;

void process(const char *imsname){
  Mat image, open;
  image = imread(imsname, CV_LOAD_IMAGE_COLOR);
  if (!image.data)
  {
    cout << "Could not open or find the image" << '\n';
    return;
  }

  Mat kernel = getStructuringElement(MORPH_RECT,Size(2,2));
  //Mat kernel2 = getStructuringElement(MORPH_RECT,Size(40,40));
  Mat kernel2 = imread("disk-30.png", CV_LOAD_IMAGE_GRAYSCALE);


  cvtColor(image,image, COLOR_BGR2HSV);

  Mat frame, frame_HSV, frame_threshold, frame_threshold_terrain;
    // Convert from BGR to HSV colorspace
  cvtColor(image, frame_HSV, COLOR_BGR2HSV);

  inRange(frame_HSV, Scalar(50, 0, 0), Scalar(120, 255, 255), frame_threshold_terrain);
  inRange(frame_HSV, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), frame_threshold);

  for(int i = 0; i < 5; i++){
    morphologyEx(frame_threshold_terrain, frame_threshold_terrain, MORPH_CLOSE, kernel2);
  }

  bitwise_not(frame_threshold_terrain, frame_threshold_terrain);
  imshow("ap", frame_threshold_terrain);
  waitKey('0');
  imshow("apr", frame_threshold);
  waitKey('0');

  Mat fr;

  subtract(frame_threshold, frame_threshold_terrain, fr);

  for(int i = 0; i < 4; i++){
    morphologyEx(frame_threshold, frame_threshold, MORPH_CLOSE, kernel);
  }
  imwrite("test_code_thomas.png",fr);
  imshow("sub", fr);
  waitKey(0);

    //Threshold
    //Filtering of binarymake

  //Canny(frame_threshold, frame_threshold, 50,300);
  //imshow("binavant", frame_threshold);
  //waitKey(0);
  //morphologyEx(frame_threshold, frame_threshold, MORPH_OPEN, kernel);
  //morphologyEx(frame_threshold, frame_threshold, MORPH_CLOSE, kernel2);

  // vector<Vec4i> lines;
  // HoughLinesP(frame_threshold, lines, 1, CV_PI/180, 20, 100, 900 );
  // for( size_t i = 0; i < lines.size(); i++ )
  // {
  //     line( image, Point(lines[i][0], lines[i][1]),
  //     Point(lines[i][2], lines[i][3]), Scalar(0,0,255), 3, 8 );
  // }
  // //Finding the lines from the rest with White/Green filter


  // namedWindow("origin", WINDOW_AUTOSIZE);
  // namedWindow("bin", WINDOW_AUTOSIZE);
  // //imshow("origin", image);
  // imshow("bin", frame_threshold);
  //imshow("final", image);

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
