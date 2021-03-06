// project includes
#include "displayer.h"
#include <opencv2/highgui.hpp>

Displayer::Displayer(const cv::Mat& image):
	m_image(image) {}

void Displayer::setImage(const cv::Mat& value)
{
	m_image = value;
}

void Displayer::setFrameProcessor(FrameProcessor* value)
{
	m_frameProcessor = value;
}

void Displayer::drawContours()
{
	if (m_image.empty())
	{
		std::cout << "Image is not set" << std::endl;
		return;
	}

	auto objects = m_frameProcessor->tracker()->objects();
	auto contours = m_frameProcessor->extractor()->contours();

	for (int i = 0; i < contours.size(); i++)
		cv::drawContours(m_image, contours, i, objects[i].m_color, 2, 8, cv::noArray(), 0, cv::Point());
}

void Displayer::drawBoundingBox()
{
	if (m_image.empty())
	{
		std::cout << "Image is not set" << std::endl;
		return;
	}

	auto objects = m_frameProcessor->tracker()->objects();
	auto contours = m_frameProcessor->extractor()->contours();

	for (int i = 0; i < objects.size(); i++)
	{
		cv::Point minCorner = { m_image.cols, m_image.rows };
		cv::Point maxCorner = { 0, 0 };

		for (int j = 0; j < contours[i].size(); j++)
		{
			minCorner.x = std::min(minCorner.x, contours[i][j].x);
			minCorner.y = std::min(minCorner.y, contours[i][j].y);

			maxCorner.x = std::max(maxCorner.x, contours[i][j].x);
			maxCorner.y = std::max(maxCorner.y, contours[i][j].y);
		}

		cv::Rect rect(minCorner, maxCorner);
		cv::rectangle(m_image, rect, { 255, 255, 255 });
		cv::putText(m_image, std::to_string(objects[i].name()), objects[i].m_location, cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(255, 255, 255));
	}
}

void Displayer::display(std::string name) const
{
	if (m_image.empty())
	{
		std::cout << "Image is not set" << std::endl;
		return;
	}

	cv::imshow(name, m_image);
}