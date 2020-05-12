#pragma once
// std includes
#include <vector>

// project includes
#include "types.h"

// cv includes
#include <opencv2/opencv.hpp>

class ContourExtractor
{

public:
	ContourExtractor();
	ContourExtractor(const cv::Mat&);

	Contours contours() const;
	Contours extractContours();
	void setMask(const cv::Mat&);

private:
	void removeBySize();
	void removeChildContours(const std::vector<cv::Vec4i>& hierarchy);

	cv::Mat m_mask;
	Contours m_contours;
	// TODO: add setters and getters
	int m_radius = 5;
	int	m_apertureSize;
	double 	m_threshold1;
	double 	m_threshold2;
};