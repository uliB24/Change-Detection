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
    //size_t size = 2;
    //Mat erosionKernel = getStructuringElement( MORPH_ELLIPSE, Size( 2*size + 1, 2*size+1 ), Point( size, size ) );
    //Mat dilatationKernel = getStructuringElement( MORPH_ELLIPSE, Size( 2*size + 1, 2*size+1 ), Point(size, size ) );
    //erode(change,change,erosionKernel);
    //dilate(change,change,dilatationKernel);

    Canny(change,edges, 50,70);
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
    drawContours(change,bigcontours,-1,Scalar(0,0,255),2);
    imshow("change", change);
    alle = change.clone();
    changeak = change.clone();
    
    Mat frameRoiContours = frame.clone();
    vector<ROI> toAdd;
    std::vector<std::vector<cv::Point> > contoursFrame;
    for( int i = 0; i< bigcontours.size(); i++ )
    {
      double area = contourArea(bigcontours[i]);
      Rect rect = boundingRect(Mat(bigcontours[i]));
      
      double dx = rect.width * 0.25;
      double dy = rect.height * 0.25;
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

      ROI ak(rect);
      float conf;
      ROI found;
      rectangle(changeak, rect.tl(), rect.br(), Scalar(255,0,0));
      if(find(ak, found))
        conf = found.conf;
      else
      {
        //ak.conf = (area * area) / 4000.0f;
        if(ak.rect.x < 3 || ak.rect.y < 3 || frame.size().width - (ak.rect.x + ak.rect.width) < 3 || frame.size().height - (ak.rect.y + ak.rect.height) < 3)
          ak.conf = 0.13 + (area * area) / 4000.0f;
        else
          ak.conf = 0;
        if(ak.conf > 1)
          ak.conf = 1;
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
    imshow("changeOpening", change);
    m_roi.insert(m_roi.end(), toAdd.begin(), toAdd.end());
    for(const ROI& roi : m_roi)
    {
      if(roi.conf > 0.2)
        rectangle(alle, roi.rect.tl(), roi.rect.br(), Scalar(255,0,0));
    }
    imshow("frameroiContours", alle);
    imshow("changeak", changeak);
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
          m_M2.at<Vec3f>(y,x).val[i] = 0;
          m_sigma.at<Vec3f>(y,x).val[i] = 0;
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
    if(it == m_roi.end())
      break;
    if((*del).conf < 0.01)
    {
      m_roi.erase(del);
    }
    
  }
  cout << "---------------------------\n";


  

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
    if(roi2.velx == 0 && roi2.vely == 0)
    {
      if(abs(roi2.rect.x - roi.rect.x) < roi2.rect.width * 0.2 && abs(roi2.rect.y - roi.rect.y) < roi2.rect.height * 0.2
        && abs(roi2.rect.width - roi.rect.width) < 20 && abs(roi2.rect.height - roi.rect.height) < 20)
      {
        roi2.conf *= 4;
        if(roi2.conf > 1)
          roi2.conf = 1;        
        ++roi2.cfound;
        roi2.velx = abs(roi2.rect.x - roi.rect.x);
        roi2.vely = abs(roi2.rect.y - roi.rect.y);
        roi2.rect = roi.rect;
        ret = roi2;
        return true;
      }
    }
    else
    {
      if(abs(roi2.rect.x - roi.rect.x) < roi2.rect.width * 0.1 + roi2.velx && abs(roi2.rect.y - roi.rect.y) < roi2.rect.height * 0.1 + roi2.vely
        && abs(roi2.rect.width - roi.rect.width) < 20 && abs(roi2.rect.height - roi.rect.height) < 20)
      {
        roi2.conf *= 4;
        if(roi2.conf > 1)
          roi2.conf = 1;        
        ++roi2.cfound;
        roi2.velx = abs(roi2.rect.x - roi.rect.x);
        roi2.vely = abs(roi2.rect.y - roi.rect.y);
        roi2.rect = roi.rect;
        ret = roi2;
        return true;
      }
    }
  }
  return false;
}