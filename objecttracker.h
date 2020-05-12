#pragma once
// std includes
#include <vector>

// project includes
#include "types.h"
#include "Object.h"

// std includes
#include <unordered_map>

class ObjectTracker
{
public:
	ObjectTracker() = default;

	/// Hungarian algorithm
	void update(const Contours&);

	std::vector<Object> objects() const;

	int totalLeft() const;
	int totalRight() const;

private:
	void registerObject(const Object&);
	void deregisterObject(const Object&);

	int m_totalLeft = 0;
	int m_totalRight = 0;

	int m_numOfObjects = 0;
	std::vector<Object> m_objects;
	std::unordered_map<int, std::vector<cv::Point>> m_trackableObject;
};