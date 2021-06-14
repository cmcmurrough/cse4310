project (cv_hello_world)
cmake_minimum_required(VERSION 3.15)
find_package(OpenCV REQUIRED)

# create create individual projects
add_executable(cv_hello_world cv_hello_world.cpp)
target_link_libraries(cv_hello_world ${OpenCV_LIBS})
