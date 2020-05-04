#pragma once

// std includes
#include <string>
#include <vector>

// cv includes
#include <opencv2/core/types.hpp>

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