// Change Detection.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip> 


#include <opencv2/opencv.hpp>
//#include <opencv2/gpu/gpu.hpp>

#include "Background.h"

#include "SOBS_CF.h"

using namespace std;
using namespace cv;
namespace
{
  //Konstanten:
  //string path = "C:/Users/Uli/Documents/Uni/Semester 6/E_trainingsdaten/trainingsdaten/dynamicbackground/canoe/input/in";
  //string path = "E:/uniSonstiges/E_trainingsdaten/trainingsdaten/dynamicbackground/fountain/input/in"; 
  //string path = "C:/Users/Uli/Documents/Uni/Semester 6/E_trainingsdaten/trainingsdaten/dynamicbackground/fountain/input/in";
  //string path = "E:/uniSonstiges/E_trainingsdaten/trainingsdaten/base/highway/input/in";
  
  //string path = "C:/Users/Uli/Documents/Uni/Semester 6/E_trainingsdaten/trainingsdaten/base/highway/input/in";
  //string path = "C:/Users/Uli/Documents/Uni/Semester 6/E_trainingsdaten/trainingsdaten/shadow/bungalows/input/in";
  string path = "C:/Users/Uli/Documents/Uni/Semester 6/E_trainingsdaten/trainingsdaten/base/office/input/in";
  //string path = "C:/Users/Uli/Documents/Uni/Semester 6/E_trainingsdaten/trainingsdaten/base/Bootstrap/b"; // Tracken!!, achtung bmp mit 5 zahlen
  //string path = "C:/Users/Uli/Documents/Uni/Semester 6/E_trainingsdaten/trainingsdaten/intermittedObjectMotion/parking/input/in";
  //string path = "C:/Users/Uli/Documents/Uni/Semester 6/E_trainingsdaten/trainingsdaten/intermittedObjectMotion/streetLight/input/in";

}

int start(const char* path);
int main(int argc, char *argv[])
{
  size_t count = 1;
  ostringstream oss2;
  oss2 << path;
  oss2 << std::setfill ('0') << std::setw (6);
  oss2 << count << ".jpg";
  // Einlesen des Bildes
  Mat frame = imread(oss2.str(), CV_LOAD_IMAGE_COLOR);
  SOBS_CF sobs(frame);
  Mat fore;
  Mat back;

  namedWindow("fore");
  namedWindow("frame");
  namedWindow("Background");


  for(;;)
  {
    ++count;
    ostringstream oss;
    oss << path;
    oss << std::setfill ('0') << std::setw (6);
    oss << count << ".jpg";
    // Einlesen des Bildes
    frame = imread(oss.str(), CV_LOAD_IMAGE_COLOR);

    sobs.update(frame,fore);

    sobs.getBackgroundImage(back);

    imshow("frame",frame);
    imshow("fore",fore);
    imshow("Background",back);
    //waitKey(0);
    if(waitKey(10) >= 0) break;
  }

  /*
  Background bg(path,1,1,4, 500, 32);

  Mat frame;
  Mat back;
  Mat fore;

  std::vector<std::vector<Point> > contours;

  namedWindow("Frame");
  namedWindow("Background");
  namedWindow("fore");
  namedWindow("forevor");
  namedWindow("forenach");

  //namedWindow("mean");
  namedWindow("var");

  namedWindow("change");

  namedWindow("Track");

  size_t count = 0;

  while(true)
  {
    ++count;
    ostringstream oss;
    oss << path;
    oss << std::setfill ('0') << std::setw (6);
    oss << count << ".jpg";
    // Einlesen des Bildes
    Mat frame = imread(oss.str(), CV_LOAD_IMAGE_COLOR);//CV_LOAD_IMAGE_GRAYSCALE
    if(!frame.data)
      return false;
    // Hintergrund updaten und Fordergrundmatrix berechnen
    bg.update(frame, fore);

    imshow("fore",fore);

    bg.getBackgroundImage(back);

    //Konturen in Vordergrundmatrix finden und einzeichen
    findContours(fore,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
    drawContours(frame,contours,-1,Scalar(0,0,255),2);

    imshow("Frame",frame);
    imshow("Background",back);
    //waitKey(0);
    if(waitKey(10) >= 0) break;
  }
  */
  return 0;
}