#pragma once

#include <opencv2/opencv.hpp>

class SOBS_CF
{
public:
  SOBS_CF(const cv::Mat& frame0, size_t M = 3, size_t N = 3, size_t kernel = 3, float fuzzyexp = -5.0f, float fuzzythresh = 0.8f);

  void update(const cv::Mat& frame, cv::Mat& fore);

  void getBackgroundImage(cv::Mat& back) const;

private:

  std::vector<std::vector<cv::Vec3f>> m_SOM;

  std::vector<std::vector<float>> m_kw;

  cv::Mat m_back;

  size_t m_M;
  size_t m_N;
  size_t m_width;
	size_t m_height;
  size_t m_widthSOM;
	size_t m_heightSOM;
	size_t m_offset;
  size_t m_K;
	size_t m_TSteps;

  float m_epsilon1;
	float m_epsilon2;
	float m_alpha1;
	float m_alpha2;
  float m_fuzzyexp;
  float m_fuzzythresh;
};