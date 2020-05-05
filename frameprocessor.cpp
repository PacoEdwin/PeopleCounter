// projects includes
#include "types.h"
#include "frameprocessor.h"

FrameProcessor::FrameProcessor()
{
	m_tracker = new ObjectTracker();
	m_extractor = new ContourExtractor();
	m_backSub = cv::createBackgroundSubtractorKNN(500, 2500, true);
}

FrameProcessor::~FrameProcessor() 
{
	delete m_tracker;
	delete m_extractor;
}

ObjectTracker* FrameProcessor::tracker() const
{
	return m_tracker;
}

ContourExtractor* FrameProcessor::extractor() const
{
	return m_extractor;
}

void FrameProcessor::process(const cv::Mat& frame)
{
	cv::Mat mask;
	m_backSub->apply(frame, mask);
	
	m_extractor->setMask(mask);
	Contours contours = m_extractor->extractContours();

	m_tracker->update(contours);
}

