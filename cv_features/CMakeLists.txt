project (cv_background)
cmake_minimum_required(VERSION 2.8)

# configure OpenCV
find_package(OpenCV REQUIRED)

# create create individual projects
add_executable(cv_SIFT cv_SIFT.cpp)
target_link_libraries(cv_SIFT ${OpenCV_LIBS})
add_executable(cv_SURF cv_SURF.cpp)
target_link_libraries(cv_SURF ${OpenCV_LIBS})
add_executable(cv_blob cv_blob.cpp)
target_link_libraries(cv_blob ${OpenCV_LIBS})
add_executable(cv_FLANN_matching cv_FLANN_matching.cpp)
target_link_libraries(cv_FLANN_matching ${OpenCV_LIBS})
add_executable(cv_homography cv_homography.cpp)
target_link_libraries(cv_homography ${OpenCV_LIBS})
