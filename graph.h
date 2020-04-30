#pragma once

// std includes
#include <string>
#include <vector>

// cv includes
#include <opencv2/core/types.hpp>

namespace math
{
	inline double euclidian(double x1, double y1, double x0, double y0)
	{
		return std::sqrt((x1 - x0)*(x1 - x0) + (y1 - y0)*(y1 - y0));
	}

	inline double euclidian(cv::Point p1, cv::Point p2)
	{
		return euclidian(p1.x, p1.y, p2.x, p2.y);
	}
}

struct node
{
	int pos;
	int cluster = -1;
	cv::Point c;
	std::string color = "white";

	node* ancestor = nullptr;

	bool operator==(const node& rhs) {
		return c == rhs.c;
	}
};

struct edge
{
	node* link = nullptr;
	double w;
};

typedef std::vector<std::vector<std::vector<edge*>>> Clusters;