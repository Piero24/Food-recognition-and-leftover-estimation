// task1.cpp
// g++ -std=c++11 -o task1 task1.cpp -I/usr/local/include/opencv4/ -lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc
// ./task1 street_scene.png

#include <iostream>
// /usr/local/Cellar/opencv/4.7.0_1/include/opencv4
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"


void task1(int argc, char** argv) {

    cv::Mat img = cv::imread(argv[1]);


    //PUT THE REST OF THE CODE HERE


    cv::namedWindow("Task1");
    cv::imshow("Task1", img);
    cv::waitKey(0);
    cv::destroyAllWindows();
}
