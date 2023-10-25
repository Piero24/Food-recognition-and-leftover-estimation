cmake_minimum_required(VERSION 3.26.0)
project(Food-Recognition VERSION 0.1.0)

find_package(OpenCV REQUIRED)
find_package(Boost COMPONENTS filesystem REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS} )
include_directories(${Boost_INCLUDE_DIRS})

add_executable( ${PROJECT_NAME} 
    src/main.cpp 
    src/uploadImage.cpp
    src/imagePreprocessing.cpp
    src/circularBoundingBoxes.cpp 
    src/rectangularBoundingBoxes.cpp 
    src/outputCombined.cpp
    src/detector.cpp
    src/classificator.cpp
    src/drawerAndTester.cpp
    src/foodCategories.cpp
    src/performanceIndicators.cpp
)

target_link_libraries( ${PROJECT_NAME} ${OpenCV_LIBS} ${Boost_FILESYSTEM_LIBRARY})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
