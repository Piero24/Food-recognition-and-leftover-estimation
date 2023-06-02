#include <iostream>
#include "opencv2/core.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/ximgproc/segmentation.hpp"

#include "../include/detector.h"


cv::Mat foodDetector(cv::Mat img, std::vector<cv::Rect> rectVector);


cv::Mat subjectIsolator(cv::Mat img, std::vector<cv::Rect> rectVector) {

    cv::Mat clonedImg = img.clone();
    std::vector<cv::Mat> imgVec;

    size_t SizeRectVector = rectVector.size();
    for (size_t i = 0; i < SizeRectVector; i++) {

        cv::Mat mask = cv::Mat::zeros(clonedImg.rows, clonedImg.cols, CV_8UC1);
        cv::Mat bgModel = cv::Mat::zeros(1, 65, CV_64FC1);
        cv::Mat fgModel = cv::Mat::zeros(1, 65, CV_64FC1);
        
        cv::grabCut(clonedImg, mask, rectVector[i], bgModel, fgModel, 5, cv::GC_INIT_WITH_RECT);

        cv::Mat mask2 = (mask == 1) + (mask == 3);  // 0 = cv::GC_BGD, 1 = cv::GC_FGD, 2 = cv::PR_BGD, 3 = cv::GC_PR_FGD
        cv::Mat dest;
        clonedImg.copyTo(dest, mask2);

        foodDetector(dest, rectVector);

        imgVec.push_back(mask);
    }
    return clonedImg;
}



cv::Mat foodDetector(cv::Mat img, std::vector<cv::Rect> rectVector)  {

    // Reshape dell'immagine in un vettore di pixel
    cv::Mat samples = img.reshape(1, img.rows * img.cols);
    samples.convertTo(samples, CV_32F);

    // Esegui il K-means
    int k = 3;
    cv::TermCriteria criteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 10, 1.0);
    cv::Mat labels, centers;
    cv::kmeans(samples, k, labels, criteria, 3, cv::KMEANS_RANDOM_CENTERS, centers);

    // Ottieni i colori dominanti dai centroidi dei cluster
    cv::Vec3b dominantColor1 = centers.at<cv::Vec3f>(0);
    cv::Vec3b dominantColor2 = centers.at<cv::Vec3f>(1);
    cv::Vec3b dominantColor3 = centers.at<cv::Vec3f>(2);
    cv::Vec3b dominantColor4 = centers.at<cv::Vec3f>(3);

    // Segmenta l'immagine utilizzando i colori dominanti
    cv::Mat segmented = img.clone();
    cv::Vec3b color1 = dominantColor1;
    cv::Vec3b color2 = dominantColor2;
    cv::Vec3b color3 = dominantColor3;
    cv::Vec3b color4 = dominantColor4;

    for (int i = 0; i < samples.rows; i++) {
        int clusterIdx = labels.at<int>(i);
        cv::Vec3b& pixel = segmented.at<cv::Vec3b>(i);
        if (clusterIdx == 0)
            pixel = color1;
        else if (clusterIdx == 1)
            pixel = color2;
        else if (clusterIdx == 2)
            pixel = color3;
        else
            pixel = color4;
    }

    // Visualizza l'immagine segmentata
    cv::imshow("Segmented Image", segmented);
    cv::waitKey(0);

    return img;

}