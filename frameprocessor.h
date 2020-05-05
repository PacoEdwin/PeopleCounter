#pragma once

// project incldes
#include "contourextractor.h"
#include "objecttracker.h"

// cv includes
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

class FrameProcessor
{
public:
	FrameProcessor();
	
	~FrameProcessor();

	void process(const cv::Mat&);

	ObjectTracker* tracker() const;
	ContourExtractor* extractor() const;

private:

	ObjectTracker* m_tracker;
	ContourExtractor* m_extractor;
	cv::Ptr<cv::BackgroundSubtractorKNN> m_backSub;
};