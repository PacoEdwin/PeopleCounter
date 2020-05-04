#pragma once
// std includes
#include <vector>

// cv includes
#include <opencv2/opencv.hpp>
#include <opencv2/core/types.hpp>

class ContourExtractor
{

public:
	ContourExtractor();
	ContourExtractor(const cv::Mat&);

	std::vector<std::vector<cv::Point>> extractContours() const;
	void setMask(const cv::Mat&);

private:
	void removeBySize(std::vector<std::vector<cv::Point>> &contours) const;
	void removeChildContours(const std::vector<cv::Vec4i>& hierarchy, std::vector<std::vector<cv::Point>> &contours) const;

	cv::Mat m_mask;

	// TODO: add setters and getters
	int	m_apertureSize;
	double 	m_threshold1;
	double 	m_threshold2;
};