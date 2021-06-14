project (cv_background)
cmake_minimum_required(VERSION 3.15)

# configure OpenCV
find_package(OpenCV REQUIRED)

# create create individual projects
add_executable(cv_pmog cv_pmog.cpp)
target_link_libraries(cv_pmog ${OpenCV_LIBS})
