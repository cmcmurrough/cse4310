project (cv_model)
cmake_minimum_required(VERSION 3.15)

# configure OpenCV
find_package(OpenCV REQUIRED)

# create create individual projects
add_executable(cv_ellipse cv_ellipse.cpp)
target_link_libraries(cv_ellipse ${OpenCV_LIBS})

add_executable(cv_ransac cv_ransac.cpp)
target_link_libraries(cv_ransac ${OpenCV_LIBS})


