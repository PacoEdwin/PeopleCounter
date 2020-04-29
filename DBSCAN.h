#pragma once
#define DBSCAN_h

// std includes
#include <math.h>
#include <vector>
#include <string>
#include <list>

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
	std::vector<int> c;
	std::string color = "white";

	node* ancestor = nullptr;
};

struct edge
{
	node* link = nullptr;
	double w;
};

typedef std::vector<std::vector<std::vector<edge*>>> Clusters;

class DBSCAN 
{
public:
	
	DBSCAN(const std::vector<node*>&);
	~DBSCAN();

	void perform();
	Clusters result();
	uint32_t numberOfClusters() const;

private:
	// maybe rewrite using dfs
	void clustering(node*);
	void clusteringBFS(node*);

	// vertices of graph
	int neighborhood_;
	uint32_t numOfClusters_ = 0;

	std::vector<node*> v_;
	std::vector<std::vector<std::vector<edge*>>> graphs_;
};