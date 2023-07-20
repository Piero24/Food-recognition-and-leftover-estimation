cmake_minimum_required(VERSION 3.26.0)
project(Food-Recognition VERSION 0.1.0)

find_package(OpenCV REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable( ${PROJECT_NAME} 
    src/main.cpp 
    src/uploadImage.cpp 
    src/prePTester.cpp
    src/imagePreprocessing.cpp 
    src/circularBoundingBoxes.cpp 
    src/rectangularBoundingBoxes.cpp 
    src/thirdSegmentator.cpp
    src/outputCombined.cpp
    src/detector.cpp
    src/matchImage.cpp
)

target_link_libraries( ${PROJECT_NAME} ${OpenCV_LIBS} )

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
