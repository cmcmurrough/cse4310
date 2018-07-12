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
 * @file cv_FLANN_matching.cpp
 * @brief C++ example for matching of SIFT features using FLANN in OpenCV
 * @author Christopher D. McMurrough
 **********************************************************************************************************************/

// include necessary dependencies
#include <iostream>
#include <string>
#include "opencv2/opencv.hpp"
#include "opencv2/xfeatures2d.hpp"
#include <iostream>
#include <stdio.h>

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
    cv::Mat imageQuery;

    // validate and parse the command line arguments
    if(argc != NUM_COMNMAND_LINE_ARGUMENTS + 1)
    {
        std::printf("USAGE: %s <scene_image> <template_image> \n", argv[0]);
        return 0;
    }
    else
    {
        imageScene = cv::imread(argv[1], CV_LOAD_IMAGE_COLOR);
        imageQuery = cv::imread(argv[2], CV_LOAD_IMAGE_COLOR);

        // check for file error
        if(!imageScene.data)
        {
            std::cout << "Error while opening file " << argv[1] << std::endl;
            return 0;
        }
        if(!imageQuery.data)
        {
            std::cout << "Error while opening file " << argv[2] << std::endl;
            return 0;
        }
    }

    // convert the images to grayscale
    cv::Mat imageSceneGray;
    cv::cvtColor(imageScene, imageSceneGray, cv::COLOR_BGR2GRAY);
    cv::Mat imageQueryGray;
    cv::cvtColor(imageQuery, imageQueryGray, cv::COLOR_BGR2GRAY);

    // detect SURF keypoints and descriptors in both images
    const int minHessian = 400;
    cv::Ptr<cv::xfeatures2d::SIFT> detector = cv::xfeatures2d::SIFT::create(minHessian);
    std::vector<cv::KeyPoint> keypointsScene;
    std::vector<cv::KeyPoint> keypointsQuery;
    cv::Mat descriptorsScene;
    cv::Mat descriptorsQuery;
    detector->detectAndCompute(imageScene, cv::Mat(), keypointsScene, descriptorsScene);
    detector->detectAndCompute(imageQueryGray, cv::Mat(), keypointsQuery, descriptorsQuery);


    // match descriptor vectors using FLANN matcher
    cv::FlannBasedMatcher matcher;
    std::vector<cv::DMatch> matches;
    matcher.match(descriptorsScene, descriptorsQuery, matches);
    double maxDistance = 0;
    double minDistance = 100;

    // calculate min and max distances between keypoints
    for( int i = 0; i < descriptorsScene.rows; i++ )
    {
        double d = matches[i].distance;
        if(d < minDistance) minDistance = d;
        if(d > maxDistance) maxDistance = d;
    }
    std::cout << "Min dist: " << minDistance << std::endl;
    std::cout << "Max dist: " << maxDistance << std::endl;

    // select "good" matches with distance less than 2 * minDistance
    std::vector<cv::DMatch> goodMatches;
    for( int i = 0; i < descriptorsScene.rows; i++ )
    {
        if(matches[i].distance <= std::max(2 * minDistance, 0.02))
        {
            goodMatches.push_back( matches[i]);
        }
    }

    // draw the good matches
    cv::Mat imageMatches;
    cv::drawMatches(imageScene, keypointsScene, imageQuery, keypointsQuery, goodMatches, imageMatches, cv::Scalar::all(-1), cv::Scalar::all(-1), std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

    // draw the keypoints
    cv::Mat imageSceneKeypoints;
    cv::Mat imageQueryKeypoints;
    cv::drawKeypoints(imageScene, keypointsScene, imageSceneKeypoints, cv::Scalar::all(-1), cv::DrawMatchesFlags::DEFAULT);
    cv::drawKeypoints(imageQuery, keypointsQuery, imageQueryKeypoints, cv::Scalar::all(-1), cv::DrawMatchesFlags::DEFAULT);

    // display the images
    cv::imshow("imageScene", imageScene);
    cv::imshow("imageQuery", imageQuery);
    cv::imshow("imageSceneKeypoints", imageSceneKeypoints);
    cv::imshow("imageQueryKeypoints", imageQueryKeypoints);
    cv::imshow("imageMatches", imageMatches);
    cv::waitKey();

    return 0;
}
