project (cv_capture)
cmake_minimum_required(VERSION 2.8)

# configure OpenCV
find_package(OpenCV REQUIRED)

# create create individual projects
add_executable(cv_capture cv_capture.cpp)
target_link_libraries(cv_capture ${OpenCV_LIBS})

add_executable(cv_load_video cv_load_video.cpp)
target_link_libraries(cv_load_video ${OpenCV_LIBS})

