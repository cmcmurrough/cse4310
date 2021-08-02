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
/***********************************************************************************************************************
* @file pcl_headless.cpp
* @brief loads a PCD file, makes some changes, and saves an output PCD file
*
* Simple example of loading and saving PCD files, can be used as a template for processing saved data
*
* @author Christopher D. McMurrough
**********************************************************************************************************************/

#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/io/pcd_io.h>
#include <pcl/io/ply_io.h>
#include <pcl/common/time.h>

#define NUM_COMMAND_ARGS 2


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
bool openCloud(pcl::PointCloud<pcl::PointXYZRGBA>::Ptr &cloudOut, std::string fileName)
{
    // handle various file types
    std::string fileExtension = fileName.substr(fileName.find_last_of(".") + 1);
    if(fileExtension.compare("pcd") == 0)
    {
        // attempt to open the file
        if(pcl::io::loadPCDFile<pcl::PointXYZRGBA>(fileName, *cloudOut) == -1)
        {
            PCL_ERROR("error while attempting to read pcd file: %s \n", fileName.c_str());
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
        if(pcl::io::loadPLYFile<pcl::PointXYZRGBA>(fileName, *cloudOut) == -1)
        {
            PCL_ERROR("error while attempting to read pcl file: %s \n", fileName.c_str());
            return false;
        }
        else
        {
            return true;
        }
    }
    else
    {
        PCL_ERROR("error while attempting to read unsupported file: %s \n", fileName.c_str());
        return false;
    }
}

/*******************************************************************************************************************//**
 * @brief Saves a point cloud to file
 *
 * Saves a given point cloud to disk in PCD format
 *
 * @param[in] cloudIn pointer to output point cloud
 * @param[in] filename path and name of output file
 * @param[in] binaryMode saves the file in binary form if true (default:false)
 * @return false if an error occured while writing file
 * @author Christopher D. McMurrough
 **********************************************************************************************************************/
bool saveCloud(const pcl::PointCloud<pcl::PointXYZRGBA>::ConstPtr &cloudIn, std::string fileName, bool binaryMode=true)
{
    // if the input cloud is empty, return
    if(cloudIn->points.size() == 0)
    {
        return false;
    }

    // attempt to save the file
    if(pcl::io::savePCDFile<pcl::PointXYZRGBA>(fileName, *cloudIn, binaryMode) == -1)
    {
        PCL_ERROR("error while attempting to save pcd file: %s \n", fileName);
        return false;
    }
    else
    {
        return true;
    }
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
	std::string inputFilePath(argv[1]);
	std::string outputFilePath(argv[2]);

    // create a stop watch for measuring time
    pcl::StopWatch watch;

    // open the point cloud
    pcl::PointCloud<pcl::PointXYZRGBA>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZRGBA>);
    openCloud(cloud, inputFilePath);

	// start timing the processing step
    watch.reset();
	
	// color all of the points random colors
	for(int i = 0; i < cloud->points.size(); i++)
	{
		cloud->points.at(i).r = rand() % 256;
		cloud->points.at(i).g = rand() % 256;
		cloud->points.at(i).b = rand() % 256;
	}

    // get the elapsed time
    double elapsedTime = watch.getTimeSeconds();
    std::cout << elapsedTime << " seconds passed " << std::endl;

    // save the point cloud
	saveCloud(cloud, outputFilePath);

    // exit program
    return 0;
}
