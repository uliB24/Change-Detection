#pragma once

#include <string>
#include <opencv2/opencv.hpp>

class Background
{
public:
	Background(std::string path, size_t erosion_size = 2, size_t dilatation_size = 2, size_t numSigma = 3, int history = 500, float varThreshold = 64.0, bool bShadowDetection = true);
	~Background(void);

public:
	void update(const cv::Mat& frame, cv::Mat& fore);

	void getBackgroundImage(cv::Mat& back) const;

private:
  bool inAnyRect(const cv::Point& p) const;


private:

	cv::BackgroundSubtractorMOG2 m_bg;
	int count;
	cv::Mat m_mean;
	cv::Mat m_M2;
	cv::Mat m_sigma;

	size_t m_n;

  size_t m_numSigma;

  cv::Mat m_erosionKernel;
  cv::Mat m_dilatationKernel;


	std::string m_path;

  cv::Mat m_lastFrame;

  std::vector<std::vector<cv::Point2f> > m_contours_prev, m_contours_next;

  std::vector<std::vector<cv::Point3i>> m_notMoved;

  std::vector<cv::Rect> m_rects;

  cv::Mat image_prev, image_next;

  cv::Rect m_frameRect;

};

