cmake_minimum_required(VERSION 3.0.0)
project(Food-Recognition VERSION 0.1.0)

find_package(OpenCV REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable( ${PROJECT_NAME} src/main.cpp src/task1.cpp src/task2.cpp src/task3.cpp src/task4.cpp src/task5.cpp)

target_link_libraries( ${PROJECT_NAME} ${OpenCV_LIBS} )

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
