// project includes
#include "dbscan.h"
#include "tools.h"

// std includes
#include <queue>
#include <iostream>

DBSCAN::DBSCAN(const cv::Mat& img, const std::vector<node*>& data) :
	v_(data),
	img_(img),
	neighborhood_(6)
{
	m_ = decltype(m_)(img_.rows, std::vector<node*>(img_.cols, nullptr));
	for (auto el : v_)
		m_[el->c.y][el->c.x] = el;
}

DBSCAN::~DBSCAN()
{
	for (auto& cluster : graphs_)
		for (auto& edges : cluster)
			for (auto el : edges)
				delete el;
}

void DBSCAN::perform()
{
	for (auto el : v_)
	{
		if (el->color == "white")
		{
			el->cluster = graphs_.size();
			graphs_.push_back(std::vector<std::vector<edge*>>());
			clustering(el);
		}
	}

	numOfClusters_ = graphs_.size();
}

uint32_t DBSCAN::numberOfClusters() const
{
	return numOfClusters_;
}


std::vector<node*> DBSCAN::getNeighbors(node* v) const
{
	std::vector<node*> output;

	int x = std::max(v->c.x - neighborhood_, 0);
	int y = std::max(v->c.y - neighborhood_, 0);

	int x_m = std::min(img_.cols - 1, v->c.x + neighborhood_);
	int y_m = std::min(img_.rows - 1, v->c.y + neighborhood_);

	for (int j = y; j <= y_m; j++)
		for (int i = x; i <= x_m; i++)
			if (m_[j][i] && m_[j][i]->color == "white" && m_[j][i] != v)
				output.push_back(m_[j][i]);

	return output;
}

void DBSCAN::clustering(node* s)
{
	std::queue<node*> q;
	q.push(s);
	s->color = "gray";

	while (!q.empty())
	{
		auto u = q.front();
		q.pop();

		auto neighbors = getNeighbors(u);

		for (auto el : neighbors)
		{
			double dist = math::euclidian(el->c, u->c);

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

		u->color = "black";
	}
}