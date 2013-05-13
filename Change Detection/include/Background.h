#pragma once

#include <string>
#include <opencv2/opencv.hpp>

class Background
{
public:
	Background(std::string path, size_t erosion_size = 2, size_t dilatation_size = 2, size_t numSigma = 3);
	~Background(void);

public:
	void update(const cv::Mat& frame, cv::Mat& fore);

	void getBackgroundImage(cv::Mat& back) const;

private:
  bool inAnyRect(cv::Point p) const;

private:

	cv::BackgroundSubtractorMOG2 m_bg;
	
	cv::Mat m_mean;
	cv::Mat m_M2;
	cv::Mat m_sigma;

	size_t m_n;

  size_t m_numSigma;

  cv::Mat m_erosionKernel;
  cv::Mat m_dilatationKernel;


	std::string m_path;

  cv::Mat m_lastFrame;

  std::list<std::pair<cv::Rect, float>> m_roi;

  

};

