#include <iostream>
#include <cstdlib>
#include <math.h>


#include <opencv2/opencv.hpp>


using namespace cv; 
using namespace std;


int low_H = 30, low_S = 194, low_V = 158;
int high_H = 50, high_S = 226, high_V = 255;

void process(const char *imsname){
  Mat image, open;
  image = imread(imsname, CV_LOAD_IMAGE_COLOR);
  if (!image.data)
  {
    cout << "Could not open or find the image" << '\n';
    return;
  }

  Mat kernel = getStructuringElement(MORPH_RECT,Size(2,2));
  Mat kernel2 = getStructuringElement(MORPH_RECT,Size(3,3));

  cvtColor(image,image, COLOR_BGR2HSV);

  Mat frame, frame_HSV, frame_threshold;
    // Convert from BGR to HSV colorspace
  cvtColor(image, frame_HSV, COLOR_BGR2HSV);

  blur(frame_HSV, frame_HSV, Size(5,5));

  //Modif d'histogramme pour faire ressortir le blanc/vert ?

    //Threshold
  inRange(frame_HSV, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), frame_threshold);
    //Filtering of binarymake
  imshow("binavant", frame_threshold);
    waitKey(0);
  for(int i=0; i < 3; i++){
    //erode(frame_threshold, frame_threshold, kernel);
    morphologyEx(frame_threshold, frame_threshold, MORPH_OPEN, kernel);
  }

  imshow("binaprès", frame_threshold);
    waitKey(0);
  //morphologyEx(frame_threshold, frame_threshold, MORPH_OPEN, kernel);
  //morphologyEx(frame_threshold, frame_threshold, MORPH_CLOSE, kernel2);

  vector<Vec4i> lines;
  HoughLinesP(frame_threshold, lines, 1, CV_PI/180, 100, 10, 500 );
  for( size_t i = 0; i < lines.size(); i++ )
  {
      line( image, Point(lines[i][0], lines[i][1]),
          Point(lines[i][2], lines[i][3]), Scalar(0,0,255), 3, 8 );
  }
  //Finding the lines from the rest with White/Green filter
  

  // namedWindow("origin", WINDOW_AUTOSIZE);
  // namedWindow("bin", WINDOW_AUTOSIZE);
  // //imshow("origin", image);
  // imshow("bin", frame_threshold);
  imshow("final", image);

  //imwrite("test_bin_simple.png", frame_threshold);

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