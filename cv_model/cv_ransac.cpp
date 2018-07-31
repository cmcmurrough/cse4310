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
 * @file cv_ransac.cpp
 * @brief C++ example of RANSAC line fitting in OpenCV
 * @author Christopher D. McMurrough
 **********************************************************************************************************************/

// include necessary dependencies
#include <iostream>
#include <string>
#include "opencv2/opencv.hpp"

// configuration parameters
#define NUM_COMNMAND_LINE_ARGUMENTS 1

/*******************************************************************************************************************//**
 * @brief function for computing the cross product of two vectors
 * @param[in] a the first vector
 * @param[in] b the second vector
 * @return return the resulting cross product
 * @author https://gist.github.com/ceykmc/8a3737376485511b5fef
 **********************************************************************************************************************/
static std::array<int, 3> cross(const std::array<int, 3> &a, const std::array<int, 3> &b)
{
	std::array<int, 3> result;
	result[0] = a[1] * b[2] - a[2] * b[1];
	result[1] = a[2] * b[0] - a[0] * b[2];
	result[2] = a[0] * b[1] - a[1] * b[0];
	return result;
}

/*******************************************************************************************************************//**
 * @brief function for computing the distance between a point and a line
 * @param[in] p the point
 * @param[in] line the line
 * @return return the resulting distance
 * @author https://gist.github.com/ceykmc/8a3737376485511b5fef
 **********************************************************************************************************************/
static double pointToLineDistance(const cv::Point &p, const cv::Vec4i &line)
{
	std::array<int, 3> pa{ { line[0], line[1], 1 } };
	std::array<int, 3> pb{ { line[2], line[3], 1 } };
	std::array<int, 3> l = cross(pa, pb);
	return std::fabs((p.x * l[0] + p.y * l[1] + l[2])) * 1.0 / std::sqrt(double(l[0] * l[0] + l[1] * l[1]));
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
    cv::Mat imageIn;

    // validate and parse the command line arguments
    if(argc != NUM_COMNMAND_LINE_ARGUMENTS + 1)
    {
        std::printf("USAGE: %s <image_path> \n", argv[0]);
        return 0;
    }
    else
    {
        imageIn = cv::imread(argv[1], CV_LOAD_IMAGE_COLOR);

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

    // locate the image contours
    std::vector<std::vector<cv::Point> > contours;
    cv::findContours(imageGray, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

    // merge all contours into a single vector
    std::vector<cv::Point> points;
    for(int i = 0; i < contours.size(); i++)
    {
        points.insert(points.end(), contours.at(i).begin(), contours.at(i).end());
    }

    // draw the contours
    cv::Mat imageContours = cv::Mat::zeros(imageGray.size(), CV_8UC3);
    cv::RNG rand(12345);
    for(int i = 0; i < contours.size(); i++)
    {
        cv::Scalar color = cv::Scalar(rand.uniform(0, 256), rand.uniform(0,256), rand.uniform(0,256));
        cv::drawContours(imageContours, contours, i, color);
    }

    // begin RANSAC iterations
    const int maxIterations = 5000;
    const int minInliers = 600;
    const double inlierDistance = 10;
    cv::Mat imageResult = cv::Mat::zeros(imageContours.size(), CV_8UC3);
    for(int i = 0; i < maxIterations; i++)
    {
        // create the inlier list
        std::vector<int> inlierIndices;
        std::vector<cv::Point> inlierPoints;

        // select 2 unique random points from the image
        int sampleIndex1 = rand.uniform(0, points.size());
        int sampleIndex2 = rand.uniform(0, points.size());
        while(sampleIndex2 == sampleIndex1)
        {
            sampleIndex2 = rand.uniform(0, points.size());
        }
        cv::Point p1 = points.at(sampleIndex1);
        cv::Point p2 = points.at(sampleIndex2);

        // create the hypothesis
        cv::Vec4i line(p1.x, p1.y, p2.x, p2.y);

        // compute the distance of each point to the line
        for(int j = 0; j < points.size(); j++)
        {
            if(pointToLineDistance(points.at(j), line) < inlierDistance)
            {
                inlierIndices.push_back(j);
                inlierPoints.push_back(points.at(j));
            }
        }

        // check for a successful model
        if(inlierIndices.size() >= minInliers)
        {
            // here is where you would perform an optional refinement step using least-squares

            // draw the model
            std::cout << " FOUND A GOOD MODEL: " << p1 << " " << p2 << std::endl;
            cv::line(imageResult, p1, p2, cv::Scalar(0, 0, 255));
            break;
        }
    }

    // display the images
    cv::imshow("imageIn", imageIn);
    cv::imshow("imageGray", imageGray);
    cv::imshow("imageContours", imageContours);
    cv::imshow("imageResult", imageResult);
    cv::waitKey();
}
