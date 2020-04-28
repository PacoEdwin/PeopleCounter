#include "DBSCAN.h"

DBSCAN::DBSCAN(const std::vector<node*>& data):
	v_(data), 
	neighborhood_(10) {}

void DBSCAN::perform()
{
	for (auto &el: v_)
	{
		if (el->color == "white")
		{
			graphs_.resize(graphs_.size() + 1);
			clustering(el);
		}
	}

	numOfClusters_ = graphs_.size();
}

DBSCAN::~DBSCAN()
{
	for (auto& cluster : graphs_)
		for (auto& edges : cluster)
			for (auto& el : edges)
				delete el;
}

Clusters DBSCAN::result()
{
	if (graphs_.empty())
		perform();

	return graphs_;
}

uint32_t DBSCAN::numberOfClusters() const
{
	return numOfClusters_;
}

// kinda dfs
void DBSCAN::clustering(node* u)
{
	u->color = "gray";

	bool hasNeighbor = false;

	for (int i = 0; i < v_.size(); i++)
	{
		double dist = math::euclidian(v_[i]->c[0], v_[i]->c[1], u->c[0], u->c[1]);

		// don't init some isolated shit
		if (v_[i]->color == "white" && dist < neighborhood_ && v_[i] != u)
		{
			hasNeighbor = true;

			edge* r = new edge;
			edge* w = new edge;

			// init edges
			{
				v_[i]->ancestor = u;
				r->link = v_[i];
				w->link = u;
				r->w = dist;
				w->w = dist;
			}

			if (graphs_[graphs_.size() - 1].size() == 0)
			{
				r->link->cluster = w->link->cluster = graphs_.size() - 1;
				w->link->pos = 0;
				r->link->pos = 1;
				graphs_[graphs_.size() - 1].resize(2);
				graphs_[graphs_.size() - 1][w->link->pos].push_back(r);
				graphs_[graphs_.size() - 1][r->link->pos].push_back(w);
				clustering(v_[i]);
			}
			else
			{
				r->link->cluster = u->cluster;
				graphs_[u->cluster][u->pos].push_back(r);

				// add back edge
				graphs_[u->cluster].resize(graphs_[u->cluster].size() + 1);
				v_[i]->pos = graphs_[u->cluster].size() - 1;
				graphs_[u->cluster][v_[i]->pos].push_back(w);

				clustering(v_[i]);
			}
		}
	}

	if (!hasNeighbor)
		u->cluster = graphs_.size() - 1;

	u->color = "black";
}