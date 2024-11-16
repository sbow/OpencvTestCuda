#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/videoio/registry.hpp>


int main() {

	cv::cuda::setDevice(0);

// Note: by specifying to capture we speed up the loading dramatically; avoids searching for compatible backends
// ...which takes a long time because it hits a bazillion dll's
#ifdef _WIN32
	cv::VideoCapture cap(0, cv::CAP_DSHOW);
#else
	cv::VideoCapture cap(0, cv::CAP_V4L2);
#endif

	//cv::VideoCapture cap(0);
	cv::Mat frame;
	cv::cuda::GpuMat Gframe, grayFrame, edgeFrame;

	if (!cap.isOpened())
	{
		std::cout << "No camera exist\n";
		return -1;
	}

	// Create a CUDA Cany edge detector
	double low_thresh = 50.0;
	double high_thersh = 150.0;
	int aperature_size = 3;
	bool L2gradient = false;
	cv::Ptr<cv::cuda::CannyEdgeDetector> canny = cv::cuda::createCannyEdgeDetector(low_thresh, high_thersh, aperature_size, L2gradient);

	while (1) {
		cap >> frame;
		if (frame.empty()) {
			std::cout << "Empty frame\n";
			continue;
		}
		// Copy from ram to GPU memory
		Gframe.upload(frame);
		if (Gframe.empty()) {
			std::cout << "Failed to upload to GPU\n";
			continue;
		}
		// Convert to gray and detect edges using CUDA in GPU
		try {
			cv::cuda::cvtColor(Gframe, grayFrame, cv::COLOR_BGR2GRAY);
			canny->detect(grayFrame, edgeFrame);
		}
		catch (const cv::Exception e) {
			std::cerr << "CUDA error: " << e.what() << std::endl;
			break;
		}
		// Copy from GPU memory to normal RAM
		edgeFrame.download(frame);
		cv::imshow("Edge detected, gray webcam", frame);
		if (cv::waitKey(1) == 27) { // Press esc
			std::cout << "End camera loop\n";
			return 1;
		}
	}
	return 0;
}