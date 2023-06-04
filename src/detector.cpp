#include <iostream>
#include "opencv2/core.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/ximgproc/segmentation.hpp"

#include "../include/detector.h"

#include "../include/rectangularBoundingBoxes.hpp"


cv::Mat noWhiteOnImg(cv::Mat img, int value1, int value2);
cv::Mat foodDetector(cv::Mat img, std::vector<cv::Rect> rectVector);


cv::Mat subjectIsolator(cv::Mat img, std::vector<cv::Vec3f>& circlesVector, std::vector<cv::Rect>& rectanglesVector) {
    
    cv::Mat clonedImg = img.clone();
    cv::Mat rectanglesMask = cv::Mat::zeros(clonedImg.size(), CV_8UC1);
    cv::Mat circlesMask = cv::Mat::zeros(clonedImg.size(), CV_8UC1);

    std::vector<cv::Rect> boundingRectangles;
    std::vector<cv::Rect> residuals;

    for (const cv::Rect& rectangle : rectanglesVector) {
        cv::rectangle(rectanglesMask, rectangle, cv::Scalar(255), -1);
    }

    for (const cv::Vec3f& circle : circlesVector) {
        cv::Point center(circle[0], circle[1]);
        int radius = circle[2];
        cv::circle(circlesMask, center, radius, cv::Scalar(255), -1);
    }
    
    for (const auto& circle : circlesVector) {

        cv::Mat circleMask = cv::Mat::zeros(clonedImg.size(), CV_8UC1);

        cv::Point center(circle[0], circle[1]);
        int radius = circle[2];
        cv::circle(circleMask, center, radius, cv::Scalar(255), -1);


        cv::Mat intersectionMask;
        cv::bitwise_and(circleMask, rectanglesMask, intersectionMask);

        // Calcola la somma di tutti i pixel dell'immagine
        cv::Scalar sum = cv::sum(intersectionMask);

        if (sum[0] > 0) {

            //--------------------------------------------------------------------------------#
            /*
            Questa parte di codice è dedicata al preprocessing della maschera per una determinata portata (es. primo, secondo)
            Qui dentro non arrivano cerchi singoli o rettangoli singoli tipo il pane, ma solo quelli che
            hanno una effettiva intersezione tra un cerchio e uno o più rettangoli.

            OBBIETTIVO: Lavorare la maschera in modo che prenda solo le porzioni di cibo coerenti e non cose extra...
            Qui sotto ci sono vari metodi mischiati per test.
            */

            cv::Mat verifyImg;
            cv::bitwise_and(clonedImg, clonedImg, verifyImg, intersectionMask);

            cv::Mat noWhite = noWhiteOnImg(verifyImg, 188, 255);
            cv::Mat noWhite2 = noWhiteOnImg(noWhite, 50, 185);


            cv::Mat grayImage;
            cv::cvtColor(verifyImg, grayImage, cv::COLOR_BGR2GRAY);

            // Crea una maschera binaria identificando le aree bianche nell'immagine
            cv::Mat mask;
            int thresholdValue = 115;
            //cv::threshold(grayImage, mask, thresholdValue, 255, cv::THRESH_BINARY_INV);
            cv::threshold(grayImage, mask, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);

            // Applica l'operazione bitwise AND tra l'immagine originale e la maschera invertita
            cv::Mat result;
            cv::bitwise_and(verifyImg, verifyImg, result, mask);

            

            cv::Mat grayasImage;
            cv::cvtColor(noWhite2, grayasImage, cv::COLOR_BGR2GRAY);

            // Applica la soglia binaria per creare la maschera
            cv::Mat finalMask;
            cv::threshold(grayasImage, finalMask, 1, 255, cv::THRESH_BINARY);

            // cv::imshow("Int", mask);
            // cv::imshow("Inte", intersectionMask);
            // cv::imshow("Inter", finalMask);
            cv::imshow("Inter", result);
            cv::waitKey(0);


            //--------------------------------------------------------------------------------#


            std::vector<std::vector<cv::Point>> contours;
            cv::findContours(intersectionMask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
            //cv::findContours(finalMask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

            cv::Rect boundingRect;
            for (const auto& contour : contours) {
                cv::Rect currentRect = cv::boundingRect(contour);

                if (currentRect.area() > 300) {
                    boundingRect = boundingRect | currentRect;
                } else {
                    residuals.push_back(currentRect);
                }
                //boundingRect = boundingRect | currentRect;
                
                std::cout << "\n\n########################################################################### AREA: " << currentRect.area() << std::endl;
            }

            boundingRectangles.push_back(boundingRect);
        }

    }

    for (const auto& rectangle : rectanglesVector) {

        int center_x = rectangle.x + rectangle.width / 2;
        int center_y = rectangle.y + rectangle.height / 2;

        uchar valorePixel = circlesMask.at<uchar>(center_y, center_x);

        if (valorePixel == 0) {
            boundingRectangles.push_back(rectangle);
        }
    }

    for (const auto& circle : circlesVector) {

        cv::Point center(circle[0], circle[1]);
        int radius = circle[2];

        uchar valorePixel = rectanglesMask.at<uchar>(center);

        if (valorePixel == 0) {

            cv::Mat tmpMask = cv::Mat::zeros(clonedImg.size(), CV_8UC1);
            cv::circle(tmpMask, center, radius, cv::Scalar(255), -1);

            std::vector<std::vector<cv::Point>> contours;
            cv::findContours(tmpMask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
            
            for (const auto& contour : contours) {
                cv::Rect boundingRect = cv::boundingRect(contour);
                boundingRectangles.push_back(boundingRect);
            }
        } 
    }

    cv::Mat outImg = img.clone();
    for (const auto& rect : boundingRectangles) {
        cv::rectangle(outImg, rect, cv::Scalar(0, 0, 255), 10);
    }

    for (const auto& rect : residuals) {
        cv::rectangle(outImg, rect, cv::Scalar(255, 0, 0), 15);
    }

    for (const cv::Vec3f& circle : circlesVector) {
        cv::Point center(circle[0], circle[1]);
        int radius = circle[2];
        cv::circle(outImg, center, radius, cv::Scalar(0, 255, 0), 15);
    }

    return outImg;





    // cv::Mat clonedImg = img.clone();
    // cv::Mat intersectionCircleMask = cv::Mat::zeros(clonedImg.size(), CV_8UC1);
    // cv::Mat intersectionRectangleMask = cv::Mat::zeros(clonedImg.size(), CV_8UC1);

    // for (const cv::Vec3f& circle : circlesVector) {
    //     cv::Point center(circle[0], circle[1]);
    //     int radius = circle[2];
    //     cv::circle(intersectionCircleMask, center, radius, cv::Scalar(255), -1);
    // }

    // for (const cv::Rect& rectangle : rectanglesVector) {
    //     cv::rectangle(intersectionRectangleMask, rectangle, cv::Scalar(255), -1);
    // }

    // cv::Mat intersectionMask;
    // cv::bitwise_and(intersectionCircleMask, intersectionRectangleMask, intersectionMask);

    // cv::Mat resultImage;
    // clonedImg.copyTo(resultImage, intersectionMask);


    // for (const auto& rectangle : rectanglesVector) {

    //     int center_x = rectangle.x + rectangle.width / 2;
    //     int center_y = rectangle.y + rectangle.height / 2;

    //     uchar valorePixel = intersectionMask.at<uchar>(center_y, center_x);

    //     if (valorePixel == 0) {
    //         cv::rectangle(intersectionMask, rectangle, cv::Scalar(255), -1);
    //     }
    // }

    // for (const auto& circle : circlesVector) {

    //     cv::Point center(circle[0], circle[1]);
    //     int radius = circle[2];

    //     uchar valorePixel = intersectionMask.at<uchar>(center);

    //     if (valorePixel == 0) {
    //         cv::circle(intersectionMask, center, radius, cv::Scalar(255), -1);
    //     } 
    // }

    // std::vector<std::vector<cv::Point>> contours;
    // cv::findContours(intersectionMask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // // Trova i rettangoli che contengono i contorni
    // std::vector<cv::Rect> boundingRectangles;
    // for (const auto& contour : contours) {
    //     cv::Rect boundingRect = cv::boundingRect(contour);
    //     boundingRectangles.push_back(boundingRect);
    // }

    // cv::Mat outImg = img.clone();
    // for (const auto& rect : boundingRectangles) {
    //     cv::rectangle(outImg, rect, cv::Scalar(0, 0, 255), 10);
    // }


    //cv::imshow("Intersection", outImg);
    //cv::waitKey(0);

    return outImg;







    // cv::Mat clonedImg = img.clone();
    // std::vector<cv::Mat> imgVec;

    // size_t SizeRectVector = rectVector.size();
    // for (size_t i = 0; i < SizeRectVector; i++) {

    //     cv::Mat mask = cv::Mat::zeros(clonedImg.rows, clonedImg.cols, CV_8UC1);
    //     cv::Mat bgModel = cv::Mat::zeros(1, 65, CV_64FC1);
    //     cv::Mat fgModel = cv::Mat::zeros(1, 65, CV_64FC1);
        
    //     cv::grabCut(clonedImg, mask, rectVector[i], bgModel, fgModel, 5, cv::GC_INIT_WITH_RECT);

    //     cv::Mat mask2 = (mask == 1) + (mask == 3);  // 0 = cv::GC_BGD, 1 = cv::GC_FGD, 2 = cv::PR_BGD, 3 = cv::GC_PR_FGD
    //     cv::Mat dest;
    //     clonedImg.copyTo(dest, mask2);

    //     foodDetector(dest, rectVector);

    //     imgVec.push_back(mask);
    // }
    // return clonedImg;
}






cv::Mat noWhiteOnImg(cv::Mat img, int value1, int value2) {

    cv::Mat tempImg = img.clone();

    cv::Mat noWhiteImg;
    cv::inRange(tempImg , cv::Scalar(value1, value1, value1), cv::Scalar(value2, value2, value2), noWhiteImg);
    cv::Mat postMask;
    tempImg.copyTo(postMask, 255 - noWhiteImg);

    return postMask;
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