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
  
  // http://en.wikipedia.org/wiki/Algorithms_for_calculating_variance
  // online_variance

  Vec3f delta;
  for(int x = 0; x < frame.size().width; ++x)
  {
    for(int y = 0; y < frame.size().height; ++y)
    {        
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
  
  erode(fore,fore,m_dilatationKernel);
  dilate(fore,fore,m_dilatationKernel);

  if(lastFrame.data)
  {
    Mat change = abs(lastFrame - frame);
    Mat show = change.clone();
    cvtColor( change, change, CV_BGR2GRAY );
    std::vector<std::vector<Point> > contours;
    findContours(change,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
    drawContours(show,contours,-1,Scalar(0,0,255),2);
    imshow("change", show);
  }

  

  lastFrame = frame.clone();
}

void Background::getBackgroundImage(cv::Mat& back) const
{
  m_bg.getBackgroundImage(back);
  //m_mean.convertTo(back, IPL_DEPTH_16U);
}