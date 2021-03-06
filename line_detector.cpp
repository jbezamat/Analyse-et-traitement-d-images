#include <iostream>
#include <cstdlib>
#include <math.h>
#include <time.h>
#include <limits.h>
#include <opencv2/opencv.hpp>
#include <string>
#include <sstream>
#include <stdio.h>

using namespace cv;
using namespace std;


int low_H = 0, low_S = 70, low_V = 85;
int high_H = 255, high_S = 255, high_V = 255; //RGB



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

Mat process(Mat image){
  Mat image2 = image.clone();
  //Elements de traitement
  Mat kernel = getStructuringElement(MORPH_RECT,Size(3,3));
  Mat kernel2 = imread("morphology/disk-30.png", CV_LOAD_IMAGE_GRAYSCALE);
  Mat kernel3 = getStructuringElement(MORPH_RECT,Size(5,5));
  Mat kernel4 = getStructuringElement(MORPH_RECT,Size(8,8));
  Mat disk2 = imread("morphology/disk-2.png", CV_LOAD_IMAGE_GRAYSCALE);
  Mat disk10 = imread("morphology/disk10.png", CV_LOAD_IMAGE_GRAYSCALE);

  Mat frame_HSV, frame_threshold, frame_threshold_terrain;
  Mat temp, fr, open, squelette, dst, color_dst;
  Mat BGR[3];
  vector<Vec4i> lines;

  //imshow("image", image);

  split(image, BGR);
  dilate(BGR[0], BGR[0], kernel3);
  dilate(BGR[1], BGR[1], kernel3);
  dilate(BGR[2], BGR[2], kernel3);
  merge(BGR, 3,frame_HSV);

  //----RGB----
  inRange(image, Scalar(0, 64, 170), Scalar(255, 255, 255), frame_threshold_terrain);

  //// imshow("lines", frame_threshold_terrain);
  morphologyEx(frame_threshold_terrain, frame_threshold_terrain, MORPH_OPEN, kernel3);
  morphologyEx(frame_threshold_terrain, frame_threshold_terrain, MORPH_CLOSE, kernel2);
  //// imshow("terrain_bin", frame_threshold_terrain);

  dilate(frame_threshold_terrain, frame_threshold_terrain, kernel3);
  dilate(frame_threshold_terrain, frame_threshold_terrain, kernel3);
  dilate(frame_threshold_terrain, frame_threshold_terrain, kernel);

  //imshow("terrain_bin", frame_threshold_terrain);

  inRange(frame_HSV, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), frame_threshold);
  morphologyEx(frame_threshold, frame_threshold, MORPH_OPEN, kernel3);
  morphologyEx(frame_threshold, frame_threshold, MORPH_CLOSE, kernel4);
  // imshow("lines erode_bin", frame_threshold);


  //----Extraction du terrain----
  subtract(frame_threshold, frame_threshold_terrain, fr);

  morphologyEx(fr, fr, MORPH_CLOSE, kernel);
  // imshow("subtract", fr);

  morphologyEx(fr, fr, MORPH_OPEN, disk2);

  // // imshow("open",open);
  // waitKey(0);

  //-----Skeleton-----
  squelette=skeleton(disk2,fr);
  // imshow("squelette",squelette);

  //-----Hough-----
  //threshold élévé : moins de lignes
  int threshold=55;
  HoughLinesP( squelette, lines, 1, CV_PI/180, threshold, 50, 100 );

  //Traitement des lignes sorties par Hough
  //---------------------------------------
  //Ajouter le coefficient directeur
  if(lines.size() != 0){
    double coeficients[lines.size()][2];

    //Attention : si erreur remplacez par double label[lines.size()];
    double label[lines.size()] = {0};

    for( size_t i = 0; i < lines.size(); i++ ){

        double dx = lines[i][2] - lines[i][0];
        double dy = lines[i][3] - lines[i][1];
        if(dx == 0){
          coeficients[i][0] = -1000;
        }
        else{
          coeficients[i][0] = dy/dx;
          coeficients[i][1] = lines[i][1]-coeficients[i][0]*(lines[i][0]);
        }
    }

    //Determination des extrémités de la ligne finale
    int lab = 0;
    for(int i = 0; i < lines.size(); i++){
      if(label[i] == 0){
        if((coeficients[i][0] != 0)&&(coeficients[i][0] != -1000)){
          lab++;
          label[i] = lab;
        }
        for( int j = i+1; j < lines.size(); j++){
          if ((abs(coeficients[i][0]) >= 2)){
            if((label[j] == 0)&&(abs(coeficients[j][0]-coeficients[i][0])<2)&&(abs(coeficients[i][1] - coeficients[j][1])/sqrt(pow(coeficients[i][0],2)+1) < 20)){
              label[j] = label[i];
            }
          }
          else if((abs(coeficients[i][0])<=0.2)){
            if((label[j] == 0)&&(abs(coeficients[j][0]-coeficients[i][0])<0.05)&&(abs(coeficients[i][1] - coeficients[j][1])/sqrt(pow(coeficients[i][0],2)+1) < 20)){
              label[j] = label[i];
            }
          }
          else {
            if((label[j] == 0)&&(abs(coeficients[j][0]-coeficients[i][0])<0.15)&&(abs(coeficients[i][1] - coeficients[j][1])/sqrt(pow(coeficients[i][0],2)+1) < 20)){
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
            if(lines[j][1] < lines[j][3]){
              final_lines[i][0] = lines[j][0];
              final_lines[i][1] = lines[j][1];
              final_lines[i][2] = lines[j][2];
              final_lines[i][3] = lines[j][3];
            }
            else{
              final_lines[i][0] = lines[j][2];
              final_lines[i][1] = lines[j][3];
              final_lines[i][2] = lines[j][0];
              final_lines[i][3] = lines[j][1];
            }
            first = false;
          }
        }
        else{
          if(label[j] == i){
            if(coeficients[j][0] > 0.07){
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
            else if(coeficients[j][0] < -0.07){
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


    for(int k = 1; k <= lab; k++){
      int R = rand()%255;
      int G = rand()%255;
      int B = rand()%255;
      line( image, Point(final_lines[k][0], final_lines[k][1]),
      Point(final_lines[k][2], final_lines[k][3]), Scalar(R,G,B), 3, 8 );
      //cout << k << endl;
    }
  }
  // namedWindow( "Detected Lines", 1 );
  imshow( "Detected Lines", image );
  waitKey(0);
  return image;
}


void process(const char *imsname){
  Mat image;
  image = imread(imsname, CV_LOAD_IMAGE_COLOR);

  if (!image.data)
  {
    cout << "Could not open or find the image" << '\n';
    return;
  }
  process(image);
}

void process_video(char *direct_name)
{
  int c = 0; // centaines
  int d = 0; // dizaines
  int u = 1; // unités
  String image_name=string(direct_name)+string("/")+to_string(c)+to_string(d)+to_string(u)+string("-rgb.png");
  Mat image=imread(image_name);

  Size S = image.size();

  String video_name=string(direct_name)+string(".avi");
  VideoWriter outputVideo(video_name , CV_FOURCC('D', 'I', 'V', 'X'), 15, S, true);  //30 for 30 fps

  if (!outputVideo.isOpened()){
      cout  << "Could not open the output video for write: "<< endl;
      return;
  }

  do{
    image = process(image);
    // imshow("image",image);
    waitKey(0);

    outputVideo << image;

    if(u==9)
    {
      u=0;
      d++;
    }
    if(d==9)
    {
      d=0;
      c++;
    }
    u++;
    image_name=string(direct_name)+string("/")+to_string(c)+to_string(d)+to_string(u)+string("-rgb.png");
    image= imread(image_name);

  }while(image.data);
}

void usage (const char *s){
  std::cerr<<"Usage for image: "<<s<<" 0 image_name\n"<<std::endl;
  std::cerr<<"Usage for video: "<<s<<" 1 directory_name\n"<<std::endl;
  exit(EXIT_FAILURE);
}

#define param 2
int main( int argc, char* argv[] ){
  if(argc != (param+1)){
    usage(argv[0]);
  }

  clock_t t;
  t = clock();

  if(atoi(argv[1]) == 0){
    process(argv[2]);
  }
  else if(atoi(argv[1]) == 1){
    process_video(argv[2]);
  }
  else{
    usage(argv[0]);
  }
  t = clock() - t;
  cout << "Exec time: " << ((float)t) / CLOCKS_PER_SEC << " s" << endl;
  return EXIT_SUCCESS;
}
