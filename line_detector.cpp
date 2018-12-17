#include <iostream>
#include <cstdlib>
#include <math.h>
#include <time.h>

#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int low_H = 7, low_S = 140, low_V = 65;
int high_H = 37, high_S = 255, high_V = 255;


bool isImgBlack(Mat img) {
    for(int i = 0; i < img.rows; i++) {
        for(int j = 0; j < img.cols; j++) {
            if(img.at<uchar>(i,j) > 0)
                return false;
        }
    }
    return true;
}

Mat skeleton(Mat se, Mat ims){

  Mat imd(ims.rows, ims.cols, CV_8UC1);

  Mat erosion(ims.rows, ims.cols, CV_8UC1);
  Mat opening(ims.rows, ims.cols, CV_8UC1);

  bool first = true;
  //int i = 0;
  do
  {
    //Erosion
    if(first) {
        first = false;
        erosion = ims.clone();
    }
    else {
      //erosion
      erode(opening.clone(),erosion,se);
    }

    //Opening
    morphologyEx(erosion, opening, MORPH_OPEN, se);

    Mat diff(ims.rows, ims.cols, CV_8UC1);
    diff = erosion - opening;

    add(imd, diff, imd);
  } while(!isImgBlack(opening));

  return(imd);
}

void process(const char *imsname){
  Mat image;
  image = imread(imsname, CV_LOAD_IMAGE_COLOR);

  if (!image.data)
  {
    cout << "Could not open or find the image" << '\n';
    return;
  }

  Mat kernel = getStructuringElement(MORPH_RECT,Size(2,2));
  Mat kernel2 = imread("disk-30.png", CV_LOAD_IMAGE_GRAYSCALE);

  blur(image, image, Size(10,10));

  Mat frame_HSV, frame_threshold, frame_threshold_terrain;
    // Convert from BGR to HSV colorspace
  cvtColor(image, frame_HSV, COLOR_BGR2HSV);

  inRange(frame_HSV, Scalar(0, 150, 150), Scalar(255, 255, 255), frame_threshold_terrain);

  for(int i = 0; i < 5; i++){
    morphologyEx(frame_threshold_terrain, frame_threshold_terrain, MORPH_CLOSE, kernel2);
  }

  //bitwise_not(frame_threshold_terrain, frame_threshold_terrain);
  // imshow("ap", frame_threshold_terrain);
  // waitKey('0');

  inRange(frame_HSV, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), frame_threshold);
  // imshow("apr", frame_threshold);
  // waitKey('0');

  Mat fr;

  subtract(frame_threshold, frame_threshold_terrain, fr);

  for(int i = 0; i < 2; i++){
    morphologyEx(fr, fr, MORPH_OPEN, kernel);
  }
  // imshow("sub", fr);
  // waitKey(0);




  Mat disk2 = imread("morphology/disk-2.png", CV_LOAD_IMAGE_GRAYSCALE);
  Mat disk10 = imread("morphology/disk10.png", CV_LOAD_IMAGE_GRAYSCALE);
  Mat open,squelette, dst, color_dst;

  // morphologyEx(fr, fr, MORPH_CLOSE, disk2);
  morphologyEx(fr, fr, MORPH_CLOSE, disk10);
  // morphologyEx(fr, fr, MORPH_OPEN, disk2);
  morphologyEx(fr, open, MORPH_OPEN, disk10);
  // imshow("open",open);
  // waitKey(0);
  
  //Skeleton
  //--------
  squelette=skeleton(disk2,open);
  // imshow("squelette",squelette);
  // waitKey(0);

  //Canny
  // Canny( squelette, dst, 50, 200, 3 );
  //cvtColor( dst, color_dst, CV_GRAY2BGR );

  //Hough
  vector<Vec4i> lines;
  //threshodl élévé : moins de lignes
  int threshold=60;
  HoughLinesP( squelette, lines, 1, CV_PI/180, threshold, 50, 200 );

    //Traitement des lignes sorties par Hough
    //---------------------------------------
    //Ajouter le coefficient directeur
  if(lines.size() != 0){
    double coefficients[lines.size()];
    double label[lines.size()] = {0};

    for( size_t i = 0; i < lines.size(); i++ ){
        //x(b)-x(a)
        double dx=lines[i][2]-lines[i][0];
        double dy=lines[i][3]-lines[i][1];

        double m=dy/dx;

        //Ajouter dans le tableau
        coefficients[i]=m;
    }
    int lab = 0;
    for(unsigned int i = 0; i < lines.size(); i++){
      if(label[i] == 0){
        lab++;
        label[i] = lab;
        for(unsigned int j = i+1; j < lines.size(); j++){
          if((label[j] == 0)&&(abs(coefficients[j]-coefficients[i])<0.1)){
            label[j] = label[i];
          }
        } 
      }   
    }

    for(int k = 1; k <= lab; k++){
      int R = rand()%255;
      int G = rand()%255;
      int B = rand()%255;
      for( size_t i = 0; i < lines.size(); i++ )
      {
        if(label[i] == k){
          line( image, Point(lines[i][0], lines[i][1]),
          Point(lines[i][2], lines[i][3]), Scalar(R,G,B), 3, 8 );
        }
      }
    }
  }
  namedWindow( "Detected Lines", 1 );
  imshow( "Detected Lines", image );
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

  clock_t t;
  t = clock();

  process(argv[1]);

  t = clock() - t;
  cout << "Exec time: " << ((float)t) / CLOCKS_PER_SEC << " s" << endl;
  return EXIT_SUCCESS;
}
