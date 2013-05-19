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
  //string path = "E:/uniSonstiges/E_trainingsdaten/trainingsdaten/dynamicbackground/fountain/input/in"; // 6 sigma + 3x3 erode
  //string path = "C:/Users/Uli/Documents/Uni/Semester 6/E_trainingsdaten/trainingsdaten/dynamicbackground/fountain/input/in"; // 6 sigma + 3x3 erode
  //string path = "E:/uniSonstiges/E_trainingsdaten/trainingsdaten/base/highway/input/in";
  
  string path = "C:/Users/Uli/Documents/Uni/Semester 6/E_trainingsdaten/trainingsdaten/base/highway/input/in"; // klappt nicht gut, da autos schon am Anfang da lang fahren
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

   void kalman();

  static inline Point calcPoint(Point2f center, double R, double angle);

static void help();



}

int start(const char* path);
int main(int argc, char *argv[])
{
  //kalman();
  //return 0;
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

  namedWindow("mean");
  namedWindow("var");

  namedWindow("changevor");
  namedWindow("changeOpening");
  namedWindow("change");
  namedWindow("changeak");

  //namedWindow("alleRechtecke");
  //namedWindow("Rechtecke");

  namedWindow("frameroiContours");

  //namedWindow("features");
  //namedWindow("status");
  //namedWindow("err");

  size_t count = 0;

  while(true)
  {
    ++count;
    ostringstream oss;
    oss << path;
    oss << std::setfill ('0') << std::setw (6);
    oss << count << ".jpg";

    Mat frame = imread(oss.str(), CV_LOAD_IMAGE_COLOR);//CV_LOAD_IMAGE_GRAYSCALE
    if(!frame.data)
      return false;

    bg.update(frame, fore);

    imshow("fore",fore);

    bg.getBackgroundImage(back);

    findContours(fore,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
    drawContours(frame,contours,-1,Scalar(0,0,255),2);

    imshow("Frame",frame);
    imshow("Background",back);
    //waitKey(0);
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

static inline Point calcPoint(Point2f center, double R, double angle)
{
    return center + Point2f((float)cos(angle), (float)-sin(angle))*(float)R;
}

static void help()
{
    printf( "\nExamle of c calls to OpenCV's Kalman filter.\n"
"   Tracking of rotating point.\n"
"   Rotation speed is constant.\n"
"   Both state and measurements vectors are 1D (a point angle),\n"
"   Measurement is the real point angle + gaussian noise.\n"
"   The real and the estimated points are connected with yellow line segment,\n"
"   the real and the measured points are connected with red line segment.\n"
"   (if Kalman filter works correctly,\n"
"    the yellow segment should be shorter than the red one).\n"
            "\n"
"   Pressing any key (except ESC) will reset the tracking with a different speed.\n"
"   Pressing ESC will stop the program.\n"
            );
}

void kalman()
{
    help();
    Mat img(500, 500, CV_8UC3);
    KalmanFilter KF(2, 1, 0);
    Mat state(2, 1, CV_32F); /* (phi, delta_phi) */
    Mat processNoise(2, 1, CV_32F);
    Mat measurement = Mat::zeros(1, 1, CV_32F);
    char code = (char)-1;

    for(;;)
    {
        randn( state, Scalar::all(0), Scalar::all(0.1) );
        KF.transitionMatrix = *(Mat_<float>(2, 2) << 1, 1, 0, 1);

        setIdentity(KF.measurementMatrix);
        setIdentity(KF.processNoiseCov, Scalar::all(1e-5));
        setIdentity(KF.measurementNoiseCov, Scalar::all(1e-1));
        setIdentity(KF.errorCovPost, Scalar::all(1));

        randn(KF.statePost, Scalar::all(0), Scalar::all(0.1));

        for(;;)
        {
            Point2f center(img.cols*0.5f, img.rows*0.5f);
            float R = img.cols/3.f;
            double stateAngle = state.at<float>(0);
            Point statePt = calcPoint(center, R, stateAngle);

            Mat prediction = KF.predict();
            double predictAngle = prediction.at<float>(0);
            Point predictPt = calcPoint(center, R, predictAngle);

            randn( measurement, Scalar::all(0), Scalar::all(KF.measurementNoiseCov.at<float>(0)));

            // generate measurement
            measurement += KF.measurementMatrix*state;

            double measAngle = measurement.at<float>(0);
            Point measPt = calcPoint(center, R, measAngle);

            // plot points
            #define drawCross( center, color, d )                                 \
                line( img, Point( center.x - d, center.y - d ),                \
                             Point( center.x + d, center.y + d ), color, 1, CV_AA, 0); \
                line( img, Point( center.x + d, center.y - d ),                \
                             Point( center.x - d, center.y + d ), color, 1, CV_AA, 0 )

            img = Scalar::all(0);
            drawCross( statePt, Scalar(255,255,255), 3 );
            drawCross( measPt, Scalar(0,0,255), 3 );
            drawCross( predictPt, Scalar(0,255,0), 3 );
            line( img, statePt, measPt, Scalar(0,0,255), 3, CV_AA, 0 );
            line( img, statePt, predictPt, Scalar(0,255,255), 3, CV_AA, 0 );

            if(theRNG().uniform(0,4) != 0)
                KF.correct(measurement);

            randn( processNoise, Scalar(0), Scalar::all(sqrt(KF.processNoiseCov.at<float>(0, 0))));
            state = KF.transitionMatrix*state + processNoise;

            imshow( "Kalman", img );
            code = (char)waitKey(100);

            if( code > 0 )
                break;
        }
        if( code == 27 || code == 'q' || code == 'Q' )
            break;
    }

}
} //namespace