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
  //lokale Funktionen
 
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



/** @brief updaten des Hintergrundes mit dem aktuellen Frame und Berechnung der Vordergrundmatrix

    Die Vordergrundmatrix ist eine 8-bit Maske mit einem Kanal.
    0: Hintergrund
    127: Schatten
    255: Vordergrundobjekt
    
    Bei der Berechnung der Vordergrundmatrix wird der BackgroundSubtractorMOG2 verwendet.
    Dieser hält für jeden Pixel mehrere Gausverteilungen(Mittelwert + Varianz).
    Wenn ein Pixelwert außerhalb der Graußverteilungen ist(es gehört somit zum Vordergrund), wird eine neue Verteilung hinzugenommen.
    Wird diese Verteilung später öfter getroffen, gehört diese zum Hintergrund

    Um dynamischen Hintergrund besser herrauszufiltern, wird für jeden Pixel die Varianz berechnet.
    Für Pixel, die sich nicht in der m_sigma-Umgebung befinden, wird der Wert aus der Vordergrundmatrix des BackgroundSubtractorMOG2 übernommen.
    Damit die Varianz aber nur in den Bereichen hoch ist, in denen sich dynamischer Hintergrund befindet und nicht dort, wo sich Vordergrundobjekte bewegen,
    werden die Vordergrundobjekte getrackt und aus der Berechnung der Varianz herreusgenommen.
*/
void Background::update(const cv::Mat& frame, cv::Mat& fore)
{
  // Vordergrundmatrix anglegen
  fore = Mat(frame.size(), CV_8U);
  ++m_n;
  cvtColor(frame, image_next, CV_BGR2GRAY);

  Mat track = frame.clone();
  m_contours_next.clear();
  m_rects.clear();
  vector<Rect> bRects;
  auto itnm = m_notMoved.begin();
  std::vector<std::vector<cv::Point2f> > contours_next;
  // Über alle Gruppen von Feature-Punkten des letzten Frames iterieren
  for(auto points : m_contours_prev)
  {
    vector<Point2f> nextPoints;
    vector<uchar> status;
    vector<float> err;
    // Featuere Punkte mit optischem Fluss tracken
    cv::calcOpticalFlowPyrLK(
      image_prev, image_next,
      points, nextPoints,
      status, err, Size(21,21), 3, TermCriteria(TermCriteria::COUNT+TermCriteria::EPS, 30, 0.01), 0, 1e-4);

    auto itm = (*itnm).begin();
    auto itp = nextPoints.begin();
    Scalar mean;
    Scalar stddev;
    meanStdDev(Mat(nextPoints), mean, stddev);
    // Über alle Feature-Punkte dieser Gruppe iterieren 
    for(int i = 0; i < nextPoints.size(); ++i)
    {
      if(status[i] == 1)
      {
        circle(track, Point(nextPoints[i].x, nextPoints[i].y), 2, Scalar(255,0,0));
      }
      bool del = false;
      if((*itm).z == 10)
      {
        // Wenn der Punkt sich in den letzten 10 Frames um weniger als sqrt(5) Pixel bewegt hat, wird er nicht mehr getrackt
        if((nextPoints[i].x - (*itm).x)*(nextPoints[i].x - (*itm).x) + (nextPoints[i].y - (*itm).y)*(nextPoints[i].y - (*itm).y) < 5)
        {
          del = true;
        }
        (*itm).z = 0;
        (*itm).x = nextPoints[i].x;
        (*itm).y = nextPoints[i].y;
      }
      else
        ++(*itm).z;
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
      if(del || !m_frameRect.contains(nextPoints[i]) ||  // Punkt hat sich lange nicht bewegt oder ist außerhalb des Bildes
        nextPoints[i].x < minx || nextPoints[i].x > maxx || nextPoints[i].y < miny || nextPoints[i].y > maxy) // oder befindet sich relativ zu den anderen Punkten weit weg  -> löschen
      {
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
      // Boundingbox von den Punkten dieser Gruppe berechnen und in abhängigkeit der Standartabweichung der Punkte vergrößern
      // Boundingbox wird umso sehr vergrößert, je kleiner die Standartabweichung ist 
      Rect rect = boundingRect(nextPoints);
      int dx = rect.width * (0.7/ sqrt(stddev[0]));
      int dy = rect.height * (0.7/ sqrt(stddev[1]));
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

      contours_next.push_back(nextPoints);
      bRects.push_back(rect);
    }
    else
    {
      // alle Punkte dieser Gruppe wurden gelöscht
      if((*itnm).empty())
      {
        itnm = m_notMoved.erase(itnm);
        nextit = true;
      }
    }
    if(!nextit)
      ++itnm;
  }
  if(!contours_next.empty())
  {
    // Zusammenfassen von Punkteregionen p1 und p2, wenn
    // sich alle Punkte von p1 in der Boundingbox von p2 befinden oder umgekehrt
    // oder sich die Boundingboxen der Punkteregionen zu min. 80% überdecken
    std::vector<std::vector<cv::Point3i>> notMoved = m_notMoved;
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
          (min.width * min.height > bRects[i].width * bRects[i].height * 0.8 || min.width * min.height > bRects[j].width * bRects[j].height * 0.8)) // Schnittrecht ist zu 80% in einem der beiden Rectecke enthalten
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
    Mat change;
    //Differenzbild der letzten beiden Frames berechnen
    absdiff(m_lastFrame, frame, change);
    Mat edges;
    std::vector<std::vector<cv::Point> > contours;
    imshow("change", change);
    Canny(change,edges, 70,90);
    // Konturen auf dem Differenzbild finden
    findContours(edges,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
    for( size_t i = 0; i< contours.size(); i++ )
    {
      Rect rect = boundingRect(contours[i]);
      vector<Point> points;
      // nur die Punkte am Bildrand werden hinzunehmen, um sie zu tracken
      if(rect.x < 10 || rect.y < 10 || frame.size().width - (rect.x + rect.width) < 10 || frame.size().height - (rect.y + rect.height) < 10)
        for(auto p : contours[i])
          if(!inAnyRect(p))
            points.push_back(p);
      // Wenn mindestens die Hälfte der Punkte dieser Kontur noch nicht in den BoundigBoxen der anderen Konturen enthalten sind, wird diese Kontur nun auch getrackt
      if(points.size() > contours[i].size() * 0.75)
      {
        vector<Point> poly;
        convexHull(contours[i], poly);
        Mat mask(frame.size(), CV_8UC1, Scalar(0));
        fillConvexPoly(mask, poly, Scalar(255));
        // in der konvexen Hülle der Konturen werden gut trackbare Features gesucht
        vector<Point2f> corners;
        goodFeaturesToTrack(image_next, corners, 10, 0.01, 2, mask);
        if(!corners.empty())
        {
          m_contours_next.push_back(corners);
          m_rects.push_back(boundingRect(corners));
          vector<Point3i> vec;
          for(auto p : corners)
          {
            vec.push_back(Point3i(p.x,p.y, 0));
          }
          m_notMoved.push_back(vec);
        }
      }
    }
  }

  // Berechnung der Varianz jedes Pixels
  // Damit nicht alle Pixelwerte gespeichert werden müssen, wird die Varianz online nach Donald Ervin Knuth berechnet:
  // http://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Online_algorithm
  // Es werden nur die Pixel mit einbezogen, die sich nicht in einer Boundingbox von getrackten Punktegruppen befinden
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
  
  // Vordergrundmatrix des BackgroundSubtractorMOG2 berechnen
  m_bg.operator ()(frame,fore);
  //imshow("forevor", fore);
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
  //imshow("forenach", fore);
  //imshow("mean", m_mean / 255);
  imshow("var", m_sigma / 255);
  
  //Opening -> enfernung von zu kleinen Struckturen
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