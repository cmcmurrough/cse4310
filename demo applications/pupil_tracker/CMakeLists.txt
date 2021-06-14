project (pupil_tracker)
cmake_minimum_required(VERSION 3.15)

# configure OpenCV
find_package(OpenCV REQUIRED)

add_executable(pupil_demo pupil_demo.cpp PupilTracker.cpp)
target_link_libraries(pupil_demo ${OpenCV_LIBS})

