// rpoject includes
#include "DBSCAN.h"
#include "Object.h"
#include "dbscanbyimage.h"
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

inline void updateObjects(vector<Object>& objects, const vector<vector<Point>>& contours)
{
	auto newCentroids = getCentroids(contours);
	vector<bool> used(newCentroids.size());
	int removeAfter = -1;

	for (auto &el : objects)
	{
		Point closest;
		int ind = -1;
		double dist = numeric_limits<double>::max();

		for (int j = 0; j < newCentroids.size(); j++)
		{
			double tmp = math::euclidian(newCentroids[j], el.location_);
			if (!used[j] && tmp < dist)
			{
				closest = el.location_;
				dist = tmp;
				ind = j;
			}
		}

		/// Then we shall remove that point
		/// There is no new point shall remove old ones
		if (ind == -1)
		{
			removeAfter = &el - &*objects.begin();
			break;
		}

		/// Mark as used
		used[ind] = true;
		/// Update location location
		el.location_ = newCentroids[ind];
	}

	objects.erase(remove_if(objects.begin(), objects.end(), [&removeAfter, &objects](const Object& value) {
		if (removeAfter == -1)
			return false;

		return (&value - &*objects.begin()) >= removeAfter;
	}), objects.end());

	/// Add new objects
	for (int i = 0; i < used.size(); i++)
		if (!used[i])
			objects.emplace_back(newCentroids[i]);
}

int main()
{
	//auto backSub = createBackgroundSubtractorMOG2(500, 200, true);
	auto backSub = createBackgroundSubtractorKNN(500, 2500/*2500*/, true);

	VideoCapture cap("./doorWay.mp4");
	//VideoCapture cap("./fella.mp4");
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

		{
			auto start = high_resolution_clock::now();
			backSub->apply(frame, mask);

			auto stop = high_resolution_clock::now();
			auto duration = duration_cast<microseconds>(stop - start);

			cout << "Apply time: "
				<< duration.count() << " microseconds" << endl;
		}

		/// Process
		{
			auto start = high_resolution_clock::now();

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


			for (auto it = h.begin(); it != h.end(); it++)
				v.push_back(it->second);
			//for (auto &el : contours)
			//{
			//	for (Point &p : el)
			//	{
			//		node* u = new node;
			//		u->c = p;

			//		v.push_back(u);
			//	}
			//}
			/// Get result of dbscam
			DBSCANByImage db(canny_output, v);
			//DBSCAN db(v);
			db.perform();

			contours.clear();
			contours.resize(db.numberOfClusters());

			/// Fill contours from clusters
			for (int i = 0; i < v.size(); i++)
			{
				contours[v[i]->cluster].emplace_back(v[i]->c);
				//for (int j = 0; j < v.size(); j++)
				//	if (math::euclidian(v[i]->c, v[j]->c) < 10 && v[i]->cluster != v[j]->cluster)
				//		cout << "lol" << endl;
			}

			/// Sort so first contour is the largest
			std::sort(contours.begin(), contours.end(), [&contours](const vector<cv::Point>& a, const vector<cv::Point>& b) {
				return a.size() > b.size();
			});

			/// Update objects location or add new
			updateObjects(objects, contours);

			auto stop = high_resolution_clock::now();
			auto duration = duration_cast<microseconds>(stop - start);

			cout << "Process time: "
				<< duration.count() << " microseconds" << endl;

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