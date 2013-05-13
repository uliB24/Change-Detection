#include "Background.h"

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <opencv2/opencv.hpp>


using namespace std;
using namespace cv;

Background::Background(string path, size_t erosion_size, size_t dilatation_size, size_t numSigma)
  : m_path(path), m_n(0), m_numSigma(numSigma)
{
	ostringstream oss;
	oss << m_path;
	oss << std::setfill ('0') << std::setw (6);
    oss << 1 << ".jpg";
	Mat frame = imread(oss.str(), CV_LOAD_IMAGE_COLOR);

	m_mean  = Mat(frame.size(), CV_32FC3, Scalar(0.0f,0.0f,0.0f));
  m_M2	= Mat(frame.size(), CV_32FC3, Scalar(0.0f,0.0f,0.0f));
  m_sigma = Mat(frame.size(), CV_32FC3, Scalar(0.0f,0.0f,0.0f));

  m_erosionKernel = getStructuringElement( MORPH_ELLIPSE, Size( 2*erosion_size + 1, 2*erosion_size+1 ), Point( erosion_size, erosion_size ) );
  m_dilatationKernel = getStructuringElement( MORPH_ELLIPSE, Size( 2*dilatation_size + 1, 2*dilatation_size+1 ), Point(dilatation_size, dilatation_size ) );
}


Background::~Background(void)
{
}

void Background::update(const cv::Mat& frame, cv::Mat& fore)
{
  fore = Mat(frame.size(), CV_8U);
  ++m_n;
  

  //vector<Rect> roi;

  for(auto it(m_roi.begin()); it != m_roi.end(); ++it)
  {
    if((*it).second > 2)
      m_roi.erase(it);
    else
      ++(*it).second;
  }

  if(m_lastFrame.data)
  {
    Mat change = abs(m_lastFrame - frame);
    imshow("changevor", change);
    //Mat show = change.clone();
    //Mat show; //= Mat(change.size(), IPL_DEPTH_16U, Scalar(0,0,0));
    Mat edges;
    std::vector<std::vector<cv::Point> > contours;
    std::vector<std::vector<cv::Point> > bigcontours;
    //cvtColor( change, change, CV_BGR2GRAY );
    //size_t size = 2;
    //Mat erosionKernel = getStructuringElement( MORPH_ELLIPSE, Size( 2*size + 1, 2*size+1 ), Point( size, size ) );
    //Mat dilatationKernel = getStructuringElement( MORPH_ELLIPSE, Size( 2*size + 1, 2*size+1 ), Point(size, size ) );
    //erode(change,change,erosionKernel);
    //dilate(change,change,dilatationKernel);

    Canny(change,edges, 50,70);
    findContours(edges,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
    std::cout << "Found " << contours.size() << " contours." << std::endl;
    for( int i = 0; i< contours.size(); i++ )
    {
      cout << "Area: " << contourArea(contours[i]) << endl;
      if(contourArea(contours[i]) > 15.0)
      {
        bigcontours.push_back(contours[i]);
        //roi.push_back(boundingRect(Mat(contours[i])));
      }
    }
    drawContours(change,bigcontours,-1,Scalar(0,0,255),2);
    imshow("change", change);

    
    Mat frameRoiContours = frame.clone();
    std::vector<std::vector<cv::Point> > contoursFrame;
    for( int i = 0; i< bigcontours.size(); i++ )
    {
      double area = contourArea(bigcontours[i]);
      Rect rect = boundingRect(Mat(bigcontours[i]));
      
      double dx = rect.width * 0.5;
      double dy = rect.height * 0.5;
      rect.x -= dx;
      rect.width += 2 * dx;
      rect.y -= dy;
      rect.height += 2 * dy;
      if(rect.x < 0)
        rect.x = 0;
      if(rect.x + rect.width > frame.size().width)
      {
        cout << "rand x\n";
        rect.width = frame.size().width - rect.x;
      }
      if(rect.y < 0)
        rect.y = 0;
      if(rect.y + rect.height > frame.size().height)
      {
        cout << "rand y\n";
        rect.height = frame.size().height - rect.y;
      }

      m_roi.push_back(make_pair(rect, 1));
      
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
    imshow("changeOpening", change);
  }


  // http://en.wikipedia.org/wiki/Algorithms_for_calculating_variance
  // online_variance

  Vec3f delta;
  for(int x = 0; x < frame.size().width; ++x)
  {
    for(int y = 0; y < frame.size().height; ++y)
    {        
      if(inAnyRect(Point(x,y)))
        continue;
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

  //Vordergrundbewegung nur behalten, wenn es nicht in die üblichen Schwankungen des Hintergrundes zählt
  // d.h der aktuelle Pixelwert befindet sich in mindestens einem Farbkanal außerhalb der sicheren Umgebung(von sigma festgelegt)
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
    
    


  

  m_lastFrame = frame.clone();
}

void Background::getBackgroundImage(cv::Mat& back) const
{
  m_bg.getBackgroundImage(back);
  //m_mean.convertTo(back, IPL_DEPTH_16U);
}

bool Background::inAnyRect(cv::Point p) const
{
  for(const pair<Rect, float>& rect : m_roi)
  {
    if(rect.first.contains(p))
      return true;
  }
  return false;
}