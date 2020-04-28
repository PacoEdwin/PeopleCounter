// rpoject includes
#include "DBSCAN.h"
#include "Object.h"

// opencv includes
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/opencv.hpp>

// std incldes
#include <iostream>

using namespace cv;
using namespace std;

RNG rng(12345);

void processFrame(Mat& frame)
{
	cv::line(frame, Point(frame.cols / 2, 0), Point(frame.cols / 2, frame.rows), Scalar(0, 255, 0), 2);
}

void showContours(Mat& output, const  vector<vector<Point>>& contours, const vector<Vec4i>& hierarchy, const vector<Object>& objects, string name = "Contours")
{
	for (int i = 0; i < contours.size(); i++)
		drawContours(output, contours, i, objects[i].color_, 2, 8, hierarchy, 0, Point());

	imshow(name, output);
}

vector<Point> getCentroids(const vector<vector<Point>>& contours)
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

namespace
{
	Mat src, src_gray;
	Mat dst, detected_edges;
	int lowThreshold = 0;
	const int max_lowThreshold = 100;
	const int ratio = 3;
	const int kernel_size = 3;
	const char* window_name = "Edge Map";

	static void CannyThreshold(int, void*)
	{
		blur(src_gray, detected_edges, Size(3, 3));
		cout << lowThreshold << endl;
		Canny(detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size);
		dst = Scalar::all(0);
		src.copyTo(dst, detected_edges);
		imshow(window_name, dst);
	}
}

int main()
{
	//auto backSub = createBackgroundSubtractorMOG2(500, 200, true);

	auto backSub = createBackgroundSubtractorKNN(500, 2500/*2500*/, true);

	//{
	//	VideoCapture cap("./doorWay.mp4");
	//	//VideoCapture cap("./fella.mp4");
	//	if (!cap.isOpened())
	//	{
	//		cout << "Couldn't open video";
	//		return 1;
	//	}

	//	int counter = 0;
	//	Mat frame;
	//	while (counter != 530)
	//	{
	//		counter++;
	//		cap >> frame;
	//	}

	//	cap.release();

	//	Mat mask;
	//	for (int i = 0; i < 500; i++)
	//		backSub->apply(frame, mask);
	//	
	//}

	VideoCapture cap("./doorWay.mp4");
	//VideoCapture cap("./fella.mp4");
	if (!cap.isOpened())
	{
		cout << "Couldn't open video";
		return 1;
	}

	// gets first frame to omparison
	{
		Mat frame0;
		cap >> frame0;
		//imwrite("./framme0.jpg", frame0);
	}

	vector<Object> objects;

	//auto backSub = createBackgroundSubtractorKNN(500, 3200/*2500*/, true);

	int counter = 0;
	while (cap.isOpened())
	{
		Mat frame, mask;
		cap >> frame;

		if (frame.empty())
			break;

		counter++;

		//cout << counter << endl;

		backSub->apply(frame, mask);

		// display
		{
			Mat canny_output;
			vector<vector<Point>> contours;
			vector<vector<Point>> parentContours;
			vector<Vec4i> hierarchy;

			/// Detect edges using canny
			cv::Canny(mask, canny_output, 250, 900, 3);
			//cv::Canny(mask, canny_output, 300, 980, 3);
			/// Find contours
			cv::findContours(canny_output, contours, hierarchy, RETR_TREE, 1, Point(0, 0));

			Mat drawing = Mat::zeros(canny_output.size(), CV_8UC3);

			vector<bool> displayed(hierarchy.size());

			/// Remove all child contours
			for (int i = 0; i < hierarchy.size(); i++)
			{
				int current = hierarchy[i][3];

				// shall display and mark as displayed
				if (current == -1)
				{
					parentContours.push_back(contours[i]);
					displayed[i] = true;
					continue;
				}

				while (hierarchy[current][3] != -1)
					current = hierarchy[current][3];

				if (!displayed[current])
				{
					parentContours.push_back(contours[current]);
					displayed[current] = true;
				}
			}

			/// Exclude countours whose size is less than 100
			parentContours.erase(std::remove_if(parentContours.begin(), parentContours.end(), [](const vector<Point>& el) ->bool {
				/// By number of pixels
				// return el.size() < 75;
				
				Point centroid;
				for (auto p : el)
					centroid += p;

				centroid.x = centroid.x / el.size();
				centroid.y = centroid.y / el.size();

				for (auto p : el)
					if (math::euclidian(centroid, p) > 5)
						return false;

				return true;
			}), parentContours.end());

			/// Init vector of nodes for DBSCAN
			vector<node*> v;
			for (auto &el : parentContours)
			{
				for (Point &p : el)
				{
					node* u = new node;
					u->c.push_back(p.x);
					u->c.push_back(p.y);

					v.push_back(u);
				}
			}

			DBSCAN db(v);
			/// Get result of dbscam
			db.perform();

			contours.clear();
			contours.resize(db.numberOfClusters());

			/// Fill contours from clusters
			for (int i = 0; i < v.size(); i++)
				contours[v[i]->cluster].emplace_back(v[i]->c[0], v[i]->c[1]);

			//contours.erase(std::remove_if(contours.begin(), contours.end(), [](const vector<Point>& el) -> bool {
			//	return el.size() < 100;
			//}), contours.end());

			std::sort(contours.begin(), contours.end(), [&contours](const vector<cv::Point>& a, const vector<cv::Point>& b) {
				return a.size() > b.size();
			});

			auto new_centroids = getCentroids(contours);
			vector<bool> used(new_centroids.size());
			int removeAfter = -1;

			for (auto &el: objects)
			{				
				Point closest(-1, -1);
				double dist = 1000000;
				int ind = -1;

				for (int j = 0; j < new_centroids.size(); j++)
				{
					double tmp = math::euclidian(new_centroids[j], el.location_);
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
					cout << removeAfter << endl;
					break;
				}

				/// Mark as used
				used[ind] = true;
				/// Update location location
				el.location_ = new_centroids[ind];
			}

			objects.erase(remove_if(objects.begin(), objects.end(), [&removeAfter, &objects](const Object& value) {
				if (removeAfter == -1)
					return false;

				return (&value - &*objects.begin()) >= removeAfter;
			}), objects.end());

			/// Add new objects
			for (int i = 0; i < used.size(); i++)
				if (!used[i])
					objects.emplace_back(new_centroids[i]);

			showContours(drawing, contours, hierarchy, objects);
			//showContours(drawing, parentContours, hierarchy);

			for (auto el : v)
				delete el;
		}

		Mat back;
		backSub->getBackgroundImage(back);

		processFrame(frame);

		imshow("Frame", frame);
		//imshow("Mask", mask);
		//imshow("Back", back);

		char c = (char)waitKey(25);
		if (c == 27)
			break;
	}

	cap.release();
	destroyAllWindows();

	return 0;
}