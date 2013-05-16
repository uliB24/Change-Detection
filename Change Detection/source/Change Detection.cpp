// Change Detection.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip> 


#include <opencv2/opencv.hpp>
#include <opencv2/gpu/gpu.hpp>

#include "Background.h"
#include "Vobject.h"

#ifdef HAVE_OPENCV_NONFREE
#include "opencv2/nonfree/gpu.hpp"
#endif


using namespace std;
using namespace cv;
using namespace cv::gpu;

namespace
{
  //Konstanten:
  //string path = "C:/Users/Uli/Documents/Uni/Semester 6/E_trainingsdaten/trainingsdaten/dynamicbackground/canoe/input/in"; // 4 sigma, 1x1 erode
  string path = "C:/workspace/trainingsdaten/dynamicbackground/fountain/input/in"; // 6 sigma + 3x3 erode
  //string path = "E:/uniSonstiges/E_trainingsdaten/trainingsdaten/base/highway/input/in";
  
  //string path = "C:/Users/Uli/Documents/Uni/Semester 6/E_trainingsdaten/trainingsdaten/base/highway/input/in"; // klappt nicht gut, da autos schon am Anfang da lang fahren
  // -> bei der erstellung der Varianz muss bereits erkannt werden, dass fahrende Autos nicht in die Varianz und den Hintergrund einfließen
  //string path = "C:/Users/Uli/Documents/Uni/Semester 6/E_trainingsdaten/trainingsdaten/shadow/bungalows/input/in"; // schtatten muss entfernt werden, autos fahren schon am begin da lang(nicht so schlimm wie bei highway)
  //string path = "C:/Users/Uli/Documents/Uni/Semester 6/E_trainingsdaten/trainingsdaten/base/office/input/in"; // mensch muss getrackt werden
  //string path = "C:/Users/Uli/Documents/Uni/Semester 6/E_trainingsdaten/trainingsdaten/base/Bootstrap/b"; // Tracken!!, achtung bmp mit 5 zahlen
  //string path = "C:/Users/Uli/Documents/Uni/Semester 6/E_trainingsdaten/trainingsdaten/intermittedObjectMotion/parking/input/in";streetLight
  //string path = "C:/Users/Uli/Documents/Uni/Semester 6/E_trainingsdaten/trainingsdaten/intermittedObjectMotion/streetLight/input/in"; //gleiches problem wie bei highway, weiße autos in mitteler Fahrspur
  //enums:
  enum Method
  {
    FGD_STAT,
    MOG,
    MOG2,
#ifdef HAVE_OPENCV_NONFREE
    VIBE,
#endif
    GMG
  };

  //freie Funktionen:
  void testGPUBackgroundSub(Method m);

  



}

int start(const char* path);
int main(int argc, char *argv[])
{
  Background bg(path,2,2,4);

  Mat frame;
  Mat back;
  Mat fore;

  std::vector<std::vector<Point> > contours;

  namedWindow("Frame");
  namedWindow("Background");
  namedWindow("fore");
  namedWindow("forevor");
  namedWindow("forenach");

  namedWindow("mean");
  namedWindow("var");

  namedWindow("changevor");
  namedWindow("changeOpening");
  namedWindow("change");
  namedWindow("changeak");

  namedWindow("alleRechtecke");
  namedWindow("Rechtecke");

  namedWindow("frameroiContours");

  size_t count = 0;

  while(true)
  {
    ++count;
    ostringstream oss;
    oss << path;
    oss << std::setfill ('0') << std::setw (6);
    oss << count << ".jpg";

    Mat frame = imread(oss.str(), CV_LOAD_IMAGE_COLOR);
    if(!frame.data)
      return false;

    bg.update(frame, fore);

    imshow("fore",fore);

    bg.getBackgroundImage(back);

    findContours(fore,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
    drawContours(frame,contours,-1,Scalar(0,0,255),2);


    imshow("Frame",frame);
    imshow("Background",back);
    
	vobject ob;
	ob.refresh(fore);
	
	/*
	if(contours.size()>0)
	{
		for(unsigned int i=0; i<contours.size(); ++i)
		{
			if(contours.at(i).size()>0)
			{
				for(unsigned int j=0; j<contours.at(i).size(); ++j)
				{			
					
					cout << "!!!!!!!!!!!!!  " << fore.at<unsigned char>(contours.at(i).at(j).y,contours.at(i).at(j).x) << " " <<
						 fore.at<char>(contours.at(i).at(j).y,contours.at(i).at(j).x) << " " <<
						  fore.at<unsigned int>(contours.at(i).at(j).y,contours.at(i).at(j).x) << " " <<
						   fore.at<int>(contours.at(i).at(j).y,contours.at(i).at(j).x) << endl;
				}
			}
		}
	}
	*/

	//waitKey(0);
    if(waitKey(30) >= 0) break;
  }
  return 0;
}

namespace
{
void testGPUBackgroundSub(Method m)
{
  Mat frame;
  ostringstream oss2;
  oss2 << path;
  oss2 << std::setfill ('0') << std::setw (6);
  oss2 << 1 << ".jpg";
  frame = imread(oss2.str(), CV_LOAD_IMAGE_COLOR);

  GpuMat d_frame(frame);

  FGDStatModel fgd_stat;
  MOG_GPU mog;
  MOG2_GPU mog2;
#ifdef HAVE_OPENCV_NONFREE
  VIBE_GPU vibe;
#endif
  GMG_GPU gmg;
  gmg.numInitializationFrames = 40;

  GpuMat d_fgmask;
  GpuMat d_fgimg;
  GpuMat d_bgimg;

  Mat fgmask;
  Mat fgimg;
  Mat bgimg;

  switch (m)
  {
  case FGD_STAT:
    fgd_stat.create(d_frame);
    break;

  case MOG:
    mog(d_frame, d_fgmask, 0.01f);
    break;

  case MOG2:
    mog2(d_frame, d_fgmask);
    break;

#ifdef HAVE_OPENCV_NONFREE
  case VIBE:
    vibe.initialize(d_frame);
    break;
#endif

  case GMG:
    gmg.initialize(d_frame.size());
    break;
  }

  namedWindow("image", WINDOW_NORMAL);
  namedWindow("foreground mask", WINDOW_NORMAL);
  namedWindow("foreground image", WINDOW_NORMAL);
  if (m != GMG
#ifdef HAVE_OPENCV_NONFREE
    && m != VIBE
#endif
    )
  {
    namedWindow("mean background image", WINDOW_NORMAL);
  }
  size_t count = 1;
  for(;;)
  {
    ostringstream oss;
    oss << path;
    oss << std::setfill ('0') << std::setw (6);
    oss << count << ".jpg";
    frame = imread(oss.str(), CV_LOAD_IMAGE_COLOR);
    ++count;
    if (frame.empty())
      break;
    d_frame.upload(frame);

    int64 start = cv::getTickCount();

    //update the model
    switch (m)
    {
    case FGD_STAT:
      fgd_stat.update(d_frame);
      d_fgmask = fgd_stat.foreground;
      d_bgimg = fgd_stat.background;
      break;

    case MOG:
      mog(d_frame, d_fgmask, 0.01f);
      mog.getBackgroundImage(d_bgimg);
      break;

    case MOG2:
      mog2(d_frame, d_fgmask);
      mog2.getBackgroundImage(d_bgimg);
      break;

#ifdef HAVE_OPENCV_NONFREE
    case VIBE:
      vibe(d_frame, d_fgmask);
      break;
#endif

    case GMG:
      gmg(d_frame, d_fgmask);
      break;
    }

    double fps = cv::getTickFrequency() / (cv::getTickCount() - start);
    std::cout << "FPS : " << fps << std::endl;

    d_fgimg.setTo(Scalar::all(0));
    d_frame.copyTo(d_fgimg, d_fgmask);

    d_fgmask.download(fgmask);
    d_fgimg.download(fgimg);
    if (!d_bgimg.empty())
      d_bgimg.download(bgimg);

    imshow("image", frame);
    imshow("foreground mask", fgmask);
    imshow("foreground image", fgimg);
    if (!bgimg.empty())
      imshow("mean background image", bgimg);

    int key = waitKey(30);
    if (key == 27)
      break;
  }
}
} //namespace