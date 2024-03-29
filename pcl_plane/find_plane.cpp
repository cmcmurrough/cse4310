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
/***********************************************************************************************************************
* @file find_plane.cpp
* @brief finds a plane in a PCD file
*
* Simple example of loading and displaying PCD files with basic plane segmentation
*
* @author Christopher D. McMurrough
**********************************************************************************************************************/

#include "CloudVisualizer.h"

#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/io/pcd_io.h>
#include <pcl/io/ply_io.h>
#include <pcl/common/time.h>
#include <pcl/sample_consensus/model_types.h>
#include <pcl/sample_consensus/method_types.h>
#include <pcl/sample_consensus/sac_model_plane.h>
#include <pcl/segmentation/sac_segmentation.h>

#define NUM_COMMAND_ARGS 1

using namespace std;

// function prototypes
void pointPickingCallback(const pcl::visualization::PointPickingEvent& event, void* cookie);
void keyboardCallback(const pcl::visualization::KeyboardEvent &event, void* viewer_void);
void segmentPlane(const pcl::PointCloud<pcl::PointXYZRGBA>::ConstPtr &cloudIn, pcl::PointIndices::Ptr &inliers, double distanceThreshold, int maxIterations);

/***********************************************************************************************************************
* @brief callback function for handling a point picking event
* @param[in] event handle generated by the visualization window
* @param[in] cookie user data passed by the event
* @author Christoper D. McMurrough
**********************************************************************************************************************/
void pointPickingCallback(const pcl::visualization::PointPickingEvent& event, void* cookie)
{
    static int pickCount = 0;
    static pcl::PointXYZRGBA lastPoint;

    pcl::PointXYZRGBA p;
    event.getPoint(p.x, p.y, p.z);

    cout << "POINT CLICKED: " << p.x << " " << p.y << " " << p.z << endl;

    // if we have picked a point previously, compute the distance
    if(pickCount % 2 == 1)
    {
        double d = std::sqrt((p.x - lastPoint.x) * (p.x - lastPoint.x) + (p.y - lastPoint.y) * (p.y - lastPoint.y) + (p.z - lastPoint.z) * (p.z - lastPoint.z));
        cout << "DISTANCE BETWEEN THE POINTS: " << d << endl;
    }

    // update the last point and pick count
    lastPoint.x = p.x;
    lastPoint.y = p.y;
    lastPoint.z = p.z;
    pickCount++;
}

/***********************************************************************************************************************
* @brief callback function for handling a keypress event
* @param[in] event handle generated by the visualization window
* @param[in] viewer_void user data passed by the event
* @author Christoper D. McMurrough
**********************************************************************************************************************/
void keyboardCallback(const pcl::visualization::KeyboardEvent &event, void* viewer_void)
{
    // handle key down events
    if(event.keyDown())
    {
        // handle any keys of interest
        switch(event.getKeyCode())
        {
            case 'a':
                cout << "KEYPRESS DETECTED: '" << event.getKeySym() << "'" << endl;
                break;
            default:
                break;
        }
    }
}

/***********************************************************************************************************************
* @brief Opens a point cloud file
*
* Opens a point cloud file in either PCD or PLY format
*
* @param[out] cloudOut pointer to opened point cloud
* @param[in] filename path and name of input file
* @return false if an error occurred while opening file
* @author Christopher D. McMurrough
**********************************************************************************************************************/
bool openCloud(pcl::PointCloud<pcl::PointXYZRGBA>::Ptr &cloudOut, const char* fileName)
{
    // convert the file name to string
    std::string fileNameStr(fileName);

    // handle various file types
    std::string fileExtension = fileNameStr.substr(fileNameStr.find_last_of(".") + 1);
    if(fileExtension.compare("pcd") == 0)
    {
        // attempt to open the file
        if(pcl::io::loadPCDFile<pcl::PointXYZRGBA>(fileNameStr, *cloudOut) == -1)
        {
            PCL_ERROR("error while attempting to read pcd file: %s \n", fileNameStr.c_str());
            return false;
        }
        else
        {
            return true;
        }
    }
    else if(fileExtension.compare("ply") == 0)
    {
        // attempt to open the file
        if(pcl::io::loadPLYFile<pcl::PointXYZRGBA>(fileNameStr, *cloudOut) == -1)
        {
            PCL_ERROR("error while attempting to read pcl file: %s \n", fileNameStr.c_str());
            return false;
        }
        else
        {
            return true;
        }
    }
    else
    {
        PCL_ERROR("error while attempting to read unsupported file: %s \n", fileNameStr.c_str());
        return false;
    }
}

/*******************************************************************************************************************//**
 * @brief Locate a plane in the cloud
 *
 * Perform planar segmentation using RANSAC, returning the plane parameters and point indices
 *
 * @param[in] cloudIn pointer to input point cloud
 * @param[out] inliers list containing the point indices of inliers
 * @param[in] distanceThreshold maximum distance of a point to the planar model to be considered an inlier
 * @param[in] maxIterations maximum number of iterations to attempt before returning
 * @return the number of inliers
 * @author Christopher D. McMurrough
 **********************************************************************************************************************/
void segmentPlane(const pcl::PointCloud<pcl::PointXYZRGBA>::ConstPtr &cloudIn, pcl::PointIndices::Ptr &inliers, double distanceThreshold, int maxIterations)
{
    // store the model coefficients
    pcl::ModelCoefficients::Ptr coefficients(new pcl::ModelCoefficients);

    // Create the segmentation object for the planar model and set the parameters
    pcl::SACSegmentation<pcl::PointXYZRGBA> seg;
    seg.setOptimizeCoefficients(true);
    seg.setModelType(pcl::SACMODEL_PLANE);
    seg.setMethodType(pcl::SAC_RANSAC);
    seg.setMaxIterations(maxIterations);
    seg.setDistanceThreshold(distanceThreshold);

    // Segment the largest planar component from the remaining cloud
    seg.setInputCloud(cloudIn);
    seg.segment(*inliers, *coefficients);
}

/***********************************************************************************************************************
* @brief program entry point
* @param[in] argc number of command line arguments
* @param[in] argv string array of command line arguments
* @returnS return code (0 for normal termination)
* @author Christoper D. McMurrough
**********************************************************************************************************************/
int main(int argc, char** argv)
{
    // validate and parse the command line arguments
    if(argc != NUM_COMMAND_ARGS + 1)
    {
        std::printf("USAGE: %s <file_name>\n", argv[0]);
        return 0;
    }

    // parse the command line arguments
    char* fileName = argv[1];

    // create a stop watch for measuring time
    pcl::StopWatch watch;

    // initialize the cloud viewer
    CloudVisualizer CV("Rendering Window");

    // start timing the processing step
    watch.reset();

    // open the point cloud
    pcl::PointCloud<pcl::PointXYZRGBA>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZRGBA>);
    openCloud(cloud, fileName);

    // segment a plane
    const float distanceThreshold = 0.0254;
    const int maxIterations = 5000;
    pcl::PointIndices::Ptr inliers(new pcl::PointIndices);
    segmentPlane(cloud, inliers, distanceThreshold, maxIterations);
    std::cout << "Segmentation result: " << inliers->indices.size() << " points" << std::endl;
    
    // color the plane inliers green
    for(int i = 0; i < inliers->indices.size(); i++)
    {
        int index = inliers->indices.at(i);
        cloud->points.at(index).r = 0;
        cloud->points.at(index).g = 255;
        cloud->points.at(index).b = 0;
    }

    // get the elapsed time
    double elapsedTime = watch.getTimeSeconds();
    std::cout << elapsedTime << " seconds passed " << std::endl;

    // render the scene
    CV.addCloud(cloud);
    CV.addCoordinateFrame(cloud->sensor_origin_, cloud->sensor_orientation_);

    // register mouse and keyboard event callbacks
    CV.registerPointPickingCallback(pointPickingCallback, cloud);
    CV.registerKeyboardCallback(keyboardCallback);

    // enter visualization loop
    while(CV.isRunning())
    {
        CV.spin(100);
    }

    // exit program
    return 0;
}
