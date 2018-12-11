#include <iostream>
#include <cstdlib>
#include <math.h>

#include <opencv2/opencv.hpp>

using namespace cv; 
using namespace std;

void process(const char *imsname){
  Mat image;
  image = imread(imsname, CV_LOAD_IMAGE_GRAYSCALE);
  if (!image.data)
  {
    cout << "Could not open or find the image" << '\n';
    return;
  }
  namedWindow(imsname, WINDOW_AUTOSIZE);
  imshow(imsname, image);

  //Otsu th ocv
  Mat otsu_th_ocv;
  int opencv_seuil = threshold(image, otsu_th_ocv, 0, 255, THRESH_BINARY+THRESH_OTSU );
  cout << "otsu threshold OPENCV = " << opencv_seuil << endl;

  namedWindow("otsu-th-ocv", WINDOW_AUTOSIZE);
  imshow("otsu-th-ocv", otsu_th_ocv);
  imwrite("otsu-th-ocv.png", otsu_th_ocv);
  

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