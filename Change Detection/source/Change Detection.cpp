// Change Detection.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip> 

#include "test.h"

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(int argc, char *argv[])
{
    Mat frame;
    Mat back;
    Mat fore;
    VideoCapture cap(0);
    BackgroundSubtractorMOG2 bg;

    //bg.nmixtures = 3;
    //bg.bShadowDetection = false;
 
    std::vector<std::vector<Point> > contours;
 
    namedWindow("Frame");
    namedWindow("Background");
 
	  size_t count = 1;
    
	  //string path = "C:/Users/Uli/Documents/Uni/Semester 6/E_trainingsdaten/trainingsdaten/dynamicbackground/canoe/input/in"; //funktioniert ganz gut (aber viele kleine Teile im Wasser)
    //string path = "C:/Users/Uli/Documents/Uni/Semester 6/E_trainingsdaten/trainingsdaten/dynamicbackground/fountain/input/in"; // Fontainen werden mitsegmentiert
    //string path = "C:/Users/Uli/Documents/Uni/Semester 6/E_trainingsdaten/trainingsdaten/base/highway/input/in"; //Einige ausreißer
    string path = "C:/Users/Uli/Documents/Uni/Semester 6/E_trainingsdaten/trainingsdaten/shadow/bungalows/input/in";
    for(;;)
    {
		  ostringstream oss;
		  oss << path;
		  oss << std::setfill ('0') << std::setw (6);
		  oss << count << ".jpg";
		  frame = imread(oss.str(), CV_LOAD_IMAGE_COLOR);
		  ++count;
		  if(!frame.data)
			  break;
        //cap >> frame;
      bg.operator ()(frame,fore);
      bg.getBackgroundImage(back);
      erode(fore,fore,Mat());
      dilate(fore,fore,Mat());
      findContours(fore,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
      drawContours(frame,contours,-1,Scalar(0,0,255),2);
      imshow("Frame",frame);
      imshow("Background",back);
      if(waitKey(10) >= 0) break;
    }
    return 0;
}
