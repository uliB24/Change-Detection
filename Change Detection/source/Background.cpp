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

  //bool inAnyRect(const cv::Point& p, const vector<cv::Rect> rects);

 
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


  m_frameRect = Rect(Point(0,0), frame.size());
}


Background::~Background(void)
{
}

/*
http://stackoverflow.com/questions/9701276/opencv-tracking-using-optical-flow
http://opencv.willowgarage.com/documentation/cpp/motion_analysis_and_object_tracking.html

http://docs.opencv.org/modules/video/doc/motion_analysis_and_object_tracking.html
Idee: 
goodFeaturesToTrack weglasssen und Inputpunkte für
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
überblick


http://www.pages.drexel.edu/~nk752/tutorials.html
Tutorials

http://dasl.mem.drexel.edu/~noahKuntz/openCVTut9.html
Tutorial Optical Flow und Kalman

Learning OpenCV: Computer Vision with the OpenCV Library (Buch 33 Doller)

cppreference.com

*/

/** @brief updaten des Hintergrundes mit dem aktuellen Frame und Berechnung der Vordergrundmatrix

    Die Vordergrundmatrix ist eine 8-bit Maske mit einem Kanal.
    0: Hintergrund
    127: Schatten
    255: Vordergrundobjekt
    
    Bei der Berechnung der Vordergrundmatrix wird der BackgroundSubtractorMOG2 verwendet.
    Dieser ...(in Buch nachlesen)

    Um dynamischen Hintergrund besser herrauszufiltern, wird für jeden Pixel die Varianz berechnet.
    Für Pixel, die sich nicht in der m_sigma-Umgebung befinden, wird der Wert aus der Vordergrundmatrix des BackgroundSubtractorMOG2 übernommen.
*/
void Background::update(const cv::Mat& frame, cv::Mat& fore)
{
  // Vordergrundmatrix anglegen, 8-bit mit einem Kanal
  fore = Mat(frame.size(), CV_8U);
  ++m_n;
  cvtColor(frame, image_next, CV_BGR2GRAY);

  Mat change;
  std::vector<std::vector<cv::Point> > strongContours;
  if(m_lastFrame.data)
  {
    absdiff(m_lastFrame, frame, change);
    /*
    size_t size = 1;
    Mat erosionKernel = getStructuringElement( MORPH_ELLIPSE, Size( 2*size + 1, 2*size+1 ), Point( size, size ) );
    Mat dilatationKernel = getStructuringElement( MORPH_ELLIPSE, Size( 2*size + 1, 2*size+1 ), Point(size, size ) );
    erode(change,change,erosionKernel);
    dilate(change,change,dilatationKernel);
    */
    Mat strongEdges;
    Canny(change,strongEdges, 170,190);
    imshow("canny", strongEdges);   
    findContours(strongEdges,strongContours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
  }
  vector<Rect> strongRects;
  
  //vector<Mat> strongMasks;
  Mat imgStrong = frame.clone();
  for(size_t i = 0; i < strongContours.size(); ++i)
  {
    strongRects.push_back(boundingRect(strongContours[i]));
    rectangle(imgStrong, strongRects.back(), Scalar(255,0,0));
  }
  imshow("strongRects",imgStrong);
  //Idee: counturen auf strken kanten finden -> boundig Rects berechnen, wenn sich ein getrackter Punkt in dem Boundig Rect befindet und wenige Punkte zu diesem getracken Punkt gehöhren -> goodFeatures finden und zu diesen Punkten hinzufügen, am bestem in der anderen Schleife oben, wegen den iteratoren, es muss nur das Differenzbild 2 mal berechnet werden(oder auch nicht, vorher berechnen)
  vector<bool> strongRectfound(strongRects.size(), false);

  Mat track = frame.clone();
  m_contours_next.clear();
  m_rects.clear();
  //size_t c_c = 0;
  vector<Rect> bRects;
  auto itnm = m_notMoved.begin();
  std::vector<std::vector<cv::Point2f> > contours_next;
  for(auto points : m_contours_prev)
  {
    vector<Point2f> nextPoints;
    vector<uchar> status;
    vector<float> err;
    cv::calcOpticalFlowPyrLK(
      image_prev, image_next, // 2 consecutive images
      points, // input point positions in first im
      nextPoints, // output point positions in the 2nd
      status,    // tracking success
      err,      // tracking error
      Size(21,21),
      3,
      TermCriteria(TermCriteria::COUNT+TermCriteria::EPS, 30, 0.01),
      0,
      1e-4
      );
    
    //if(nextPoints.size() != (*itnm).size())
      //cout << "Points: " << points.size() << " nextPoints: " << nextPoints.size() << " m_notMoved: " << (*itnm).size() << "\n";
    auto itm = (*itnm).begin();
    auto itp = nextPoints.begin();
    //vector<std::vector<std::vector<int>>::iterator> toDelete;
    Scalar mean;
    Scalar stddev;
    meanStdDev(Mat(nextPoints), mean, stddev);
    cout << "x: " << stddev[0] << " y: " << stddev[1] << endl;
    for(int i = 0; i < nextPoints.size(); ++i)
    {
      if(status[i] == 1)
      {
        circle(track, Point(nextPoints[i].x, nextPoints[i].y), 2, Scalar(255,0,0));
        //cout << err[i] << endl;
      }
      int vel = (nextPoints[i].x - points[i].x)*(nextPoints[i].x - points[i].x) + (nextPoints[i].y - points[i].y)*(nextPoints[i].y - points[i].y);
      //int vel = abs(nextPoints[i] - points[i]);
      if(vel < 1)
      {
        (*itnm)[i].first += 1;
        
      }
      else
      {
        (*itnm)[i].first -= vel;
        if((*itnm)[i].first < -3)
          (*itnm)[i].first = -3;
      }
      bool del = false;
      if((*itnm)[i].second.z == 8)
      {
        if((nextPoints[i].x - (*itnm)[i].second.x)*(nextPoints[i].x - (*itnm)[i].second.x) + (nextPoints[i].y - (*itnm)[i].second.y)*(nextPoints[i].y - (*itnm)[i].second.y) < 5)
          del = true;
        (*itnm)[i].second.z == 0;
      }
      else
        ++(*itnm)[i].second.z;
      const int sigma = 3;
      int minx = mean[0] - sigma * stddev[0];
      if(minx < 0)
        minx = 0;
      int miny = mean[1] - sigma * stddev[1];
      if(miny < 0)
        miny = 0;
      int maxx = mean[0] + sigma * stddev[0];
      if(maxx > m_frameRect.br().x)
        maxx = m_frameRect.br().x;
      int maxy = mean[1] + sigma * stddev[1];
      if(maxy > m_frameRect.br().y)
        maxy = m_frameRect.br().y;
      if(del || (*itnm)[i].first > 5 || !m_frameRect.contains(nextPoints[i]) ||  // Punkt hat sich lange nicht bewegt oder ist außerhalb des Bildes
        nextPoints[i].x < minx || nextPoints[i].x > maxx || nextPoints[i].y < miny || nextPoints[i].y > maxy) // oder befindet wich relativ zu den anderen Punkten weit weg  -> löschen
      {
        // Das ist ziemlich unschön und fehleranfällig
        --i; // muss um eins verringert werden, da durch erase alle nachfolgenden Elemente umkopiert werden
        itm = (*itnm).erase(itm);
        itp = nextPoints.erase(itp);
      }      
      else
      {
        ++itm;
        ++itp;
      }
      if(nextPoints.size() == 0)
        break;
    }

    bool nextit = false;
    if(!nextPoints.empty())
    {
      Rect rect = boundingRect(nextPoints);
      double dx = rect.width * (0.7/ sqrt(stddev[0]));
      double dy = rect.height * (0.7/ sqrt(stddev[1]));
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

      
      vector<int> numInRect(strongRects.size(), 0);
      for(auto p : nextPoints)
      {
        for(size_t i = 0; i < strongRects.size(); ++i)
        {
          if(strongRects[i].contains(p))
          {
            ++numInRect[i];
            strongRectfound[i] = true;
          }
        }
      }
      size_t idx;
      int max = -1;
      for(size_t i = 0; i < numInRect.size(); ++i)
      {
        if(numInRect[i] > max)
        {
          idx = i;
          max = numInRect[i];          
        }
      }
      /*
      if(points.size() < 10) // neue Punkte für dieses zu Trackende finden, da nicht alle Punkte mitgetrackt werden
      {
        if(max > nextPoints.size() * 0.75)  //min 66% der Punkte sind in dieser Boundingbox
        {
          
          //Punkte auffrischen
          vector<Point> poly;
          convexHull(strongContours[idx], poly);
          Mat mask(frame.size(), CV_8UC1, Scalar(0));
          fillConvexPoly(mask, poly, Scalar(255));
          
          vector<Point2f> corners;
          goodFeaturesToTrack(image_next, corners, 10, 0.01, 2, mask);
          if(!corners.empty())
          {
            nextPoints.insert(nextPoints.end(), corners.begin(), corners.end());
            //bRects.push_back(boundingRect(corners));
            (*itnm).insert((*itnm).end(), corners.size(), 0);
            //m_notMoved.push_back(vector<int>(corners.size(), 0));
          }
        }
      }
      */
      contours_next.push_back(nextPoints);
      bRects.push_back(rect);
      //if(rect.width * rect.height > 400)
        //rectangle(track, m_rects.back().tl(), m_rects.back().br(), Scalar(0,255,0));
    }
    else
    {
      if((*itnm).empty())
      {
        itnm = m_notMoved.erase(itnm);
        nextit = true;
      }
      else
        cout << "Achtung\n";
    }
    if(!nextit)
      ++itnm;
  }
  if(!contours_next.empty())
  {
    for(int i = 0; i < strongRectfound.size(); ++i)
    {
      // starke Konturen, zu denen es keine passenden zu trackenden Punkte gibt, werden hinzugefügt
      if(!strongRectfound[i])
      {
        vector<Point> poly;
        convexHull(strongContours[i], poly);
        Mat mask(frame.size(), CV_8UC1, Scalar(0));
        fillConvexPoly(mask, poly, Scalar(255));

        vector<Point2f> corners;
        goodFeaturesToTrack(image_next, corners, 10, 0.1, 2, mask);
        if(!corners.empty())
        {
          contours_next.push_back(corners);
          bRects.push_back(boundingRect(corners));
          vector<pair<int, Point3i>> vec;
          for(auto p : corners)
          {
            vec.push_back(make_pair(0, Point3i(p.x,p.y, 0)));
          }
          m_notMoved.push_back(vec);
        }
      }
    }
    std::vector<std::vector<std::pair<int, cv::Point3i>>> notMoved = m_notMoved;
    vector<bool> merged(bRects.size(), false);
    for(size_t i = 0; i < bRects.size(); ++i)
    {
      if(merged[i])
        continue;
      if(i+1 >= bRects.size())
        break;
      for(size_t j = 0; j < bRects.size(); ++j)
      {     
        if(i == j)
          continue;
        if(bRects[i].contains(bRects[j].tl()) && bRects[i].contains(bRects[j].br())) // 2. rechteck ist in erstem enthalten
        {
          merged[j] = true;
          contours_next[j].insert(contours_next[j].end(), contours_next[i].begin(), contours_next[i].end());
          notMoved[j].insert(notMoved[j].end(), notMoved[i].begin(), notMoved[i].end());
          continue;
        }
        if(bRects[j].contains(bRects[i].tl()) && bRects[j].contains(bRects[i].br())) // 1. rechteck ist in 2. enthalten
        {
          merged[i] = true;
          contours_next[i].insert(contours_next[i].end(), contours_next[j].begin(), contours_next[j].end());
          notMoved[i].insert(notMoved[i].end(), notMoved[j].begin(), notMoved[j].end());
          continue;
        }
        if(merged[j] || merged[i])   // 2.Rechteck wurde schon dazugemerged         
          continue;
        Rect min = bRects[i] & bRects[j];
        if((min.width != 0 || min.height != 0) &&  // Schnitt
          (min.width * min.height > bRects[i].width * bRects[i].height * 0.8 || min.width * min.height > bRects[j].width * bRects[j].height * 0.8)) // Schnittrecht ist zu 80% in einem der beiden Rectecke drin
        {
          bRects[i] |= bRects[j];
          merged[j] = true;
          contours_next[i].insert(contours_next[i].end(), contours_next[j].begin(), contours_next[j].end());
          notMoved[i].insert(notMoved[i].end(), notMoved[j].begin(), notMoved[j].end());
        }
      }
    }
    m_notMoved.clear();
    for(size_t i = 0; i < bRects.size(); ++i)
    {
      if(!merged[i])
      {
        m_contours_next.push_back(contours_next[i]);
        m_notMoved.push_back(notMoved[i]);
        m_rects.push_back(bRects[i]);
        if(bRects[i].width * bRects[i].height > 400)      
          rectangle(track, bRects[i].tl(), bRects[i].br(), Scalar(0,255,0));
      }
    }
  }

  imshow("Track", track);

  if(m_lastFrame.data)
  {
    //Mat change = abs(m_lastFrame - frame);
    imshow("changevor", change);
    //Mat show = change.clone();
    //Mat show; //= Mat(change.size(), IPL_DEPTH_16U, Scalar(0,0,0));
    Mat edges;
    std::vector<std::vector<cv::Point> > contours;
    //cvtColor( change, change, CV_BGR2GRAY );
    /*size_t size = 0;
    Mat erosionKernel = getStructuringElement( MORPH_ELLIPSE, Size( 2*size + 1, 2*size+1 ), Point( size, size ) );
    Mat dilatationKernel = getStructuringElement( MORPH_ELLIPSE, Size( 2*size + 1, 2*size+1 ), Point(size, size ) );
    erode(change,change,erosionKernel);
    dilate(change,change,dilatationKernel);*/
    imshow("change", change);
    Canny(change,edges, 70,80);
    findContours(edges,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);

    //std::cout << "Found " << contours.size() << " contours." << std::endl;
    for( size_t i = 0; i< contours.size(); i++ )
    {
      //cout << "Area: " << contourArea(contours[i]) << endl;
      if(contourArea(contours[i]) > 0.0)
      {
        Rect rect = boundingRect(contours[i]);
        vector<Point> points;
        // nur die Punkte am Anfang hinzunehmen, die am Rand sind
        if(rect.x < 10 || rect.y < 10 || frame.size().width - (rect.x + rect.width) < 10 || frame.size().height - (rect.y + rect.height) < 10)
          for(auto p : contours[i])
            if(!inAnyRect(p))
              points.push_back(p);
        if(points.size() > contours[i].size() * 0.75)  // Wenn mindestens die Hälfte der Punkte dieser Kontur noch nicht in den BoundigBoxen der anderen Konturen enthalten sind, wird diese Kontur nun auch getrackt
        {
          vector<Point> poly;
          convexHull(contours[i], poly);
          Mat mask(frame.size(), CV_8UC1, Scalar(0));
          fillConvexPoly(mask, poly, Scalar(255));
          
          vector<Point2f> corners;
          goodFeaturesToTrack(image_next, corners, 10, 0.01, 2, mask);
          if(!corners.empty())
          {
            m_contours_next.push_back(corners);
            m_rects.push_back(boundingRect(corners));
            vector<pair<int, Point3i>> vec;
            for(auto p : corners)
            {
              vec.push_back(make_pair(0, Point3i(p.x,p.y, 0)));
            }
            m_notMoved.push_back(vec);
          }
            /*
          vector<Point2f> p3f;
          for(auto p : contours[i])
            p3f.push_back(Point2f(p.x,p.y));
          m_contours_next.push_back(p3f);
          m_rects.push_back(rect);
          */
        }
          //features_next.insert(features_next.end(), contours[i].begin(), contours[i].end());
      }
    }
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
          m_M2.at<Vec3f>(y,x).val[i] *= 0.9f;
          m_sigma.at<Vec3f>(y,x).val[i] *= 0.9f;
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
  for(int x = 0; x < fore.size().width; ++x)
  {
    for(int y = 0; y < fore.size().height; ++y)
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


  m_lastFrame = frame.clone();
  image_prev = image_next.clone();
  m_contours_prev = m_contours_next;
}

void Background::getBackgroundImage(cv::Mat& back) const
{
  m_bg.getBackgroundImage(back);
  //m_mean.convertTo(back, IPL_DEPTH_16U);
}


bool Background::inAnyRect(const cv::Point& p) const
{
  for(const Rect& rect : m_rects)
  {
    if(rect.width * rect.height > 400 && rect.contains(p))
      return true;
  }
  return false;
}

namespace
{

} // namespace