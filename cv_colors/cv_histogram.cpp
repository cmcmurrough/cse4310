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
 * @file cv_colors.cpp
 * @brief C++ example of color histograms and image normalization in OpenCV
 * @author Christopher D. McMurrough
 **********************************************************************************************************************/

// include necessary dependencies
#include <iostream>
#include <string>
#include "opencv2/opencv.hpp"

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>

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

    // split the input image into individual channels
    cv::Mat channels[3];
    cv::split(imageIn, channels);

    // set histogram ranges and settings
    const int histogramSize = 256;
    const float range[] = {0, histogramSize} ;
    const float* histogramRange = {range};

    cv::Mat histogramBlue;
    cv::Mat histogramGreen;
    cv::Mat histogramRed;

    // compute histograms
    cv::calcHist(&channels[0], 1, 0, cv::Mat(), histogramBlue, 1, &histogramSize, &histogramRange);
    cv::calcHist(&channels[1], 1, 0, cv::Mat(), histogramGreen, 1, &histogramSize, &histogramRange);
    cv::calcHist(&channels[2], 1, 0, cv::Mat(), histogramRed, 1, &histogramSize, &histogramRange);

    // create the histogram image
    int hist_w = 512;
    int hist_h = 400;
    int bin_w = round( (double) hist_w / histogramSize );
    cv::Mat histogramImage(hist_h, hist_w, CV_8UC3, cv::Scalar(0,0,0));

    // normalize the result to [ 0, histImage.rows ]
    cv::normalize(histogramBlue, histogramBlue, 0, histogramImage.rows, cv::NORM_MINMAX, -1, cv::Mat());
    cv::normalize(histogramGreen, histogramGreen, 0, histogramImage.rows, cv::NORM_MINMAX, -1, cv::Mat());
    cv::normalize(histogramRed, histogramRed, 0, histogramImage.rows, cv::NORM_MINMAX, -1, cv::Mat());

    // plot each channel
    for( int i = 1; i < histogramSize; i++ )
    {
        cv::line(histogramImage, cv::Point(bin_w*(i-1), hist_h - round(histogramBlue.at<float>(i-1))), cv::Point(bin_w*(i), hist_h - round(histogramBlue.at<float>(i))), cv::Scalar(255, 0, 0), 2, 8, 0);
        cv::line(histogramImage, cv::Point(bin_w*(i-1), hist_h - round(histogramGreen.at<float>(i-1))), cv::Point(bin_w*(i), hist_h - round(histogramGreen.at<float>(i))), cv::Scalar(0, 255, 0), 2, 8, 0);
        cv::line(histogramImage, cv::Point(bin_w*(i-1), hist_h - round(histogramRed.at<float>(i-1))), cv::Point(bin_w*(i), hist_h - round(histogramRed.at<float>(i))), cv::Scalar(0, 0, 255), 2, 8, 0);
    }

    // display the images
    cv::imshow("imageIn", imageIn);
    cv::imshow("histogramImage", histogramImage);
    cv::waitKey();

    return 0;
}
