// project include
#include "objecttracker.h"
#include "tools.h"

// std includes
#include <algorithm>

std::vector<Object> ObjectTracker::objects() const
{
	return m_objects;
}

void ObjectTracker::update(const std::vector<std::vector<cv::Point>>& contours)
{
	auto newCentroids = getCentroids(contours);

	std::vector<bool> newUsed(newCentroids.size());
	std::vector<bool> oldUsed(m_objects.size());

	int n = std::min(newCentroids.size(), m_objects.size());
	int m = std::max(newCentroids.size(), m_objects.size());

	std::vector<std::vector<int>> a(n + 1, std::vector<int>(m + 1));

	if (newCentroids.size() > m_objects.size())
	{
		for (int i = 1; i < n + 1; i++)
			for (int j = 1; j < m + 1; j++)
				a[i][j] = math::euclidian(m_objects[i - 1].location_, newCentroids[j - 1]);
	}
	else
	{
		for (int i = 1; i < n + 1; i++)
			for (int j = 1; j < m + 1; j++)
				a[i][j] = math::euclidian(m_objects[j - 1].location_, newCentroids[i - 1]);
	}

	std::vector<int> u(n + 1), v(m + 1), p(m + 1), way(m + 1);
	for (int i = 1; i <= n; ++i)
	{
		p[0] = i;
		int j0 = 0;
		std::vector<int> minv(m + 1, INF(int));
		std::vector<char> used(m + 1, false);

		do {
			used[j0] = true;
			int i0 = p[j0], delta = INF(int), j1;

			for (int j = 1; j <= m; ++j)
				if (!used[j])
				{
					int cur = a[i0][j] - u[i0] - v[j];
					if (cur < minv[j])
						minv[j] = cur, way[j] = j0;
					if (minv[j] < delta)
						delta = minv[j], j1 = j;
				}

			for (int j = 0; j <= m; ++j)
				if (used[j])
					u[p[j]] += delta, v[j] -= delta;
				else
					minv[j] -= delta;

			j0 = j1;
		} while (p[j0] != 0);

		do
		{
			int j1 = way[j0];
			p[j0] = p[j1];
			j0 = j1;
		} while (j0);
	}

	std::vector<int> ans(n + 1);
	for (int j = 1; j <= m; ++j)
		ans[p[j]] = j;

	if (newCentroids.size() > m_objects.size())
	{
		for (int i = 1; i < n + 1; i++)
		{
			/// Presumably ans[i] never 0 
			if (!ans[i] == 0)
			{
				m_objects[i - 1].location_ = newCentroids[ans[i] - 1];
				newUsed[ans[i] - 1] = true;
			}
		}

		for (int i = 0; i < m; i++)
			if (!newUsed[i])
				m_objects.emplace_back(newCentroids[i]);
	}
	else
	{
		for (int i = 1; i < n + 1; i++)
		{
			m_objects[ans[i] - 1].location_ = newCentroids[i - 1];
			oldUsed[ans[i] - 1] = true;
		}

		m_objects.erase(remove_if(m_objects.begin(), m_objects.end(), [this, &oldUsed](const Object& value) {
			return !oldUsed[&value - &*m_objects.begin()];
		}), m_objects.end());
	}
}