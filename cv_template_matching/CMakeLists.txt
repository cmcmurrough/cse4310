project (cv_background)
cmake_minimum_required(VERSION 3.15)

# configure OpenCV
find_package(OpenCV REQUIRED)

# create create individual projects
add_executable(cv_template_matching cv_template_matching.cpp)
target_link_libraries(cv_template_matching ${OpenCV_LIBS})
