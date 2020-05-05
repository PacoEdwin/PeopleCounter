#pragma once

// project includes
#include "types.h"
#include "Object.h"
#include "frameprocessor.h"

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
	void setFrameProcessor(FrameProcessor* value);

	void drawContours();
	void drawBoundingBox();
	void display(std::string name = "contours") const;

private:
	cv::Mat m_image;
	FrameProcessor* m_frameProcessor = nullptr;
};