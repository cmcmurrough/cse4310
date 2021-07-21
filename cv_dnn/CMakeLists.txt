project (cv_dnn)
cmake_minimum_required(VERSION 3.15)

# configure OpenCV
find_package(OpenCV REQUIRED)

# create create individual projects
add_executable(cv_yolo cv_yolo.cpp)
target_link_libraries(cv_yolo ${OpenCV_LIBS})

add_executable(cv_maskrcnn cv_maskrcnn.cpp)
target_link_libraries(cv_maskrcnn ${OpenCV_LIBS})


