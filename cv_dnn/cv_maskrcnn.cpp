
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdio>
#include <string.h>

#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>


using namespace cv;
using namespace dnn;
using namespace std;

// Initialize the parameters
float confThreshold = 0.5; // Confidence threshold
float maskThreshold = 0.3; // Mask threshold

vector<string> classes;
vector<Scalar> colors;

// Draw the predicted bounding box
void drawBox(Mat& frame, int classId, float conf, Rect box, Mat& objectMask);

// Postprocess the neural network's output for each frame
void postprocess(Mat& frame, const vector<Mat>& outs);



// configuration parameters
#define NUM_COMNMAND_LINE_ARGUMENTS 1
#define DISPLAY_WINDOW_NAME "Video Frame"

/*******************************************************************************************************************//**
 * @brief program entry point
 * @param[in] argc number of command line arguments
 * @param[in] argv string array of command line arguments
 * @return return code (0 for normal termination)
 * @author Christoper D. McMurrough
 **********************************************************************************************************************/
int main(int argc, char **argv)
{
    // store video capture parameters
    std::string fileName;
    int trackerSelection = 0;

    // validate and parse the command line arguments
    if(argc != NUM_COMNMAND_LINE_ARGUMENTS + 1)
    {
        std::printf("USAGE: %s <file_path> <tracker_type> \n", argv[0]);
        return 0;
    }
    else
    {
        fileName = argv[1];
        trackerSelection = std::atoi(argv[2]);
    }

    // open the video file
    cv::VideoCapture capture(fileName);
    if(!capture.isOpened())
    {
        std::printf("Unable to open video source, terminating program! \n");
        return 0;
    }

    // get the video source parameters
    int captureWidth = static_cast<int>(capture.get(cv::CAP_PROP_FRAME_WIDTH));
    int captureHeight = static_cast<int>(capture.get(cv::CAP_PROP_FRAME_HEIGHT));
    int captureFPS = static_cast<int>(capture.get(cv::CAP_PROP_FPS));
    std::cout << "Video source opened successfully (width=" << captureWidth << " height=" << captureHeight << " fps=" << captureFPS << ")!" << std::endl;

    // create image window
    cv::namedWindow(DISPLAY_WINDOW_NAME, cv::WINDOW_AUTOSIZE);



	// Load names of classes
	string classesFile = "./mask_rcnn_inception_v2_coco_2018_01_28/mscoco_labels.names";
	ifstream ifs(classesFile.c_str());
	string line;
	while (getline(ifs, line)) classes.push_back(line);

	// Load the colors
	string colorsFile = "./mask_rcnn_inception_v2_coco_2018_01_28/colors.txt";
	ifstream colorFptr(colorsFile.c_str());
	while (getline(colorFptr, line)) 
	{
		char* pEnd;
		double r, g, b;
		r = strtod(line.c_str(), &pEnd);
		g = strtod(pEnd, NULL);
		b = strtod(pEnd, NULL);
		Scalar color = Scalar(r, g, b, 255.0);
		colors.push_back(Scalar(r, g, b, 255.0));
	}

	// Give the configuration and weight files for the model
	String textGraph = "./mask_rcnn_inception_v2_coco_2018_01_28.pbtxt";
	String modelWeights = "./mask_rcnn_inception_v2_coco_2018_01_28/frozen_inference_graph.pb";

	// Load the network
	Net net = readNetFromTensorflow(modelWeights, textGraph);
	net.setPreferableBackend(DNN_BACKEND_OPENCV);
	net.setPreferableTarget(DNN_TARGET_CPU);

	// Open a video file or an image file or a camera stream.
	string str, outputFile;
	//VideoCapture capture(0);//Depending on the camera port id, you can modify it.
	//VideoWriter video;
	Mat frame, blob;

	// Create a window
	static const string kWinName = "Deep learning object detection in OpenCV";
	namedWindow(kWinName, WINDOW_NORMAL);

	// Process frames.
	while (waitKey(1) < 0)
	{
		// get frame from the video
		capture >> frame;

		// Stop the program if reached end of video
		if (frame.empty()) 
		{
			cout << "Done processing !!!" << endl;
			cout << "Output file is stored as " << outputFile << endl;
			waitKey(3000);
			break;
		}
		// Create a 4D blob from a frame.
		blobFromImage(frame, blob, 1.0, Size(frame.cols, frame.rows), Scalar(), true, false);
		//blobFromImage(frame, blob);

		//Sets the input to the network
		net.setInput(blob);

		// Runs the forward pass to get output from the output layers
		std::vector<String> outNames(2);
		outNames[0] = "detection_out_final";
		outNames[1] = "detection_masks";
		vector<Mat> outs;
		net.forward(outs, outNames);

		// Extract the bounding box and mask for each of the detected objects
		postprocess(frame, outs);

		// Put efficiency information. The function getPerfProfile returns the overall time for inference(t) and the timings for each of the layers(in layersTimes)
		vector<double> layersTimes;
		double freq = getTickFrequency() / 1000;
		double t = net.getPerfProfile(layersTimes) / freq;
		string label = format("Mask-RCNN on 2.5 GHz Intel Core i7 CPU, Inference time for a frame : %0.0f ms", t);
		putText(frame, label, Point(0, 15), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0));

		// Write the frame with the detection boxes
		Mat detectedFrame;
		frame.convertTo(detectedFrame, CV_8U);

		imshow(kWinName, frame);

	}
	capture.release();
	return 0;
}

// For each frame, extract the bounding box and mask for each detected object
void postprocess(Mat& frame, const vector<Mat>& outs)
{
	Mat outDetections = outs[0];
	Mat outMasks = outs[1];

	// Output size of masks is NxCxHxW where
	// N - number of detected boxes
	// C - number of classes (excluding background)
	// HxW - segmentation shape
	const int numDetections = outDetections.size[2];
	const int numClasses = outMasks.size[1];

	outDetections = outDetections.reshape(1, outDetections.total() / 7);
	for (int i = 0; i < numDetections; ++i)
	{
		float score = outDetections.at<float>(i, 2);
		if (score > confThreshold)
		{
			// Extract the bounding box
			int classId = static_cast<int>(outDetections.at<float>(i, 1));
			int left = static_cast<int>(frame.cols * outDetections.at<float>(i, 3));
			int top = static_cast<int>(frame.rows * outDetections.at<float>(i, 4));
			int right = static_cast<int>(frame.cols * outDetections.at<float>(i, 5));
			int bottom = static_cast<int>(frame.rows * outDetections.at<float>(i, 6));

			left = max(0, min(left, frame.cols - 1));
			top = max(0, min(top, frame.rows - 1));
			right = max(0, min(right, frame.cols - 1));
			bottom = max(0, min(bottom, frame.rows - 1));
			Rect box = Rect(left, top, right - left + 1, bottom - top + 1);

			// Extract the mask for the object
			Mat objectMask(outMasks.size[2], outMasks.size[3], CV_32F, outMasks.ptr<float>(i, classId));

			// Draw bounding box, colorize and show the mask on the image
			drawBox(frame, classId, score, box, objectMask);

		}
	}
}

// Draw the predicted bounding box, colorize and show the mask on the image
void drawBox(Mat& frame, int classId, float conf, Rect box, Mat& objectMask)
{
	//Draw a rectangle displaying the bounding box
	rectangle(frame, Point(box.x, box.y), Point(box.x + box.width, box.y + box.height), Scalar(255, 178, 50), 3);

	//Get the label for the class name and its confidence
	string label = format("%.2f", conf);
	if (!classes.empty())
	{
		CV_Assert(classId < (int)classes.size());
		label = classes[classId] + ":" + label;
	}

	//Display the label at the top of the bounding box
	int baseLine;
	Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
	box.y = max(box.y, labelSize.height);
	rectangle(frame, Point(box.x, box.y - round(1.5*labelSize.height)), Point(box.x + round(1.5*labelSize.width), box.y + baseLine), Scalar(255, 255, 255), FILLED);
	putText(frame, label, Point(box.x, box.y), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 0, 0), 1);

	Scalar color = colors[classId%colors.size()];

	// Resize the mask, threshold, color and apply it on the image
	resize(objectMask, objectMask, Size(box.width, box.height));
	Mat mask = (objectMask > maskThreshold);
	Mat coloredRoi = (0.3 * color + 0.7 * frame(box));
	coloredRoi.convertTo(coloredRoi, CV_8UC3);

	// Draw the contours on the image
	vector<Mat> contours;
	Mat hierarchy;
	mask.convertTo(mask, CV_8U);
	findContours(mask, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);
	drawContours(coloredRoi, contours, -1, color, 5, LINE_8, hierarchy, 100);
	coloredRoi.copyTo(frame(box), mask);

}

