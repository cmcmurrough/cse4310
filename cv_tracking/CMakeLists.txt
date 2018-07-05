project (cv_background)
cmake_minimum_required(VERSION 2.8)

# configure OpenCV
find_package(OpenCV REQUIRED)

# create create individual projects
add_executable(cv_tracking cv_tracking.cpp)
target_link_libraries(cv_tracking ${OpenCV_LIBS})
