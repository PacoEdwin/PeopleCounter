// rpoject includes
#include "DBSCAN.h"
#include "Object.h"
//#include "dbscanbyimage.h"
// opencv includes
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/opencv.hpp>

// std incldes
#include <iostream>
#include <chrono>
#include <unordered_map>

using namespace cv;
using namespace std;
using namespace std::chrono;

#define INF(s) std::numeric_limits<s>::max() 

RNG rng(12345);

inline void processFrame(Mat& frame)
{
	cv::line(frame, Point(frame.cols / 2, 0), Point(frame.cols / 2, frame.rows), Scalar(0, 255, 0), 2);
}

inline void showContours(Mat& output, const  vector<vector<Point>>& contours, const vector<Vec4i>& hierarchy, const vector<Object>& objects, string name = "Contours")
{
	for (int i = 0; i < contours.size(); i++)
		drawContours(output, contours, i, objects[i].color_, 2, 8, hierarchy, 0, Point());

	imshow(name, output);
}

inline vector<Point> getCentroids(const vector<vector<Point>>& contours)
{
	vector<Point> output;

	for (auto &el : contours)
	{
		Point centroid;
		for (auto &v : el)
			centroid += v;

		centroid.x = centroid.x / el.size();
		centroid.y = centroid.y / el.size();
		output.push_back(centroid);
	}

	return output;
}

inline void removeChildContours(const vector<Vec4i>& hierarchy, vector<vector<Point>> &contours)
{
	vector<vector<Point>> tmpContours;
	vector<bool> displayed(hierarchy.size());

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

inline void removeBySize(vector<vector<Point>> &contours)
{
	contours.erase(std::remove_if(contours.begin(), contours.end(), [](const vector<Point>& el) ->bool {
		Point centroid;
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

/// Hunarian algorithm
inline void updateObjects(vector<Object>& objects, const vector<vector<Point>>& contours)
{
	auto newCentroids = getCentroids(contours);

	vector<bool> newUsed(newCentroids.size());
	vector<bool> oldUsed(objects.size());

	int n = std::min(newCentroids.size(), objects.size());
	int m = std::max(newCentroids.size(), objects.size());

	vector<vector<int>> a(n + 1, vector<int>(m + 1));

	if (newCentroids.size() > objects.size())
	{
		for (int i = 1; i < n + 1; i++)
			for (int j = 1; j < m + 1; j++)
				a[i][j] = math::euclidian(objects[i - 1].location_, newCentroids[j - 1]);
	}
	else
	{
		for (int i = 1; i < n + 1; i++)
			for (int j = 1; j < m + 1; j++)
				a[i][j] = math::euclidian(objects[j - 1].location_, newCentroids[i - 1]);
	}

	vector<int> u(n + 1), v(m + 1), p(m + 1), way(m + 1);
	for (int i = 1; i <= n; ++i)
	{
		p[0] = i;
		int j0 = 0;
		vector<int> minv(m + 1, INF(int));
		vector<char> used(m + 1, false);

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

	vector<int> ans(n + 1);
	for (int j = 1; j <= m; ++j)
		ans[p[j]] = j;

	if (newCentroids.size() > objects.size())
	{
		for (int i = 1; i < n + 1; i++)
		{
			/// Presumably ans[i] never 0 
			if (!ans[i] == 0)
			{
				objects[i - 1].location_ = newCentroids[ans[i] - 1];
				newUsed[ans[i] - 1] = true;
			}
		}

		for (int i = 0; i < m; i++)
			if (!newUsed[i])
				objects.emplace_back(newCentroids[i]);
	}
	else
	{
		for (int i = 1; i < n + 1; i++)
		{
			objects[ans[i] - 1].location_ = newCentroids[i - 1];
			oldUsed[ans[i] - 1] = true;
		}

		objects.erase(remove_if(objects.begin(), objects.end(), [&objects, &oldUsed](const Object& value) {
			return !oldUsed[&value - &*objects.begin()];
		}), objects.end());
	}
}

int main()
{
	//auto backSub = createBackgroundSubtractorMOG2(500, 200, true);
	auto backSub = createBackgroundSubtractorKNN(500, 2500, true);

	VideoCapture cap("./fella.mp4");
	if (!cap.isOpened())
	{
		cout << "Couldn't open video" << endl;
		return 1;
	}

	int counter = 0;
	Mat frame, mask;
	vector<Object> objects;

	while (cap.isOpened())
	{
		cap >> frame;

		if (frame.empty())
			break;

		backSub->apply(frame, mask);

		/// Process
		{
			Mat canny_output;
			vector<Vec4i> hierarchy;
			vector<vector<Point>> contours;

			/// Detect edges using canny
			cv::Canny(mask, canny_output, 250, 900, 3);
			/// Find contours
			cv::findContours(canny_output, contours, hierarchy, RETR_TREE, 1, Point(0, 0));
			/// Leave only outer contours
			removeChildContours(hierarchy, contours);
			/// Exclude countours whose size is less than 100
			removeBySize(contours);			
			/// Init vector of nodes for DBSCAN
			vector<node*> v;
			unordered_map<int, node*> h;

			for (auto &el : contours)
			{
				for (Point &p : el)
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

			vector<vector<Point>> hull(contours.size());
			for (size_t i = 0; i < contours.size(); i++)
				convexHull(contours[i], hull[i]);

			vector<bool> toDelete(contours.size());
			for(int i = 0; i< contours.size(); i++)
				for (int j = 0; j < contours.size(); j++)
				{
					if (i == j)
						continue;

					if (pointPolygonTest(hull[i], hull[j].front(), false) == 1)
						toDelete[j] = true;
				}

			/// check if point inside
			contours.erase(remove_if(contours.begin(), contours.end(), [&contours, &toDelete](const vector<cv::Point>& value){
				return toDelete[&value - &*contours.begin()];
			}), contours.end());

			/// Sort so first contour is the largest
			std::sort(contours.begin(), contours.end(), [&contours](const vector<cv::Point>& a, const vector<cv::Point>& b) {
				return a.size() > b.size();
			});

			/// Update objects location or add new
			/// Using Hungarian algorithm
			updateObjects(objects, contours);

			Mat drawing = Mat::zeros(canny_output.size(), CV_8UC3);

			showContours(drawing, contours, hierarchy, objects);

			for (auto el : v)
				delete el;
		}

		Mat back;
		backSub->getBackgroundImage(back);

		processFrame(frame);

		imshow("Frame", frame);
		//imshow("Mask", mask);
		//imshow("Back", back);

		counter++;

		char c = (char)waitKey(25);
		if (c == 27)
			break;
	}

	cap.release();
	destroyAllWindows();

	return 0;
}