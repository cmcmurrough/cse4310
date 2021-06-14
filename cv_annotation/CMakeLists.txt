project (cv_annotation)
cmake_minimum_required(VERSION 3.15)

# configure OpenCV
find_package(OpenCV REQUIRED)

# create create individual projects
add_executable(cv_annotation cv_annotation.cpp)
target_link_libraries(cv_annotation ${OpenCV_LIBS})


