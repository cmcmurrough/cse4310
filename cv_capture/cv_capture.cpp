//
//    Copyright 2018 Christopher D. McMurrough
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
 * @file cv_capture.cpp
 * @brief C++ example for acquiring and processing image frames with OpenCV
 * @author Christopher D. McMurrough
 **********************************************************************************************************************/

// include necessary dependencies
#include <iostream>
#include <cstdio>
#include "opencv2/opencv.hpp"

// configuration parameters
#define NUM_COMNMAND_LINE_ARGUMENTS 1
#define DISPLAY_WINDOW_NAME "Camera Image"

// capture settings
#define CAMERA_FRAME_WIDTH 1024
#define CAMERA_FRAME_HEIGHT 768
#define CAMERA_FORMAT CV_8UC1
#define CAMERA_FPS 30
#define CAMERA_BRIGHTNESS 128
#define CAMERA_CONTRAST 10
#define CAMERA_SATURATION 0
#define CAMERA_HUE 0
#define CAMERA_GAIN 0
#define CAMERA_EXPOSURE -6
#define CAMERA_CONVERT_RGB false

// declare function prototypes
bool processFrame(const cv::Mat &imageIn, cv::Mat &imageOut);

/*******************************************************************************************************************//**
 * @brief Process a single image frame
 * @param[in] imageIn the input image frame
 * @param[out] imageOut the processed image frame
 * @return true if frame was processed successfully
 * @author Christopher D. McMurrough
 **********************************************************************************************************************/
bool processFrame(const cv::Mat &imageIn, cv::Mat &imageOut)
{
    // copy the input image frame to the ouput image (deep copy)
    imageOut = imageIn.clone();

    // copy the input image frame to the ouput image (shallow copy, if you just want to display)
    //imageOut = imageIn;

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
    int cameraIndex = 0;

    // validate and parse the command line arguments
    if(argc != NUM_COMNMAND_LINE_ARGUMENTS + 1)
    {
        std::printf("USAGE: %s <camera_index> \n", argv[0]);
        std::printf("WARNING: Proceeding with default execution parameters... \n");
        cameraIndex = 0;
    }
    else
    {
        cameraIndex = atoi(argv[1]);
    }

    // display the OpenCV vesion
    std::cout << "OpenCV version: " << CV_VERSION << std::endl;

    // initialize the camera capture
    cv::VideoCapture capture(cameraIndex);
    if(!capture.isOpened())
    {
        std::printf("Unable to open video source, terminating program! \n");
        return 0;
    }

    // set optional video capture parameters (WARNING: not all video sources support these)
    //capture.set(CV_CAP_PROP_FRAME_WIDTH, CAMERA_FRAME_WIDTH);
    //capture.set(CV_CAP_PROP_FRAME_HEIGHT, CAMERA_FRAME_HEIGHT);
    //capture.set(CV_CAP_PROP_FORMAT, CAMERA_FORMAT);
    //capture.set(CV_CAP_PROP_FPS, CAMERA_FPS);
    //capture.set(CV_CAP_PROP_BRIGHTNESS, CAMERA_BRIGHTNESS);
    //capture.set(CV_CAP_PROP_CONTRAST, CAMERA_CONTRAST);
    //capture.set(CV_CAP_PROP_SATURATION, CAMERA_SATURATION);
    //capture.set(CV_CAP_PROP_HUE, CAMERA_HUE);
    //capture.set(CV_CAP_PROP_GAIN, CAMERA_GAIN);
    //capture.set(CV_CAP_PROP_EXPOSURE, CAMERA_EXPOSURE);
    //capture.set(CV_CAP_PROP_CONVERT_RGB, CAMERA_CONVERT_RGB);

    // get the video source parameters
    int captureWidth = static_cast<int>(capture.get(CV_CAP_PROP_FRAME_WIDTH));
    int captureHeight = static_cast<int>(capture.get(CV_CAP_PROP_FRAME_HEIGHT));
    std::cout << "Video source opened successfully (width=" << captureWidth << " height=" << captureHeight << ")!" << std::endl;

    // create image window
    cv::namedWindow(DISPLAY_WINDOW_NAME, CV_WINDOW_AUTOSIZE);

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
            processFrame(captureFrame, processedFrame);

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
