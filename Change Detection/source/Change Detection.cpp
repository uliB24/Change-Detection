// Change Detection.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip> 

#include "test.h"

#include <opencv2/opencv.hpp>
#include "opencv2/gpu/gpu.hpp"

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
  string path = "C:/Users/Uli/Documents/Uni/Semester 6/E_trainingsdaten/trainingsdaten/dynamicbackground/fountain/input/in"; // 6 sigma + 3x3 erode
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
  //return start("C:/Users/Uli/Documents/Uni/Semester 6/E_trainingsdaten/trainingsdaten/dynamicbackground/fountain/input/in000001.jpg");
  //testGPUBackgroundSub(Method::FGD_STAT);

  //return 0;
  /*
  float x[15] = {1,4,9,5,3,4,5,4,6,7,4,4,6,3,5};
  float n = 0;
  float m = 0;
  float m2 = 0;
  float v = 0;
  for(int i = 0; i < 15; ++i)
  {
    ++n;
    float delta = x[i] - m;
    m += delta / n; 
    m2 += delta * (x[i] - m);
    if(n>1)
      v = m2 / (n-1);
    cout << v << endl;
  }
  return 0;
  */


    Mat frame;
    Mat back;
    Mat fore;
    //VideoCapture cap(0);
    BackgroundSubtractorMOG2 bg;      
    int size2 = 2;
    Mat erosion = getStructuringElement( MORPH_ELLIPSE,
                                       Size( 2*size2 + 1, 2*size2+1 ),
                                       Point( size2, size2 ) );
    Mat dilatation = getStructuringElement( MORPH_ELLIPSE,
                                       Size( 2*size2 + 1, 2*size2+1 ),
                                       Point(size2, size2 ) );


    //bg.nmixtures = 3;
    //bg.bShadowDetection = false;
 
    std::vector<std::vector<Point> > contours;
 
    namedWindow("Frame");
    namedWindow("Background");
    namedWindow("fore");
    namedWindow("forevor");
    namedWindow("forenach");

    namedWindow("mean");
    namedWindow("var");
 
	  size_t count = 1;
    ostringstream oss2;
		  oss2 << path;
		  oss2 << std::setfill ('0') << std::setw (6);
		  oss2 << count << ".jpg";
    frame = imread(oss2.str(), CV_LOAD_IMAGE_COLOR);
    
    Mat mean(frame.size(), CV_32FC3, Scalar(0.0f,0.0f,0.0f));
    Mat M2(frame.size(), CV_32FC3, Scalar(0.0f,0.0f,0.0f));
    Mat var(frame.size(), CV_32FC3, Scalar(0.0f,0.0f,0.0f));

    float n = 0;

    Vec3f delta;
    Size size = frame.size();
    for(;;)
    {
      
		  ostringstream oss;
		  oss << path;
		  oss << std::setfill ('0') << std::setw (6);
		  oss << count << ".jpg";
		  frame = imread(oss.str(), CV_LOAD_IMAGE_COLOR);
      //frame.convertTo(frame, CV_32FC3);
		  ++count;
		  if(!frame.data)
			  break;
        //cap >> frame;
      ++n;
      /*
      http://en.wikipedia.org/wiki/Algorithms_for_calculating_variance
      online_variance
      */

      if(n < 200)
      {
      for(int x = 0; x < size.width; ++x)
      {
        for(int y = 0; y < size.height; ++y)
        {
          
          for(int i = 0; i < 3; ++i)
          {
            float f = frame.at<Vec3b>(y, x).val[i];
            float me = mean.at<Vec3f>(y, x).val[i];
            delta.val[i] = frame.at<Vec3b>(y, x).val[i] - mean.at<Vec3f>(y, x).val[i];
            mean.at<Vec3f>(y, x).val[i] += delta.val[i] / n;
            M2.at<Vec3f>(y,x).val[i] += delta.val[i] * (frame.at<Vec3b>(y, x).val[i] - mean.at<Vec3f>(y, x).val[i]);
            if(n>198)
              var.at<Vec3f>(y,x).val[i] = sqrt(M2.at<Vec3f>(y,x).val[i] / (n-1));
            //if(x == 60 && y == 120)
              //cout << var.at<Vec3f>(y,x).val[i] << endl;
          }
        }
      }
      }

      

      /*
      http://de.wikipedia.org/wiki/Standardabweichung#Berechnung_f.C3.BCr_auflaufende_Messwerte
      
      for(int x = 0; x < size.width; ++x)
      {
        for(int y = 0; y < size.height; ++y)
        {
          mean.at<Vec3b>(y, x) += frame.at<Vec3b>(y, x);
          for(int i = 0; i < 3; ++i)
          {
            M2.at<Vec3b>(y,x).val[i] += frame.at<Vec3b>(y, x).val[i] * frame.at<Vec3b>(y, x).val[i];
            if(n>1)
              var.at<Vec3b>(y,x).val[i] = sqrt((1/(n-1)) * ( M2.at<Vec3b>(y,x).val[i] - (1/n) * mean.at<Vec3b>(y, x).val[i] * mean.at<Vec3b>(y, x).val[i]));
          }
        }
      }
      */
      imshow("mean",mean / 255);
      imshow("var",var / 255);


      bg.operator ()(frame,fore);
      imshow("forevor",fore);

      bg.getBackgroundImage(back);
      for(size_t x = 0; x < fore.size().width; ++x)
      {
        for(size_t y = 0; y < fore.size().height; ++y)
        {
          //for(int i = 0; i < 3; ++i)
            //if(x == 110 && y == 145)
              //cout << "Diff: " << abs(frame.at<Vec3b>(y,x).val[i] - back.at<Vec3b>(y,x).val[i]) << endl << "var: " << var.at<Vec3f>(y,x).val[i] << "\n";
          //cout << endl;
          if(abs(frame.at<Vec3b>(y,x).val[0] - back.at<Vec3b>(y,x).val[0]) < 3*(var.at<Vec3f>(y,x).val[0]) ||
             abs(frame.at<Vec3b>(y,x).val[1] - back.at<Vec3b>(y,x).val[1]) < 3*(var.at<Vec3f>(y,x).val[1]) ||
             abs(frame.at<Vec3b>(y,x).val[2] - back.at<Vec3b>(y,x).val[2]) < 3*(var.at<Vec3f>(y,x).val[2]))
          {
            fore.at<uchar>(y,x) = 0;
          }
        }
      }
      imshow("forenach",fore);


      erode(fore,fore,erosion);
      dilate(fore,fore,dilatation);
      imshow("fore",fore);
      findContours(fore,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
      drawContours(frame,contours,-1,Scalar(0,0,255),2);
      
      imshow("Frame",frame);
      imshow("Background",back);
      if(waitKey(10) >= 0) break;
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