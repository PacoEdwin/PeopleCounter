#pragma once
// std includes
#include <vector>

// project includes
#include "types.h"
#include "Object.h"

class ObjectTracker
{
public:
	ObjectTracker() = default;

	/// Hungarian algorithm
	void update(const Contours&);

	std::vector<Object> objects() const;

private:
	int m_numOfObjects = 0;
	std::vector<Object> m_objects;
};