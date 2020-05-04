// rpoject includes
#include "tools.h"
#include "DBSCAN.h"
#include "objecttracker.h"
#include "contourextractor.h"

// opencv includes
#include <opencv2/video.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/core.hpp>

using namespace cv;
using namespace std;

RNG rng(12345);

inline void processFrame(Mat& frame)
{
	cv::line(frame, Point(frame.cols / 2, 0), Point(frame.cols / 2, frame.rows), Scalar(0, 255, 0), 2);
}

inline void showContours(Mat& output, const vector<vector<Point>>& contours, const vector<Vec4i>& hierarchy, const vector<Object>& objects, string name = "Contours")
{
	for (int i = 0; i < contours.size(); i++)
		drawContours(output, contours, i, objects[i].color_, 2, 8, noArray(), 0, Point());
}

inline void boundingBox(Mat& output, const vector<Object>& objects, const vector<vector<Point>>& contours)
{
	for (int i = 0; i < objects.size(); i++)
	{
		Point minCorner = { output.cols, output.rows };
		Point maxCorner = { 0, 0 };

		for (int j = 0; j < contours[i].size(); j++)
		{
			minCorner.x = std::min(minCorner.x, contours[i][j].x);
			minCorner.y = std::min(minCorner.y, contours[i][j].y);

			maxCorner.x = std::max(maxCorner.x, contours[i][j].x);
			maxCorner.y = std::max(maxCorner.y, contours[i][j].y);
		}

		Rect rect(minCorner, maxCorner);
		cv::rectangle(output, rect, { 255, 255, 255 });
		cv::putText(output, std::to_string(objects[i].id()), objects[i].location_, FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
	}
}

inline void display(Mat& output, const vector<vector<Point>>& contours, const vector<Vec4i>& hierarchy, const vector<Object>& objects, string name = "Contours")
{
	showContours(output, contours, hierarchy, objects, name);
	boundingBox(output, objects, contours);

	imshow(name, output);
}

int main()
{
	auto backSub = createBackgroundSubtractorKNN(500, 2500, true);

	VideoCapture cap("./doorway.mp4");
	if (!cap.isOpened())
	{
		cout << "Couldn't open video" << endl;
		return 1;
	}

	int counter = 0;
	Mat frame, mask;

	ObjectTracker tracker;

	while (cap.isOpened())
	{
		cap >> frame;

		if (frame.empty())
			break;

		backSub->apply(frame, mask);

		/// Process
		{
			ContourExtractor extractor(mask);
			auto contours = extractor.extractContours();

			tracker.update(contours);

			//Mat drawing = Mat::zeros(frame.size(), CV_8UC3);

			//display(drawing, contours, hierarchy, objects);
		}

		Mat back;
		backSub->getBackgroundImage(back);

		processFrame(frame);

		imshow("Frame", frame);

		counter++;

		char c = (char)waitKey(25);
		if (c == 27)
			break;
	}

	cap.release();
	destroyAllWindows();

	return 0;
}