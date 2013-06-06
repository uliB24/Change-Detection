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
  
  //string path = "C:/Users/Uli/Documents/Uni/Semester 6/E_trainingsdaten/trainingsdaten/dynamicbackground/fountain/input/in";
  
  string path = "E:/uniSonstiges/E_trainingsdaten/trainingsdaten/base/highway/input/in"; //sehr gut, aber Auto im Schtten-> schattiger Teil teilweise nicht segmentiert
  //string path = "E:/uniSonstiges/E_trainingsdaten/trainingsdaten/dynamicbackground/canoe/input/in"; //gut, Köpfe teilweise abgeschnitten
  //string path = "E:/uniSonstiges/E_trainingsdaten/trainingsdaten/dynamicbackground/fountain/input/in"; // viel bei Fontainen
  
  //string path = "E:/uniSonstiges/E_trainingsdaten/trainingsdaten/base/office/input/in"; //gut, Hase wird etwas aufgefressen
  //string path = "E:/uniSonstiges/E_trainingsdaten/trainingsdaten/base/Bootstrap/b"; //gut, aber da wo Leute während der gesamten Lernphase stehen Probleme
  //string path = "E:/uniSonstiges/E_trainingsdaten/trainingsdaten/dynamicbackground/WavingTrees/b"; sehr gut, wenig Rauschen bei Baum
  
  //string path = "E:/uniSonstiges/E_trainingsdaten/trainingsdaten/intermittedObjectMotion/ForegroundAperture/b"; //schlecht, schlefender Typ bleibt Hintergrund
  //string path = "E:/uniSonstiges/E_trainingsdaten/trainingsdaten/intermittedObjectMotion/streetLight/input/in"; //gut, aber Steilweise bei Schtten auf Lastern doof
  //string path = "E:/uniSonstiges/E_trainingsdaten/trainingsdaten/intermittedObjectMotion/parking/input/in"; //ok

  //string path = "E:/uniSonstiges/E_trainingsdaten/trainingsdaten/shadow/backdoor/input/in"; //Probleme mit Somme, ansonsten ok
  //string path = "E:/uniSonstiges/E_trainingsdaten/trainingsdaten/shadow/bungalows/input/in"; //ok, Probleme bei Spiegelung, Schtten noch rausnehmen?
  //string path = "E:/uniSonstiges/E_trainingsdaten/trainingsdaten/shadow/copyMachine/input/in"; //bei kürzerer Lernrate besser(80), schatten
  
  //string path = "E:/uniSonstiges/E_trainingsdaten/sofa/sofa/input/in";
  
  //string path = "C:/Users/Uli/Documents/Uni/Semester 6/E_trainingsdaten/trainingsdaten/base/highway/input/in";
  //string path = "C:/Users/Uli/Documents/Uni/Semester 6/E_trainingsdaten/trainingsdaten/shadow/bungalows/input/in";
  //string path = "C:/Users/Uli/Documents/Uni/Semester 6/E_trainingsdaten/trainingsdaten/base/office/input/in";
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
  //oss2 << std::setfill ('0') << std::setw (5);
  oss2 << count << ".jpg";
  //oss2 << count << ".bmp";
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
    //oss << std::setfill ('0') << std::setw (5);
    oss << count << ".jpg";
    //oss << count << ".bmp";
    // Einlesen des Bildes
    frame = imread(oss.str(), CV_LOAD_IMAGE_COLOR);
    if(!frame.data)
      break;

    sobs.update(frame,fore);

    sobs.getBackgroundImage(back);

    imshow("frame",frame);
    imshow("fore",fore);
    imshow("Background",back);
    //if(count > 1080)
      //waitKey(0);
    if(waitKey(10) >= 0) 
      break;
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