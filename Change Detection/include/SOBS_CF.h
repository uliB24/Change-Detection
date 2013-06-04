#pragma once

#include <opencv2/opencv.hpp>

class SOBS_CF
{
public:
  SOBS_CF(const cv::Mat& frame0, int M = 3, int N = 3, size_t kernel = 3, int kNaerHalf = 1);

  void update(const cv::Mat& frame, cv::Mat& fore);

  void getBackgroundImage(cv::Mat& back) const;

private:

  std::vector<std::vector<cv::Vec3f>> m_SOM;

  std::vector<std::vector<float>> m_kw;

  cv::Mat m_back;

  int m_M;
  int m_N;
  size_t m_width;
	size_t m_height;
  size_t m_widthSOM;
	size_t m_heightSOM;
	size_t m_offset;
  size_t m_K;
	size_t m_TSteps;
  int m_kNearHalf;

  float m_epsilon1;
	float m_epsilon2;
	float m_alpha1;
	float m_alpha2;
};