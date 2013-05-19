#include "Background.h"

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <opencv2/opencv.hpp>


using namespace std;
using namespace cv;

namespace
{
  struct comRect
  {
    comRect() {}
    comRect(Rect r, bool m, double a) : rect(r), merged(m), maxArea(a) {}
    Rect rect;
    bool merged;
    double maxArea;
  };

  Rect bRect(const vector<Point>& vec);

 
}

Background::Background(string path, size_t erosion_size, size_t dilatation_size, size_t numSigma, int history, float varThreshold, bool bShadowDetection)
  : m_path(path), m_n(0), m_numSigma(numSigma), m_bg(history, varThreshold, bShadowDetection)
{
	ostringstream oss;
	oss << m_path;
	oss << std::setfill ('0') << std::setw (6);
  oss << 1 << ".jpg";
	Mat frame = imread(oss.str(), CV_LOAD_IMAGE_COLOR);//CV_LOAD_IMAGE_GRAYSCALE

	m_mean  = Mat(frame.size(), CV_32FC3, Scalar(0.0f,0.0f,0.0f));
  m_M2	= Mat(frame.size(), CV_32FC3, Scalar(0.0f,0.0f,0.0f));
  m_sigma = Mat(frame.size(), CV_32FC3, Scalar(0.0f,0.0f,0.0f));

  m_erosionKernel = getStructuringElement( MORPH_ELLIPSE, Size( 2*erosion_size + 1, 2*erosion_size+1 ), Point( erosion_size, erosion_size ) );
  m_dilatationKernel = getStructuringElement( MORPH_ELLIPSE, Size( 2*dilatation_size + 1, 2*dilatation_size+1 ), Point(dilatation_size, dilatation_size ) );

  /*
  //Mat f;
  //cv::cvtColor(frame,image_next, CV_BGR2GRAY);
  //imshow("fratures", f);
  //buildOpticalFlowPyramid(f, image_next, Size(21,21), 2);
image_next = frame;

//frame.convertTo(image_next, CV_32F);
  // Obtain initial set of features
  cv::goodFeaturesToTrack(image_next, // the image 
  features_next,   // the output detected features
  20,  // the maximum number of features 
  0.01,     // quality level
  10     // min distance between two features
);
  count = 0;
  */
}


Background::~Background(void)
{
}

/*
http://stackoverflow.com/questions/9701276/opencv-tracking-using-optical-flow
http://opencv.willowgarage.com/documentation/cpp/motion_analysis_and_object_tracking.html

http://docs.opencv.org/modules/video/doc/motion_analysis_and_object_tracking.html
Idee: 
goodFeaturesToTrack weglasssen und Inputpunkte f�r
calcOpticalFlowPyrLK selber bestimmen (Dinge vom Rand)
-> Tracken mit calcOpticalFlowPyrLK



http://docs.opencv.org/modules/imgproc/doc/histograms.html
EMD -> minimale Distanz zwischen 2 gewichteten Punktwolken


http://docs.opencv.org/trunk/modules/videostab/doc/global_motion.html#videostab-motionmodel
motionModel
+ samples/videostab.cpp

http://opencv.willowgarage.com/documentation/c/imgproc_structural_analysis_and_shape_descriptors.html
cvMatchShapes
2 Bilder oder Punktewolken vergleichen

http://bytesandlogics.wordpress.com/2012/08/23/detectionbasedtracker-opencv-implementation/
DetectionBasedTracker 
OpenCV Haar implementation

http://www.intorobotics.com/how-to-detect-and-track-object-with-opencv/
�berblick


http://www.pages.drexel.edu/~nk752/tutorials.html
Tutorials

http://dasl.mem.drexel.edu/~noahKuntz/openCVTut9.html
Tutorial Optical Flow und Kalman

Learning OpenCV: Computer Vision with the OpenCV Library (Buch 33 Doller)

cppreference.com

*/

void Background::update(const cv::Mat& frame, cv::Mat& fore)
{
  fore = Mat(frame.size(), CV_8U);
  ++m_n;
  /*
  
  image_prev = image_next.clone();
  features_prev = features_next;
  //Mat f;
  //cv::cvtColor(frame,f, CV_BGR2GRAY);
  //buildOpticalFlowPyramid(f, image_next, Size(21,21), 2);
  vector<uchar> status;
  image_next = frame;
  //frame.convertTo(image_next, CV_32F);
  //cv::cvtColor(frame,image_next, CV_BGR2GRAY);
  vector<float> err;
  int a = Mat(features_prev).checkVector(2, CV_32F, true);
  if(count == 0)
  {
    ++count;
    return;
  }
    // Find position of feature in new image
    cv::calcOpticalFlowPyrLK(
      image_prev, image_next, // 2 consecutive images
      features_prev, // input point positions in first im
      features_next, // output point positions in the 2nd
      status,    // tracking success
      err      // tracking error
    );
    imshow("fratures", features_next);
    imshow("status", status);
    imshow("err", err);
    return;
  
  
  */

  //vector<Rect> roi;
  /*
  for(auto it(m_roi.begin()); it != m_roi.end(); ++it)
  {
    if((*it).second > 2)
      m_roi.erase(it);
    else
      ++(*it).second;
  }*/

  if(m_lastFrame.data)
  {
    Mat alle;
    Mat changeak;
    Mat change = abs(m_lastFrame - frame);
    imshow("changevor", change);
    //Mat show = change.clone();
    //Mat show; //= Mat(change.size(), IPL_DEPTH_16U, Scalar(0,0,0));
    Mat edges;
    std::vector<std::vector<cv::Point> > contours;
    std::vector<std::vector<cv::Point> > bigcontours;
    //cvtColor( change, change, CV_BGR2GRAY );
    /*size_t size = 0;
    Mat erosionKernel = getStructuringElement( MORPH_ELLIPSE, Size( 2*size + 1, 2*size+1 ), Point( size, size ) );
    Mat dilatationKernel = getStructuringElement( MORPH_ELLIPSE, Size( 2*size + 1, 2*size+1 ), Point(size, size ) );
    erode(change,change,erosionKernel);
    dilate(change,change,dilatationKernel);*/

    Canny(change,edges, 70,80);
    findContours(edges,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
    //std::cout << "Found " << contours.size() << " contours." << std::endl;
    for( int i = 0; i< contours.size(); i++ )
    {
      //cout << "Area: " << contourArea(contours[i]) << endl;
      if(contourArea(contours[i]) > 0.0)
      {
        bigcontours.push_back(contours[i]);
        //roi.push_back(boundingRect(Mat(contours[i])));
      }
    }

    drawContours(change,bigcontours,-1,Scalar(0,0,255),1);
    imshow("change", change);
    alle = change.clone();
    changeak = change.clone();
    Mat r1 = change.clone();
    Mat r2 = change.clone();
    
    Mat frameRoiContours = frame.clone();
    vector<ROI> toAdd;
    std::vector<std::vector<cv::Point> > contoursFrame;
    vector<comRect> allRects;
    for( int i = 0; i< bigcontours.size(); i++ )
    {
      double area = contourArea(bigcontours[i]);
      Rect rect = boundingRect(Mat(bigcontours[i]));
      
      double dx = rect.width * 0.2;
      double dy = rect.height * 0.2;
      rect.x -= dx;
      rect.width += 2 * dx;
      rect.y -= dy;
      rect.height += 2 * dy;
      if(rect.x < 0)
        rect.x = 0;
      if(rect.x + rect.width > frame.size().width)
        rect.width = frame.size().width - rect.x;
      if(rect.y < 0)
        rect.y = 0;
      if(rect.y + rect.height > frame.size().height)
        rect.height = frame.size().height - rect.y;
      allRects.push_back(comRect(rect,false, area));
      rectangle(r1, rect.tl(), rect.br(), Scalar(255,0,0));
    }
    vector<Rect> bRects;
    for(size_t i = 0; i < allRects.size(); ++i)
    {
      if(allRects[i].merged)
          continue;
      if(i+1 >= allRects.size())
        break;
      for(size_t j = 0; j < allRects.size(); ++j)
      {     
        if(i == j)
          continue;
        if(allRects[i].rect.contains(allRects[j].rect.tl()) && allRects[i].rect.contains(allRects[j].rect.br())) // 2. rechteck ist in erstem enthalten
        {
          allRects[j].merged = true;
          continue;
        }
        if(allRects[j].rect.contains(allRects[i].rect.tl()) && allRects[j].rect.contains(allRects[i].rect.br())) // 1. rechteck ist in 2. enthalten
        {
          allRects[i].merged = true;
          continue;
        }
        if(allRects[j].merged || allRects[i].merged)   // 2.Rechteck wurde schon dazugemerged         
          continue;
        Rect min = allRects[i].rect & allRects[j].rect;
        if(min.width != 0 || min.height != 0) // Schnitt
        {
          allRects[i].rect |= allRects[j].rect;
          allRects[j].merged = true;
          allRects[i].maxArea = max(allRects[i].maxArea, allRects[j].maxArea);
        }
      }
    }
    for(size_t i = 0; i < allRects.size(); ++i)
    {
      if(!allRects[i].merged)
        bRects.push_back(allRects[i].rect);
    }
    for( const Rect& rect : bRects)
    {
      rectangle(r2, rect.tl(), rect.br(), Scalar(255,0,0));
      float area = rect.width * rect.height;
      ROI ak(rect);
      float conf;
      ROI found;
      rectangle(changeak, rect.tl(), rect.br(), Scalar(255,0,0));
      if(find(ak, found))
        conf = found.conf;
      else
      {
        //ak.conf = (area * area) / 4000.0f;
        if(ak.rect.x < 10 || ak.rect.y < 10 || frame.size().width - (ak.rect.x + ak.rect.width) < 10 || frame.size().height - (ak.rect.y + ak.rect.height) < 10)
          ak.conf = 0.15 + (area * area) / 4000.0f;
        else
          ak.conf = 0;
        //if(ak.conf > 1)
          //ak.conf = 1;
        conf = ak.conf;
        toAdd.push_back(ak);
      }
      if(conf > 0.2)
        rectangle(change, rect.tl(), rect.br(), Scalar(255,0,0));
      /*
      std::vector<std::vector<cv::Point> > contoursTmp;
      Mat frameROI(frame, rect);
      Mat roiedges;
      Canny(frameROI,roiedges, 80,100);
      findContours(roiedges,contoursTmp,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE, Point(rect.x, rect.y));
      drawContours(frameRoiContours,contoursTmp,-1,Scalar(0,0,255),2);
      for( int i = 0; i< contoursTmp.size(); i++ )
      {
      if(contourArea(contoursTmp[i]) < 25.0)
      continue;
      Rect rect2 = boundingRect(Mat(contoursTmp[i]));
      rectangle(frameRoiContours, rect2.tl(), rect2.br(), Scalar(255,0,0));
      }
      */
    }
    //imshow("changeOpening", change);
    m_roi.insert(m_roi.end(), toAdd.begin(), toAdd.end());
    for(const ROI& roi : m_roi)
    {
      if(roi.conf > 0.2)
        rectangle(alle, roi.rect.tl(), roi.rect.br(), Scalar(255,0,0));
    }
    imshow("frameroiContours", alle);
    imshow("changeak", changeak);


    //imshow("alleRechtecke", r1);
    //imshow("Rechtecke", r2);
  }
  // http://en.wikipedia.org/wiki/Algorithms_for_calculating_variance
  // online_variance

  Vec3f delta;
  for(int x = 0; x < frame.size().width; ++x)
  {
    for(int y = 0; y < frame.size().height; ++y)
    {        
      if(inAnyRect(Point(x,y)))
      {
        for(int i = 0; i < 3; ++i)
        {
          m_M2.at<Vec3f>(y,x).val[i] *= 0.9;
          m_sigma.at<Vec3f>(y,x).val[i] = 0.9;
        }
        continue;
      }
      for(int i = 0; i < 3; ++i)
      {
        float f = frame.at<Vec3b>(y, x).val[i];
        float me = m_mean.at<Vec3f>(y, x).val[i];
        delta.val[i] = frame.at<Vec3b>(y, x).val[i] - m_mean.at<Vec3f>(y, x).val[i];
        m_mean.at<Vec3f>(y, x).val[i] += delta.val[i] / m_n;
        m_M2.at<Vec3f>(y,x).val[i] += delta.val[i] * (frame.at<Vec3b>(y, x).val[i] - m_mean.at<Vec3f>(y, x).val[i]);
        if(m_n>1)
          m_sigma.at<Vec3f>(y,x).val[i] = sqrt(m_M2.at<Vec3f>(y,x).val[i] / (m_n-1));
      }
    }
  }

  //Mat back;
  //getBackgroundImage(back);
  //fore = abs(back - frame);
  //cvtColor( fore, fore, CV_BGR2GRAY );
  
  m_bg.operator ()(frame,fore);
  imshow("forevor", fore);
  Mat back;
  getBackgroundImage(back);

  //Vordergrundbewegung nur behalten, wenn es nicht in die �blichen Schwankungen des Hintergrundes z�hlt
  // d.h der aktuelle Pixelwert befindet sich in mindestens einem Farbkanal au�erhalb der sicheren Umgebung(von sigma festgelegt)
  for(size_t x = 0; x < fore.size().width; ++x)
  {
    for(size_t y = 0; y < fore.size().height; ++y)
    {
      if(abs(frame.at<Vec3b>(y,x).val[0] - back.at<Vec3b>(y,x).val[0]) < m_numSigma*(m_sigma.at<Vec3f>(y,x).val[0]) ||
         abs(frame.at<Vec3b>(y,x).val[1] - back.at<Vec3b>(y,x).val[1]) < m_numSigma*(m_sigma.at<Vec3f>(y,x).val[1]) ||
         abs(frame.at<Vec3b>(y,x).val[2] - back.at<Vec3b>(y,x).val[2]) < m_numSigma*(m_sigma.at<Vec3f>(y,x).val[2]))
      {
        fore.at<uchar>(y,x) = 0;
      }
    }
  }
  imshow("forenach", fore);
  imshow("mean", m_mean / 255);
  imshow("var", m_sigma / 255);
  
  //Opening
  erode(fore,fore,m_erosionKernel);
  dilate(fore,fore,m_dilatationKernel);


    //imshow("frameroiContours", frameRoiContours);
    //for(size_t x = 0; x < change.size().width; ++x)
    //{
    //  for(size_t y = 0; y < change.size().height; ++y)
    //  {
    //    //for( int i = 0; i< bigcontours.size(); i++ )
    //      //if(pointPolygonTest(bigcontours[i], Point(x,y), false) > 0)
    //        //change.at<Vec3b>(y,x) = Vec3b(255,0,0);
    //  }
    //}
    
    /*
  for(auto it(m_roi.begin()); it != m_roi.end();)
  {
    if((*it).conf > 0.0)
    {
      cout << "x: " << (*it).rect.x << " y: " << (*it).rect.y << " w: " << (*it).rect.width << " h: " << (*it).rect.height << endl;
      cout << "velx: " << (*it).velx << " vely: " << (*it).vely << " conf: " << (*it).conf << " cframes: " << (*it).cframes << " cfound: " << (*it).cfound << endl;
      cout << endl;
    }
    ++(*it).cframes;
    float diff = static_cast<float>((*it).cframes - (*it).cfound);
    if(diff <= 1)
      diff = 1.82;
    (*it).conf /= (0.55f * diff);
    auto del = it;
    ++it;
    if((*del).conf < 0.01)
    {
      m_roi.erase(del);
    }
    if(it == m_roi.end())
      break;
    
    
  }
  cout << "---------------------------\n";
  */

  

  m_lastFrame = frame.clone();
}

void Background::getBackgroundImage(cv::Mat& back) const
{
  m_bg.getBackgroundImage(back);
  //m_mean.convertTo(back, IPL_DEPTH_16U);
}

bool Background::inAnyRect(const cv::Point& p) const
{
  for(const ROI& roi : m_roi)
  {
    if(roi.conf > 0.2 && roi.rect.contains(p))
      return true;
  }
  return false;
}

bool Background::find(const ROI& roi, ROI& ret)
{
  for( ROI& roi2 : m_roi)
  {
    //if(roi2.velx == 0 && roi2.vely == 0)
    //{
      if(abs(roi2.rect.x - roi.rect.x) < roi2.rect.width * 0.3 && abs(roi2.rect.y - roi.rect.y) < roi2.rect.height * 0.3
        && abs(roi2.rect.width - roi.rect.width) < roi2.rect.width * 0.35 && abs(roi2.rect.height - roi.rect.height) < roi2.rect.height * 0.35)
      {
        
        //if(roi2.conf > 1)
          //roi2.conf = 1;        
        ++roi2.cfound;
        size_t tmp = 4;
        roi2.conf *= max(tmp , roi2.cfound);
        if(roi2.conf > 1000000)
          roi2.conf = 1000000;  
        roi2.velx = abs(roi2.rect.x - roi.rect.x);
        roi2.vely = abs(roi2.rect.y - roi.rect.y);
        roi2.rect = roi.rect;
        ret = roi2;
        return true;
      }
    /*}
    else
    {
      if(abs(roi2.rect.x - roi.rect.x) < roi2.rect.width * 0.1 + roi2.velx && abs(roi2.rect.y - roi.rect.y) < roi2.rect.height * 0.1 + roi2.vely
        && abs(roi2.rect.width - roi.rect.width) < 20 && abs(roi2.rect.height - roi.rect.height) < 20)
      {
        roi2.conf *= 4;
        //if(roi2.conf > 1)
          //roi2.conf = 1;        
        ++roi2.cfound;
        roi2.velx = abs(roi2.rect.x - roi.rect.x);

        roi2.vely = abs(roi2.rect.y - roi.rect.y);
        roi2.rect = roi.rect;
        ret = roi2;
        return true;
      }
    }*/
  }
  return false;
}

namespace
{

Rect bRect(const vector<Point>& vec)
{
  size_t minx = 1000000;  //hier besset boost::numeric::bounds nutzen
  size_t miny = 1000000;
  size_t maxx = 0;
  size_t maxy = 0;
  for(const Point& p : vec)
  {
    if(p.x < minx)
      minx = p.x;
    if(p.x > maxx)
      maxx = p.x;
    if(p.y < miny)
      miny = p.y;
    if(p.y > maxy)
      maxy = p.y;
  }
  return Rect(minx, miny, maxx - minx, maxy - miny);
}


} // namespace