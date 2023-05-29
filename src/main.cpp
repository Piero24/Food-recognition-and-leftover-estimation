// make
// ./Food-Recognition ./Food-recognition-and-leftover-estimation/dataset/tray1/
// ./Food-Recognition ./Food-recognition-and-leftover-estimation/dataset/tray2/

#include <iostream>
#include "opencv2/core.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/ximgproc/segmentation.hpp"

#include "../include/Tasks.h"


static void help() {
    std::cout << std::endl <<
    "Usage:" << std::endl <<
    "./ssearch input_image (f|q)" << std::endl <<
    "f=fast, q=quality" << std::endl <<
    "Use l to display less rects, m to display more rects, q to quit" << std::endl;
}

int main(int argc, char** argv) {

    std::vector<cv::Mat> trayVector;
	std::vector<cv::String> fileInFolderVec;

    cv::glob(argv[1], fileInFolderVec, false);

    size_t count = fileInFolderVec.size();
    for (size_t i = 0; i < count; i++) {
        if (fileInFolderVec[i].substr(fileInFolderVec[i].size() - 4) == ".jpg" ||
            fileInFolderVec[i].substr(fileInFolderVec[i].size() - 4) == ".png" ||
            fileInFolderVec[i].substr(fileInFolderVec[i].size() - 4) == ".bmp") {

            cv::Mat img = cv::imread(fileInFolderVec[i]);
            trayVector.push_back(img);
        }
    }

    size_t ch = trayVector.size();
    for (size_t i = 0; i < ch; i++) {

        //cv::Mat grayImage;
        //cv::cvtColor(trayVector[i].clone(), grayImage, cv::COLOR_BGR2GRAY);
	cv::Mat light = img.clone();
	double alpha = 1.0;
	int beta = 200;
	img.convertTo(light, -1, alpha, beta);
	
	    
        cv::Mat blur;
        cv::Mat img = trayVector[i].clone();

        cv::Mat kernel2 = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(25, 25));
        cv::erode(img, blur, kernel2);

        GaussianBlur(blur, blur, cv::Size(5, 5), 0);
        //GaussianBlur(img, blur, cv::Size(5, 5), 0);

        // Convert the image into a matrix of pixel (float)
        cv::Mat points(img.rows * img.cols, 3, CV_32F);

        for(int y = 0; y < img.rows; y++) {
            for(int x = 0; x < img.cols; x++) {
                for(int z = 0; z < 3; z++) {

                    points.at<float>(y + x * img.rows, z) = blur.at<cv::Vec3b>(y,x)[z];
                }
            }
        }

        //Number of clusters
        int K = 3;
        // Output vector
        cv::Mat labels;
        cv::Mat centers;

        cv::kmeans(points, K, labels,
            cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 10, 1.0), 3, cv::KMEANS_PP_CENTERS, centers);

        // Assigns a color to pixels based on the cluster they belong to
        cv::Mat new_image(img.size(), img.type());

        for(int x = 0; x < img.cols; x++) {
            for(int y = 0; y < img.rows; y++) {

                //std::cout << labels << std::endl;
                int cluster_idx = labels.at<int>(y + x * img.rows, 0);
                new_image.at<cv::Vec3b>(y,x)[0] = centers.at<float>(cluster_idx, 0);
                new_image.at<cv::Vec3b>(y,x)[1] = centers.at<float>(cluster_idx, 1);
                new_image.at<cv::Vec3b>(y,x)[2] = centers.at<float>(cluster_idx, 2);
            }
        }

        cv::imshow("Task" + std::to_string(i), new_image);
        cv::waitKey(0);

    //     // create Selective Search Segmentation Object using default parameters
    //     cv::Ptr<cv::ximgproc::segmentation::SelectiveSearchSegmentation> ss = cv::ximgproc::segmentation::createSelectiveSearchSegmentation();

    //     // set input image on which we will run segmentation
    //     ss -> setBaseImage(blur);

    //     std::string dd = "f";

    //     if (dd == "f") {
    //         // Switch to fast but low recall Selective Search method
    //         ss -> switchToSelectiveSearchFast();

    //     } else if (dd == "q") {
    //         // Switch to high recall but slow Selective Search method
    //         ss -> switchToSelectiveSearchQuality();
    //     } else {
    //         // if argument is neither f nor q print help message
    //         help();
    //         return -2;
    //     }

    //     // run selective search segmentation on input image
    //     std::vector<cv::Rect> rects;
    //     ss -> process(rects);
    //     std::cout << "Total Number of Region Proposals: " << rects.size() << std::endl;

    //     // number of region proposals to show
    //     int numShowRects = 10;

    //      // create a copy of original image
    //     cv::Mat imOut = img.clone();

    //     size_t aa = rects.size();
    //     for (size_t j = 0; j < aa; j++) {

    //         if (j < numShowRects) {
    //             rectangle(imOut, rects[j], cv::Scalar(0, 255, 0));
    //         }
    //         else {break;}
    //     }

    //     cv::imshow("Task" + std::to_string(i), imOut);
    //     cv::waitKey(0);
    }


    // char in;
    // std::cout << "Press enter to start:" << std::endl;
    // std::cin.get(in);

    // std::cout << "Task 1 press any key to continue." << std::endl;
    // task1(argc, argv);

    // std::cout << "Task 2 press any key to continue." << std::endl;
    // task2(argc, argv);

    // std::cout << "Task 3 press any key to continue." << std::endl;
    // task3(argc, argv);

    // std::cout << "Task 4 press any key to continue." << std::endl;
    // task4(argc, argv);

    // std::cout << "Task 5 press any key to continue." << std::endl;
    // task5(argc, argv);

    // std::cout << "Tasks terminated." << std::endl;

    return 0;
}
