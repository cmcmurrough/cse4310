project (cv_lines)
cmake_minimum_required(VERSION 2.8)

# configure OpenCV
find_package(OpenCV REQUIRED)

# create create individual projects
add_executable(cv_lines cv_lines.cpp)
target_link_libraries(cv_lines ${OpenCV_LIBS})


