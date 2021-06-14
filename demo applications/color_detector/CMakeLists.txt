project (cv_capture_example)
cmake_minimum_required(VERSION 3.15)

# configure OpenCV
find_package(OpenCV REQUIRED)

# create create individual projects
add_executable (cv_capture_example cv_capture_example.cpp)
target_link_libraries(cv_capture_example ${OpenCV_LIBS})

add_executable (cv_color_detector cv_color_detector.cpp)
target_link_libraries(cv_color_detector ${OpenCV_LIBS})
