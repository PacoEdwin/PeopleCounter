#pragma once

#include <opencv2/core/types.hpp>

class Object 
{
public:
	Object();
	Object(const cv::Point&);
	Object(const Object&);

	int id() const;

	cv::Point location_;
	cv::Scalar color_;
private:
	int id_;
};