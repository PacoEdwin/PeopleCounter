#pragma once

// std includes
#include <vector>
#include <string>

// cv includes
#include <opencv2/core/types.hpp>

class Object 
{
public:
	Object();
	Object(const Object&);
	Object(const cv::Point&);

	int id() const;

	uint name() const;
	void setName(uint);

	cv::Point m_location;
	cv::Scalar m_color;

private:
	int m_id;
	uint m_name;
};

typedef std::vector<Object> Objects;