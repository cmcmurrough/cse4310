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
 * @file cv_clickable.cpp
 * @brief C++ example of basic interaction with an image in OpenCV
 * @author Christopher D. McMurrough
 **********************************************************************************************************************/

// include necessary dependencies
#include <iostream>
#include <string>
#include "opencv2/opencv.hpp"

// function prototypes
static void clickCallback(int event, int x, int y, int flags, void* param);

/*******************************************************************************************************************//**
 * @brief handler for image click callbacks
 * @param[in] event mouse event type
 * @param[in] x x coordinate of event
 * @param[in] y y coordinate of event
 * @param[in] flags additional event flags 
 * @param[in] param user data passed as void* (in this case, the input image)
 * @author Christoper D. McMurrough
 **********************************************************************************************************************/
static void clickCallback(int event, int x, int y, int flags, void* param)
{
    // cast userdata to a cv::Mat
    cv::Mat imageIn = *(cv::Mat *)param;
    
    // handle the mouse event types
    if(event == cv::EVENT_LBUTTONDOWN)
    {
        std::cout << "LEFT CLICK (" << x << ", " << y << ")" << std::endl;
        
        // get the color value at the clicked pixel location and print to console
        cv::Vec3b pixel = imageIn.at<cv::Vec3b>(y, x);
        std::cout << pixel << std::endl;
    }
    else if(event == cv::EVENT_RBUTTONDOWN)
    {
        std::cout << "RIGHT CLICK (" << x << ", " << y << ")" << std::endl;
    }
    else if(event == cv::EVENT_MBUTTONDOWN)
    {
        std::cout << "MIDDLE CLICK (" << x << ", " << y << ")" << std::endl;
    }
    else if(event == cv::EVENT_MOUSEMOVE)
    {
        std::cout << "MOUSE OVER (" << x << ", " << y << ")" << std::endl;
    }
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
    // open the input image
    std::string inputFileName = "test.png";
    cv::Mat imageIn;
    imageIn = cv::imread(inputFileName, cv::IMREAD_COLOR);

    // check for file error
    if(!imageIn.data)
    {
        std::cout << "Error while opening file " << inputFileName << std::endl;
        return 0;
    }
    
    // display the input image
    cv::imshow("imageIn", imageIn);
    cv::setMouseCallback("imageIn", clickCallback, &imageIn);
    cv::waitKey();
}
