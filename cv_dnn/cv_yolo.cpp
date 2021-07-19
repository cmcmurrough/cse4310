//
//    Copyright 2021 Christopher D. McMurrough
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

/*******************************************************************************************************************//**
 * @file cv_yolo.cpp
 * @brief C++ example for processing frames with Yolo using OpenCV DNN module
 * @author Christopher D. McMurrough
 **********************************************************************************************************************/

// include necessary dependencies
#include <iostream>
#include <cstdio>
#include "opencv2/opencv.hpp"
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

// configuration parameters
#define NUM_COMNMAND_LINE_ARGUMENTS 1
#define DISPLAY_WINDOW_NAME "Video Frame"

// declare function prototypes
bool processFrame(const cv::Mat &imageIn, cv::Mat &imageOut);

/*******************************************************************************************************************//**
 * @brief Process a single image frame
 * @param[in] imageIn the input image frame
 * @param[out] imageOut the processed image frame
 * @param[in] network input DNN network
 * @return true if frame was processed successfully
 * @author Christopher D. McMurrough
 **********************************************************************************************************************/
bool processFrame(const cv::Mat &imageIn, cv::Mat &imageOut, cv::dnn::Net &network)
{
    // copy the input image frame to the ouput image (deep copy)
    imageOut = imageIn.clone();











		static cv::Mat blobFromImg;
        bool swapRB = true;
        cv::dnn::blobFromImage(imageIn, blobFromImg, 1, cv::Size(416, 416), cv::Scalar(), swapRB, false);
        //std::cout << blobFromImg.size() << endl; 
        
        float scale = 1.0 / 255.0;
        cv::Scalar mean = 0;
        network.setInput(blobFromImg, "", scale, mean);

        cv::Mat outMat;
        network.forward(outMat);
            // rows represent number of detected object (proposed region)
            int rowsNoOfDetection = outMat.rows;

            // The coluns looks like this, The first is region center x, center y, width
            // height, The class 1 - N is the column entries, which gives you a number, 
            // where the biggist one corrsponding to most probable class. 
            // [x ; y ; w; h; class 1 ; class 2 ; class 3 ;  ; ;....]
            //  
            int colsCoordinatesPlusClassScore = outMat.cols;
            // Loop over number of detected object. 
            for (int j = 0; j < rowsNoOfDetection; ++j)
            {
                // for each row, the score is from element 5 up
                // to number of classes index (5 - N columns)
                cv::Mat scores = outMat.row(j).colRange(5, colsCoordinatesPlusClassScore);

                cv::Point PositionOfMax;
                double confidence;

                // This function find indexes of min and max confidence and related index of element. 
                // The actual index is match to the concrete class of the object.
                // First parameter is Mat which is row [5fth - END] scores,
                // Second parameter will gives you min value of the scores. NOT needed 
                // confidence gives you a max value of the scores. This is needed, 
                // Third parameter is index of minimal element in scores
                // the last is position of the maximum value.. This is the class!!
                cv::minMaxLoc(scores, 0, &confidence, 0, &PositionOfMax);
            
                if (confidence > 0.0001)
                {
// thease four lines are
// [x ; y ; w; h;
                    int centerX = (int)(outMat.at<float>(j, 0) * imageOut.cols); 
                    int centerY = (int)(outMat.at<float>(j, 1) * imageOut.rows); 
                    int width =   (int)(outMat.at<float>(j, 2) * imageOut.cols+20); 
                   int height =   (int)(outMat.at<float>(j, 3) * imageOut.rows+100); 

                    int left = centerX - width / 2;
                    int top = centerY - height / 2;


                    std::stringstream ss;
                    ss << PositionOfMax.x;
                    std::string clas = ss.str();
                    int color = PositionOfMax.x * 10;
                    cv::putText(imageOut, clas, cv::Point(left, top), 1, 2, cv::Scalar(color, 255, 255), 2, false);
                    std::stringstream ss2;
                    ss << confidence;
                    std::string conf = ss.str();

                    cv::rectangle(imageOut, cv::Rect(left, top, width, height), cv::Scalar(color, 0, 0), 2, 8, 0);
                }
            }
			
			
			
			
			
			
			

    // return true on success
    return true;
}








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
    std::string videoFileName;

    // validate and parse the command line arguments
    if(argc != NUM_COMNMAND_LINE_ARGUMENTS + 1)
    {
        std::printf("USAGE: %s <file_path> \n", argv[0]);
        return 0;
    }
    else
    {
        videoFileName = argv[1];
    }

    // open the video file
    cv::VideoCapture capture(videoFileName);
    if(!capture.isOpened())
    {
        std::printf("Unable to open video source, terminating program! \n");
        return 0;
    }
	
	// initialize YOLO
	std::string model = "yolov3-tiny.weights";
    std::string config = "yolov3-tiny.cfg"; 
    cv::dnn::Net network = cv::dnn::readNet(model, config,"Darknet");
    network.setPreferableBackend(cv::dnn::DNN_BACKEND_DEFAULT);
    network.setPreferableTarget(cv::dnn::DNN_TARGET_OPENCL);
	

    // get the video source parameters
    int captureWidth = static_cast<int>(capture.get(cv::CAP_PROP_FRAME_WIDTH));
    int captureHeight = static_cast<int>(capture.get(cv::CAP_PROP_FRAME_HEIGHT));
    int captureFPS = static_cast<int>(capture.get(cv::CAP_PROP_FPS));
    std::cout << "Video source opened successfully (width=" << captureWidth << " height=" << captureHeight << " fps=" << captureFPS << ")!" << std::endl;

    // create image window
    cv::namedWindow(DISPLAY_WINDOW_NAME, cv::WINDOW_AUTOSIZE);

    // process data until program termination
    bool doCapture = true;
    int frameCount = 0;
    while(doCapture)
    {
        // get the start time
        double startTicks = static_cast<double>(cv::getTickCount());

        // attempt to acquire and process an image frame
        cv::Mat captureFrame;
        cv::Mat processedFrame;
        bool captureSuccess = capture.read(captureFrame);
        if(captureSuccess)
        {
            // process the image frame
            processFrame(captureFrame, processedFrame, network);

            // increment the frame counter
            frameCount++;
        }
        else
        {
            std::printf("Unable to acquire image frame! \n");
        }

        // update the GUI window if necessary
        if(captureSuccess)
        {
            cv::imshow(DISPLAY_WINDOW_NAME, processedFrame);

            // get the number of milliseconds per frame
            int delayMs = (1.0 / captureFPS) * 1000;

            // check for program termination
            if(((char) cv::waitKey(1)) == 'q')
            {
                doCapture = false;
            }
        }

        // compute the frame processing time
        double endTicks = static_cast<double>(cv::getTickCount());
        double elapsedTime = (endTicks - startTicks) / cv::getTickFrequency();
        std::cout << "Frame processing time: " << elapsedTime << std::endl;
    }

    // release program resources before returning
    capture.release();
    cv::destroyAllWindows();
}
