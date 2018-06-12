project (cv_containers)
cmake_minimum_required(VERSION 2.8)

# configure OpenCV
find_package(OpenCV REQUIRED)

# create create individual projects
add_executable(cv_containers cv_containers.cpp)
target_link_libraries(cv_containers ${OpenCV_LIBS})

add_executable(cv_clickable cv_clickable.cpp)
target_link_libraries(cv_clickable ${OpenCV_LIBS})

