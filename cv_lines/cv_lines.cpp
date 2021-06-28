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
 * @file cv_lines.cpp
 * @brief C++ example of Canny edge detection and Hough line fitting using OpenCV
 * @author Christopher D. McMurrough
 **********************************************************************************************************************/

// include necessary dependencies
#include <iostream>
#include <string>
#include "opencv2/opencv.hpp"

// configuration parameters
#define NUM_COMNMAND_LINE_ARGUMENTS 1

/*******************************************************************************************************************//**
 * @brief program entry point
 * @param[in] argc number of command line arguments
 * @param[in] argv string array of command line arguments
 * @return return code (0 for normal termination)
 * @author Christoper D. McMurrough
 **********************************************************************************************************************/
int main(int argc, char **argv)
{
    cv::Mat imageIn;

    // validate and parse the command line arguments
    if(argc != NUM_COMNMAND_LINE_ARGUMENTS + 1)
    {
        std::printf("USAGE: %s <image_path> \n", argv[0]);
        return 0;
    }
    else
    {
        imageIn = cv::imread(argv[1], cv::IMREAD_COLOR);

        // check for file error
        if(!imageIn.data)
        {
            std::cout << "Error while opening file " << argv[1] << std::endl;
            return 0;
        }
    }

	// get the image size
	std::cout << "image width: " << imageIn.size().width << std::endl;
	std::cout << "image height: " << imageIn.size().height << std::endl;
	std::cout << "image channels: " << imageIn.channels() << std::endl;

    // convert the image to grayscale
    cv::Mat imageGray;
    cv::cvtColor(imageIn, imageGray, cv::COLOR_BGR2GRAY);

    // find the image edges
	cv::Mat imageEdges;
	const double cannyThreshold1 = 100;
	const double cannyThreshold2 = 200;
	const int cannyAperture = 3;
	cv::Canny(imageGray, imageEdges, cannyThreshold1, cannyThreshold2, cannyAperture);

    // find the image lines
    std::vector<cv::Vec4i> lines;
    const double rho = 1;               // resolution of rho parameter in pixels
    const double theta = CV_PI / 180;   // resolution of theta parameter in radians
    const double houghThreshold = 50;   // minimum number of intersections to detect a line
    const double minLineLength = 200;    // minimum number of points on a line
    const double maxLineGap = 40;       // maximum gap between points on a line
    cv::HoughLinesP(imageEdges, lines, rho, theta, houghThreshold, minLineLength, maxLineGap);
    for( size_t i = 0; i < lines.size(); i++ )
    {
        cv::Vec4i l = lines[i];
        cv::line(imageIn, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0, 0, 255), 3, cv::LINE_AA);
    }

    // display the images
	cv::imshow("imageIn", imageIn);
	cv::imshow("imageGray", imageGray);
	cv::imshow("imageEdges", imageEdges);
	cv::waitKey();
}
