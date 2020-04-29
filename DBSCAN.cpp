// projects includes
#include "DBSCAN.h"

// std includes
#include <queue>

DBSCAN::DBSCAN(const std::vector<node*>& data):
	v_(data), 
	neighborhood_(10) {}

void DBSCAN::perform()
{
	while(!v_.empty())
	{
		auto el = v_[0];
		if (el->color == "white")
		{
			el->cluster = graphs_.size();
			graphs_.resize(graphs_.size() + 1);
			//v_.erase(v_.begin());
			clusteringBFS(el);
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

void DBSCAN::clusteringBFS(node* s)
{
	std::queue<node*> q;
	q.push(s);
	s->color = "gray";

	while (!q.empty())
	{
		auto u = q.front();
		q.pop();

		int i = 0;
		int j = v_.size();
		while(i != j)
		{
			auto &el = v_[i];
			// don't init some isolated shit
			if (el->color == "white")
			{
				double dist = math::euclidian(el->c[0], el->c[1], u->c[0], u->c[1]);
				if (dist < neighborhood_ && el != u)
				{
					double dist = math::euclidian(el->c[0], el->c[1], u->c[0], u->c[1]);

					edge* r = new edge;
					edge* w = new edge;

					// init edges
					{
						el->ancestor = u;
						r->link = el;
						w->link = u;
						r->w = dist;
						w->w = dist;
						r->link->cluster = w->link->cluster = u->cluster;
					}

					if (graphs_[graphs_.size() - 1].size() == 0)
					{
						graphs_[graphs_.size() - 1].push_back(std::vector<edge*>());
						w->link->pos = 0;
					}

					graphs_[u->cluster][u->pos].push_back(r);
					graphs_[u->cluster].push_back(std::vector<edge*>());
					el->pos = graphs_[u->cluster].size() - 1;
					graphs_[u->cluster][el->pos].push_back(w);

					q.push(el);

					el->color = "gray";
				}
				else
					i++;
			}
			else
			{
				j--;
				std::swap(v_[i], v_[j]);
			}

			u->color = "black";
		}

		v_.resize(j);
	}
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
				r->link->cluster = u->cluster;
			}

			if (graphs_[graphs_.size() - 1].size() == 0)
			{
				w->link->pos = 0;
				r->link->pos = 1;
				graphs_[graphs_.size() - 1].resize(2);
				graphs_[graphs_.size() - 1][w->link->pos].push_back(r);
				graphs_[graphs_.size() - 1][r->link->pos].push_back(w);
			}
			else
			{
				// add edges
				graphs_[u->cluster][u->pos].push_back(r);
				graphs_[u->cluster].resize(graphs_[u->cluster].size() + 1);
				v_[i]->pos = graphs_[u->cluster].size() - 1;
				graphs_[u->cluster][v_[i]->pos].push_back(w);
			}

			clustering(v_[i]);
		}
	}

	u->color = "black";
}