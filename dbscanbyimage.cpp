// project includes
#include "dbscanbyimage.h"

// std includes
#include <queue>
#include <iostream>

DBSCANByImage::DBSCANByImage(const cv::Mat& img, const std::vector<node*>& data):
	v_(data),
	img_(img),
	neighborhood_(10)
{
	m_ = decltype(m_)(img_.rows, std::vector<node*>(img_.cols, nullptr));
	for (auto el : v_)
		m_[el->c.y][el->c.x] = el;
}

DBSCANByImage::~DBSCANByImage()
{
	for (auto& cluster : graphs_)
		for (auto& edges : cluster)
			for (auto& el : edges)
				delete el;
}

void DBSCANByImage::perform()
{
	for(auto el: v_)
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

uint32_t DBSCANByImage::numberOfClusters() const
{
	return numOfClusters_;
}


std::vector<node*> DBSCANByImage::getNeighbors(node* v) const
{
	std::vector<node*> output;

	int x = std::max(v->c.x - neighborhood_, 0);
	int y = std::max(v->c.y - neighborhood_, 0);

	int x_m = std::min(img_.cols, v->c.x + neighborhood_);
	int y_m = std::min(img_.rows, v->c.y + neighborhood_);

	//for (; y < y_m; y++)
	//{
	//	std::cout << y << std::endl;
	//	for (; x < x_m; x++)
	//	{
	//		std::cout << x << " ";
	//		//if (i == v->c.x && j == v->c.y)
	//		//	std::cout << "sssssssss" << std::endl;

	//		if (m_[y][x] && !(x == v->c.x && y == v->c.y))
	//		{
	//			output.push_back(m_[y][x]);
	//			//std::cout << "xx" << std::endl;
	//		}

	//		//if (m_[j][i])
	//		//	std::cout << "ssxz" << std::endl;

	//		//std::cout << (int)img_.at<uchar>(x, y) << std::endl;
	//	}
	//}

	for (int j = y; j < y_m; j++)
	{
		for (int i = x ; i < x_m; i++)
		{
			//if (i == v->c.x && j == v->c.y)
			//	std::cout << "sssssssss" << std::endl;

			if (m_[j][i] && m_[j][i]->color == "white" && !(i == v->c.x && j == v->c.y))
			{
				output.push_back(m_[j][i]);
				//std::cout << "xx" << std::endl;
			}

			//if (m_[j][i])
			//	std::cout << "ssxz" << std::endl;

			//std::cout << (int)img_.at<uchar>(x, y) << std::endl;
		}
	}

	//std::cout << "emd" << std::endl;

	return output;
}

void DBSCANByImage::clustering(node* s)
{
	std::queue<node*> q;
	q.push(s);
	s->color = "gray";

	while (!q.empty())
	{
		auto u = q.front();
		q.pop();

		auto neighbors = getNeighbors(u);
		for(auto el: neighbors)
		{
			//// don't init some isolated shit
			//if (el->color == "white")
			//{
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
			//}
		}

		u->color = "black";
	}

	int i = 0;
}