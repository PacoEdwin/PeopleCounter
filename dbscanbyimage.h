#pragma once

#include <opencv2/opencv.hpp>
// project includes
#include "graph.h"

class DBSCANByImage
{
public:

	DBSCANByImage(const cv::Mat&, const std::vector<node*>&);
	~DBSCANByImage();

	void perform();
	Clusters result();
	uint32_t numberOfClusters() const;

private:
	// maybe rewrite using dfs
	void clustering(node*);
	std::vector<node*> getNeighbors(node*) const;
	void clusteringBFS(node*);

	// vertices of graph
	int neighborhood_;
	uint32_t numOfClusters_ = 0;
	
	cv::Mat img_;
	std::vector<node*> v_;
	std::vector<std::vector<node*>> m_;
	std::vector<std::vector<std::vector<edge*>>> graphs_;
};