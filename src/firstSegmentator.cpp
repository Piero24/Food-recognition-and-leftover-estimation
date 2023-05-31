#include <iostream>
#include <locale>
#include <cmath>

#include "opencv2/core.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/ximgproc/segmentation.hpp"


#include "../include/firstSegmentator.h"

// Pallino giallo:
//     Codice Unicode: \U0001F7E1
//     Valore RGB: (255, 255, 0)
//     Valore BGR: (0, 255, 255)

cv::Mat firstSegmentationFunc(cv::Mat img) {

    cv::Mat image = img.clone();
    cv::Mat grayImage;
    cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);

    cv::Mat cannyImg;
    cv::Canny(grayImage, cannyImg, 44, 264);

    cv::Mat aftGauss;
    cv::GaussianBlur(cannyImg, aftGauss, cv::Size(5, 5), 0);

    cv::imshow("Result", aftGauss);
    cv::waitKey(0);

    std::vector<cv::Vec3f> circles;
    cv::HoughCircles(aftGauss, circles, cv::HOUGH_GRADIENT, 1, 1, 200, 30, 190, 400);

    std::sort(circles.begin(), circles.end(), compareCircles);
    std::vector<cv::Vec3f> outerCircles = removeInnerCircles(circles);

    for (const cv::Vec3f& circ2 : outerCircles) {
        std::cout << "Outer Circle: " << circ2 << std::endl;
    }

    cv::Mat resultImage = image.clone();
    for (const auto& circle : outerCircles)
    {
        cv::Point center(cvRound(circle[0]), cvRound(circle[1]));
        int radius = cvRound(circle[2]);
        cv::circle(resultImage, center, radius, cv::Scalar(0, 255, 0), 2);
    }

    // Visualizza l'immagine risultante
    cv::imshow("Result", resultImage);
    cv::waitKey(0);

    std::cout << "\U0001F7E1  WARNING: the thirdSegmentationFunc() method isn't complete" << std::endl;

    return img;

}


bool compareCircles(const cv::Vec3f& circ1, const cv::Vec3f& circ2) {

    float radius1 = circ1[2];
    float radius2 = circ2[2];
    double pi = M_PI;

    int area1 = circ1[2] * circ1[2] * pi;
    int area2 = circ2[2] * circ2[2] * pi;

    return area1 > area2;
}

// Funzione per controllare se due rettangoli si sovrappongono
bool isOverlap(const cv::Vec3f& circ1, const cv::Vec3f& circ2) {

    cv::Point2f center1(circ1[0], circ1[1]);
    cv::Point2f center2(circ2[0], circ2[1]);

    float radius1 = circ1[2];
    float radius2 = circ2[2];

    // Threshold che permette una leggera sorapposizione
    float threshold = 100;
    float distance = cv::norm(center1 - center2);
    float sumRadii = radius1 + radius2 - threshold;
    

    return distance < sumRadii;
}


// Funzione per rimuovere i interni interni
std::vector<cv::Vec3f> removeInnerCircles(const std::vector<cv::Vec3f>& circles) {
    std::vector<cv::Vec3f> result;

    for (const cv::Vec3f& circ : circles) {
        bool isInner = false;

        for (const cv::Vec3f& existingCirc : result) {
            if (isOverlap(circ, existingCirc)) {
                isInner = true;
                break;
            }
        }

        if (!isInner) {
            result.push_back(circ);
        }
    }

    return result;
}