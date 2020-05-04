#pragma once

// cv includes
#include <opencv2/core/types.hpp>

class Object 
{
public:
	Object();
	Object(const Object&);
	Object(const cv::Point&);

	int id() const;

	cv::Point location_;
	cv::Scalar color_;

private:
	int id_;
};