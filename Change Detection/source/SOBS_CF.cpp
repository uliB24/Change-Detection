#include "SOBS_CF.h"

#include <opencv2/opencv.hpp>

#include <iostream>


using namespace std;
using namespace cv;

#define generateBackground

namespace
{

  float distHSVsquared(const cv::Vec3f& v1, const cv::Vec3f& v2);
  void Vec3bToVec3fHSV(const cv::Vec3b& vb, cv::Vec3f& vf);
  static const float pi = 3.14159265359f; 
  static const float pim2 = 3.14159265359f*2.0f; 
  static const float deg2rad = pi / 180.0f;
  static const float rad2deg = 180.0f / pi;
}


SOBS_CF::SOBS_CF(const cv::Mat& frame0, float epsilon1, float epsilon2, float alpha1, float alpha2, size_t trainingsphase , int M, int N, size_t kernel, int kNaerHalf)
  : m_N(N), m_M(M), m_kNearHalf(kNaerHalf)
{
  m_offset = (kernel - 1)/2;
  m_widthSOM = frame0.size().width*M + 2*m_offset;
  m_heightSOM = frame0.size().height*N  + 2*m_offset;

  m_width = frame0.size().width;
	m_height = frame0.size().height;
  m_SOM.resize(m_heightSOM, vector<Vec3f>(m_widthSOM));

  Mat frame0hsv;
  cvtColor(frame0, frame0hsv, CV_BGR2HSV);

  //Initialiesierung des Hintergrundmodells: jedes Neuron bekommt Farbe des ersten Frames als Gewicht
  for(size_t y = 0; y < m_height; ++y)
  {
    for(size_t x = 0; x < m_width; ++x)
    {
      for(size_t n = 0; n < N; ++n)     
      {
        for(size_t m = 0; m < M; ++m)
        {
          Vec3bToVec3fHSV(frame0hsv.at<Vec3b>(y, x), m_SOM[M * y + m + m_offset][N * x + n + m_offset]);
        }
      }
    }
  }

  m_kw.resize(kernel, vector<float>(kernel));

  float cM;
	float cN;
	float Wmax = FLT_MIN;
  // Initialisierung des Kernels mit den Binomialfilterwerten
	cN = 1.0f;
	for (size_t j=0;j<kernel;j++) {
		cM = 1.0f;		
		for (size_t i=0;i<kernel;i++) {
			m_kw[j][i] = cN*cM;
      if (m_kw[j][i]>Wmax) Wmax = m_kw[j][i]; 
			cM = cM * (kernel - 1 - i) / (i + 1);
		}
		cN = cN * (kernel - 1 - j) / (j + 1);
	}

  m_epsilon1 = epsilon1;
  m_epsilon2 = epsilon2;

	m_alpha1 = alpha1/Wmax;
	m_alpha2 = alpha2/Wmax;

	m_K = 0;
  m_K2 = 0;
  m_TSteps = trainingsphase;
  m_ReduceESteps = 50;

  m_back = frame0.clone();
}

/** @brief Updaten des Hintergrundmodells mit dem aktuellen Frame und Ausgabe der Vordergrundmaske

    Ansatz entnommen aus:
    A fuzzy spatial coherence-based approach to background/foreground separation for moving object detection
    Autoren: Lucia Maddalena und Alfredo Petrosino
    Web: http://cvprlab.uniparthenope.it/publications/A%20fuzzy%20spatial%20coherence-based%20approach%20to%20background%20foreground%20separation%20for%20moving%20object%20detection.pdf


    Das Hintergrundmodell basiert auf einer Self-Organizing Map. Für jeden Pixel werden n x n Neuronen angelegt.
    Der Gewichtsvektor jedes Neurons enthält eine Farbe im HSV-Farbraum. Initialisiert werden sie mit den korrespondierenden Werten des ersten Frames.

    Beim Updaten mittels des aktuellen frames wird zuerst zu jedem Neuron die Distanz zum entsprechenden Pixel des aktuellen Frames berechnet.
    Im HSV-Farbraum sind die Lichtintensität und die Farbe explizit getrennt, sodass die Distanzmetrik weniger von der Beleuchtungsstärke abhängt.
    Um Probleme mit der Periodizität von h (Hue) und der Instabilität von h bei kleinen Werten von s (saturation) zu umgehen, wird die Distanz folgendermaßen berechnet:
    d(p1,p2) = euklidische Distanz von (v1*s1*cos(h1), v1*s1*sin(h1), v1) und (v2*s2*cos(h2), v2*s2*sin(h2), v2), wobei p1 = (h1,s1,v1) und p2 = (h2,s2,v2)

    Als nächstes wird zu jedem Pixel p des aktuellen Frames der Gewichtsvektor mit der minimalen Distanz aus der zugehörigen n x n Umgebung im Hintergrundmodell gesucht.
    Ist dieser Gewichtsvektor an Position (x,y) im Hintergrundmodell H und wird Frame Nummer t bearbeitet, wird die n x n Nachbarschaft in H folgendermaßen geupdated:
    H(i,j, t) = (1 - Alpha(i,j,t)) * H(i,j, t-1) + Alpha(i,j,t) * p, wobei i = x - (n/2), ... , x + (n/2) und j = y - (n/2), ... , y + (n/2).

    Alpha(i,j,t) bezeichnet die zeitabhängige Lernrate an der Position i,j und ist ein Wert zwischen 0 und 1. Dieser wird wie folgt berechnet:
    Alpha(i,j,t) = F1(p) * F2(p) * Alpha(t) * w(i,j)

    w(i,j) sind dabei der Gewichte eines Binomialfilters.
    Alpha(t) ist die zeitabhängige Lernrate. Zu Beginn (z.b. in den ersten 150 Frames) wird das Hintergrundmodell gelernt(Trainingsphase).
    In dieser Trainingsphase ist die Lernrate am höchsten(alpha1) und sinkt linear mit der Anzahl der Frames(t) bis sie die untere Grenze alpha2 erreicht.
    Da Alpha(t) mit w(i,j) gewichtet wird und die Lernrate eine Zahl zwischen 0 und 1 bleibt, wird Alpha(t) mit dem Maximum des Binomialfilters normiert.

    F1(p) ist die erste Fuzzy-Regel, welche ermöglicht Unsicherheit mit einzubeziehen und wird wie folgt berechnet:
    F1(p) = 1 - dmin(H,p)/epsilon ,wenn dmin(H,p) < epsilon, sonst 0
    D.h. je kleiner der minimale Abstand des Pixels p an der Stelle (x,y) zum Hintergrundmodell ist, desto sicherer ist, dass p zum Hintergrund gehört und desto mehr geht es in das Updaten ein.
    Falls es keinen Gewichtvektor gibt, der matcht, wird der Hintergrund für dieses Pixel nicht geupdated.

    F2(p) ist die Fuzzy-Regel für die räumliche Kohärenz.
    Die räumliche Kohärenz(NCF) wird berechnet mit: NCF = Anzahl der Nachbarschaftspixel mit minimalem Abstand kleiner epsilon durch Anzahl der Nachbarschaftspixel.
    Sie ist also ein relatives Maß wie viele Pixel in der Nachbarschaft im Hintergrundmodell gut repräsentiert sind.
    Ist die räumliche Kohärenz größer als 0.5, sind die meisten Nachbarschaftspixel gut im Hintergrundmodell repräsentiert und es ist davon auszugehen, dass auch der Pixel zum Hintergrund gehört.
    F2(p) = 2 * NCF(p) - 1 ,wenn NCF > 0.5, sonst 0
    D.h. je größer die räumliche Kohärenz, desto stärker geht der Pixel in das Hintergrundmodell ein.
    
    Ist NCF größer als 0.5 wird der Pixel als Hintergrund klassifiziert, ansonsten gehört er zum Vordergrund.


aktuelle Probleme:

- globale Beleuchtungsänderungen:
Detektieren?! und neue Trainingsphase beginnen

- bootstrapping:
nicht lösbar ohne Objektwissen (vergleich office und ForegroundAperture Szene), bzw. vom Benutzer neu spezifizierte Trainingsphase

- Schatten:
Schattenerkennung (mögliche Erweiterung)

- geringe Farbunterschiede (z.B. office: Hose -> Leiste zwischen Wand und Boden):
Textturbasierte Farbunterschiede?

- fountain: zu großer Bereich von bewegtem Hintergrund: Kann nicht auf Pixellevel behandelt werden.
*/
void SOBS_CF::update(const cv::Mat& frame, cv::Mat& fore)
{
  fore = Mat(frame.size(), CV_8U);

  Mat frameHSV;
  cvtColor(frame, frameHSV, CV_BGR2HSV);

  float alpha,a;
	float epsilon;

	if (m_K <= m_TSteps)    // Trainingsphase
  {									
    //epsilon = m_epsilon1 - m_K*(m_epsilon1-m_epsilon2)/m_TSteps;
    epsilon = m_epsilon1;
		alpha = (m_alpha1-m_K*(m_alpha1-m_alpha2)/m_TSteps);
		m_K++;
	}
  else
  {
    if(m_K2 <= m_ReduceESteps)
    {
      ++m_K2;
      epsilon = m_epsilon1 - (m_K2)*(m_epsilon1-m_epsilon2)/m_ReduceESteps;
      alpha = m_alpha2;
    }
    else
    {
		  epsilon = m_epsilon2;
		  alpha = m_alpha2;
    }
	}

  Mat dist(Size(m_SOM[0].size(),m_SOM.size()), CV_32FC1);
  Mat srcf(Size(m_width + 1, m_height + 1), CV_32FC3);
  int jj;
  int ii;
  size_t x=0;
  // Berechnung der Distanz jedes Neurons zu den entsprechenden Pixeln des aktuellen Frames
  // Die Bildspalten sollen parrallel durchlaufen werden
  #pragma omp parallel for private(x) 
  for (int y=0;y<m_height;y++)
  {
    jj = m_offset + y*m_N;
    for (x=0;x<m_width;x++) 
    {
      ii = m_offset + x*m_M;
      Vec3f src;
      Vec3bToVec3fHSV(frameHSV.at<Vec3b>(y, x), src);
      srcf.at<Vec3f>(y,x) = src;		
      for (int l=0; l < m_N ;l++) 
      {
        for (int k=0; k < m_M;k++) 
        {
          dist.at<float>(jj + l,ii + k) = distHSVsquared(m_SOM[jj + l][ii + k], src);
        }
      }    
    }
  }
  Mat distMin(Size(m_width + 1, m_height + 1), CV_32FC3);
  //#pragma omp parallel for private(x) 
  // Zu jedem Pixel den Gewichtsvektor mit der kleinsten Distanz ermitteln
  for (int y=0;y<m_height;y++)
  {
    jj = m_offset + y*m_N;
    for (x=0;x<m_width;x++) 
    {
      ii = m_offset + x*m_M;
      float dqmin = FLT_MAX;
			size_t xHit = ii;
			size_t yHit = jj;
      for (int l=0; l < m_N ;l++) 
      {
        for (int k=0; k < m_M;k++) 
        {
          const float dq = dist.at<float>(jj + l,ii + k);

          if (dq < dqmin)
          {
            dqmin = dq;
            xHit = ii + k;
            yHit = jj + l;
          }
        }
      }
      distMin.at<Vec3f>(y,x) = Vec3f(dqmin, static_cast<float>(xHit), static_cast<float>(yHit));
    }
  }
  #pragma omp parallel for private(x) 
  for (int y=0;y<m_height;y++)
  {
    for (x=0;x<m_width;x++) 
    {

      const Vec3f src = srcf.at<Vec3f>(y, x);
     
      float omega = 0;
      float neighbours = 0;
      // Zählen wie viele Pixel in der Umgebung eine kleinere Distanz als epsilon haben
      for (int l= -m_kNearHalf; l < (m_kNearHalf + 1) ;l++) 
      {
        for (int k=-m_kNearHalf; k < (m_kNearHalf + 1);k++) 
        {
          if(y+l >= 0 && y+l < m_height && x+k >= 0 && x+k < m_width)
          {
            ++neighbours;
            if(distMin.at<Vec3f>(y + l,x + k)[0] < epsilon)
            {
              ++omega;
            }       
          }
        }
      }
      
      const float dqmin = distMin.at<Vec3f>(y,x)[0];
			const size_t xHit = static_cast<size_t>(distMin.at<Vec3f>(y,x)[1]);
			const size_t yHit = static_cast<size_t>(distMin.at<Vec3f>(y,x)[2]);

      
      // Fuzzy update (fuzzyUp): 
      // ermöglicht Unsicherheit mit einzubeziehen
      // je kleiner der minimale Abstand des Pixels p an der Stelle (x,y) zum SOM ist, desto sicherer ist, dass p zum Hintergrund gehört und desto mehr geht es in das Updaten ein
      float fuzzyUp = 0.0f;
			if ( dqmin < epsilon ) 
				fuzzyUp =  1.0f - dqmin/epsilon;

      // räumliche Kohärenz: Anzahl der Nachbarschaftspixel mit minimalem Abstand kleiner epsilon durch Anzahl der Nachbarschaftspixel
      // relatives Maß wie viele Pixel in der Nachbarschaft im Hintergrundmodell gut repräsentiert sind
      const float NCF = omega / neighbours;

      // Ist die räumliche Kohärenz größer als 0.5, sind die meisten Nachbarschaftspixel gut im Hintergrundmodell repräsentiert
      // und es ist davon auszugehen, dass auch der Pixel zum Hintergrund gehört
      // -> je größer die räumliche Kohärenz, desto stärker geht der Pixel in das Hintergrundmodell ein
      float fuzzyCoherence = 0.0f;
      if(NCF > 0.5f)
        fuzzyCoherence = 2 * NCF - 1;

      //Berechnung der Lernrate
      const float alphaxy = fuzzyUp * fuzzyCoherence * alpha;

      //SOM der Nachbarschaftsgröße MxN updaten
			for (size_t l=yHit-m_offset;l<=(yHit+m_offset);l++) 
      {
				for (size_t k=xHit-m_offset;k<=(xHit+m_offset);k++)
        {
          // alpha mit Kernel-Gewichten wichten
					a = alphaxy*m_kw[l-yHit+m_offset][k-xHit+m_offset];
						
          m_SOM[l][k].val[0] = (1 - a) * m_SOM[l][k].val[0] + a * src[0];
          m_SOM[l][k].val[1] = (1 - a) * m_SOM[l][k].val[1] + a * src[1];
          m_SOM[l][k].val[2] = (1 - a) * m_SOM[l][k].val[2] + a * src[2];

				}
			}

      if(NCF > 0.5f)
      {
        // Hintergrund
        fore.at<uchar>(y,x) = 0;
#ifdef generateBackground
        // falls hier ein Hintergrundbild zum anzeigen generiert werden soll, wird für pos x,y  m_SOM[yHit][xHit] übernommen
        m_back.at<Vec3b>(y,x)[0] = m_SOM[yHit][xHit].val[0] * rad2deg;
        m_back.at<Vec3b>(y,x)[1] = m_SOM[yHit][xHit].val[1] * 255.0f;
        m_back.at<Vec3b>(y,x)[2] = m_SOM[yHit][xHit].val[2] * 255.0f;
#endif
      }
      else
      {
        //Vordergrund
        fore.at<uchar>(y,x) = 255;
      }

    }
  }

  
}

void SOBS_CF::getBackgroundImage(cv::Mat& back) const
{
#ifdef generateBackground
  cvtColor(m_back, back, CV_HSV2BGR);
#endif
}

namespace
{

  float distHSVsquared(const cv::Vec3f& v1, const cv::Vec3f& v2)
  {
    const float v1_21 = v1[2] * v1[1];
    const float v2_21 = v2[2] * v2[1];
    return (v1_21 * cos(v1[0]) - v2_21 * cos(v2[0])) * (v1_21 * cos(v1[0]) - v2_21 * cos(v2[0])) +
           (v1_21 * sin(v1[0]) - v2_21 * sin(v2[0])) * (v1_21 * sin(v1[0]) - v2_21 * sin(v2[0])) +
           (v1[2] - v2[2]) * (v1[2] - v2[2]);
  }

  void Vec3bToVec3fHSV(const cv::Vec3b& vb, cv::Vec3f& vf)
  {
    vf[0] = static_cast<float>(vb[0]) * deg2rad;
		vf[1] = static_cast<float>(vb[1]) / 255.0f;
		vf[2] = static_cast<float>(vb[2]) / 255.0f;
  }

} // namespace