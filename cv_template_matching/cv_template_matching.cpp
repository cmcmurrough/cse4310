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
 * @file cv_template_matching.cpp
 * @brief C++ example for template matching with OpenCV
 * @author Christopher D. McMurrough
 **********************************************************************************************************************/

// include necessary dependencies
#include <stdio.h>
#include <iostream>
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc_c.h"

// configuration parameters
#define NUM_COMNMAND_LINE_ARGUMENTS 2

/*******************************************************************************************************************//**
 * @brief program entry point
 * @param[in] argc number of command line arguments
 * @param[in] argv string array of command line arguments
 * @return return code (0 for normal termination)
 * @author Christoper D. McMurrough
 **********************************************************************************************************************/
int main(int argc, char **argv)
{
    cv::Mat imageScene;
    cv::Mat imageTemplate;

    // validate and parse the command line arguments
    if(argc != NUM_COMNMAND_LINE_ARGUMENTS + 1)
    {
        std::printf("USAGE: %s <scene_image> <template_image> \n", argv[0]);
        return 0;
    }
    else
    {
        imageScene = cv::imread(argv[1], cv::IMREAD_COLOR);
        imageTemplate = cv::imread(argv[2], cv::IMREAD_COLOR);

        // check for file error
        if(!imageScene.data)
        {
            std::cout << "Error while opening file " << argv[1] << std::endl;
            return 0;
        }
        if(!imageTemplate.data)
        {
            std::cout << "Error while opening file " << argv[2] << std::endl;
            return 0;
        }
    }

    // convert the images to grayscale
    cv::Mat imageSceneGray;
    cv::cvtColor(imageScene, imageSceneGray, cv::COLOR_BGR2GRAY);
    cv::Mat imageTemplateGray;
    cv::cvtColor(imageTemplate, imageTemplateGray, cv::COLOR_BGR2GRAY);

    // perform the matching step and normalize the searchResult
    cv::Mat searchResult;
    int match_method = CV_TM_CCORR_NORMED;
    cv::matchTemplate(imageSceneGray, imageTemplateGray, searchResult, match_method);
    cv::normalize(searchResult, searchResult, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());

    // find the location of the best fit
    double minVal;
    double maxVal;
    cv::Point minLocation;
    cv::Point maxLocation;
    cv::Point matchLocation;
    cv::minMaxLoc(searchResult, &minVal, &maxVal, &minLocation, &maxLocation, cv::Mat());
    if(match_method  == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED)
    {
        matchLocation = minLocation;
    }
    else
    {
        matchLocation = maxLocation;
    }

    // annotate the scene image
    cv::rectangle(imageScene, matchLocation, cv::Point(matchLocation.x + imageTemplate.cols , matchLocation.y + imageTemplate.rows), CV_RGB(255,0,0), 3);

    // display the images
    cv::imshow("imageScene", imageScene);
    cv::imshow("imageTemplate", imageTemplate);
    cv::imshow("searchResult", searchResult);
    cv::waitKey();

    return 0;
}
