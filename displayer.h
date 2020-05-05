#pragma once

// project includes
#include "types.h"
#include "Object.h"

// std includes
#include <string>
#include <vector>

// cv includes
#include "opencv2/opencv.hpp"

class Displayer
{
public:
	Displayer() = default;

	Displayer(const cv::Mat&);

	void setImage(const cv::Mat&);

	void drawContours(const Objects&, const Contours&);
	void drawBoundingBox(const Objects&, const Contours&);
	void display(std::string name = "contours") const;

private:
	cv::Mat m_image;
};