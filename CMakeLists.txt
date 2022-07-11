project (cv_background)
cmake_minimum_required(VERSION 3.15)

# configure OpenCV
find_package(OpenCV REQUIRED)

# create create individual projects
add_executable(cv_optic_flow cv_optic_flow.cpp)
target_link_libraries(cv_optic_flow ${OpenCV_LIBS})
