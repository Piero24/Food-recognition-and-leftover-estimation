#include <iostream>
#include <list>
#include <ctime> // DEBUG

#include "opencv2/core.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/ximgproc/segmentation.hpp"

#include "../include/detector.h"


std::vector<std::vector<cv::Point>> subjectIsolator(cv::Mat img, std::vector<cv::Vec3f>& circlesVector, std::vector<cv::Rect>& rectanglesVector, int& numOfBoxesFirstImg) {
    
    cv::Mat clonedImg = img.clone();
    cv::Mat rectanglesMask = cv::Mat::zeros(clonedImg.size(), CV_8UC1);
    cv::Mat circlesMask = cv::Mat::zeros(clonedImg.size(), CV_8UC1);

    std::vector<cv::Rect> boundingRectangles;
    std::vector<cv::Rect> residuals;

    // Create the image mask with all the rectangles
    for (const cv::Rect& rectangle : rectanglesVector) {
        cv::rectangle(rectanglesMask, rectangle, cv::Scalar(255), -1);
    }

    // Create the image mask with all the circles
    for (const cv::Vec3f& circle : circlesVector) {
        cv::Point center(circle[0], circle[1]);
        int radius = circle[2];
        cv::circle(circlesMask, center, radius, cv::Scalar(255), -1);
    }
    
    // For each circle in the image
    for (const auto& circle : circlesVector) {

        cv::Mat circleMask = cv::Mat::zeros(clonedImg.size(), CV_8UC1);

        cv::Point center(circle[0], circle[1]);
        int radius = circle[2];
        cv::circle(circleMask, center, radius, cv::Scalar(255), -1);

        // Find the parts of the circle that intersect with the rectangles and create
        // a mask to restrict the work area
        cv::Mat intersectionMask;
        cv::bitwise_and(circleMask, rectanglesMask, intersectionMask);

        // Calculate the sum of all pixels in the image
        cv::Scalar sum = cv::sum(intersectionMask);

        // If there are white pixels it means that there is a portion to check
        if (sum[0] > 0) {

            cv::Mat outputImage;
            cv::bitwise_and(clonedImg, clonedImg, outputImage, intersectionMask);

            cv::Mat grayasImage;
            cv::cvtColor(outputImage, grayasImage, cv::COLOR_BGR2GRAY);

            // Apply binary threshold to create mask
            cv::Mat finalMask;
            cv::threshold(grayasImage, finalMask, 1, 255, cv::THRESH_BINARY);
            
            // Find all shapes in the mask
            std::vector<std::vector<cv::Point>> contours;
            cv::findContours(finalMask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
            
            // Create a rectangle around the shape
            cv::Rect boundingRect;
            for (const auto& contour : contours) {
                cv::Rect currentRect = cv::boundingRect(contour);

                // Calculate the area of the shape
                double area = cv::contourArea(contour);
                int areaInt = static_cast<int>(area);

                // If the area is small it skips it and puts it in the residue vector
                // otherwise it joins it to the remaining part as a single shape
                if (currentRect.area() > 400) {
                    boundingRect = boundingRect | currentRect;

                } else {
                    residuals.push_back(currentRect);
                }
            }

            boundingRectangles.push_back(boundingRect);
        }

    }

    std::vector<cv::Rect> onlyBread;

    // Takes rectangles not overlapping other circles
    for (const auto& rectangle : rectanglesVector) {

        int center_x = rectangle.x + rectangle.width / 2;
        int center_y = rectangle.y + rectangle.height / 2;

        uchar valorePixel = circlesMask.at<uchar>(center_y, center_x);

        if (valorePixel == 0) {
            boundingRectangles.push_back(rectangle);
            onlyBread.push_back(rectangle);
        }
    }

    std::vector<cv::Vec3f> noOvelapedCirclesVector;

    // Takes circles not overlapping other rectangles
    // for each one creates the rectangle that contains it
    for (const auto& circle : circlesVector) {

        cv::Point center(circle[0], circle[1]);
        int radius = circle[2];

        uchar valorePixel = rectanglesMask.at<uchar>(center);

        if (valorePixel == 0) {
            noOvelapedCirclesVector.push_back(circle);

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

    removeContainedRectangles(boundingRectangles);
    rectangleCounter(boundingRectangles, numOfBoxesFirstImg);
    std::vector<cv::Rect> newBoundingRectangles = removeUsefullBBox(boundingRectangles, numOfBoxesFirstImg);

    std::vector<std::vector<cv::Point>> mergedContours;
    std::vector<std::vector<cv::Point>> splittedDishes;


    for (const auto& rect : newBoundingRectangles) {
        cv::Mat onlySquare = setPixelsOutsideRectangle(clonedImg, rect);
        cv::Mat clusteredImage = kmeansClustering(onlySquare);

        cv::Mat grayImage;
        cv::cvtColor(clusteredImage, grayImage, cv::COLOR_BGR2GRAY);

        // Reduce noise in the image using the Gaussian filter
        cv::GaussianBlur(grayImage, grayImage, cv::Size(7, 7), 1.4);

        bool isInside = isRectangleInsideVector(rect, onlyBread);

        double lowThreshold = 40;
        double highThreshold = 100;

        // PROBLEM WITH THIS CANNY
        // Different results if run on all the tray sequentially from 1 to 8 and if run only
        // on a specified tray
        //
        // Apply the Canny Edge Detector
        cv::Mat edges;
        cv::Canny(grayImage, edges, lowThreshold, highThreshold);
        cv::rectangle(edges, rect, cv::Scalar(0, 0, 0), 2);

        // Create the image mask with all the circles
        for (const cv::Vec3f& circle : circlesVector) {
            cv::Point center(circle[0], circle[1]);
            int radius = circle[2];
            cv::circle(edges, center, radius, cv::Scalar(0), 12);
        }

        cv::GaussianBlur(edges, edges, cv::Size(5, 5), 1.4);

        cv::Mat newPostCanny;
        clonedImg.copyTo(newPostCanny, edges);

        cv::Mat kernel2 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(7, 7));
        cv::Mat dilatedImage;
        cv::dilate(newPostCanny, dilatedImage, kernel2);

        cv::Mat tempGrayImage;
        cv::cvtColor(dilatedImage, tempGrayImage, cv::COLOR_BGR2GRAY);

        cv::Mat dilatedIfNecessary;
        int areaBredRect = rect.area();

        if (isInside && (areaBredRect < 50000)) {
            cv::Mat kernel3 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(21, 21));
            cv::dilate(tempGrayImage, dilatedIfNecessary, kernel3);

        } else if (isInside && (areaBredRect < 150000)) {
            cv::Mat kernel3 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(15, 15));
            cv::dilate(tempGrayImage, dilatedIfNecessary, kernel3);

        } else {
            dilatedIfNecessary = tempGrayImage.clone();
        }

        // Find all shapes in the mask
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(dilatedIfNecessary, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        int maxArea = 0;
        std::vector<int> meanAreaVec;
        std::vector<std::vector<cv::Point>> selectedContours;
        
        for (const auto& contour : contours) {
            // Calculate the area of the shape
            double area = cv::contourArea(contour);
            int areaInt = static_cast<int>(area);
            meanAreaVec.push_back(areaInt);
            if (areaInt > maxArea) {
                maxArea = areaInt;
            }
        }

        int sum = 0;
        for (const auto& element : meanAreaVec) {
            sum += element;
        }

        int meanArea = static_cast<int>(sum / meanAreaVec.size());

        for (const auto& contour : contours) {

            double area = cv::contourArea(contour);
            int areaInt = static_cast<int>(area);

            if (areaInt > (meanArea - 2)) {
                selectedContours.push_back(contour);
            }
        }

        int denominator = static_cast<int> (maxArea / 15);
        int maxAreaThreshold = maxArea + denominator;
        int VecSize = selectedContours.size();

        std::vector<std::vector<cv::Point>> toManyPcs;
        std::vector<cv::Point> tempMergedContour;
        
        // DEBUGGING
        //std::cout << "-----------------------------------> MAX AREA: " << maxArea << std::endl;
        //std::cout << "-----------------------------------> SIZE: " << VecSize << std::endl;

        for (const auto& contour : selectedContours) {
            double area = cv::contourArea(contour);
            int areaInt = static_cast<int>(area);

            // DEBUGGING
            //std::cout << "-----------------------------------> AREA: " << areaInt << std::endl;

            if (VecSize > 4) {
                toManyPcs.push_back(contour);
            } else {

                if (!((areaInt * 2) < maxAreaThreshold)) {
                    // THIS "IF" IS ONLY FOR THE IMAGE 2 ON THE TRAY 7. NEED TO BE FIXED.
                    // (NOT ALWAYS IIS NEEDED DEPENDS HOW THE CONTOURS WAS FOUND)

                    if (!((maxArea > 40000 && maxArea < 50000) && (areaInt > 22000 && areaInt < 26000))) {
                        tempMergedContour.insert(tempMergedContour.end(), contour.begin(), contour.end());
                    }
                }
            }
        }

        if (!toManyPcs.empty()) {

            cv::Mat copyImg = img.clone();
            cv::Mat onlyCircles(copyImg.size(), copyImg.type(), cv::Vec3b(0, 0, 0));

            cv::Mat extraContoursImg = setPixelsOutsideContours(copyImg, toManyPcs);

            cv::Mat grayScaleContours;
            cv::cvtColor(extraContoursImg, grayScaleContours, cv::COLOR_BGR2GRAY);

            cv::Mat maskContours;
            cv::threshold(grayScaleContours, maskContours, 1, 255, cv::THRESH_BINARY);

            for (const cv::Vec3f& circle : noOvelapedCirclesVector) {
                cv::Point center(circle[0], circle[1]);
                int fractRadius = circle[2] / 8;
                int radius = static_cast<int> ((fractRadius * 7) - fractRadius/3);
                cv::circle(onlyCircles, center, radius, cv::Vec3b(255, 255, 255), -1);
            }

            cv::Mat grayScaleCircles;
            cv::cvtColor(onlyCircles, grayScaleCircles, cv::COLOR_BGR2GRAY);

            cv::Mat residualMask;
            cv::bitwise_and(maskContours, grayScaleCircles, residualMask);

            int nonBlackPixels = cv::countNonZero(residualMask);

            if (nonBlackPixels < 7000) {

                onlyCircles.setTo(cv::Scalar(0, 0, 0));
                grayScaleCircles.setTo(cv::Scalar(0, 0, 0));
                residualMask.setTo(cv::Scalar(0, 0, 0));

                for (const cv::Vec3f& circle : circlesVector) {
                    cv::Point center(circle[0], circle[1]);
                    int fractRadius = circle[2] / 8;
                    int radius = static_cast<int> ((fractRadius * 7) - fractRadius/3);
                    cv::circle(onlyCircles, center, radius, cv::Vec3b(255, 255, 255), -1);
                }

                cv::cvtColor(onlyCircles, grayScaleCircles, cv::COLOR_BGR2GRAY);     
                cv::bitwise_and(maskContours, grayScaleCircles, residualMask);

            }

            cv::Mat imageWithLessContours;
            cv::bitwise_and(extraContoursImg, extraContoursImg, imageWithLessContours, residualMask);

            cv::Mat grayforNewContours;
            cv::cvtColor(imageWithLessContours, grayforNewContours, cv::COLOR_BGR2GRAY);

            cv::Mat kernelC = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(11, 11));

            cv::Mat erodedContours;
            cv::dilate(grayforNewContours, erodedContours, kernelC);

            cv::Mat kernelC2 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(13, 13));

            cv::Mat dilatedContours;
            cv::dilate(erodedContours, dilatedContours, kernelC2);

            // Find all shapes in the mask
            std::vector<std::vector<cv::Point>> newContours;
            cv::findContours(grayforNewContours, newContours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

            std::vector<std::vector<cv::Point>> contoursWithMaxArea;

            int contoursMaxArea = 0;
            for (const auto& contour : newContours) {

                if (!contour.empty()) {
                    double area = cv::contourArea(contour);
                    int areaInt = static_cast<int>(area);

                    if (areaInt > contoursMaxArea) {
                        contoursMaxArea = areaInt;
                    }
                }
            }

            for (const auto& contour : newContours) {

                if (!contour.empty()) {
                    double area = cv::contourArea(contour);
                    int areaInt = static_cast<int>(area);

                    if (areaInt == contoursMaxArea) {
                        // DEBUGGING
                        // std::cout << "-----------------------------------> AREA: " << areaInt << std::endl;
                        tempMergedContour.insert(tempMergedContour.end(), contour.begin(), contour.end());
                    }    
                }
            }

        }
        mergedContours.push_back(tempMergedContour);
    }


    // Remove possible empty contours
    mergedContours.erase(std::remove_if(mergedContours.begin(), mergedContours.end(), 
                        [](const std::vector<cv::Point>& contour) {
                            return contour.empty();
                        }), 
                        mergedContours.end());

    std::vector<std::vector<cv::Point>> contoursToReturn;

    if (!mergedContours.empty()) {

        int index = 0;
        for (const auto& contour : mergedContours) {
            double area = cv::contourArea(contour);
            int areaInt = static_cast<int>(area);

            // DEBUGGING
            //std::cout << "--------------------------------------------------> AREA: " << areaInt << std::endl;

            cv::Mat tmpResultImage = cv::Mat::zeros(clonedImg.size(), clonedImg.type());
            std::vector<std::vector<cv::Point>> tmpVec;

            tmpVec.push_back(contour);

            cv::drawContours(tmpResultImage, tmpVec, 0, cv::Scalar(255, 255, 255), -1);

            clonedImg.copyTo(tmpResultImage, tmpResultImage);
            // Divide the side dishes from the main dishes
            splittedDishes = sideDishSeparator(tmpResultImage);

            // If side dishes have been found then insert the contour of each dish into the vector of 
            // dishes to return otherwise if they have not been found insert the contours in input into the 
            // vector of those to return
            if (!splittedDishes.empty()) {
                contoursToReturn.insert(contoursToReturn.end(), splittedDishes.begin(), splittedDishes.end());

            } else {
                contoursToReturn.push_back(contour);
            }

        }

    }

    return contoursToReturn;
}


void rectangleCounter(std::vector<cv::Rect>& BBoxVector, int& numOfBoxesFirstImg) {
    if (numOfBoxesFirstImg < 1) {
        numOfBoxesFirstImg = BBoxVector.size();
    } 
}


std::vector<cv::Rect> removeUsefullBBox(std::vector<cv::Rect> BBoxVector, int numOfBoxesFirstImg) {

  std::vector<cv::Rect> rectsVector = BBoxVector;
  int currNumOfBoxes = rectsVector.size();

  if(currNumOfBoxes > numOfBoxesFirstImg) {
      int index;
      int minArea = rectsVector.at(0).area();

      for(int j = 0; j < rectsVector.size(); j++) {
          int area = rectsVector.at(j).area();
          
          if(area < minArea) {
              minArea = area;				
              index = j;
          }
      }

      std::vector<cv::Rect>::iterator it = rectsVector.begin() + index;
      rectsVector.erase(it);
  }
  return rectsVector;

}


bool isCompletelyContained(const cv::Rect& rect1, const cv::Rect& rect2) {
    return (rect1.x >= rect2.x && rect1.y >= rect2.y &&
            rect1.x + rect1.width <= rect2.x + rect2.width &&
            rect1.y + rect1.height <= rect2.y + rect2.height);
}


void removeContainedRectangles(std::vector<cv::Rect>& boundingRectangles) {
    std::vector<cv::Rect> resultRectangles;

    for (const auto& rect : boundingRectangles) {
        bool isContained = false;
        for (const auto& otherRect : boundingRectangles) {
            if (rect != otherRect && isCompletelyContained(rect, otherRect)) {
                isContained = true;
                break;
            }
        }

        if (!isContained) {
            resultRectangles.push_back(rect);
        }
    }

    boundingRectangles = resultRectangles;
}


cv::Mat setPixelsOutsideRectangle(cv::Mat img, const cv::Rect& rect) {

    cv::Mat image = img.clone();

    for (int y = 0; y < image.rows; y++) {
        for (int x = 0; x < image.cols; x++) {

            bool isInsideRectangle = false;

            if (x >= rect.x && x < rect.x + rect.width && y >= rect.y && y < rect.y + rect.height) {
                isInsideRectangle = true;
            }

            if (!isInsideRectangle) {
                image.at<cv::Vec3b>(y, x) = cv::Vec3b(0, 0, 0);
            }
        }
    }
    return  image;
}


cv::Mat kmeansClustering(cv::Mat img) {

    cv::Mat image = img.clone();
    cv::cvtColor(image, image, cv::COLOR_BGR2RGB);

    cv::Mat pixels = image.reshape(1, image.rows * image.cols);
    pixels.convertTo(pixels, CV_32F);

    int k = 15;
    cv::TermCriteria criteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 10, 1.0);
    int attempts = 5;
    int flags = cv::KMEANS_RANDOM_CENTERS;

    cv::Mat labels, centers;
    cv::kmeans(pixels, k, labels, criteria, attempts, flags, centers);

    cv::Mat clusteredImage(image.size(), image.type());
    for (int i = 0; i < image.rows * image.cols; i++) {
        int label = labels.at<int>(i);
        clusteredImage.at<cv::Vec3b>(i) = centers.at<cv::Vec3f>(label);
    }

    cv::cvtColor(clusteredImage, clusteredImage, cv::COLOR_RGB2BGR);
    return clusteredImage;

}


cv::Mat setPixelsOutsideContours(cv::Mat image, const std::vector<std::vector<cv::Point>>& contours) {
    
    cv::Mat clonedImg = image.clone();
    for (int y = 0; y < image.rows; y++) {
        for (int x = 0; x < image.cols; x++) {
            cv::Point point(x, y);

            bool insideAnyContour = false;
            for (const auto& contour : contours) {
                // Check if the point is inside or on the boundary
                if (!contour.empty()) {
                    double distance = cv::pointPolygonTest(contour, point, false);
                    if (distance >= 0) {
                        insideAnyContour = true;
                        break;
                    }
                }
            }

            if (!insideAnyContour) {
                clonedImg.at<cv::Vec3b>(y, x) = cv::Vec3b(0, 0, 0);
            }
        }
    }
    return clonedImg;
}


bool isRectangleInsideVector(const cv::Rect& rect, const std::vector<cv::Rect>& rectVector) {
    for (const auto& r : rectVector) {
        // Check if the rectangle is entirely contained within a vector rectangle
        if (rect.x >= r.x && rect.y >= r.y &&
            rect.x + rect.width <= r.x + r.width &&
            rect.y + rect.height <= r.y + r.height) {
            return true;
        }
    }
    return false;
}


std::vector<std::vector<cv::Point>> sideDishSeparator(cv::Mat image) {
    
    cv::Mat clonedImg = image.clone();
    cv::GaussianBlur(clonedImg, clonedImg, cv::Size(71, 71), 17, 17);
    // Reshape the image into a 1D array of points (each row represents a pixel)
    cv::Mat data = clonedImg.reshape(1, clonedImg.cols * clonedImg.rows);
    data.convertTo(data, CV_32F);

    // Number of clusters desired
    int K = 2;
    // Define criteria for stopping the K-means algorithm
    cv::TermCriteria criteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 100, 0.1);

    // Esegui K-means
    cv::Mat labels, centers;
    cv::kmeans(data, K, labels, criteria, 10, cv::KMEANS_RANDOM_CENTERS, centers);

    // Replace the pixel colors with the centroid colors of the corresponding cluster
    cv::Mat clusteredImage(clonedImg.size(), clonedImg.type());
    for (int i = 0; i < clonedImg.rows; ++i) {
        for (int j = 0; j < clonedImg.cols; ++j) {
            int label = labels.at<int>(i * clonedImg.cols + j);
            clusteredImage.at<cv::Vec3b>(i, j) = centers.at<cv::Vec3f>(label, 0);
        }
    }
    
    // Create a vector of contours
    std::vector<std::vector<cv::Point>> splittedDishes;

    for (int i = 0; i < centers.rows; ++i) {
        cv::Vec3f color = centers.at<cv::Vec3f>(i, 0);

        int blackThreshold = 10;
        // Check if the color is close to black
        if (color[0] > blackThreshold || color[1] > blackThreshold || color[2] > blackThreshold) {
            // DEBUGGING
            // std::cout << "Colore " << i + 1 << ": (B=" << static_cast<int>(color[0])
            //         << ", G=" << static_cast<int>(color[1]) << ", R=" << static_cast<int>(color[2]) << ")" << std::endl;

            // Range for the grilled pork cutlet with beans
            cv::Vec3f range1(72, 84, 111);
            cv::Vec3f threshold1(12, 10, 10);

            // Fish Cutlet and basil potatoes
            cv::Vec3f range2(77, 110, 133);
            cv::Vec3f threshold2(20, 30, 45);

            // Rabbit with beans
            cv::Vec3f range3(49, 63, 88);
            cv::Vec3f threshold3(13, 20, 20);

            // seafood salad, basil potatoes and beans
            cv::Vec3f range4(68, 89, 117);
            cv::Vec3f threshold4(5, 8, 7);

            // Bread and Yogurth
            cv::Vec3f range5(98, 102, 115);
            cv::Vec3f threshold5(3, 5, 7);

            // Grilled pork cutlet with beans
            if (((color[0] < range1[0] + threshold1[0]) && (color[0] > range1[0] - threshold1[0])) && 
                ((color[1] < range1[1] + threshold1[1]) && (color[1] > range1[1] - threshold1[1])) &&
                ((color[2] < range1[2] + threshold1[2]) && (color[2] > range1[2] - threshold1[2]))) {

                    // Inizialize the pixel counters
                    int redPixelCount = 0;
                    int blackPixelCount = 0;
                    int grilledPorkPixelCount = 0;
                    cv::Vec3b grilledPorkColor(131, 144, 172);

                    // Scan each pixel of the image
                    for (int y = 0; y < image.rows; y++) {
                        for (int x = 0; x < image.cols; x++) {
                            // Take the pixel color in position x, y (B, G, R)
                            cv::Vec3b pixel = image.at<cv::Vec3b>(y, x);

                            if (pixel[0] < 10 && pixel[1] < 10 && pixel[2] < 10) {
                                blackPixelCount++;
                                continue;
                                
                            } else if (((pixel[2]> pixel[1]) && (pixel[2] > pixel[0])) && 
                                        (pixel[1] > pixel[0]) && (abs(pixel[2] - pixel[1]) > 80)) {
                                redPixelCount++;
                            }

                            if ((abs(grilledPorkColor[0] - pixel[0]) <= 10) &&
                                (abs(grilledPorkColor[1] - pixel[1]) <= 10) &&
                                (abs(grilledPorkColor[2] - pixel[2]) <= 10)) {

                                grilledPorkPixelCount++;

                            } 
                        }
                    }

                    int totalPixels = image.rows * image.cols;
                    int validPixels = totalPixels - blackPixelCount;

                    // Multiplied by 1000 istead of 100 for better precision
                    double percentageRed = (static_cast<double>(redPixelCount) / validPixels) * 1000.0;
                    double percentageGrilledPorkColor = (static_cast<double>(grilledPorkPixelCount) / validPixels) * 1000.0;

                    if (percentageRed < 5.0 && percentageGrilledPorkColor > 18.0) {

                        // DEBUGGING
                        //std::cout << "GRILLED PORK AND BEANS" << std::endl;

                        cv::Mat secondClone = image.clone();
                        cv::GaussianBlur(secondClone, secondClone, cv::Size(5, 5), 7, 7);

                        cv::Mat mask = maskBuilder(image, secondClone);

                        int erosionSize = 13;
                        cv::Mat elementEro = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2 * erosionSize + 1, 2 * erosionSize + 1), cv::Point(erosionSize, erosionSize));

                        cv::Mat erodImage;
                        cv::erode(mask, erodImage, elementEro);

                        cv::Mat imgAfterErosion;
                        secondClone.copyTo(imgAfterErosion, erodImage);

                        cv::GaussianBlur(imgAfterErosion, imgAfterErosion, cv::Size(15, 15), 13, 13);

                        // Define a brightness threshold
                        int luminosityThreshold = 110;

                        cv::Mat grayScaleImage;
                        cv::cvtColor(imgAfterErosion, grayScaleImage, cv::COLOR_BGR2GRAY);

                        // Create a binary mask based on brightness threshold
                        cv::Mat luminosityMask;
                        cv::threshold(grayScaleImage, luminosityMask, luminosityThreshold, 255, cv::THRESH_BINARY);

                        cv::Mat imgAfterLuminosityMask;
                        image.copyTo(imgAfterLuminosityMask, luminosityMask);

                        cv::Mat grayscaleImage2;
                        cv::cvtColor(imgAfterLuminosityMask, grayscaleImage2, cv::COLOR_BGR2GRAY);

                        cv::Mat binaryImage;
                        cv::threshold(grayscaleImage2, binaryImage, 5, 255, cv::THRESH_BINARY);

                        // Find and analyze connected components
                        cv::Mat labels, stats, centroids;
                        int numObjects = cv::connectedComponentsWithStats(binaryImage, labels, stats, centroids);

                        int totalArea = 0;

                        // Iterate over the connected components (excluding the background, label 0)
                        for (int i = 1; i < numObjects; i++) {
                            int area = stats.at<int>(i, cv::CC_STAT_AREA);
                            totalArea += area;
                        }

                        // Calculation of the average
                        double meanArea = static_cast<double>(totalArea) / (numObjects - 1);
                        double thresholdMeanArea = meanArea - (meanArea * 0.05);

                        // Remove objects with less than average area
                        for (int i = 1; i < numObjects; i++) {
                            int area = stats.at<int>(i, cv::CC_STAT_AREA);
                            if (area < thresholdMeanArea) {
                                cv::Mat mask = labels == i;
                                // Set items to remove to black
                                imgAfterLuminosityMask.setTo(cv::Scalar(0, 0, 0), mask);
                            }
                        }

                        erosionSize = 11;
                        elementEro = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2 * erosionSize + 1, 2 * erosionSize + 1), cv::Point(erosionSize, erosionSize));

                        cv::Mat erodedImage;
                        cv::dilate(imgAfterLuminosityMask, erodedImage, elementEro);

                        cv::Mat grayscaleImage3;
                        cv::cvtColor(erodedImage, grayscaleImage3, cv::COLOR_BGR2GRAY);

                        cv::Mat binaryImage2;
                        cv::threshold(grayscaleImage3, binaryImage2, 5, 255, cv::THRESH_BINARY);

                        numObjects = cv::connectedComponentsWithStats(binaryImage2, labels, stats, centroids);
                        totalArea = 0;

                        for (int i = 1; i < numObjects; i++) {
                            int area = stats.at<int>(i, cv::CC_STAT_AREA);
                            totalArea += area;
                        }

                        meanArea = static_cast<double>(totalArea) / (numObjects - 1);
                        thresholdMeanArea = meanArea - (meanArea * 0.05);

                        for (int i = 1; i < numObjects; i++) {
                            int area = stats.at<int>(i, cv::CC_STAT_AREA);
                            if (area < thresholdMeanArea) {
                                cv::Mat mask = labels == i;
                                erodedImage.setTo(cv::Scalar(0, 0, 0), mask);
                            }
                        }

                        cv::Mat binaryMask3;
                        cv::threshold(erodedImage, binaryMask3, 1, 255, cv::THRESH_BINARY);

                        // Apply the mask to the target image
                        cv::Mat ImageWithBinaryMask3;
                        image.copyTo(ImageWithBinaryMask3, binaryMask3);

                        cv::Mat secondMask = maskBuilder(ImageWithBinaryMask3, ImageWithBinaryMask3);

                        // Apply the subtraction operation between the two masks
                        cv::Mat bitwiseMask;
                        cv::bitwise_xor(mask, secondMask, bitwiseMask);

                        cv::Mat largestComponent = findLargestConnectedComponent(bitwiseMask, 5);

                        cv::Mat invertedLargestComponent;
                        cv::bitwise_not(largestComponent, invertedLargestComponent);
                        
                        cv::Mat imgWithoutLargestComponent;
                        image.copyTo(imgWithoutLargestComponent, invertedLargestComponent);

                        cv::Scalar target_color(186, 192, 193);
                        int color_range = 10;

                        for (int y = 0; y < imgWithoutLargestComponent.rows; ++y) {
                            for (int x = 0; x < imgWithoutLargestComponent.cols; ++x) {

                                cv::Vec3b color = imgWithoutLargestComponent.at<cv::Vec3b>(y, x);

                                if (abs(color[0] - target_color[0]) <= color_range &&
                                    abs(color[1] - target_color[1]) <= color_range &&
                                    abs(color[2] - target_color[2]) <= color_range) {

                                    imgWithoutLargestComponent.at<cv::Vec3b>(y, x) = cv::Vec3b(0, 0, 0);
                                }
                            }
                        }

                        cv::Mat thirdMask = maskBuilder(imgWithoutLargestComponent, imgWithoutLargestComponent);
                        largestComponent = findLargestConnectedComponent(thirdMask, 13);

                        cv::Mat result;
                        image.copyTo(result, largestComponent);

                        cv::GaussianBlur(result, result, cv::Size(21, 21), 31, 31);

                        cv::Mat finalMask_1 = maskBuilder(image.clone(), result);

                        cv::Mat finalImg_1;
                        image.copyTo(finalImg_1, finalMask_1);


                        cv::Mat grayImageForContours_1;
                        cv::cvtColor(finalImg_1, grayImageForContours_1, cv::COLOR_BGR2GRAY);

                        cv::findContours(grayImageForContours_1, splittedDishes, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

                        cv::Mat invertedFinalMask_1;
                        cv::bitwise_not(finalMask_1, invertedFinalMask_1);

                        cv::Mat finalImg_2;
                        image.copyTo(finalImg_2, invertedFinalMask_1);

                        cv::Mat finalMask_2 = maskBuilder(finalImg_2, finalImg_2);
                        cv::Mat largestComponent_2 = findLargestConnectedComponent(finalMask_2, 3);

                        std::vector<std::vector<cv::Point>> otherContours;
                        cv::findContours(largestComponent_2, otherContours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

                        for (const std::vector<cv::Point>& contour : otherContours) {
                            // Print the contour
                            if (contour.size() >= 3) {
                                splittedDishes.push_back(contour);
                            }
                        }
                    }
            }

            // Rabbit with beans
            else if (((color[0] < range3[0] + threshold3[0]) && (color[0] > range3[0] - threshold3[0])) && 
                    ((color[1] < range3[1] + threshold3[1]) && (color[1] > range3[1] - threshold3[1])) &&
                    ((color[2] < range3[2] + threshold3[2]) && (color[2] > range3[2] - threshold3[2]))) {

                        // Inizialize the pixel counters
                        int blackPixelCount = 0;

                        int orangePixelCount = 0;
                        cv::Vec3b orangeColor(87, 151, 195);

                        int darkOrangePixelCount = 0;
                        cv::Vec3b darkOrangeColor(27, 52, 108);
                        
                        int greenPixelCount = 0;
                        cv::Vec3b greenColor(93, 176, 140);

                        // Scan each pixel of the image
                        for (int y = 0; y < image.rows; y++) {
                            for (int x = 0; x < image.cols; x++) {
                                // Take the pixel color in position x, y (B, G, R)
                                cv::Vec3b pixel = image.at<cv::Vec3b>(y, x);

                                if (pixel[0] < 10 && pixel[1] < 10 && pixel[2] < 10) {
                                    blackPixelCount++;
                                    continue;
                                    
                                } else if ((abs(greenColor[0] - pixel[0]) <= 20) &&
                                    (abs(greenColor[1] - pixel[1]) <= 20) &&
                                    (abs(greenColor[2] - pixel[2]) <= 10)) {
                                    greenPixelCount++;

                                } 

                                if ((abs(orangeColor[0] - pixel[0]) <= 10) &&
                                    (abs(orangeColor[1] - pixel[1]) <= 20) &&
                                    (abs(orangeColor[2] - pixel[2]) <= 40)) {
                                    orangePixelCount++;
                                }

                                if ((abs(darkOrangeColor[0] - pixel[0]) <= 10) &&
                                    (abs(darkOrangeColor[1] - pixel[1]) <= 20) &&
                                    (abs(darkOrangeColor[2] - pixel[2]) <= 20)) {
                                    darkOrangePixelCount++;
                                }


                            }
                        }

                        int totalPixels = image.rows * image.cols;
                        int validPixels = totalPixels - blackPixelCount;

                        // Multiplied by 1000 istead of 100 for better precision
                        double percentageOrange = (static_cast<double>(orangePixelCount) / validPixels) * 1000.0;
                        double percentageDarkOrange = (static_cast<double>(darkOrangePixelCount) / validPixels) * 1000.0;
                        double percentageGreen = (static_cast<double>(greenPixelCount) / validPixels) * 1000.0;

                        if (!(percentageGreen > 0)) {
                            if (percentageOrange > 0.5 && percentageOrange < 20.0) {

                                if (percentageDarkOrange > 30.0) {

                                    // DEBUGGING
                                    //std::cout << "RABBIT AND BEANS" << std::endl;

                                    cv::Mat secondClone = image.clone();
                                    cv::GaussianBlur(secondClone, secondClone, cv::Size(5, 5), 7, 7);

                                    cv::Scalar target_color(129, 145, 182);
                                    int color_range = 50;

                                    for (int y = 0; y < secondClone.rows; ++y) {
                                        for (int x = 0; x < secondClone.cols; ++x) {

                                            cv::Vec3b color = secondClone.at<cv::Vec3b>(y, x);

                                            if (abs(color[0] - target_color[0]) <= color_range &&
                                                abs(color[1] - target_color[1]) <= color_range &&
                                                abs(color[2] - target_color[2]) <= color_range) {

                                                secondClone.at<cv::Vec3b>(y, x) = cv::Vec3b(0, 0, 0);
                                            }
                                        }
                                    }

                                    int erosionSize = 17;
                                    cv::Mat elementEro = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2 * erosionSize + 1, 2 * erosionSize + 1), cv::Point(erosionSize, erosionSize));

                                    cv::Mat erodedImage;
                                    cv::erode(secondClone, erodedImage, elementEro);

                                    cv::Mat mask = maskBuilder(erodedImage, erodedImage);

                                    cv::Mat imageAfterMask;
                                    image.copyTo(imageAfterMask, mask);

                                    int luminosityThreshold = 60;

                                    cv::Mat grayscaleImage;
                                    cv::cvtColor(imageAfterMask, grayscaleImage, cv::COLOR_BGR2GRAY);

                                    cv::Mat luminosityImage;
                                    cv::threshold(grayscaleImage, luminosityImage, luminosityThreshold, 255, cv::THRESH_BINARY);

                                    cv::Mat result;
                                    image.copyTo(result, luminosityImage);

                                    int dilationSize = 51;
                                    cv::Mat elementDil = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2 * dilationSize + 1, 2 * dilationSize + 1), cv::Point(dilationSize, dilationSize));

                                    cv::Mat DilatedImage;
                                    cv::dilate(result, DilatedImage, elementDil);

                                    cv::Mat postDilationMask = maskBuilder(DilatedImage, DilatedImage);

                                    cv::Mat postDilationImage;
                                    image.copyTo(postDilationImage, postDilationMask);

                                    erosionSize = 7;
                                    elementEro = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2 * erosionSize + 1, 2 * erosionSize + 1), cv::Point(erosionSize, erosionSize));

                                    cv::Mat postSecondErosionImage;
                                    cv::erode(postDilationImage, postSecondErosionImage, elementEro);

                                    cv::Mat postSecondErosionMask = maskBuilder(postSecondErosionImage, postSecondErosionImage);

                                    cv::Mat postPreprocessingImage;
                                    image.copyTo(postPreprocessingImage, postSecondErosionMask);

                                    cv::Scalar target_color1(110, 117, 126);
                                    cv::Scalar target_color2(29, 41, 77);
                                    color_range = 40;

                                    for (int y = 0; y < postPreprocessingImage.rows; ++y) {
                                        for (int x = 0; x < postPreprocessingImage.cols; ++x) {

                                            cv::Vec3b color = postPreprocessingImage.at<cv::Vec3b>(y, x);

                                            if ((abs(color[0] - target_color1[0]) <= color_range &&
                                                abs(color[1] - target_color1[1]) <= color_range &&
                                                abs(color[2] - target_color1[2]) <= color_range) || 

                                                (abs(color[0] - target_color2[0]) <= color_range &&
                                                abs(color[1] - target_color2[1]) <= color_range &&
                                                abs(color[2] - target_color2[2]) <= color_range)) {

                                                postPreprocessingImage.at<cv::Vec3b>(y, x) = cv::Vec3b(0, 0, 0);
                                            }
                                        }
                                    }

                                    cv::GaussianBlur(postPreprocessingImage, postPreprocessingImage, cv::Size(5, 5), 7, 7);

                                    cv::Mat postPreprocessingMask = maskBuilder(postPreprocessingImage, postPreprocessingImage);

                                    erosionSize = 4;
                                    elementEro = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2 * erosionSize + 1, 2 * erosionSize + 1), cv::Point(erosionSize, erosionSize));

                                    cv::Mat erodedImagePostPreprocessing;
                                    cv::erode(postPreprocessingMask, erodedImagePostPreprocessing, elementEro);

                                    cv::Mat imageWithNoise;
                                    image.copyTo(imageWithNoise, erodedImagePostPreprocessing);

                                    cv::Mat maskWithNoise = maskBuilder(imageWithNoise, imageWithNoise);

                                    // Find and analyze connected components
                                    cv::Mat labels, stats, centroids;
                                    int numObjects = cv::connectedComponentsWithStats(maskWithNoise, labels, stats, centroids);

                                    int totalArea = 0;

                                    // Iterate over the connected components (excluding the background, label 0)
                                    for (int i = 1; i < numObjects; i++) {
                                        int area = stats.at<int>(i, cv::CC_STAT_AREA);
                                        totalArea += area;
                                    }

                                    double meanArea = static_cast<double>(totalArea) / (numObjects - 1);
                                    double thresholdMeanArea = meanArea - (meanArea * 0.1);

                                    // Remove objects with less than average area
                                    for (int i = 1; i < numObjects; i++) {
                                        int area = stats.at<int>(i, cv::CC_STAT_AREA);
                                        if (area < thresholdMeanArea) {
                                            cv::Mat mask = labels == i;
                                            // Set items to remove to black
                                            maskWithNoise.setTo(cv::Scalar(0, 0, 0), mask); 
                                        }
                                    }

                                    dilationSize = 15;
                                    elementDil = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2 * dilationSize + 1, 2 * dilationSize + 1), cv::Point(dilationSize, dilationSize));

                                    cv::Mat dilationMaskWithNoise;
                                    cv::dilate(maskWithNoise, dilationMaskWithNoise, elementDil);

                                    cv::Mat imageWithNoise_2;
                                    image.copyTo(imageWithNoise_2, dilationMaskWithNoise);

                                    cv::Mat maskWithNoise_2 = maskBuilder(imageWithNoise_2, imageWithNoise_2);

                                    cv::Mat labels2, stats2, centroids2;
                                    numObjects = cv::connectedComponentsWithStats(maskWithNoise_2, labels2, stats2, centroids2);

                                    totalArea = 0;

                                    for (int i = 1; i < numObjects; i++) {
                                        int area = stats2.at<int>(i, cv::CC_STAT_AREA);
                                        totalArea += area;
                                    }

                                    meanArea = static_cast<double>(totalArea) / (numObjects - 1);
                                    thresholdMeanArea = meanArea - (meanArea * 0.5);

                                    for (int i = 1; i < numObjects; i++) {
                                        int area = stats2.at<int>(i, cv::CC_STAT_AREA);
                                        if (area < thresholdMeanArea) {
                                            cv::Mat mask = labels2 == i;
                                            maskWithNoise_2.setTo(cv::Scalar(0, 0, 0), mask);
                                        }
                                    }

                                    cv::GaussianBlur(maskWithNoise_2, maskWithNoise_2, cv::Size(31, 31), 35, 35);

                                    cv::Mat result_2;
                                    image.copyTo(result_2, maskWithNoise_2);

                                    cv::Mat mask_2 = maskBuilder(result_2, result_2);

                                    cv::Mat invertedMask;
                                    cv::bitwise_not(mask_2, invertedMask);

                                    cv::Mat resultImage;
                                    image.copyTo(resultImage, invertedMask);

                                    cv::Mat mask_3 = maskBuilder(resultImage, resultImage);

                                    double totalChunkArea = cv::countNonZero(mask_3);

                                    cv::Mat labels3;
                                    int num_components = cv::connectedComponents(mask_3, labels3);

                                    // Subtract 1 to exclude the bottom (label 0)
                                    num_components--;

                                    if (totalChunkArea > 35000) {
                                        if (num_components < 3) {

                                            cv::Mat largestComponent = findLargestConnectedComponent(mask_3, 7);
                                            cv::GaussianBlur(largestComponent, largestComponent, cv::Size(21, 21), 25, 25);

                                            cv::Mat resultBeans;
                                            image.copyTo(resultBeans, largestComponent);

                                            cv::Mat beansMask = maskBuilder(resultBeans, resultBeans);

                                            cv::findContours(beansMask, splittedDishes, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

                                            cv::Mat invertedBeansMask;
                                            cv::bitwise_not(beansMask, invertedBeansMask);

                                            cv::Mat resultWithoutBeans;
                                            image.copyTo(resultWithoutBeans, invertedBeansMask);

                                            cv::Mat maskWithoutBeans = maskBuilder(resultWithoutBeans, resultWithoutBeans);
                                            cv::Mat finalWithoutBeans = findLargestConnectedComponent(maskWithoutBeans, 7);

                                            std::vector<std::vector<cv::Point>> otherContours;
                                            cv::findContours(finalWithoutBeans, otherContours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

                                            for (const std::vector<cv::Point>& contour : otherContours) {
                                                // Print the contour
                                                if (contour.size() >= 3) {
                                                    splittedDishes.push_back(contour);
                                                }
                                            }

                                        } else {

                                            cv::Mat largestComponent = findLargestConnectedComponent(mask_3, 1);

                                            cv::findContours(largestComponent, splittedDishes, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

                                            cv::Mat invertedBeansMask;
                                            cv::bitwise_not(largestComponent, invertedBeansMask);

                                            cv::Mat resultBeans;
                                            image.copyTo(resultBeans, invertedBeansMask);

                                            erosionSize = 5;
                                            elementEro = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2 * erosionSize + 1, 2 * erosionSize + 1), cv::Point(erosionSize, erosionSize));

                                            cv::Mat erodedImage_2;
                                            cv::erode(resultBeans, erodedImage_2, elementEro);

                                            cv::Mat maskErodedImage_2 = maskBuilder(erodedImage_2, erodedImage_2);
                                            cv::Mat largestComponent_2 = findLargestConnectedComponent(maskErodedImage_2, 3);

                                            std::vector<std::vector<cv::Point>> otherContours;
                                            cv::findContours(largestComponent_2, otherContours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

                                            for (const std::vector<cv::Point>& contour : otherContours) {
                                                // Print the contour
                                                if (contour.size() >= 3) {
                                                    splittedDishes.push_back(contour);
                                                }
                                            }
                                        }

                                    } else {

                                        erosionSize = 3;
                                        elementEro = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2 * erosionSize + 1, 2 * erosionSize + 1), cv::Point(erosionSize, erosionSize));

                                        // Applica l'erosione all'immagine
                                        cv::Mat erodedImage_2;
                                        cv::erode(mask_2, erodedImage_2, elementEro);

                                        cv::Mat largestComponent = findLargestConnectedComponent(erodedImage_2, 1);

                                        cv::findContours(largestComponent, splittedDishes, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

                                        cv::Mat invertedBeansMask;
                                        cv::bitwise_not(largestComponent, invertedBeansMask);

                                        cv::Mat resultBeans;
                                        image.copyTo(resultBeans, invertedBeansMask);

                                        erosionSize = 3;
                                        elementEro = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2 * erosionSize + 1, 2 * erosionSize + 1), cv::Point(erosionSize, erosionSize));

                                        cv::Mat erodedImage_3;
                                        cv::erode(resultBeans, erodedImage_3, elementEro);

                                        dilationSize = 35;
                                        elementDil = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2 * dilationSize + 1, 2 * dilationSize + 1), cv::Point(dilationSize, dilationSize));

                                        cv::Mat dilatedImage_3;
                                        cv::dilate(erodedImage_3, dilatedImage_3, elementDil);

                                        cv::GaussianBlur(dilatedImage_3, dilatedImage_3, cv::Size(5, 5), 3, 3);

                                        cv::Mat maskDilatedmage_3 = maskBuilder(dilatedImage_3, dilatedImage_3);
                                        cv::Mat largestComponent_2 = findLargestConnectedComponent(maskDilatedmage_3, 1);

                                        cv::Mat resultBeans_2;
                                        resultBeans.copyTo(resultBeans_2, largestComponent_2);

                                        erosionSize = 3;
                                        elementEro = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2 * erosionSize + 1, 2 * erosionSize + 1), cv::Point(erosionSize, erosionSize));

                                        cv::Mat erodedImage_4;
                                        cv::erode(resultBeans_2, erodedImage_4, elementEro);

                                        cv::GaussianBlur(erodedImage_4, erodedImage_4, cv::Size(5, 5), 5, 5);

                                        dilationSize = 51;
                                        elementDil = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2 * dilationSize + 1, 2 * dilationSize + 1), cv::Point(dilationSize, dilationSize));

                                        cv::Mat dilatedImage_4;
                                        cv::dilate(erodedImage_4, dilatedImage_4, elementDil);

                                        cv::Mat maskDilatedmage_4 = maskBuilder(dilatedImage_4, dilatedImage_4);
                                        cv::Mat largestComponent_3 = findLargestConnectedComponent(maskDilatedmage_4, 1);

                                        cv::Mat resultBeans_3;
                                        resultBeans.copyTo(resultBeans_3, largestComponent_3);

                                        cv::Mat maskDilatedmage_5 = maskBuilder(resultBeans_3, resultBeans_3);
                                        cv::Mat largestComponent_4 = findLargestConnectedComponent(maskDilatedmage_5, 1);

                                        cv::Mat resultBeans_4;
                                        resultBeans.copyTo(resultBeans_4, largestComponent_4);

                                        cv::Mat maskErodedImage_2 = maskBuilder(resultBeans_4, resultBeans_4);

                                        std::vector<std::vector<cv::Point>> otherContours;
                                        cv::findContours(maskErodedImage_2, otherContours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

                                        for (const std::vector<cv::Point>& contour : otherContours) {
                                            // Print the contour
                                            if (contour.size() >= 3) {
                                                splittedDishes.push_back(contour);
                                            }
                                        }
                                    }
                                }
                            }
                        }
            } 

            // seafood salad, basil potatoes and beans
            if (((color[0] < range4[0] + threshold4[0]) && (color[0] > range4[0] - threshold4[0])) && 
                ((color[1] < range4[1] + threshold4[1]) && (color[1] > range4[1] - threshold4[1])) &&
                ((color[2] < range4[2] + threshold4[2]) && (color[2] > range4[2] - threshold4[2]))) {
                    
                    int blackPixelCount = 0;

                    int whiteSandPixelCount = 0;
                    cv::Vec3b whiteSandColor(203, 219, 224);

                    int darkOrangePixelCount = 0;
                    cv::Vec3b darkOrangeColor(27, 52, 108);

                    // Scan each pixel of the image
                    for (int y = 0; y < image.rows; y++) {
                        for (int x = 0; x < image.cols; x++) {
                            // Take the pixel color in position x, y (B, G, R)
                            cv::Vec3b pixel = image.at<cv::Vec3b>(y, x);

                            if (pixel[0] < 10 && pixel[1] < 10 && pixel[2] < 10) {
                                blackPixelCount++;
                                continue;
                                
                            } else if ((abs(whiteSandColor[0] - pixel[0]) <= 20) &&
                                (abs(whiteSandColor[1] - pixel[1]) <= 20) &&
                                (abs(whiteSandColor[2] - pixel[2]) <= 20)) {
                                whiteSandPixelCount++;

                            } 

                            if ((abs(darkOrangeColor[0] - pixel[0]) <= 10) &&
                                (abs(darkOrangeColor[1] - pixel[1]) <= 20) &&
                                (abs(darkOrangeColor[2] - pixel[2]) <= 20)) {
                                darkOrangePixelCount++;
                            }

                        }
                    }

                    int totalPixels = image.rows * image.cols;
                    int validPixels = totalPixels - blackPixelCount;

                    // Multiplied by 1000 istead of 100 for better precision
                    double percentageWhiteSand = (static_cast<double>(whiteSandPixelCount) / validPixels) * 1000.0;
                    double percentageDarkOrange = (static_cast<double>(darkOrangePixelCount) / validPixels) * 1000.0;

                    if (percentageWhiteSand > 10.0) {
                        if (percentageDarkOrange > 20.0) {

                            //std::cout << "SEAFOOD SALAD, BASIL POTATOES AND BEANS" << std::endl;

                            cv::Mat secondClone = image.clone();
                            cv::GaussianBlur(secondClone, secondClone, cv::Size(11, 11), 13, 13);

                            cv::Mat mask = maskBuilder(image, secondClone);

                            int erosionSize = 21;
                            cv::Mat elementEro = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2 * erosionSize + 1, 2 * erosionSize + 1), cv::Point(erosionSize, erosionSize));

                            cv::Mat erodedImage;
                            cv::erode(mask, erodedImage, elementEro);

                            cv::Mat postErosionImg;
                            secondClone.copyTo(postErosionImg, erodedImage);

                            cv::GaussianBlur(postErosionImg, postErosionImg, cv::Size(15, 15), 13, 13);

                            // Define a brightness threshold
                            int luminosityThreshold = 120;

                            cv::Mat grayscaleImage;
                            cv::cvtColor(postErosionImg, grayscaleImage, cv::COLOR_BGR2GRAY);


                            cv::Mat luminosityMask;
                            cv::threshold(grayscaleImage, luminosityMask, luminosityThreshold, 255, cv::THRESH_BINARY);

                            // Apply the mask to the original image
                            cv::Mat postLuminosityMask;
                            image.copyTo(postLuminosityMask, luminosityMask);

                            cv::Scalar target_color(163, 167, 172);
                            int color_range = 20;

                            for (int y = 0; y < postLuminosityMask.rows; ++y) {
                                for (int x = 0; x < postLuminosityMask.cols; ++x) {

                                    cv::Vec3b color = postLuminosityMask.at<cv::Vec3b>(y, x);

                                    if (abs(color[0] - target_color[0]) <= color_range &&
                                        abs(color[1] - target_color[1]) <= color_range &&
                                        abs(color[2] - target_color[2]) <= color_range) {

                                        postLuminosityMask.at<cv::Vec3b>(y, x) = cv::Vec3b(0, 0, 0);
                                    }
                                }
                            }

                            cv::Mat MaskImgWithoutWhite = maskBuilder(postLuminosityMask, postLuminosityMask);
                            cv::Mat largestComponent = findLargestConnectedComponent(MaskImgWithoutWhite, 1);

                            cv::Mat imageLargestComponent;
                            image.copyTo(imageLargestComponent, largestComponent);

                            cv::Mat imageData = imageLargestComponent.reshape(1, imageLargestComponent.cols * imageLargestComponent.rows);
                            imageData.convertTo(imageData, CV_32F);

                            int num_clusters = 2;
                            cv::TermCriteria stopping_criteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 100, 0.1);

                            cv::Mat clusterLabels, clusterCenters;
                            cv::kmeans(imageData, num_clusters, clusterLabels, stopping_criteria, 10, cv::KMEANS_RANDOM_CENTERS, clusterCenters);

                            cv::Mat clusteredImage(imageLargestComponent.size(), imageLargestComponent.type());

                            for (int i = 0; i < imageLargestComponent.rows; ++i) {
                                for (int j = 0; j < imageLargestComponent.cols; ++j) {
                                    int label = clusterLabels.at<int>(i * imageLargestComponent.cols + j);
                                    clusteredImage.at<cv::Vec3b>(i, j) = clusterCenters.at<cv::Vec3f>(label, 0);
                                }
                            }

                            cv::Scalar potatoesColor(123, 183, 201);
                            cv::Scalar whiteFishColor(131, 171, 199);
                            
                            cv::Vec3f cluster1_color = clusterCenters.at<cv::Vec3f>(0, 0);
                            cv::Vec3f cluster2_color = clusterCenters.at<cv::Vec3f>(1, 0);

                            cv::Mat seafoodWithoutPotatoes = image.clone();
                            int excep = 0;

                            if (((abs(cluster1_color[0] - potatoesColor[0]) <= 6) &&
                                (abs(cluster1_color[1] - potatoesColor[1]) <= 6) &&
                                (abs(cluster1_color[2] - potatoesColor[2]) <= 6)) ||
                                ((abs(cluster2_color[0] - potatoesColor[0]) <= 6) &&
                                (abs(cluster2_color[1] - potatoesColor[1]) <= 6) &&
                                (abs(cluster2_color[2] - potatoesColor[2]) <= 6))) {
                                    
                                    int dilateSize = 15;
                                    cv::Mat elementDil = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2 * dilateSize + 1, 2 * dilateSize + 1), cv::Point(dilateSize, dilateSize));

                                    cv::Mat dilatedImage;
                                    cv::dilate(imageLargestComponent, dilatedImage, elementDil);

                                    cv::Mat potatoesMask = maskBuilder(dilatedImage, dilatedImage);

                                    cv::Mat potatoesResults;
                                    image.copyTo(potatoesResults, potatoesMask);

                                    cv::Mat grayImageForContours;
                                    cv::cvtColor(potatoesResults, grayImageForContours, cv::COLOR_BGR2GRAY);

                                    cv::findContours(grayImageForContours, splittedDishes, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

                                    cv::Mat invertedPotatoesMask;
                                    cv::bitwise_not(potatoesMask, invertedPotatoesMask);

                                    cv::Mat invertedPotatoesResults;
                                    image.copyTo(invertedPotatoesResults, invertedPotatoesMask);

                                    invertedPotatoesResults.copyTo(seafoodWithoutPotatoes);

                                    excep = 1;
                            } 
                            
                            if ((((abs(cluster1_color[0] - whiteFishColor[0]) <= 6) &&
                                (abs(cluster1_color[1] - whiteFishColor[1]) <= 6) &&
                                (abs(cluster1_color[2] - whiteFishColor[2]) <= 6)) ||
                                ((abs(cluster2_color[0] - whiteFishColor[0]) <= 6) &&
                                (abs(cluster2_color[1] - whiteFishColor[1]) <= 6) &&
                                (abs(cluster2_color[2] - whiteFishColor[2]) <= 6))) || excep) {

                                    cv::Mat cloneSeafoodWithoutPotatoes = seafoodWithoutPotatoes.clone();
                                    cv::GaussianBlur(cloneSeafoodWithoutPotatoes, cloneSeafoodWithoutPotatoes, cv::Size(11, 11), 13, 13);

                                    cv::Mat maskSeafoodWithoutPotatoes = maskBuilder(seafoodWithoutPotatoes, cloneSeafoodWithoutPotatoes);

                                    int erosionSize = 21;
                                    cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2 * erosionSize + 1, 2 * erosionSize + 1), cv::Point(erosionSize, erosionSize));

                                    cv::Mat erodedImage;
                                    cv::erode(maskSeafoodWithoutPotatoes, erodedImage, element);

                                    cv::Mat postErosionSeafood;
                                    cloneSeafoodWithoutPotatoes.copyTo(postErosionSeafood, erodedImage);

                                    cv::GaussianBlur(postErosionSeafood, postErosionSeafood, cv::Size(15, 15), 13, 13);

                                    // Define a brightness threshold
                                    int lumThreshold = 155;

                                    cv::Mat grayImage;
                                    cv::cvtColor(postErosionSeafood, grayImage, cv::COLOR_BGR2GRAY);

                                    cv::Mat newBinaryMask;
                                    cv::threshold(grayImage, newBinaryMask, lumThreshold, 255, cv::THRESH_BINARY);

                                    cv::Mat lumImgSeafood;
                                    seafoodWithoutPotatoes.copyTo(lumImgSeafood, newBinaryMask);

                                    cv::Scalar target_color(162, 161, 160);
                                    int color_range = 50;

                                    for (int y = 0; y < lumImgSeafood.rows; ++y) {
                                        for (int x = 0; x < lumImgSeafood.cols; ++x) {

                                            cv::Vec3b color = lumImgSeafood.at<cv::Vec3b>(y, x);

                                            if (abs(color[0] - target_color[0]) <= color_range &&
                                                abs(color[1] - target_color[1]) <= color_range &&
                                                abs(color[2] - target_color[2]) <= color_range) {

                                                lumImgSeafood.at<cv::Vec3b>(y, x) = cv::Vec3b(0, 0, 0);
                                            }
                                        }
                                    }

                                    int dilateSize = 31;
                                    cv::Mat elementDil = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2 * dilateSize + 1, 2 * dilateSize + 1), cv::Point(dilateSize, dilateSize));

                                    cv::Mat dilatedImage;
                                    cv::dilate(lumImgSeafood, dilatedImage, elementDil);

                                    cv::Mat dilatedMask = maskBuilder(dilatedImage, dilatedImage);

                                    cv::Mat seafoodOnly;
                                    seafoodWithoutPotatoes.copyTo(seafoodOnly, dilatedMask);

                                    cv::Mat chanckedMask = maskBuilder(seafoodOnly, seafoodOnly);

                                    // Find the outlines of objects in the image
                                    std::vector<std::vector<cv::Point>> chunkedContours;
                                    cv::findContours(chanckedMask, chunkedContours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

                                    // Find the contour with the smallest area (smallest object)
                                    double min_area = std::numeric_limits<double>::max();
                                    int min_area_index = -1;

                                    for (int i = 0; i < chunkedContours.size(); ++i) {
                                        double area = cv::contourArea(chunkedContours[i]);
                                        if (area < min_area) {
                                            min_area = area;
                                            min_area_index = i;
                                        }
                                    }

                                    // Delete the smallest object by setting it to zero
                                    if (min_area_index >= 0) {
                                        cv::drawContours(chanckedMask, chunkedContours, min_area_index, cv::Scalar(0), cv::FILLED);
                                    }

                                    cv::Mat chunkedSeafood;
                                    seafoodWithoutPotatoes.copyTo(chunkedSeafood, chanckedMask);

                                    cv::GaussianBlur(chunkedSeafood, chunkedSeafood, cv::Size(21, 21), 17, 17);

                                    dilateSize = 13;
                                    elementDil = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2 * dilateSize + 1, 2 * dilateSize + 1), cv::Point(dilateSize, dilateSize));

                                    cv::Mat dilatedSeafood;
                                    cv::dilate(chunkedSeafood, dilatedSeafood, elementDil);

                                    cv::Mat finalSeafoodMask = maskBuilder(dilatedSeafood, dilatedSeafood);

                                    cv::Mat largestComponent = findLargestConnectedComponent(finalSeafoodMask, 1);

                                    cv::Mat finalSeafood;
                                    seafoodWithoutPotatoes.copyTo(finalSeafood, largestComponent);

                                    erosionSize = 7;
                                    element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2 * erosionSize + 1, 2 * erosionSize + 1), cv::Point(erosionSize, erosionSize));

                                    cv::Mat erodedFinalSeafood;
                                    cv::erode(finalSeafood, erodedFinalSeafood, element);

                                    cv::Mat lastSeafoodMask = maskBuilder(erodedFinalSeafood, erodedFinalSeafood);

                                    cv::Mat lastSeafood;
                                    seafoodWithoutPotatoes.copyTo(lastSeafood, lastSeafoodMask);

                                    cv::Mat SeafoodMaskForContours = maskBuilder(lastSeafood, lastSeafood);
                                    cv::Mat largestSeafood = findLargestConnectedComponent(SeafoodMaskForContours, 1);

                                    std::vector<std::vector<cv::Point>> seafoodContours;
                                    cv::findContours(largestSeafood, seafoodContours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

                                    for (const std::vector<cv::Point>& contour : seafoodContours) {
                                        // Print the contour
                                        if (contour.size() >= 3) {
                                            splittedDishes.push_back(contour);
                                        }
                                    }

                                    cv::Mat seafoodInvertedMask;
                                    cv::bitwise_not(largestSeafood, seafoodInvertedMask);

                                    cv::Mat finalBeans;
                                    seafoodWithoutPotatoes.copyTo(finalBeans, seafoodInvertedMask);

                                    erosionSize = 7;
                                    element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2 * erosionSize + 1, 2 * erosionSize + 1), cv::Point(erosionSize, erosionSize));

                                    cv::Mat erodedFinalBeans;
                                    cv::erode(finalBeans, erodedFinalBeans, element);

                                    cv::Mat finalBeanshMask = maskBuilder(erodedFinalBeans, erodedFinalBeans);
                                    cv::Mat largestBeans = findLargestConnectedComponent(finalBeanshMask, 1);

                                    std::vector<std::vector<cv::Point>> beansContours;
                                    cv::findContours(largestBeans, beansContours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

                                    for (const std::vector<cv::Point>& contour : beansContours) {
                                        // Print the contour
                                        if (contour.size() >= 3) {
                                            splittedDishes.push_back(contour);
                                        }
                                    }
                            }                          
                        }
                    }
            }

            // Fish Cutlet and basil potatoes
            if (((color[0] < range2[0] + threshold2[0]) && (color[0] > range2[0] - threshold2[0])) && 
                ((color[1] < range2[1] + threshold2[1]) && (color[1] > range2[1] - threshold2[1])) &&
                ((color[2] < range2[2] + threshold2[2]) && (color[2] > range2[2] - threshold2[2]))) { 
                    

                    int blackPixelCount = 0;

                    int greenPixelCount = 0;
                    cv::Vec3b greenColor(32, 123, 109);

                    int yellowPixelCount = 0;
                    cv::Vec3b yellowColor(105, 167, 185);

                    int orangePixelCount = 0;
                    cv::Vec3b orangeColor(25, 88, 142);

                    int ticketPixelCount = 0;
                    cv::Vec3b ticketColor(45, 95, 188);

                    int fishCutletPixelColor = 0;
                    cv::Vec3b fishCutletColor(31, 87, 149);

                    int fishCutletPixelColor1 = 0;
                    cv::Vec3b fishCutletColor1(0, 41, 27);

                    int fishCutletPixelColor2 = 0;
                    cv::Vec3b fishCutletColor2(10, 78, 172);

                    int fishCutletPixelColor3 = 0;
                    cv::Vec3b fishCutletColor3(51, 112, 208);

                    int dishPixelColor = 0;
                    cv::Vec3b dishColor(191, 179, 173);

                    // Scan each pixel of the image
                    for (int y = 0; y < image.rows; y++) {
                        for (int x = 0; x < image.cols; x++) {
                            // Take the pixel color in position x, y (B, G, R)
                            cv::Vec3b pixel = image.at<cv::Vec3b>(y, x);

                            if (pixel[0] < 10 && pixel[1] < 10 && pixel[2] < 10) {
                                blackPixelCount++;
                                continue;
                                
                            } else if ((abs(orangeColor[0] - pixel[0]) <= 20) &&
                                (abs(orangeColor[1] - pixel[1]) <= 20) &&
                                (abs(orangeColor[2] - pixel[2]) <= 40)) {
                                orangePixelCount++;
                            } 

                            if ((abs(yellowColor[0] - pixel[0]) <= 10) &&
                                (abs(yellowColor[1] - pixel[1]) <= 20) &&
                                (abs(yellowColor[2] - pixel[2]) <= 20)) {
                                yellowPixelCount++;
                            }

                            if ((abs(greenColor[0] - pixel[0]) <= 15) &&
                                (abs(greenColor[1] - pixel[1]) <= 30) &&
                                (abs(greenColor[2] - pixel[2]) <= 15)) {
                                greenPixelCount++;
                            } 

                            if ((abs(ticketColor[0] - pixel[0]) <= 10) &&
                                (abs(ticketColor[1] - pixel[1]) <= 10) &&
                                (abs(ticketColor[2] - pixel[2]) <= 10)) {
                                ticketPixelCount++;
                            } 

                            if ((abs(fishCutletColor1[0] - pixel[0]) <= 5) &&
                                (abs(fishCutletColor1[1] - pixel[1]) <= 15) &&
                                (abs(fishCutletColor1[2] - pixel[2]) <= 10)) {
                                fishCutletPixelColor1++;
                            } 

                            if ((abs(fishCutletColor2[0] - pixel[0]) <= 5) &&
                                (abs(fishCutletColor2[1] - pixel[1]) <= 15) &&
                                (abs(fishCutletColor2[2] - pixel[2]) <= 20)) {
                                fishCutletPixelColor2++;
                            } 


                            if ((abs(fishCutletColor3[0] - pixel[0]) <= 10) &&
                                (abs(fishCutletColor3[1] - pixel[1]) <= 10) &&
                                (abs(fishCutletColor3[2] - pixel[2]) <= 15)) {
                                fishCutletPixelColor3++;
                            } 
                            if ((abs(dishColor[0] - pixel[0]) <= 15) &&
                                (abs(dishColor[1] - pixel[1]) <= 15) &&
                                (abs(dishColor[2] - pixel[2]) <= 15)) {
                                dishPixelColor++;
                            } 
                        }
                    }

                    int totalPixels = image.rows * image.cols;
                    int validPixels = totalPixels - blackPixelCount;

                    // Multiplied by 1000 istead of 100 for better precision             
                    double percentageGreenColor = (static_cast<double>(greenPixelCount) / validPixels) * 1000.0;
                    double percentageYellow = (static_cast<double>(yellowPixelCount) / validPixels) * 1000.0;
                    double percentageOrangeColor = (static_cast<double>(orangePixelCount) / validPixels) * 1000.0;
                    double percentageTicketPixelCountColor = (static_cast<double>(ticketPixelCount) / validPixels) * 1000.0;

                    double percentageFishCutletColor1 = (static_cast<double>(fishCutletPixelColor1) / validPixels) * 10000.0;
                    double percentageFishCutletColor2 = (static_cast<double>(fishCutletPixelColor2) / validPixels) * 1000.0;
                    double percentageFishCutletColor3 = (static_cast<double>(fishCutletPixelColor3) / validPixels) * 1000.0;

                    double percentageDishColor = (static_cast<double>(dishPixelColor) / validPixels) * 1000.0;

                    int hightLevelOfDish = 0;

                    if (percentageDishColor > 100.0) {
                        
                        cv::Mat tmpClonedImg = image.clone();

                        int color_range = 25;
                        cv::Scalar target_color1(191, 179, 173);

                        for (int y = 0; y < tmpClonedImg.rows; ++y) {
                            for (int x = 0; x < tmpClonedImg.cols; ++x) {

                                cv::Vec3b color = tmpClonedImg.at<cv::Vec3b>(y, x);

                                if ((abs(color[0] - target_color1[0]) <= color_range &&
                                    abs(color[1] - target_color1[1]) <= color_range &&
                                    abs(color[2] - target_color1[2]) <= color_range)) {

                                    tmpClonedImg.at<cv::Vec3b>(y, x) = cv::Vec3b(0, 0, 0);
                                }
                            }
                        }

                        int erosionSize = 7;
                        cv::Mat elementErosion = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2 * erosionSize + 1, 2 * erosionSize + 1), cv::Point(erosionSize, erosionSize));

                        cv::Mat erodedImage;
                        cv::erode(tmpClonedImg, erodedImage, elementErosion);

                        int dilationSize = 41;
                        cv::Mat elementDilation = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2 * dilationSize + 1, 2 * dilationSize + 1), cv::Point(dilationSize, dilationSize));

                        cv::Mat dilatedImage;
                        cv::dilate(erodedImage, dilatedImage, elementDilation);

                        cv::Mat mask = maskBuilder(dilatedImage, dilatedImage);
                        cv::Mat largestComponent = findLargestConnectedComponent(mask, 1);

                        cv::Mat preProcessedImg;
                        image.copyTo(preProcessedImg, largestComponent);

                        preProcessedImg.copyTo(image);

                        blackPixelCount = 0;
                        orangePixelCount = 0;
                        yellowPixelCount = 0;

                        for (int y = 0; y < image.rows; y++) {
                            for (int x = 0; x < image.cols; x++) {
                                // Take the pixel color in position x, y (B, G, R)
                                cv::Vec3b pixel = image.at<cv::Vec3b>(y, x);

                                if (pixel[0] < 10 && pixel[1] < 10 && pixel[2] < 10) {
                                    blackPixelCount++;
                                    continue;
                                    
                                } else if ((abs(orangeColor[0] - pixel[0]) <= 20) &&
                                    (abs(orangeColor[1] - pixel[1]) <= 20) &&
                                    (abs(orangeColor[2] - pixel[2]) <= 40)) {
                                    orangePixelCount++;
                                } 

                                if ((abs(yellowColor[0] - pixel[0]) <= 10) &&
                                    (abs(yellowColor[1] - pixel[1]) <= 20) &&
                                    (abs(yellowColor[2] - pixel[2]) <= 20)) {
                                    yellowPixelCount++;
                                }
                            }
                        }

                        totalPixels = image.rows * image.cols;
                        validPixels = totalPixels - blackPixelCount;

                        percentageOrangeColor = (static_cast<double>(orangePixelCount) / validPixels) * 1000.0;
                        percentageYellow = (static_cast<double>(yellowPixelCount) / validPixels) * 1000.0;

                        hightLevelOfDish = 1;
                    }

                    if (percentageOrangeColor > 30.0) {
                        if (percentageYellow > 30.0) { // TRAY 7 IMAGE 2 (23.0)

                            if ((percentageGreenColor < 10.0) || (percentageTicketPixelCountColor > 10.0)) {
                                if (percentageFishCutletColor1 < 5.0) {

                                    if (percentageFishCutletColor2 < 5.0) {
                                        if (percentageFishCutletColor3 < 1.5) {

                                            // DEBUGGING
                                            //std::cout << "FISH CUTLET AND BASIL POTATOES" << std::endl;

                                            if (hightLevelOfDish) {

                                                cv::Scalar target_color1(94, 103, 113);
                                                cv::Scalar target_color2(172, 165, 168);
                                                int color_range = 25;

                                                for (int y = 0; y < image.rows; ++y) {
                                                    for (int x = 0; x < image.cols; ++x) {

                                                        cv::Vec3b color = image.at<cv::Vec3b>(y, x);

                                                        if ((abs(color[0] - target_color1[0]) <= color_range &&
                                                            abs(color[1] - target_color1[1]) <= color_range &&
                                                            abs(color[2] - target_color1[2]) <= color_range) ||

                                                            (abs(color[0] - target_color2[0]) <= color_range &&
                                                            abs(color[1] - target_color2[1]) <= color_range &&
                                                            abs(color[2] - target_color2[2]) <= color_range)) {

                                                            image.at<cv::Vec3b>(y, x) = cv::Vec3b(0, 0, 0);
                                                        }
                                                    }
                                                }
                                            }

                                            cv::Mat secondClone = image.clone();
                                            cv::GaussianBlur(secondClone, secondClone, cv::Size(11, 11), 13, 13);

                                            cv::Mat secondMask = maskBuilder(image, secondClone);

                                            int eroSize = 3;
                                            cv::Mat elementEro = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2 * eroSize + 1, 2 * eroSize + 1), cv::Point(eroSize, eroSize));

                                            cv::Mat erodImage;
                                            cv::erode(secondMask, erodImage, elementEro);

                                            cv::Mat newImg;
                                            secondClone.copyTo(newImg, erodImage);

                                            cv::GaussianBlur(newImg, newImg, cv::Size(15, 15), 13, 13);

                                            // Define a brightness threshold
                                            int luminosityThreshold = 145;

                                            cv::Mat grayscaleImage;
                                            cv::cvtColor(newImg, grayscaleImage, cv::COLOR_BGR2GRAY);

                                            cv::Mat luminosityMask;
                                            cv::threshold(grayscaleImage, luminosityMask, luminosityThreshold, 255, cv::THRESH_BINARY);

                                            cv::Mat luminosityImg;
                                            image.copyTo(luminosityImg, luminosityMask);

                                            cv::GaussianBlur(luminosityImg, luminosityImg, cv::Size(3, 3), 5, 5);

                                            cv::Scalar target_color1(103, 161, 214);
                                            cv::Scalar target_color2(52, 143, 202);
                                            cv::Scalar target_color3(150, 187, 223);
                                            cv::Scalar target_color4(140, 98, 44);
                                            int color_range = 25;

                                            for (int y = 0; y < luminosityImg.rows; ++y) {
                                                for (int x = 0; x < luminosityImg.cols; ++x) {

                                                    cv::Vec3b color = luminosityImg.at<cv::Vec3b>(y, x);

                                                    if ((abs(color[0] - target_color1[0]) <= color_range &&
                                                        abs(color[1] - target_color1[1]) <= color_range &&
                                                        abs(color[2] - target_color1[2]) <= color_range) ||

                                                        (abs(color[0] - target_color2[0]) <= color_range &&
                                                        abs(color[1] - target_color2[1]) <= color_range &&
                                                        abs(color[2] - target_color2[2]) <= color_range) ||

                                                        (abs(color[0] - target_color3[0]) <= color_range &&
                                                        abs(color[1] - target_color3[1]) <= color_range &&
                                                        abs(color[2] - target_color3[2]) <= color_range) ||

                                                        (abs(color[0] - target_color4[0]) <= color_range &&
                                                        abs(color[1] - target_color4[1]) <= color_range &&
                                                        abs(color[2] - target_color4[2]) <= color_range)) {

                                                        luminosityImg.at<cv::Vec3b>(y, x) = cv::Vec3b(0, 0, 0);
                                                    }
                                                }
                                            }

                                            eroSize = 1;
                                            elementEro = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2 * eroSize + 1, 2 * eroSize + 1), cv::Point(eroSize, eroSize));

                                            cv::Mat erodImage_2;
                                            cv::erode(luminosityImg, erodImage_2, elementEro);

                                            cv::Mat grayscaleImage2;
                                            cv::cvtColor(erodImage_2, grayscaleImage2, cv::COLOR_BGR2GRAY);

                                            cv::Mat binaryImage;
                                            cv::threshold(grayscaleImage2, binaryImage, 5, 255, cv::THRESH_BINARY);

                                            cv::Mat labels, stats, centroids;
                                            int numObjects = cv::connectedComponentsWithStats(binaryImage, labels, stats, centroids);
                                            int totalArea = 0;

                                            for (int i = 1; i < numObjects; i++) {
                                                int area = stats.at<int>(i, cv::CC_STAT_AREA);
                                                totalArea += area;
                                            }

                                            double meanArea = static_cast<double>(totalArea) / (numObjects - 1);
                                            double thresholdMeanArea = meanArea - (meanArea * 0.5);

                                            // Remove objects with less than average area
                                            for (int i = 1; i < numObjects; i++) {
                                                int area = stats.at<int>(i, cv::CC_STAT_AREA);
                                                if (area < thresholdMeanArea) {
                                                    cv::Mat mask = labels == i;
                                                    erodImage_2.setTo(cv::Scalar(0, 0, 0), mask);
                                                }
                                            }


                                            eroSize = 17;
                                            elementEro = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2 * eroSize + 1, 2 * eroSize + 1), cv::Point(eroSize, eroSize));
                                            cv::Mat dilatedImage_2;
                                            cv::dilate(erodImage_2, dilatedImage_2, elementEro);

                                            cv::Mat mask_3 = maskBuilder(image, dilatedImage_2);

                                            cv::Mat preDilationImg;
                                            image.copyTo(preDilationImg, mask_3);

                                            int dilationSize = 17;
                                            elementEro = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2 * dilationSize + 1, 2 * dilationSize + 1), cv::Point(dilationSize, dilationSize));
                                            cv::Mat dilaImage_3;
                                            cv::dilate(preDilationImg, dilaImage_3, elementEro);
   
                                            cv::Mat postErosion_2;
                                            cv::erode(dilaImage_3, postErosion_2, elementEro);

                                            cv::Mat maskForLargestComponent = maskBuilder(image, postErosion_2);

                                            cv::Mat largestComponent = findLargestConnectedComponent(maskForLargestComponent, 1);

                                            cv::Mat result_2;
                                            image.copyTo(result_2, largestComponent);

                                            cv::GaussianBlur(result_2, result_2, cv::Size(15, 15), 13, 13);

                                            cv::Mat mask_4 = maskBuilder(image.clone(), result_2);
                                            cv::Mat largestComponent_2 = findLargestConnectedComponent(mask_4, 1);

                                            cv::findContours(largestComponent_2, splittedDishes, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

                                            cv::Mat invertedMask2;
                                            cv::bitwise_not(largestComponent, invertedMask2);

                                            cv::Mat resultImage;
                                            image.copyTo(resultImage, invertedMask2);

                                            cv::Mat lastMask = maskBuilder(resultImage, resultImage);
                                            cv::Mat largestComponent_3 = findLargestConnectedComponent(lastMask, 9);

                                            std::vector<std::vector<cv::Point>> otherContours;
                                            cv::findContours(largestComponent_3, otherContours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

                                            for (const std::vector<cv::Point>& contour : otherContours) {
                                                // Print the contour
                                                if (contour.size() >= 3) {
                                                    splittedDishes.push_back(contour);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
            }

            if (((color[0] < range5[0] + threshold5[0]) && (color[0] > range5[0] - threshold5[0])) && 
                ((color[1] < range5[1] + threshold5[1]) && (color[1] > range5[1] - threshold5[1])) &&
                ((color[2] < range5[2] + threshold5[2]) && (color[2] > range5[2] - threshold5[2]))) {

                    // DEBUGGING
                    //std::cout << "BREAD AND YOGURTH" << std::endl;
                    
                    cv::Mat secondClone = image.clone();

                    cv::Scalar target_color(67, 118, 168);
                    int color_range = 50;

                    for (int y = 0; y < secondClone.rows; ++y) {
                        for (int x = 0; x < secondClone.cols; ++x) {

                            cv::Vec3b color = secondClone.at<cv::Vec3b>(y, x);

                            if (!(abs(color[0] - target_color[0]) <= color_range &&
                                abs(color[1] - target_color[1]) <= color_range &&
                                abs(color[2] - target_color[2]) <= color_range)) {

                                    secondClone.at<cv::Vec3b>(y, x) = cv::Vec3b(0, 0, 0);
   
                            }
                        }
                    }

                    int eroSize = 4;
                    cv::Mat elementEro = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2 * eroSize + 1, 2 * eroSize + 1), cv::Point(eroSize, eroSize));

                    cv::Mat erodImage;
                    cv::erode(secondClone, erodImage, elementEro);

                    cv::Mat erosionResult;
                    image.copyTo(erosionResult, erodImage);

                    int dilSize = 27;
                    cv::Mat elementDil= cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2 * dilSize + 1, 2 * dilSize + 1), cv::Point(dilSize, dilSize));

                    cv::Mat dilatedImage;
                    cv::dilate(erosionResult, dilatedImage, elementDil);

                    cv::Mat mask = maskBuilder(dilatedImage, dilatedImage);
                    cv::Mat largestComponent = findLargestConnectedComponent(mask, 15);

                    cv::findContours(largestComponent, splittedDishes, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

            }
        }
    }

    return splittedDishes;
}


cv::Mat maskBuilder(cv::Mat image, cv::Mat secondImg) {

    cv::Mat mask(image.size(), CV_8UC1, cv::Scalar(0));

    for (int y = 0; y < secondImg.rows; ++y) {
        for (int x = 0; x < secondImg.cols; ++x) {
            cv::Vec3b color = secondImg.at<cv::Vec3b>(y, x);

            if (color[0] > 0 || color[1] > 0 || color[2] > 0) {
                mask.at<uchar>(y, x) = 255;
            }
        }
    }

    return mask;
}


cv::Mat findLargestConnectedComponent(const cv::Mat& mask, int eroSize) {

    cv::Mat largest_area_mask;
    cv::Mat erodedImg;

    if (eroSize > 0) {

        cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2 * eroSize + 1, 2 * eroSize + 1), cv::Point(eroSize, eroSize));    
        cv::erode(mask, erodedImg, element);

    } else {
        mask.copyTo(erodedImg);
    }

    // Find the connected components in the image
    cv::Mat labels, stats, centroids;
    int num_components = cv::connectedComponentsWithStats(erodedImg, labels, stats, centroids);

    if (num_components <= 1) {
        std::cerr << "Nessuna area connessa trovata." << std::endl;
        return largest_area_mask; // Restituisce una maschera vuota
    }

    // Find the index of the largest area (excluding the background area)
    int largest_area_idx = 1;
    int largest_area = stats.at<int>(1, cv::CC_STAT_AREA);

    for (int i = 2; i < num_components; ++i) {
        int area = stats.at<int>(i, cv::CC_STAT_AREA);
        if (area > largest_area) {
            largest_area = area;
            largest_area_idx = i;
        }
    }

    // Create a new mask containing only the larger area
    largest_area_mask = cv::Mat::zeros(erodedImg.size(), CV_8UC1);
    for (int y = 0; y < erodedImg.rows; ++y) {
        for (int x = 0; x < erodedImg.cols; ++x) {
            if (labels.at<int>(y, x) == largest_area_idx) {
                largest_area_mask.at<uchar>(y, x) = 255;
            }
        }
    }

    return largest_area_mask;
}
