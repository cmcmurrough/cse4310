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
    cv::Mat imageObject;

    // validate and parse the command line arguments
    if(argc != NUM_COMNMAND_LINE_ARGUMENTS + 1)
    {
        std::printf("USAGE: %s <scene_image> <object_image> \n", argv[0]);
        return 0;
    }
    else
    {
        imageScene = cv::imread(argv[1], CV_LOAD_IMAGE_COLOR);
        imageObject = cv::imread(argv[2], CV_LOAD_IMAGE_COLOR);

        // check for file error
        if(!imageScene.data)
        {
            std::cout << "Error while opening file " << argv[1] << std::endl;
            return 0;
        }
        if(!imageObject.data)
        {
            std::cout << "Error while opening file " << argv[2] << std::endl;
            return 0;
        }
    }

    // convert the images to grayscale
    cv::Mat imageSceneGray;
    cv::cvtColor(imageScene, imageSceneGray, cv::COLOR_BGR2GRAY);
    cv::Mat imageObjectGray;
    cv::cvtColor(imageObject, imageObjectGray, cv::COLOR_BGR2GRAY);

    // detect SURF keypoints and descriptors in both images
    const int minHessian = 400;
    cv::Ptr<cv::xfeatures2d::SURF> detector = cv::xfeatures2d::SURF::create(minHessian);
    std::vector<cv::KeyPoint> keypointsScene;
    std::vector<cv::KeyPoint> keypointsObject;
    cv::Mat descriptorsScene;
    cv::Mat descriptorsObject;
    detector->detectAndCompute(imageScene, cv::Mat(), keypointsScene, descriptorsScene);
    detector->detectAndCompute(imageObjectGray, cv::Mat(), keypointsObject, descriptorsObject);

    // draw the keypoints
    cv::Mat imageSceneKeypoints;
    cv::Mat imageObjectKeypoints;
    cv::drawKeypoints(imageScene, keypointsScene, imageSceneKeypoints, cv::Scalar::all(-1), cv::DrawMatchesFlags::DEFAULT);
    cv::drawKeypoints(imageObject, keypointsObject, imageObjectKeypoints, cv::Scalar::all(-1), cv::DrawMatchesFlags::DEFAULT);

    // match descriptor vectors using FLANN matcher
    cv::FlannBasedMatcher matcher;
    std::vector<cv::DMatch> matches;
    matcher.match(descriptorsObject, descriptorsScene, matches);

    // calculate min and max distances between keypoints
    double minDistance = 100;
    double maxDistance = 0;
    for(int i = 0; i < descriptorsObject.rows; i++)
    {
        double d = matches[i].distance;
        if(d < minDistance) minDistance = d;
        if(d > maxDistance) maxDistance = d;
    }
    std::cout << "Min dist: " << minDistance << std::endl;
    std::cout << "Max dist: " << maxDistance << std::endl;

    // select "good" matches with distance less than 2 * minDistance
    std::vector<cv::DMatch> goodMatches;
    for( int i = 0; i < descriptorsObject.rows; i++ )
    {
        if(matches[i].distance <= 3 * minDistance)
        {
            goodMatches.push_back( matches[i]);
        }
    }

    // draw the good matches
    cv::Mat imageMatches;
    cv::drawMatches(imageObject, keypointsObject, imageScene, keypointsScene, goodMatches, imageMatches, cv::Scalar::all(-1), cv::Scalar::all(-1), std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

    // localize the object using homography
    std::vector<cv::Point2f> goodObjectKeypoints;
    std::vector<cv::Point2f> goodSceneKeypoints;

    for(int i = 0; i < goodMatches.size(); i++)
    {
        // get the individual components from each "good" keypoint match
        goodObjectKeypoints.push_back(keypointsObject[goodMatches[i].queryIdx].pt);
        goodSceneKeypoints.push_back(keypointsScene[goodMatches[i].trainIdx].pt);
    }
    cv::Mat H = findHomography(goodObjectKeypoints, goodSceneKeypoints, cv::RANSAC);

    // transform the object corners to the scene coordinate system
    std::vector<cv::Point2f> objectCorners(4);
    objectCorners[0] = cv::Point(0,0);
    objectCorners[1] = cv::Point(imageObject.cols, 0);
    objectCorners[2] = cv::Point(imageObject.cols, imageObject.rows);
    objectCorners[3] = cv::Point(0, imageObject.rows);
    std::vector<cv::Point2f> sceneCorners(4);
    cv::perspectiveTransform(objectCorners, sceneCorners, H);

    // draw lines between each corner
    cv::line(imageMatches, sceneCorners[0] + cv::Point2f(imageObject.cols, 0), sceneCorners[1] + cv::Point2f(imageObject.cols, 0), cv::Scalar(0, 255, 0), 4);
    cv::line(imageMatches, sceneCorners[1] + cv::Point2f(imageObject.cols, 0), sceneCorners[2] + cv::Point2f(imageObject.cols, 0), cv::Scalar(0, 255, 0), 4);
    cv::line(imageMatches, sceneCorners[2] + cv::Point2f(imageObject.cols, 0), sceneCorners[3] + cv::Point2f(imageObject.cols, 0), cv::Scalar(0, 255, 0), 4);
    cv::line(imageMatches, sceneCorners[3] + cv::Point2f(imageObject.cols, 0), sceneCorners[0] + cv::Point2f(imageObject.cols, 0), cv::Scalar(0, 255, 0), 4);

    // display the images
    cv::imshow("imageScene", imageScene);
    cv::imshow("imageObject", imageObject);
    cv::imshow("imageSceneKeypoints", imageSceneKeypoints);
    cv::imshow("imageObjectKeypoints", imageObjectKeypoints);
    cv::imshow("imageMatches", imageMatches);
    cv::waitKey();

    return 0;
}
