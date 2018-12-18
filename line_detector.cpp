#include <iostream>
#include <cstdlib>
#include <math.h>
#include <time.h>
#include <limits.h>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

// int low_H = 7, low_S = 140, low_V = 65;
// int high_H = 37, high_S = 255, high_V = 255;
int low_H = 0, low_S = 72, low_V = 81;
int high_H = 41, high_S = 175, high_V = 190; //RGB



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
  Mat image, image2;
  image = imread(imsname, CV_LOAD_IMAGE_COLOR);
  image2 = imread(imsname, CV_LOAD_IMAGE_COLOR);

  if (!image.data)
  {
    cout << "Could not open or find the image" << '\n';
    return;
  }

  Mat kernel = getStructuringElement(MORPH_RECT,Size(3,3));
  Mat kernel2 = imread("disk-30.png", CV_LOAD_IMAGE_GRAYSCALE);
  Mat frame_HSV, frame_threshold, frame_threshold_terrain;

  blur(image, frame_HSV, Size(5,5));
  imshow("image", frame_HSV);
    // Convert from BGR to HSV colorspace
  //cvtColor(image, frame_HSV, COLOR_BGR2HSV);

  // inRange(frame_HSV, Scalar(0, 150, 150), Scalar(255, 255, 255), frame_threshold_terrain);
  inRange(frame_HSV, Scalar(0, 64, 167), Scalar(255, 255, 255), frame_threshold_terrain); //RGB


  //for(int i = 0; i < 1; i++){
  morphologyEx(frame_threshold_terrain, frame_threshold_terrain, MORPH_CLOSE, kernel2);
  dilate(frame_threshold_terrain, frame_threshold_terrain, kernel);
  dilate(frame_threshold_terrain, frame_threshold_terrain, kernel);
  dilate(frame_threshold_terrain, frame_threshold_terrain, kernel);

  //}

  //bitwise_not(frame_threshold_terrain, frame_threshold_terrain);
  //imshow("ap", frame_threshold_terrain);
 

  inRange(frame_HSV, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), frame_threshold);
  morphologyEx(frame_threshold, frame_threshold, MORPH_CLOSE, kernel);

  //imshow("apr", frame_threshold);


  Mat fr;

  subtract(frame_threshold, frame_threshold_terrain, fr);

  //for(int i = 0; i < 1; i++){
  morphologyEx(fr, fr, MORPH_CLOSE, kernel2);
  //}
  imshow("sub", fr);
  waitKey(0);

  Mat disk2 = imread("morphology/disk-2.png", CV_LOAD_IMAGE_GRAYSCALE);
  Mat disk10 = imread("morphology/disk10.png", CV_LOAD_IMAGE_GRAYSCALE);

  Mat open,squelette, dst, color_dst;

  //morphologyEx(fr, fr, MORPH_CLOSE, disk10);
  morphologyEx(fr, fr, MORPH_OPEN, disk2);
  //morphologyEx(fr, fr, MORPH_OPEN, disk10);
  //morphologyEx(fr, open, MORPH_CLOSE, disk10);
  // imshow("open",open);
  // waitKey(0);
  
  //Skeleton
  //--------
  squelette=skeleton(disk2,fr);
  imshow("squelette",squelette);
  waitKey(0);

  //Canny
  // Canny( squelette, dst, 50, 200, 3 );
  //cvtColor( dst, color_dst, CV_GRAY2BGR );

  //Hough
  vector<Vec4i> lines;
  //threshodl élévé : moins de lignes
  int threshold=35;
  HoughLinesP( squelette, lines, 1, CV_PI/180, threshold, 50, 100 );

    //Traitement des lignes sorties par Hough
    //---------------------------------------
    //Ajouter le coefficient directeur
  if(lines.size() != 0){
    //double equations[lines.size()][2] = {0,0};
    double coeficients[lines.size()][2];
    double label[lines.size()] = {0};

    for( size_t i = 0; i < lines.size(); i++ ){
        // Mat x;
        // Mat A = (Mat_<double>(2,2) << lines[i][0], 1,
        //                               lines[i][2], 1);
        // Mat B = (Mat_<double>(2,1) << lines[i][1], lines[i][3]);

        // solve(A, B, x);

        // //Ajouter dans le tableau
        // equations[i][0]=x.at<double>(0,0);
        // equations[i][0]=x.at<double>(1,0);
        // cout << equations[i][0] << " " << equations[i][1] << endl;

        double dx = lines[i][2] - lines[i][0];
        double dy = lines[i][3] - lines[i][1];
        coeficients[i][0] = dy/dx;
        coeficients[i][1] = lines[i][1]-coeficients[i][0]*(lines[i][0]);

        //cout << coeficients[i][0] << " " << coeficients[i][1] << endl;
    }


    int lab = 0;
    for( int i = 0; i < lines.size(); i++){
      if(label[i] == 0){
        if((coeficients[i][0] != 0)){
          lab++;
          label[i] = lab;
        }
        for( int j = i+1; j < lines.size(); j++){
          if (abs(coeficients[i][0]) >= 2){
            if((label[j] == 0)&&(abs(coeficients[j][0]-coeficients[i][0])<2)){
              label[j] = label[i];
            }
          }
          else if(abs(coeficients[i][0])<=0.2){
            if((label[j] == 0)&&(abs(coeficients[j][0]-coeficients[i][0])<0.05)){
              label[j] = label[i];
            }
          }
          else {
            if((label[j] == 0)&&(abs(coeficients[j][0]-coeficients[i][0])<0.15)){
              label[j] = label[i];
            }
          }
        } 
      }
    }
    int final_lines[lab][4];
    bool first = true;

    for(int i = 1; i <= lab; i++){
      first = true;
      for(int j = 0; j < lines.size(); j++){
        if(first == true){
          if(label[j] == i){
            final_lines[i][0] = lines[j][0];
            final_lines[i][1] = lines[j][1];
            final_lines[i][2] = lines[j][2];
            final_lines[i][3] = lines[j][3];
            first = false;
          }
        }
        else{
          if(label[j] == i){
            if(coeficients[j][0] > 0.05){
              if((lines[j][0] < final_lines[i][0])||(lines[j][1] < final_lines[i][1])){
                final_lines[i][0] = lines[j][0];
                final_lines[i][1] = lines[j][1];
              }
              if((lines[j][2] < final_lines[i][0])||(lines[j][3] < final_lines[i][1])){
                final_lines[i][0] = lines[j][2];
                final_lines[i][1] = lines[j][3];
              }
              if((lines[j][0] > final_lines[i][2])||(lines[j][1] > final_lines[i][3])){
                final_lines[i][2] = lines[j][0];
                final_lines[i][3] = lines[j][1];
              }
              if((lines[j][2] > final_lines[i][2])||(lines[j][3] > final_lines[i][3])){
                final_lines[i][2] = lines[j][2];
                final_lines[i][3] = lines[j][3];
              }
            }
            else if(coeficients[j][0] < -0.05){
              if((lines[j][0] > final_lines[i][0])||(lines[j][1] < final_lines[i][1])){
                final_lines[i][0] = lines[j][0];
                final_lines[i][1] = lines[j][1];
              }
              if((lines[j][2] > final_lines[i][0])||(lines[j][3] < final_lines[i][1])){
                final_lines[i][0] = lines[j][2];
                final_lines[i][1] = lines[j][3];
              }
              if((lines[j][0] < final_lines[i][2])||(lines[j][1] > final_lines[i][3])){
                final_lines[i][2] = lines[j][0];
                final_lines[i][3] = lines[j][1];
              }
              if((lines[j][2] < final_lines[i][2])||(lines[j][3] > final_lines[i][3])){
                final_lines[i][2] = lines[j][2];
                final_lines[i][3] = lines[j][3];
              }
            }
            else{
              if((lines[j][1] < final_lines[i][1])){
                final_lines[i][0] = lines[j][0];
                final_lines[i][1] = lines[j][1];
              }
              if((lines[j][3] < final_lines[i][1])){
                final_lines[i][0] = lines[j][2];
                final_lines[i][1] = lines[j][3];
              }
              if((lines[j][1] > final_lines[i][3])){
                final_lines[i][2] = lines[j][0];
                final_lines[i][3] = lines[j][1];
              }
              if((lines[j][3] > final_lines[i][3])){
                final_lines[i][2] = lines[j][2];
                final_lines[i][3] = lines[j][3];
              }
            }
          }
        }
      }
    }

    // for(int k = 1; k <= lab; k++){
    //   int R = rand()%255;
    //   int G = rand()%255;
    //   int B = rand()%255;
    //   for( size_t i = 1; i <= lines.size(); i++ )
    //   {
    //     if(label[i] == k){
    //       line( image, Point(lines[i][0], lines[i][1]),
    //       Point(lines[i][2], lines[i][3]), Scalar(0,0,0), 3, 8 );
    //       cout << coeficients[i][0] << " et " << label[i] <<endl;
    //     }
    //   }
    // }
    for(int k = 1; k <= lab; k++){
      int R = rand()%255;
      int G = rand()%255;
      int B = rand()%255;
      line( image, Point(final_lines[k][0], final_lines[k][1]),
      Point(final_lines[k][2], final_lines[k][3]), Scalar(R,G,B), 3, 8 );
      //cout << k << endl;
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
