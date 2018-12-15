#include <iostream>
#include <cstdlib>
#include <math.h>

#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;


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
  Mat source,close1,close2,open,squelette, dst, color_dst;

  source=imread( imsname, IMREAD_GRAYSCALE );


  if(!source.data)
		cout <<  "Could not open or find the source image" << endl;

  //imshow( "Source", source );
  //waitKey(0);

  //Forme
  Mat disk2 = imread("morphology/disk-2.png", CV_LOAD_IMAGE_GRAYSCALE);
  Mat disk10 = imread("morphology/disk10.png", CV_LOAD_IMAGE_GRAYSCALE);

  //2 closing
  //---------
  //1 closing disk2
  morphologyEx(source, close1, MORPH_CLOSE, disk2);
  //imshow("close1",close1);
  //waitKey(0);

  //1 closing disk10
  morphologyEx(close1, close2, MORPH_CLOSE, disk10);
  //imshow("close2",close2);
  //waitKey(0);

  //opening
  //-------
  morphologyEx(close2, open, MORPH_OPEN, disk2);
  //imshow("open",open);
  //waitKey(0);

  //Skeleton
  //--------
  squelette=skeleton(disk2,open);
  imshow("squelette",squelette);
  waitKey(0);

  //Canny
  Canny( squelette, dst, 50, 200, 3 );
  cvtColor( dst, color_dst, CV_GRAY2BGR );

  //Hough
  vector<Vec4i> lines;
  //threshodl élévé : moins de lignes
  int threshold=80;
  HoughLinesP( dst, lines, 1, CV_PI/180, threshold, 30, 10 );
    for( size_t i = 0; i < lines.size(); i++ )
    {
        line( color_dst, Point(lines[i][0], lines[i][1]),
            Point(lines[i][2], lines[i][3]), Scalar(0,0,255), 3, 8 );
    }

    namedWindow( "Detected Lines", 1 );
    imshow( "Detected Lines", color_dst );
    waitKey(0);

    //Traitement des lignes sorties par Hough
    //---------------------------------------
    //Ajouter le coefficient directeur
    double coefficients[lines.size()];
    for( size_t i = 0; i < lines.size(); i++ )
    {
        //x(b)-x(a)
        double dx=lines[i][2]-lines[i][0];
        cout<<"dx "<<i<<" = "<<dx<<endl;
        //y(b)-y(a)
        double dy=lines[i][3]-lines[i][1];
        cout<<"dy "<<i<<" = "<<dy<<endl;
        //coefficient directeur m=dy/dx
        double m=dy/dx;

        //Ajouter dans le tableau
        coefficients[i]=m;
        cout<<"Coefficient "<<i<<" = "<<m<<endl;
    }

    double ref=coefficients[0];
    vector<Vec4i> sameLine;
    for( size_t i = 1; i < coefficients.size(); i++ ){
      if(coefficients[i]>ref-0.1 && coefficients[i]<ref+0.1){

      }
    }




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
