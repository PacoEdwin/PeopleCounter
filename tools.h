#pragma once

// std includes
#include <vector>
#include <limits>

// project includes
#include "types.h"

#define INF(s) std::numeric_limits<s>::max() 

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

	inline std::vector<cv::Point> getCentroids(const Contours& contours)
	{
		std::vector<cv::Point> output;

		for (const Contour &el: contours)
		{
			cv::Point centroid;
			for (auto &v : el)
				centroid += v;

			centroid.x = centroid.x / el.size();
			centroid.y = centroid.y / el.size();
			output.push_back(centroid);
		}

		return output;
	}
}
