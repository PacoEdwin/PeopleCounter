// project includes
#include "tools.h"
#include "DBSCAN.h"
#include "displayer.h"
#include "frameprocessor.h"

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

int main()
{
	VideoCapture cap("./PeopleCounter/doorWay.mp4");
	if (!cap.isOpened())
	{
		cout << "Couldn't open video" << endl;
		return 1;
	}

	int counter = 0;
	Mat frame;

	FrameProcessor* processor = new FrameProcessor();
	Displayer displayer;
	displayer.setFrameProcessor(processor);

	while (cap.isOpened())
	{
		cap >> frame;

		if (frame.empty())
			break;

		processor->process(frame);

		/// display
		{
			// Maybe nees init function
			Mat drawing = Mat::zeros(frame.size(), CV_8UC3);
			displayer.setImage(drawing);
			displayer.drawContours();
			displayer.drawBoundingBox();

			displayer.display();
		}

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