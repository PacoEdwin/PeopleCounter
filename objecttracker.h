#pragma once
// std includes
#include <vector>

// project includes
#include "Object.h"

class ObjectTracker
{
public:
	ObjectTracker() = default;

	/// Hunarian algorithm
	void update(const std::vector<std::vector<cv::Point>>&);

	std::vector<Object> objects() const;

private:
	std::vector<Object> m_objects;
};