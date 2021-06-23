project (cv_colors)
cmake_minimum_required(VERSION 2.8)

# configure OpenCV
find_package(OpenCV REQUIRED)

# create create individual projects

add_executable(cv_normalize cv_normalize.cpp)
target_link_libraries(cv_normalize ${OpenCV_LIBS})

add_executable(cv_histogram cv_histogram.cpp)
target_link_libraries(cv_histogram ${OpenCV_LIBS})

add_executable(cv_equalize cv_equalize.cpp)
target_link_libraries(cv_equalize ${OpenCV_LIBS})

add_executable(cv_threshold cv_threshold.cpp)
target_link_libraries(cv_threshold ${OpenCV_LIBS})
