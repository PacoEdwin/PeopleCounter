#pragma once
#define DBSCAN_h

// std includes
#include <math.h>
#include <list>

// project includes
#include "graph.h"

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

