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
 * @file cv_load_video.cpp
 * @brief C++ example for acquiring and processing image frames with OpenCV
 * @author Christopher D. McMurrough
 **********************************************************************************************************************/

// include necessary dependencies
#include <iostream>
#include <cstdio>
#include "opencv2/opencv.hpp"

// configuration parameters
#define NUM_COMNMAND_LINE_ARGUMENTS 1
#define DISPLAY_WINDOW_NAME "Video Frame"

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
    std::string fileName;

    // validate and parse the command line arguments
    if(argc != NUM_COMNMAND_LINE_ARGUMENTS + 1)
    {
        std::printf("USAGE: %s <file_path> \n", argv[0]);
        return 0;
    }
    else
    {
        fileName = argv[1];
    }

    // open the video file
    cv::VideoCapture capture(fileName);
    if(!capture.isOpened())
    {
        std::printf("Unable to open video source, terminating program! \n");
        return 0;
    }

    // get the video source parameters
    int captureWidth = static_cast<int>(capture.get(CV_CAP_PROP_FRAME_WIDTH));
    int captureHeight = static_cast<int>(capture.get(CV_CAP_PROP_FRAME_HEIGHT));
    int captureFPS = static_cast<int>(capture.get(CV_CAP_PROP_FPS));
    std::cout << "Video source opened successfully (width=" << captureWidth << " height=" << captureHeight << " fps=" << captureFPS << ")!" << std::endl;

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
