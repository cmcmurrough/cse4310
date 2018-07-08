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
 * @file cv_tracking.cpp
 * @brief C++ example for selecting and tracking motion with OpenCV
 * @author Christopher D. McMurrough
 **********************************************************************************************************************/

// include necessary dependencies
#include <iostream>
#include <cstdio>
#include "opencv2/opencv.hpp"

#include <opencv2/tracking.hpp>
#include <opencv2/core/ocl.hpp>

// configuration parameters
#define NUM_COMNMAND_LINE_ARGUMENTS 2
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
    int captureWidth = static_cast<int>(capture.get(CV_CAP_PROP_FRAME_WIDTH));
    int captureHeight = static_cast<int>(capture.get(CV_CAP_PROP_FRAME_HEIGHT));
    int captureFPS = static_cast<int>(capture.get(CV_CAP_PROP_FPS));
    std::cout << "Video source opened successfully (width=" << captureWidth << " height=" << captureHeight << " fps=" << captureFPS << ")!" << std::endl;

    // create image window
    cv::namedWindow(DISPLAY_WINDOW_NAME, CV_WINDOW_AUTOSIZE);

    // create the tracker object
    std::string trackerTypes[7] = {"BOOSTING", "MIL", "KCF", "TLD","MEDIANFLOW", "GOTURN", "MOSSE"};
    std::string trackerType = trackerTypes[trackerSelection];
    cv::Ptr<cv::Tracker> tracker;
    #if (CV_MINOR_VERSION < 3)
    {
        tracker = Tracker::create(trackerType);
    }
    #else
    {
        if (trackerType == "BOOSTING")
            tracker = cv::TrackerBoosting::create();
        if (trackerType == "MIL")
            tracker = cv::TrackerMIL::create();
        if (trackerType == "KCF")
            tracker = cv::TrackerKCF::create();
        if (trackerType == "TLD")
            tracker = cv::TrackerTLD::create();
        if (trackerType == "MEDIANFLOW")
            tracker = cv::TrackerMedianFlow::create();
        if (trackerType == "GOTURN")
            tracker = cv::TrackerGOTURN::create();
        //if (trackerType == "MOSSE")
            //tracker = cv::TrackerMOSSE::create();
    }
    #endif
    cv::Rect2d roi;

    // process data until program termination
    bool doCapture = true;
    int frameCount = 0;
    while(doCapture)
    {
        // get the start time
        double startTicks = static_cast<double>(cv::getTickCount());

        // attempt to acquire and process an image frame
        cv::Mat captureFrame;
        bool captureSuccess = capture.read(captureFrame);
        bool trackerSuccess = false;
        if(captureSuccess)
        {
            // check for first frame
            if(frameCount == 0)
            {
                // obtain the initial ROI and initialize tracker
                roi = cv::selectROI(DISPLAY_WINDOW_NAME, captureFrame, true, false);
                tracker->init(captureFrame, roi);
                trackerSuccess = true;
            }
            else
            {
                // perform one iteration of tracking
                trackerSuccess = tracker->update(captureFrame, roi);
            }

            // increment the frame counter
            frameCount++;
        }
        else
        {
            std::printf("Unable to acquire image frame! \n");
            continue;
        }

        // update the GUI window if necessary
        if(captureSuccess)
        {
            // display the frame
            if(trackerSuccess)
            {
                cv::rectangle(captureFrame, roi, cv::Scalar( 255, 0, 0 ), 2, 1);
            }
            cv::imshow(DISPLAY_WINDOW_NAME, captureFrame);

            // get the number of milliseconds per frame
            int delayMs = (1.0 / captureFPS) * 1000;

            // check for program termination
            if(((char) cv::waitKey(delayMs)) == 'q')
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
