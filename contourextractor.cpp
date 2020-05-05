// project incldes
#include "contourextractor.h"
#include "DBSCAN.h"
#include "tools.h"

// std incudes
#include <unordered_map>

ContourExtractor::ContourExtractor():
	m_apertureSize(3),
	m_threshold1(250),
	m_threshold2(900) {}

ContourExtractor::ContourExtractor(const cv::Mat& mask):
	m_mask(mask),
	m_apertureSize(3),
	m_threshold1(250),
	m_threshold2(900) {}

void ContourExtractor::removeBySize(std::vector<std::vector<cv::Point>> &contours) const
{
	contours.erase(std::remove_if(contours.begin(), contours.end(), [](const std::vector<cv::Point>& el) ->bool {
		cv::Point centroid;
		for (auto p : el)
			centroid += p;

		centroid.x = centroid.x / el.size();
		centroid.y = centroid.y / el.size();

		for (auto p : el)
			if (math::euclidian(centroid, p) > 5)
				return false;

		return true;
	}), contours.end());
}

void ContourExtractor::removeChildContours(const std::vector<cv::Vec4i>& hierarchy, std::vector<std::vector<cv::Point>> &contours) const
{
	std::vector<std::vector<cv::Point>> tmpContours;
	std::vector<bool> displayed(hierarchy.size());

	/// Remove all child contours
	for (int i = 0; i < hierarchy.size(); i++)
	{
		int current = hierarchy[i][3];

		// shall display and mark as displayed
		if (current == -1)
		{
			tmpContours.push_back(contours[i]);
			displayed[i] = true;
			continue;
		}

		/// Can be optimized by marking as displayed while ascending
		while (hierarchy[current][3] != -1)
			current = hierarchy[current][3];

		if (!displayed[current])
		{
			tmpContours.push_back(contours[current]);
			displayed[current] = true;
		}
	}

	// maybe make contours as list or use erase-remove idiom
	contours = tmpContours;
}

std::vector<std::vector<cv::Point>> ContourExtractor::extractContours() const
{
	cv::Mat canny_output;
	std::vector<cv::Vec4i> hierarchy;
	std::vector<std::vector<cv::Point>> contours;

	/// Detect edges using canny
	cv::Canny(m_mask, canny_output, m_threshold1, m_threshold2, m_apertureSize);
	/// Find contours
	cv::findContours(canny_output, contours, hierarchy, cv::RETR_TREE, 1, cv::Point(0, 0));
	/// Leave only outer contours
	removeChildContours(hierarchy, contours);
	/// Exclude countours whose size is less than 100
	removeBySize(contours);
	/// Init vector of nodes for DBSCAN
	std::vector<node*> v;
	std::unordered_map<int, node*> h;

	for (auto &el : contours)
	{
		for (cv::Point &p : el)
		{
			node* u = new node;
			u->c = p;

			h[p.y*canny_output.rows + p.x] = u;
		}
	}

	/// To get rid of duplicate points
	for (auto it = h.begin(); it != h.end(); it++)
		v.push_back(it->second);

	/// Get result of dbscam
	DBSCAN db(canny_output, v);
	db.perform();

	contours.clear();
	contours.resize(db.numberOfClusters());

	/// Fill contours from clusters
	for (int i = 0; i < v.size(); i++)
		contours[v[i]->cluster].emplace_back(v[i]->c);

	std::vector<std::vector<cv::Point>> hull(contours.size());
	for (size_t i = 0; i < contours.size(); i++)
		convexHull(contours[i], hull[i]);

	std::vector<bool> toDelete(contours.size());
	for (int i = 0; i < contours.size(); i++)
		for (int j = 0; j < contours.size(); j++)
		{
			if (i == j)
				continue;

			if (pointPolygonTest(hull[i], hull[j].front(), false) == 1)
				toDelete[j] = true;
		}

	/// check if point inside
	contours.erase(remove_if(contours.begin(), contours.end(), [&contours, &toDelete](const std::vector<cv::Point>& value) {
		return toDelete[&value - &*contours.begin()];
	}), contours.end());

	return contours;
}

void ContourExtractor::setMask(const cv::Mat& value)
{
	m_mask = value;
}