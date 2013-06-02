#include "SOBS_CF.h"

#include <opencv2/opencv.hpp>

#include <iostream>


using namespace std;
using namespace cv;

namespace
{

  float distHSVsquared(const cv::Vec3f& v1, const cv::Vec3f& v2);
  void Vec3bToVec3fHSV(const cv::Vec3b& vb, cv::Vec3f& vf);

}

/*
eukidische bgr distance ausprobieren
opening -> Feedback
+ feedback von Tracking

*/
SOBS_CF::SOBS_CF(const cv::Mat& frame0, size_t M, size_t N, size_t kernel, float fuzzyexp, float fuzzythresh)
  : m_N(N), m_M(M), m_fuzzyexp(fuzzyexp), m_fuzzythresh(fuzzythresh)
{
  m_offset = (kernel - 1)/2;
  m_widthSOM = frame0.size().width*M + 2*m_offset;
  m_heightSOM = frame0.size().height*N  + 2*m_offset;

  m_width = frame0.size().width;
	m_height = frame0.size().height;
  m_SOM.resize(m_heightSOM, vector<Vec3f>(m_widthSOM));

  Mat frame0hsv;
  cvtColor(frame0, frame0hsv, CV_BGR2HSV);

  for(size_t y = 0; y < m_height; ++y)
  {
    for(size_t x = 0; x < m_width; ++x)
    {
      for(size_t n = 0; n < N; ++n)     
      {
        for(size_t m = 0; m < M; ++m)
        {
          Vec3bToVec3fHSV(frame0hsv.at<Vec3b>(y, x), m_SOM[M * y + m + m_offset][N * x + n + m_offset]);

          //m_SOM[M * y + m + m_offset][N * x + n + m_offset].val[0] = static_cast<float>(frame0hsv.at<Vec3b>(y, x).val[0]) * (CV_PI / 180.0f);
          //m_SOM[M * y + m + m_offset][N * x + n + m_offset].val[1] = static_cast<float>(frame0hsv.at<Vec3b>(y, x).val[1]) / 255.0f;
          //m_SOM[M * y + m + m_offset][N * x + n + m_offset].val[2] = static_cast<float>(frame0hsv.at<Vec3b>(y, x).val[2]) / 255.0f;

          //m_SOM[M * y + m + m_offset][N * x + n + m_offset] = Vec3f(frame0hsv.at<Vec3b>(y, x));
        }
      }
    }
  }


  m_kw.resize(kernel, vector<float>(kernel));

  float cM;
	float cN;
	float Wmax = FLT_MIN;

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

	// Parameter

	//m_epsilon1 = 100.0f*100.0f;
	//m_epsilon2 = 20.0f*20.0f;
  m_epsilon1 = 0.1f;
  m_epsilon2 = 0.09f;

	m_alpha1 = 1.0f/Wmax;
	m_alpha2 = 0.05f/Wmax;

	m_K = 0;
	m_TSteps = 100;

  m_back = frame0.clone();
}

void SOBS_CF::update(const cv::Mat& frame, cv::Mat& fore)
{
  fore = Mat(frame.size(), CV_8U);

  Mat frameHSV;
  cvtColor(frame, frameHSV, CV_BGR2HSV);

  float alpha,a;
	float epsilon;

	if (m_K <= m_TSteps) {									// calibration phase
		epsilon = m_epsilon1;
		alpha = (m_alpha1-m_K*(m_alpha1-m_alpha2)/m_TSteps);
		m_K++;
	} else {														// online phase
		epsilon = m_epsilon2;
		alpha = m_alpha2;
	}

  for (size_t y=0;y<m_height;y++)
  {
    int jj = m_offset + y*m_N;
    for (size_t x=0;x<m_width;x++) 
    {
			int ii = m_offset + x*m_M;

   //   const float srcH = static_cast<float>(frameHSV.at<Vec3b>(y, x).val[0]) * (CV_PI / 180.0f);
			//const float srcS = static_cast<float>(frameHSV.at<Vec3b>(y, x).val[1]) / 255.0f;
			//const float srcV = static_cast<float>(frameHSV.at<Vec3b>(y, x).val[2]) / 255.0f;

      Vec3f src;
      Vec3bToVec3fHSV(frameHSV.at<Vec3b>(y, x), src);


      float d2min = FLT_MAX;
			size_t xHit = ii;
			size_t yHit = jj;

      float omega = 0;

      for (size_t l=0;l<m_N;l++) 
      {
        for (size_t k=0;k<m_M;k++) 
        {
       //   const float somH = m_SOM[jj+l][ii+k].val[0];
			    //const float somS = m_SOM[jj+l][ii+k].val[1];
			    //const float somV = m_SOM[jj+l][ii+k].val[2];
        
          const float d2 = distHSVsquared(m_SOM[jj+l][ii+k], src);

          //const float d2 = (srcV * srcS * cos(srcH) - somV * somS * cos(somH)) * (srcV * srcS * cos(srcH) - somV * somS * cos(somH)) +
          //                  (srcV * srcS * sin(srcH) - somV * somS * sin(somH)) * (srcV * srcS * sin(srcH) - somV * somS * sin(somH)) +
          //                  (srcV - somV) * (srcV - somV);
          //cout << "d: " << d2 << endl;
          if(d2 < epsilon)
            ++omega;
          if (d2 < d2min)
          {
            d2min = d2;
            xHit = ii + k;
            yHit = jj + l;
          }
        }
      }

      // Update SOM

      // Berechne Lernrate alpha
      float fuzzyBG = 0.0f;
			if ( d2min < epsilon ) 
				fuzzyBG =  1.0f - d2min/epsilon;

      const float NCF = omega / static_cast<float>(m_M * m_N);

      float fuzzyCoherence = 0.0f;
      if(NCF > 0.5f)
        fuzzyCoherence = 2 * NCF - 1;

			//const float alphamax = alpha*exp(m_fuzzyexp*fuzzyBG);
      //cout << "d2min: " << d2min << endl;
      //cout << "fuzzyBG: " << fuzzyBG << endl;
      //cout << "fuzzyCoherence: " << fuzzyCoherence << endl;
      //cout << "fuzzy: " << fuzzyBG*fuzzyCoherence << endl;

      const float alphaxy = fuzzyBG * fuzzyCoherence * alpha;

      //cout << "alphaxy: " << alphaxy << endl;

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
        // falls hier ein Hintergrundbild zum anzeigen generiert werden soll, kann für pos x,y  m_SOM[yHit][xHit] übernommen werden

        m_back.at<Vec3b>(y,x)[0] = m_SOM[yHit][xHit].val[0] * (180.f / CV_PI);
        m_back.at<Vec3b>(y,x)[1] = m_SOM[yHit][xHit].val[1] * 255.0f;
        m_back.at<Vec3b>(y,x)[2] = m_SOM[yHit][xHit].val[2] * 255.0f;
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
  cvtColor(m_back, back, CV_HSV2BGR);
}

namespace
{

  float distHSVsquared(const cv::Vec3f& v1, const cv::Vec3f& v2)
  {
    return (v1[2] * v1[1] * cos(2.0f * CV_PI * v1[0]) - v2[2] * v2[1] * cos(2.0f * CV_PI * v2[0])) * (v1[2] * v1[1] * cos(2.0f * CV_PI * v1[0]) - v2[2] * v2[1] * cos(2.0f * CV_PI * v2[0])) +
           (v1[2] * v1[1] * sin(2.0f * CV_PI * v1[0]) - v2[2] * v2[1] * sin(2.0f * CV_PI * v2[0])) * (v1[2] * v1[1] * sin(2.0f * CV_PI * v1[0]) - v2[2] * v2[1] * sin(2.0f * CV_PI * v2[0])) +
           (v1[2] - v2[2]) * (v1[2] - v2[2]);
  }

  void Vec3bToVec3fHSV(const cv::Vec3b& vb, cv::Vec3f& vf)
  {
    vf[0] = static_cast<float>(vb[0]) * (CV_PI / 180.0f);
		vf[1] = static_cast<float>(vb[1]) / 255.0f;
		vf[2] = static_cast<float>(vb[2]) / 255.0f;
  }

}

/*
SOBS_CF::SOBS_CF(const cv::Mat& frame0, size_t M, size_t N, size_t kernel, double fuzzyexp, double fuzzythresh)
  : m_N(N), m_M(M), m_fuzzyexp(fuzzyexp), m_fuzzythresh(fuzzythresh)
{
  m_offset = (kernel - 1)/2;
  m_widthSOM = frame0.size().width*M + 2*m_offset;
  m_heightSOM = frame0.size().height*N + 2*m_offset;

  m_width = frame0.size().width;
	m_height = frame0.size().height;
  m_SOM.resize(m_heightSOM, vector<Vec3d>(m_widthSOM));
  for(size_t x = 0; x < frame0.size().width; ++x)
  {
    //std::vector<cv::Point3d> vec(m_heightSOM);
    for(size_t y = 0; y < frame0.size().height; ++y)
    {
      for(size_t n = 0; n < N; ++n)     
      {
        for(size_t m = 0; m < M; ++m)
        {
          //m_SOM[x + n][y + m] = Vec3f(frame0.at<Vec3b>(y, x).val[0], frame0.at<Vec3b>(y, x).val[1], frame0.at<Vec3b>(y, x).val[2]);
          m_SOM[M * y + m][N * x + n] = Vec3f(frame0.at<Vec3b>(y, x));
        }
      }
    }
  }


  m_kw.resize(kernel, vector<double>(kernel));

  int cM;
	int cN;
	double Wmax = DBL_MIN;

	cN = 1;
	for (size_t j=0;j<kernel;j++) {
		cM = 1;		
		for (size_t i=0;i<kernel;i++) {
			m_kw[j][i] = cN*cM;
      if (m_kw[j][i]>Wmax) Wmax = m_kw[j][i]; 
			cM = cM * (kernel - 1 - i) / (i + 1);
		}
		cN = cN * (kernel - 1 - j) / (j + 1);
	}

	// Parameter

	m_epsilon1 = 100.0*100.0;
	m_epsilon2 = 20.0*20.0;

	m_alpha1 = 1.0/Wmax;
	m_alpha2 = 0.05/Wmax;

	m_K = 0;
	m_TSteps = 100;
}


void SOBS_CF::update(const cv::Mat& frame, cv::Mat& fore)
{
  fore = Mat(frame.size(), CV_8U);

  double alpha,a;
	double epsilon;

	if (m_K <= m_TSteps) {									// calibration phase
		epsilon = m_epsilon1;
		alpha = (m_alpha1-m_K*(m_alpha1-m_alpha2)/m_TSteps);
		m_K++;
	} else {														// online phase
		epsilon = m_epsilon2;
		alpha = m_alpha2;
	}

  for (size_t y=0;y<m_height;y++)
  {
    int jj = m_offset + y*m_N;
    for (size_t x=0;x<m_width;x++) 
    {
			int ii = m_offset + x*m_M;

      //BGR
      double srcR = static_cast<double>(frame.at<Vec3b>(y, x).val[2]);
			double srcG = static_cast<double>(frame.at<Vec3b>(y, x).val[1]);
			double srcB = static_cast<double>(frame.at<Vec3b>(y, x).val[0]);

      double d2min = DBL_MAX;
			size_t xHit = ii;
			size_t yHit = jj;

      for (size_t l=0;l<m_N;l++) 
      {
        for (size_t k=0;k<m_M;k++) 
        {

          double dr = srcR - m_SOM[jj+l][ii+k].val[2];
          double dg = srcG - m_SOM[jj+l][ii+k].val[1];
          double db = srcB - m_SOM[jj+l][ii+k].val[0];

          double d2 = dr*dr + dg*dg + db*db;

          if (d2 < d2min)
          {
            d2min = d2;
            xHit = ii + k;
            yHit = jj + l;
          }
        }
      }

      double fuzzyBG = 1.0;

			if ( d2min < epsilon ) 
				fuzzyBG = d2min/epsilon;

      // Update SOM

			double alphamax = alpha*exp(m_fuzzyexp*fuzzyBG);

			for (size_t l=yHit-m_offset;l<=(yHit+m_offset);l++) 
      {
				for (size_t k=xHit-m_offset;k<=(xHit+m_offset);k++)
        {

					a = alphamax*m_kw[l-yHit+m_offset][k-xHit+m_offset];
						
						// speed hack.. avoid very small increment values. abs() is sloooow.

					double d;
						
          d = srcR - m_SOM[l][k].val[2];
					if (d*d > DBL_MIN)
            m_SOM[l][k].val[2] += a*d;

					d = srcG - m_SOM[l][k].val[1];
					if (d*d > DBL_MIN)
						m_SOM[l][k].val[1] += a*d;

					d = srcB - m_SOM[l][k].val[0];
					if (d*d > DBL_MIN)
						m_SOM[l][k].val[0] += a*d;

				}
			}

      if (fuzzyBG >= m_fuzzythresh) {
				
				// Set foreground image
        fore.at<uchar>(y,x) = 255;

			} else {

				// Set background image
        
				//prgbBG->Red = m_ppSOM[jjHit][iiHit].Red;
				//prgbBG->Green = m_ppSOM[jjHit][iiHit].Green;
				//prgbBG->Blue =  m_ppSOM[jjHit][iiHit].Blue;
        
				// Set foreground image
        fore.at<uchar>(y,x) = 0;
			}

    }
  }

  
}

*/