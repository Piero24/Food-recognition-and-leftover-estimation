#include <iostream>
#include <vector>
#include <tuple>
#include <string>

#include "opencv2/core.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/ximgproc/segmentation.hpp"

#include "../include/foodCategories.h"


FoodResult foodSelection(cv::Mat img, cv::Mat clusteredImg, cv::Mat labels, cv::Mat centers) {

    FoodResult result;
    std::vector<std::tuple<cv::Vec3f, double, double>> pixelColorVector = clusterColorPixelCounter(clusteredImg, labels, centers);

    if (pastaWithPesto(img, clusteredImg, pixelColorVector)) {
        result.foodName = "Pasta with pesto";
        result.foodNumber = 1;
        result.foodColor = cv::Scalar(0, 0, 255);   // Red
        return result;

    } else if (pastaWithTomatoSauce(img, clusteredImg, pixelColorVector)) {
        result.foodName = "Pasta with tomato sauce";
        result.foodNumber = 2;
        result.foodColor = cv::Scalar(0, 255, 0);   // Green
        return result;

    } else if (pastaWithMeatSauce(img, clusteredImg, pixelColorVector)) {
        result.foodName = "Pasta with meat sauce";
        result.foodNumber = 3;
        result.foodColor = cv::Scalar(255, 0, 0);   // Blue
        return result;

    } else if (pastaWithClamsMussels(img, clusteredImg, pixelColorVector)) {
        result.foodName = "Pasta with clams and mussels";
        result.foodNumber = 4;
        result.foodColor = cv::Scalar(255, 0, 255); // Magenta
        return result;


    } else if (pilawRice(img, clusteredImg, pixelColorVector)) {
        result.foodName = "Pilaw rice with peppers and peas";
        result.foodNumber = 5;
        result.foodColor = cv::Scalar(0, 255, 255); // Cyan
        return result;


    } else if (grilledPorkCutlet(img, clusteredImg, pixelColorVector)) {
        result.foodName = "Grilled pork cutlet";
        result.foodNumber = 6;
        result.foodColor = cv::Scalar(255, 255, 0); // Yellow
        return result;


    } else if (fishCutlet(img, clusteredImg, pixelColorVector)) {
        result.foodName = "Fish cutlet";
        result.foodNumber = 7;
        result.foodColor = cv::Scalar(128, 0, 128); // Viola
        return result;


    } else if (rabbit(img, clusteredImg, pixelColorVector)) {
        result.foodName = "Rabbit";
        result.foodNumber = 8;
        result.foodColor = cv::Scalar(0, 128, 128); // Dark green
        return result;


    } else if (seafoodSalad(img, clusteredImg, pixelColorVector)) {
        result.foodName = "Seafood salad";
        result.foodNumber = 9;
        result.foodColor = cv::Scalar(128, 128, 0); // Orange
        return result;


    } else if (beans(img, clusteredImg, pixelColorVector)) {
        result.foodName = "Beans";
        result.foodNumber = 10;
        result.foodColor = cv::Scalar(255, 165, 0); // Dark orange
        return result;


    } else if (basilPotatoes(img, clusteredImg, pixelColorVector)) {
        result.foodName = "Basil potatoes";
        result.foodNumber = 11;
        result.foodColor = cv::Scalar(0, 128, 255); // Light blue
        return result;


    } else if (salad(img, clusteredImg, pixelColorVector)) {
        result.foodName = "Salad";
        result.foodNumber = 12;
        result.foodColor = cv::Scalar(255, 128, 0); // Light red
        return result;


    } else if (bread(img, clusteredImg, pixelColorVector)) {  
        result.foodName = "Bread";
        result.foodNumber = 13;
        result.foodColor = cv::Scalar(128, 128, 128); // Grey
        return result;


    } else {
        result.foodName = "None";
        result.foodNumber = 0;
        result.foodColor = cv::Scalar(0, 0, 0); // Black
        return result;

    }
}


std::vector<std::tuple<cv::Vec3f, double, double>> clusterColorPixelCounter(cv::Mat clusteredImg, cv::Mat labels, cv::Mat centers) {

    std::vector<std::tuple<cv::Vec3f, double, double>> result;
    int coloredPixelCount = 0;

    for (int i = 0; i < clusteredImg.rows; ++i) {
        for (int j = 0; j < clusteredImg.cols; ++j) {
            cv::Vec3b pixelColor = clusteredImg.at<cv::Vec3b>(i, j);
            
            int blackThreshold = 5;
            if (pixelColor[0] > blackThreshold || pixelColor[1] > blackThreshold || pixelColor[2] > blackThreshold) {
                coloredPixelCount++;
            }
        }
    }

    for (int i = 0; i < centers.rows; ++i) {
        cv::Vec3f color = centers.at<cv::Vec3f>(i, 0);

        int blackThreshold = 5;

        if (color[0] > blackThreshold || color[1] > blackThreshold || color[2] > blackThreshold) {

            cv::Mat colorMask;
            cv::inRange(clusteredImg, color, color, colorMask);
            int pixelCount = cv::countNonZero(colorMask);
            result.push_back(std::make_tuple(color, coloredPixelCount, pixelCount));
        }
    }
    return result;
}


std::vector<std::tuple<cv::Vec3f, double>> pixelPercentage(std::vector<std::tuple<cv::Vec3f, double, double>> pixelColorVector) {
    
    std::vector<std::tuple<cv::Vec3f, double>> result;

    for (int i = 0; i < pixelColorVector.size(); ++i) {

        cv::Vec3f color = std::get<0>(pixelColorVector[i]);
        double pixelCount = std::get<1>(pixelColorVector[i]);
        double coloredPixelCount = std::get<2>(pixelColorVector[i]);

        double percentage = (coloredPixelCount / pixelCount) * 100.0;
        double roundedPercentage = std::round(percentage * 10.0) / 10.0;

        // DEBUGGING
        // std::cout << "Colore " << i + 1 << ": (B=" << static_cast<int>(color[0])
        //     << ", G=" << static_cast<int>(color[1]) << ", R=" << static_cast<int>(color[2])
        //     << ") - Percentuale: " << roundedPercentage << "%" << std::endl;

        result.push_back(std::make_tuple(color, roundedPercentage));
    }

    return result;
}


bool pastaWithPesto(cv::Mat img, cv::Mat clusteredImg, std::vector<std::tuple<cv::Vec3f, double, double>> pixelColorVector) {
    
    // std::vector<std::tuple<cv::Vec3f, double>> colorClusterPercentageVec = pixelPercentage(pixelColorVector);

    cv::Vec3f targetColorImg_1(64.0f, 145.0f, 116.0f);
    cv::Vec3f targetColorImg_2(19.0f, 51.0f, 41.0f);
    cv::Vec3f targetColorImg_3(151.0f, 201.0f, 178.0f);
    float colorRange_1 = 40.0f;

    cv::Vec3f targetColorImg_4(55.0f, 161.0f, 194.0f);
    cv::Vec3f targetColorImg_5(1.0f, 67.0f, 137.0f);
    float colorRange_2 = 55.0f;

    cv::Vec3f targetColorClusteredImg_1(39.0f, 92.0f, 106.0f);
    float clusterColorRange = 15.0f;

    double correspondenceOnPixel_1 = 0;
    double correspondenceOnPixel_2 = 0;
    double coloredPixelCount = 0;

    for (int i = 0; i < img.rows; ++i) {
        for (int j = 0; j < img.cols; ++j) {
            cv::Vec3b pixelColor = img.at<cv::Vec3b>(i, j);
            
            int blackThreshold = 5;
            if (pixelColor[0] > blackThreshold || pixelColor[1] > blackThreshold || pixelColor[2] > blackThreshold) {
                coloredPixelCount++;
            }

            cv::Vec3f pixelColorFloat(static_cast<float>(pixelColor[0]), static_cast<float>(pixelColor[1]), static_cast<float>(pixelColor[2]));

            float colorDistance_1 = cv::norm(pixelColorFloat - targetColorImg_1);
            float colorDistance_2 = cv::norm(pixelColorFloat - targetColorImg_2);
            float colorDistance_3 = cv::norm(pixelColorFloat - targetColorImg_3);
            float colorDistance_4 = cv::norm(pixelColorFloat - targetColorImg_4);
            float colorDistance_5 = cv::norm(pixelColorFloat - targetColorImg_5);

            if (colorDistance_1 <= colorRange_1 || colorDistance_2 <= colorRange_1 || colorDistance_3 <= colorRange_1) {
                correspondenceOnPixel_1++;
            }
            if (colorDistance_4 <= colorRange_2 || colorDistance_5 <= colorRange_2) {
                correspondenceOnPixel_2++;
            }

        }
    }

    double percentage_1 = (correspondenceOnPixel_1 / coloredPixelCount) * 100.0;
    double roundedPercentage_1 = std::round(percentage_1 * 10.0) / 10.0;

    double percentage_2 = (correspondenceOnPixel_2 / coloredPixelCount) * 100.0;
    double roundedPercentage_2 = std::round(percentage_2 * 10.0) / 10.0;
    
    double totalPercentage = ((correspondenceOnPixel_1 + correspondenceOnPixel_2) / coloredPixelCount) * 100.0;
    double totalRoundedPercentage = std::round(totalPercentage * 10.0) / 10.0;

    for (const auto& tuple : pixelColorVector) {

        cv::Vec3f color = std::get<0>(tuple);
        double pixelCount = std::get<1>(tuple);
        double coloredPixelCount = std::get<2>(tuple);
        float colorDistance = cv::norm(color - targetColorClusteredImg_1);

        if ((colorDistance <= clusterColorRange) && (totalRoundedPercentage > 50.0) && (roundedPercentage_1 > 20.0) && (roundedPercentage_2 > 20.0)) {
            return true;
        }
    }
    
    return false;
}


bool pastaWithTomatoSauce(cv::Mat img, cv::Mat clusteredImg, std::vector<std::tuple<cv::Vec3f, double, double>> pixelColorVector) {

    cv::Vec3f blackTargetColorImg_1(0.0f, 8.0f, 147.0f);
    float blackRange_1 = 20.0f;

    cv::Vec3f greenTargetColorImg_2(40.0f, 145.0f, 128.0f);
    cv::Vec3f greenTargetColorImg_3(168.0f, 218.0f, 218.0f);
    float greenRange_1 = 25.0f;

    cv::Vec3f targetColorImg_1(0.0f, 2.0f, 65.0f);
    cv::Vec3f targetColorImg_2(22.0f, 33.0f, 150.0f);
    cv::Vec3f targetColorImg_3(48.0f, 64.0f, 186.0f);
    float colorRange_1 = 45.0f;

    cv::Vec3f targetColorImg_4(55.0f, 161.0f, 194.0f);
    cv::Vec3f targetColorImg_5(1.0f, 67.0f, 137.0f);
    float colorRange_2 = 25.0f;

    cv::Vec3f targetColorClusteredImg_1(41.0f, 83.0f, 137.0f);
    float clusterColorRange = 25.0f;

    double correspondenceOnPixel_1 = 0;
    double correspondenceOnPixel_2 = 0;
    double coloredPixelCount = 0;
    double greenPixelCount = 0;
    double blackPixelCount = 0;

    for (int i = 0; i < img.rows; ++i) {
        for (int j = 0; j < img.cols; ++j) {
            cv::Vec3b pixelColor = img.at<cv::Vec3b>(i, j);
            
            int blackThreshold = 5;
            if (pixelColor[0] > blackThreshold || pixelColor[1] > blackThreshold || pixelColor[2] > blackThreshold) {
                coloredPixelCount++;
            }

            cv::Vec3f pixelColorFloat(static_cast<float>(pixelColor[0]), static_cast<float>(pixelColor[1]), static_cast<float>(pixelColor[2]));

            float blackDistance_1 = cv::norm(pixelColorFloat - blackTargetColorImg_1);
            
            float greenDistance_2 = cv::norm(pixelColorFloat - greenTargetColorImg_2);
            float greenDistance_3 = cv::norm(pixelColorFloat - greenTargetColorImg_3);

            float colorDistance_1 = cv::norm(pixelColorFloat - targetColorImg_1);
            float colorDistance_2 = cv::norm(pixelColorFloat - targetColorImg_2);
            float colorDistance_3 = cv::norm(pixelColorFloat - targetColorImg_3);
            float colorDistance_4 = cv::norm(pixelColorFloat - targetColorImg_4);
            float colorDistance_5 = cv::norm(pixelColorFloat - targetColorImg_5);

            if (colorDistance_1 <= colorRange_1 || colorDistance_2 <= colorRange_1 || colorDistance_3 <= colorRange_1) {
                correspondenceOnPixel_1++;
            }
            if (colorDistance_4 <= colorRange_2 || colorDistance_5 <= colorRange_2) {
                correspondenceOnPixel_2++;
            }
            if (greenDistance_2 <= greenRange_1 || greenDistance_3 <= greenRange_1) {
                greenPixelCount++;
            }
            if (blackDistance_1 <= blackRange_1) {
                blackPixelCount++;
            }
        }
    }

    double percentage_1 = (correspondenceOnPixel_1 / coloredPixelCount) * 100.0;
    double roundedPercentage_1 = std::round(percentage_1 * 10.0) / 10.0;

    double percentage_2 = (correspondenceOnPixel_2 / coloredPixelCount) * 100.0;
    double roundedPercentage_2 = std::round(percentage_2 * 10.0) / 10.0;
    
    double totalPercentage = ((correspondenceOnPixel_1 + correspondenceOnPixel_2) / coloredPixelCount) * 100.0;
    double totalRoundedPercentage = std::round(totalPercentage * 10.0) / 10.0;

    double greenPercentage = (greenPixelCount / coloredPixelCount) * 1000.0;
    double greenRoundedPercentage = std::round(greenPercentage * 10.0) / 10.0;

    double blackPercentage = (blackPixelCount / coloredPixelCount) * 1000.0;
    double blackRoundedPercentage = std::round(blackPercentage * 10.0) / 10.0;

    for (const auto& tuple : pixelColorVector) {

        cv::Vec3f color = std::get<0>(tuple);
        double pixelCount = std::get<1>(tuple);
        double coloredPixelCount = std::get<2>(tuple);
        float colorDistance = cv::norm(color - targetColorClusteredImg_1);

        if ((colorDistance <= clusterColorRange) && (totalRoundedPercentage > 25.0) && (roundedPercentage_1 > 22.0) && 
            (roundedPercentage_2 > 4.0) && ((greenRoundedPercentage > 0.9) &&(greenRoundedPercentage < 7.5)) && 
            (blackRoundedPercentage > 3.0)) {
            return true;
        }
    }

    return false;
}


bool pastaWithMeatSauce(cv::Mat img, cv::Mat clusteredImg, std::vector<std::tuple<cv::Vec3f, double, double>> pixelColorVector) {

    cv::Vec3f greenTargetColorImg_2(40.0f, 145.0f, 128.0f);
    cv::Vec3f greenTargetColorImg_3(168.0f, 218.0f, 218.0f);
    float greenRange_1 = 25.0f;

    cv::Vec3f targetColorClusteredImg_1(49.0f, 95.0f, 137.0f);
    float clusterColorRange = 15.0f;

    double coloredPixelCount = 0;
    double greenPixelCount = 0;

    for (int i = 0; i < img.rows; ++i) {
        for (int j = 0; j < img.cols; ++j) {
            cv::Vec3b pixelColor = img.at<cv::Vec3b>(i, j);
            
            int blackThreshold = 5;
            if (pixelColor[0] > blackThreshold || pixelColor[1] > blackThreshold || pixelColor[2] > blackThreshold) {
                coloredPixelCount++;
            }

            cv::Vec3f pixelColorFloat(static_cast<float>(pixelColor[0]), static_cast<float>(pixelColor[1]), static_cast<float>(pixelColor[2]));

            float greenDistance_2 = cv::norm(pixelColorFloat - greenTargetColorImg_2);
            float greenDistance_3 = cv::norm(pixelColorFloat - greenTargetColorImg_3);

            if (greenDistance_2 <= greenRange_1 || greenDistance_3 <= greenRange_1) {
                greenPixelCount++;
            }
        }
    }

    double greenPercentage = (greenPixelCount / coloredPixelCount) * 1000.0;
    double greenRoundedPercentage = std::round(greenPercentage * 10.0) / 10.0;

    for (const auto& tuple : pixelColorVector) {

        cv::Vec3f color = std::get<0>(tuple);
        double pixelCount = std::get<1>(tuple);
        double coloredPixelCount = std::get<2>(tuple);
        float colorDistance = cv::norm(color - targetColorClusteredImg_1);

        if ((colorDistance <= clusterColorRange) && ((greenRoundedPercentage > 6.0) && (greenRoundedPercentage < 20.0))) {
            return true;
        }
    }
    return false;
}


bool pastaWithClamsMussels(cv::Mat img, cv::Mat clusteredImg, std::vector<std::tuple<cv::Vec3f, double, double>> pixelColorVector) {
    
    cv::Vec3f blackTargetColorImg_1(0.0f, 8.0f, 147.0f);
    float blackRange_1 = 20.0f;

    cv::Vec3f yellowTargetColorImg_2(91.0f, 153.0f, 189.0f);
    float yellowRange_1 = 15.0f;

    cv::Vec3f targetColorImg_1(0.0f, 2.0f, 65.0f);
    cv::Vec3f targetColorImg_2(22.0f, 33.0f, 150.0f);
    cv::Vec3f targetColorImg_3(48.0f, 64.0f, 186.0f);
    float colorRange_1 = 45.0f;

    cv::Vec3f targetColorImg_4(55.0f, 161.0f, 194.0f);
    cv::Vec3f targetColorImg_5(1.0f, 67.0f, 137.0f);
    float colorRange_2 = 30.0f;

    cv::Vec3f targetColorClusteredImg_1(34.0f, 77.0f, 130.0f);
    float clusterColorRange = 30.0f;

    double correspondenceOnPixel_1 = 0;
    double correspondenceOnPixel_2 = 0;
    double coloredPixelCount = 0;
    double blackPixelCount = 0;
    double yellowPixelCount = 0;

    for (int i = 0; i < img.rows; ++i) {
        for (int j = 0; j < img.cols; ++j) {
            cv::Vec3b pixelColor = img.at<cv::Vec3b>(i, j);
            
            int blackThreshold = 5;
            if (pixelColor[0] > blackThreshold || pixelColor[1] > blackThreshold || pixelColor[2] > blackThreshold) {
                coloredPixelCount++;
            }

            cv::Vec3f pixelColorFloat(static_cast<float>(pixelColor[0]), static_cast<float>(pixelColor[1]), static_cast<float>(pixelColor[2]));

            float blackDistance_1 = cv::norm(pixelColorFloat - blackTargetColorImg_1);

            float colorDistance_1 = cv::norm(pixelColorFloat - targetColorImg_1);
            float colorDistance_2 = cv::norm(pixelColorFloat - targetColorImg_2);
            float colorDistance_3 = cv::norm(pixelColorFloat - targetColorImg_3);
            float colorDistance_4 = cv::norm(pixelColorFloat - targetColorImg_4);
            float colorDistance_5 = cv::norm(pixelColorFloat - targetColorImg_5);
            float colorDistance_6 = cv::norm(pixelColorFloat - yellowTargetColorImg_2);

            if (colorDistance_1 <= colorRange_1 || colorDistance_2 <= colorRange_1 || colorDistance_3 <= colorRange_1) {
                correspondenceOnPixel_1++;
            }
            if (colorDistance_4 <= colorRange_2 || colorDistance_5 <= colorRange_2) {
                correspondenceOnPixel_2++;
            }
            if (blackDistance_1 <= blackRange_1) {
                blackPixelCount++;
            }
            if (colorDistance_6 <= yellowRange_1) {
                yellowPixelCount++;
            }
        }
    }

    double percentage_1 = (correspondenceOnPixel_1 / coloredPixelCount) * 100.0;
    double roundedPercentage_1 = std::round(percentage_1 * 10.0) / 10.0;

    double percentage_2 = (correspondenceOnPixel_2 / coloredPixelCount) * 100.0;
    double roundedPercentage_2 = std::round(percentage_2 * 10.0) / 10.0;
    
    double totalPercentage = ((correspondenceOnPixel_1 + correspondenceOnPixel_2) / coloredPixelCount) * 100.0;
    double totalRoundedPercentage = std::round(totalPercentage * 10.0) / 10.0;

    double blackPercentage = (blackPixelCount / coloredPixelCount) * 1000.0;
    double blackRoundedPercentage = std::round(blackPercentage * 10.0) / 10.0;

    double yellowPercentage = (yellowPixelCount / coloredPixelCount) * 1000.0;
    double yellowRoundedPercentage = std::round(yellowPercentage * 10.0) / 10.0;

    for (const auto& tuple : pixelColorVector) {

        cv::Vec3f color = std::get<0>(tuple);
        double pixelCount = std::get<1>(tuple);
        double coloredPixelCount = std::get<2>(tuple);
        float colorDistance = cv::norm(color - targetColorClusteredImg_1);

        if ((colorDistance <= clusterColorRange) && (totalRoundedPercentage > 30.0) && (roundedPercentage_1 > 16.0) && 
            (roundedPercentage_2 > 10.0) && (blackRoundedPercentage < 1.0) && (yellowRoundedPercentage < 15.0)) {
            return true;
        }
    }

    return false;
}


bool pilawRice(cv::Mat img, cv::Mat clusteredImg, std::vector<std::tuple<cv::Vec3f, double, double>> pixelColorVector) {

    cv::Mat clonedImg = img.clone();

    cv::Mat blurredImage;
    cv::GaussianBlur(clonedImg, blurredImage, cv::Size(151, 151), 0);

    cv::Vec3f blurredTargetColorImg_1(70.0f, 110.0f, 135.0f);
    float blurredRange = 5.0f; //15.0f;

    cv::Vec3f blackTargetColorImg_2(162.0f, 195.0f, 218.0f);
    cv::Vec3f blackTargetColorImg_3(129.0f, 190.0f, 220.0f);
    float blackRange = 15.0f;

    cv::Vec3f targetColorClusteredImg_1(52.0f, 89.0f, 117.0f);
    cv::Vec3f targetColorClusteredImg_2(35.0f, 60.0f, 80.0f);
    float clusterColorRange_1 = 15.0f;
    float clusterColorRange_2 = 10.0f;

    double coloredPixelCount = 0;
    double blackPixelCount = 0;

    double blurredPixelCount = 0;
    double blurredInRangeCount = 0;

    for (int i = 0; i < img.rows; ++i) {
        for (int j = 0; j < img.cols; ++j) {
            cv::Vec3b pixelColor = img.at<cv::Vec3b>(i, j);
            
            int blackThreshold = 5;
            if (pixelColor[0] > blackThreshold || pixelColor[1] > blackThreshold || pixelColor[2] > blackThreshold) {
                coloredPixelCount++;
            }

            cv::Vec3f pixelColorFloat(static_cast<float>(pixelColor[0]), static_cast<float>(pixelColor[1]), static_cast<float>(pixelColor[2]));

            float blackDistance_2 = cv::norm(pixelColorFloat - blackTargetColorImg_2);
            float blackDistance_3 = cv::norm(pixelColorFloat - blackTargetColorImg_3);

            if ( blackDistance_2 <= blackRange || blackDistance_3 <= blackRange) {
                blackPixelCount++;
            }

        }
    }

    for (int i = 0; i < blurredImage.rows; ++i) {
        for (int j = 0; j < blurredImage.cols; ++j) {
            cv::Vec3b pixelColor = blurredImage.at<cv::Vec3b>(i, j);
            
            int blackThreshold = 5;
            if (pixelColor[0] > blackThreshold || pixelColor[1] > blackThreshold || pixelColor[2] > blackThreshold) {
                blurredPixelCount++;
            }

            cv::Vec3f pixelColorFloat(static_cast<float>(pixelColor[0]), static_cast<float>(pixelColor[1]), static_cast<float>(pixelColor[2]));

            float blurredDistance_1 = cv::norm(pixelColorFloat - blurredTargetColorImg_1);

            if (blurredDistance_1 <= blurredRange) {
                blurredInRangeCount++;
            }

        }
    }

    double blackPercentage = (blackPixelCount / coloredPixelCount) * 1000.0;
    double blackRoundedPercentage = std::round(blackPercentage * 10.0) / 10.0;

    double blurredPercentage = (blurredInRangeCount / blurredPixelCount) * 1000.0;
    double blurredRoundedPercentage = std::round(blurredPercentage * 10.0) / 10.0;


    for (const auto& tuple : pixelColorVector) {

        cv::Vec3f color = std::get<0>(tuple);
        double pixelCount = std::get<1>(tuple);
        double coloredPixelCount = std::get<2>(tuple);

        float colorDistance_1 = cv::norm(color - targetColorClusteredImg_1);
        float colorDistance_2 = cv::norm(color - targetColorClusteredImg_2);

        if (((colorDistance_1 <= clusterColorRange_1) || (colorDistance_2 <= clusterColorRange_2)) && (blurredRoundedPercentage > 5.0)) {
            return true;
        }
    }

    return false;
}


bool grilledPorkCutlet(cv::Mat img, cv::Mat clusteredImg, std::vector<std::tuple<cv::Vec3f, double, double>> pixelColorVector) {
        
    cv::Vec3f greenTargetColorImg_2(40.0f, 145.0f, 128.0f);
    cv::Vec3f greenTargetColorImg_3(168.0f, 218.0f, 218.0f);
    float greenRange_1 = 25.0f;

    cv::Vec3f targetColorImg_1(0.0f, 2.0f, 65.0f);
    cv::Vec3f targetColorImg_2(22.0f, 33.0f, 150.0f);
    cv::Vec3f targetColorImg_3(48.0f, 64.0f, 186.0f);
    float colorRange_1 = 45.0f;

    cv::Vec3f targetColorImg_4(55.0f, 161.0f, 194.0f);
    cv::Vec3f targetColorImg_5(1.0f, 67.0f, 137.0f);
    float colorRange_2 = 30.0f;

    cv::Vec3f targetColorClusteredImg_1(56.0f, 74.0f, 98.0f);
    float clusterColorRange = 20.0f;

    double correspondenceOnPixel_1 = 0;
    double correspondenceOnPixel_2 = 0;
    double coloredPixelCount = 0;
    double greenPixelCount = 0;

    for (int i = 0; i < img.rows; ++i) {
        for (int j = 0; j < img.cols; ++j) {
            cv::Vec3b pixelColor = img.at<cv::Vec3b>(i, j);
            
            int blackThreshold = 5;
            if (pixelColor[0] > blackThreshold || pixelColor[1] > blackThreshold || pixelColor[2] > blackThreshold) {
                coloredPixelCount++;
            }

            cv::Vec3f pixelColorFloat(static_cast<float>(pixelColor[0]), static_cast<float>(pixelColor[1]), static_cast<float>(pixelColor[2]));

            float greenDistance_2 = cv::norm(pixelColorFloat - greenTargetColorImg_2);
            float greenDistance_3 = cv::norm(pixelColorFloat - greenTargetColorImg_3);

            float colorDistance_1 = cv::norm(pixelColorFloat - targetColorImg_1);
            float colorDistance_2 = cv::norm(pixelColorFloat - targetColorImg_2);
            float colorDistance_3 = cv::norm(pixelColorFloat - targetColorImg_3);
            float colorDistance_4 = cv::norm(pixelColorFloat - targetColorImg_4);
            float colorDistance_5 = cv::norm(pixelColorFloat - targetColorImg_5);

            if (colorDistance_1 <= colorRange_1 || colorDistance_2 <= colorRange_1 || colorDistance_3 <= colorRange_1) {
                correspondenceOnPixel_1++;
            }
            if (colorDistance_4 <= colorRange_2 || colorDistance_5 <= colorRange_2) {
                correspondenceOnPixel_2++;
            }
            if (greenDistance_2 <= greenRange_1 || greenDistance_3 <= greenRange_1) {
                greenPixelCount++;
            }
        }
    }

    double percentage_1 = (correspondenceOnPixel_1 / coloredPixelCount) * 100.0;
    double roundedPercentage_1 = std::round(percentage_1 * 10.0) / 10.0;

    double percentage_2 = (correspondenceOnPixel_2 / coloredPixelCount) * 100.0;
    double roundedPercentage_2 = std::round(percentage_2 * 10.0) / 10.0;

    double greenPercentage = (greenPixelCount / coloredPixelCount) * 1000.0;
    double greenRoundedPercentage = std::round(greenPercentage * 10.0) / 10.0;

    for (const auto& tuple : pixelColorVector) {

        cv::Vec3f color = std::get<0>(tuple);
        double pixelCount = std::get<1>(tuple);
        double coloredPixelCount = std::get<2>(tuple);
        float colorDistance = cv::norm(color - targetColorClusteredImg_1);

        if ((colorDistance <= clusterColorRange) && (roundedPercentage_1 < 15.0) && (roundedPercentage_2 < 0.5) && (greenRoundedPercentage < 5.0)) {
            return true;
        }
    }

    return false;
}


bool fishCutlet(cv::Mat img, cv::Mat clusteredImg, std::vector<std::tuple<cv::Vec3f, double, double>> pixelColorVector) {
    
    cv::Vec3f blackTargetColorImg_1(0.0f, 8.0f, 147.0f);
    float blackRange_1 = 20.0f;

    cv::Vec3f greenTargetColorImg_2(40.0f, 145.0f, 128.0f);
    cv::Vec3f greenTargetColorImg_3(168.0f, 218.0f, 218.0f);
    float greenRange_1 = 25.0f;

    cv::Vec3f targetColorImg_1(0.0f, 2.0f, 65.0f);
    cv::Vec3f targetColorImg_2(22.0f, 33.0f, 150.0f);
    cv::Vec3f targetColorImg_3(48.0f, 64.0f, 186.0f);
    float colorRange_1 = 45.0f;

    cv::Vec3f targetColorImg_4(55.0f, 161.0f, 194.0f);
    cv::Vec3f targetColorImg_5(1.0f, 67.0f, 137.0f);
    float colorRange_2 = 30.0f;

    cv::Vec3f targetColorClusteredImg_1(42.0f, 78.0f, 118.0f);
    float clusterColorRange = 30.0f;

    double correspondenceOnPixel_1 = 0;
    double correspondenceOnPixel_2 = 0;
    double coloredPixelCount = 0;
    double greenPixelCount = 0;
    double blackPixelCount = 0;

    for (int i = 0; i < img.rows; ++i) {
        for (int j = 0; j < img.cols; ++j) {
            cv::Vec3b pixelColor = img.at<cv::Vec3b>(i, j);
            
            int blackThreshold = 5;
            if (pixelColor[0] > blackThreshold || pixelColor[1] > blackThreshold || pixelColor[2] > blackThreshold) {
                coloredPixelCount++;
            }

            cv::Vec3f pixelColorFloat(static_cast<float>(pixelColor[0]), static_cast<float>(pixelColor[1]), static_cast<float>(pixelColor[2]));

            float blackDistance_1 = cv::norm(pixelColorFloat - blackTargetColorImg_1);
            
            float greenDistance_2 = cv::norm(pixelColorFloat - greenTargetColorImg_2);
            float greenDistance_3 = cv::norm(pixelColorFloat - greenTargetColorImg_3);

            float colorDistance_1 = cv::norm(pixelColorFloat - targetColorImg_1);
            float colorDistance_2 = cv::norm(pixelColorFloat - targetColorImg_2);
            float colorDistance_3 = cv::norm(pixelColorFloat - targetColorImg_3);
            float colorDistance_4 = cv::norm(pixelColorFloat - targetColorImg_4);
            float colorDistance_5 = cv::norm(pixelColorFloat - targetColorImg_5);

            if (colorDistance_1 <= colorRange_1 || colorDistance_2 <= colorRange_1 || colorDistance_3 <= colorRange_1) {
                correspondenceOnPixel_1++;
            }
            if (colorDistance_4 <= colorRange_2 || colorDistance_5 <= colorRange_2) {
                correspondenceOnPixel_2++;
            }
            if (greenDistance_2 <= greenRange_1 || greenDistance_3 <= greenRange_1) {
                greenPixelCount++;
            }
            if (blackDistance_1 <= blackRange_1) {
                blackPixelCount++;
            }
        }
    }

    double percentage_1 = (correspondenceOnPixel_1 / coloredPixelCount) * 100.0;
    double roundedPercentage_1 = std::round(percentage_1 * 10.0) / 10.0;

    double percentage_2 = (correspondenceOnPixel_2 / coloredPixelCount) * 100.0;
    double roundedPercentage_2 = std::round(percentage_2 * 10.0) / 10.0;
    
    double greenPercentage = (greenPixelCount / coloredPixelCount) * 1000.0;
    double greenRoundedPercentage = std::round(greenPercentage * 10.0) / 10.0;

    double blackPercentage = (blackPixelCount / coloredPixelCount) * 1000.0;
    double blackRoundedPercentage = std::round(blackPercentage * 10.0) / 10.0;

    for (const auto& tuple : pixelColorVector) {

        cv::Vec3f color = std::get<0>(tuple);
        double pixelCount = std::get<1>(tuple);
        double coloredPixelCount = std::get<2>(tuple);
        float colorDistance = cv::norm(color - targetColorClusteredImg_1);

        if ((colorDistance <= clusterColorRange) && (roundedPercentage_2 > 3.0) && (roundedPercentage_1 < 30.0) && 
            (greenRoundedPercentage < 10.0) && (blackRoundedPercentage < 1.0)) {
            return true;
        }
    }

    return false;
}


bool rabbit(cv::Mat img, cv::Mat clusteredImg, std::vector<std::tuple<cv::Vec3f, double, double>> pixelColorVector) {

    cv::Vec3f blackTargetColorImg_1(0.0f, 8.0f, 147.0f);
    float blackRange_1 = 20.0f;

    cv::Vec3f greenTargetColorImg_2(40.0f, 145.0f, 128.0f);
    cv::Vec3f greenTargetColorImg_3(168.0f, 218.0f, 218.0f);
    float greenRange_1 = 25.0f;

    cv::Vec3f targetColorImg_1(55.0f, 66.0f, 116.0f);
    cv::Vec3f targetColorImg_2(91.0f, 122.0f, 175.0f);
    float colorRange_1 = 10.0f;

    cv::Vec3f targetColorImg_4(55.0f, 161.0f, 194.0f);
    cv::Vec3f targetColorImg_5(1.0f, 67.0f, 137.0f);
    float colorRange_2 = 30.0f;

    cv::Vec3f targetColorClusteredImg_1(31.0f, 47.0f, 75.0f);
    float clusterColorRange = 25.0f;

    cv::Vec3f whiteTargetColorImg(180.0f, 181.0f, 183.0f);
    float whiteColorRange = 15.0f;

    double correspondenceOnPixel_2 = 0;
    double coloredPixelCount = 0;
    double greenPixelCount = 0;
    double blackPixelCount = 0;

    double whitePixelCount = 0;

    double correspondenceOnPixel_10 = 0;
    double correspondenceOnPixel_20 = 0;

    for (int i = 0; i < img.rows; ++i) {
        for (int j = 0; j < img.cols; ++j) {
            cv::Vec3b pixelColor = img.at<cv::Vec3b>(i, j);
            
            int blackThreshold = 5;
            if (pixelColor[0] > blackThreshold || pixelColor[1] > blackThreshold || pixelColor[2] > blackThreshold) {
                coloredPixelCount++;
            }

            cv::Vec3f pixelColorFloat(static_cast<float>(pixelColor[0]), static_cast<float>(pixelColor[1]), static_cast<float>(pixelColor[2]));

            float blackDistance_1 = cv::norm(pixelColorFloat - blackTargetColorImg_1);
            
            float greenDistance_2 = cv::norm(pixelColorFloat - greenTargetColorImg_2);
            float greenDistance_3 = cv::norm(pixelColorFloat - greenTargetColorImg_3);

            float colorDistance_1 = cv::norm(pixelColorFloat - targetColorImg_1);
            float colorDistance_2 = cv::norm(pixelColorFloat - targetColorImg_2);

            float colorDistance_4 = cv::norm(pixelColorFloat - targetColorImg_4);
            float colorDistance_5 = cv::norm(pixelColorFloat - targetColorImg_5);

            float colorDistance_6 = cv::norm(pixelColorFloat - whiteTargetColorImg);

            if (colorDistance_1 <= colorRange_1) {
                correspondenceOnPixel_10++;
            }
            if (colorDistance_2 <= colorRange_1) {
                correspondenceOnPixel_20++;
            }
            if (colorDistance_4 <= colorRange_2 || colorDistance_5 <= colorRange_2) {
                correspondenceOnPixel_2++;
            }
            if (greenDistance_2 <= greenRange_1 || greenDistance_3 <= greenRange_1) {
                greenPixelCount++;
            }
            if (blackDistance_1 <= blackRange_1) {
                blackPixelCount++;
            }

            if (colorDistance_6<= whiteColorRange) {
                whitePixelCount++;
            }

        }
    }

    double percentage_10 = (correspondenceOnPixel_10 / coloredPixelCount) * 100.0;
    double roundedPercentage_10 = std::round(percentage_10 * 10.0) / 10.0;

    double percentage_20 = (correspondenceOnPixel_20 / coloredPixelCount) * 100.0;
    double roundedPercentage_20 = std::round(percentage_20 * 10.0) / 10.0;

    double percentage_2 = (correspondenceOnPixel_2 / coloredPixelCount) * 100.0;
    double roundedPercentage_2 = std::round(percentage_2 * 10.0) / 10.0;

    double greenPercentage = (greenPixelCount / coloredPixelCount) * 1000.0;
    double greenRoundedPercentage = std::round(greenPercentage * 10.0) / 10.0;

    double blackPercentage = (blackPixelCount / coloredPixelCount) * 1000.0;
    double blackRoundedPercentage = std::round(blackPercentage * 10.0) / 10.0;

    double whitePercentage = (whitePixelCount / coloredPixelCount) * 1000.0;
    double whiteRoundedPercentage = std::round(whitePercentage * 10.0) / 10.0;


    for (const auto& tuple : pixelColorVector) {

        cv::Vec3f color = std::get<0>(tuple);
        double pixelCount = std::get<1>(tuple);
        double coloredPixelCount = std::get<2>(tuple);
        float colorDistance = cv::norm(color - targetColorClusteredImg_1);

        if ((colorDistance <= clusterColorRange) && (whiteRoundedPercentage < 50.0) && (greenRoundedPercentage < 10.0) && 
            (roundedPercentage_2 > 0.0)  && (blackRoundedPercentage < 1.0)) {
            
            if (!(roundedPercentage_10 > 2.0 || roundedPercentage_20 > 2.0)) {
                return true;
            }
        }
    }

    return false;
}


bool seafoodSalad(cv::Mat img, cv::Mat clusteredImg, std::vector<std::tuple<cv::Vec3f, double, double>> pixelColorVector) {

    cv::Vec3f targetColorImg_4(55.0f, 161.0f, 194.0f);
    cv::Vec3f targetColorImg_5(1.0f, 67.0f, 137.0f);
    float colorRange_2 = 30.0f;

    cv::Vec3f targetColorClusteredImg_1(45.0f, 69.0f, 96.0f);
    cv::Vec3f targetColorClusteredImg_2(70.0f, 100.0f, 125.0f);
    float clusterColorRange = 10.0f;

    double correspondenceOnPixel_2 = 0;
    double coloredPixelCount = 0;

    for (int i = 0; i < img.rows; ++i) {
        for (int j = 0; j < img.cols; ++j) {
            cv::Vec3b pixelColor = img.at<cv::Vec3b>(i, j);
            
            int blackThreshold = 5;
            if (pixelColor[0] > blackThreshold || pixelColor[1] > blackThreshold || pixelColor[2] > blackThreshold) {
                coloredPixelCount++;
            }

            cv::Vec3f pixelColorFloat(static_cast<float>(pixelColor[0]), static_cast<float>(pixelColor[1]), static_cast<float>(pixelColor[2]));

            float colorDistance_4 = cv::norm(pixelColorFloat - targetColorImg_4);
            float colorDistance_5 = cv::norm(pixelColorFloat - targetColorImg_5);

            if (colorDistance_4 <= colorRange_2 || colorDistance_5 <= colorRange_2) {
                correspondenceOnPixel_2++;
            }
        }
    }

    double percentage_2 = (correspondenceOnPixel_2 / coloredPixelCount) * 100.0;
    double roundedPercentage_2 = std::round(percentage_2 * 10.0) / 10.0;

    for (const auto& tuple : pixelColorVector) {

        cv::Vec3f color = std::get<0>(tuple);
        double pixelCount = std::get<1>(tuple);
        double coloredPixelCount = std::get<2>(tuple);
        float colorDistance_1 = cv::norm(color - targetColorClusteredImg_1);
        float colorDistance_2 = cv::norm(color - targetColorClusteredImg_2);

        if (((colorDistance_1 <= clusterColorRange) || (colorDistance_2 <= clusterColorRange)) && (roundedPercentage_2 > 1.5)) {
            return true;
        }
    }

    return false;
}


bool beans(cv::Mat img, cv::Mat clusteredImg, std::vector<std::tuple<cv::Vec3f, double, double>> pixelColorVector) {

    cv::Vec3f blackTargetColorImg_1(0.0f, 8.0f, 147.0f);
    float blackRange_1 = 20.0f;

    cv::Vec3f greenTargetColorImg_2(40.0f, 145.0f, 128.0f);
    cv::Vec3f greenTargetColorImg_3(168.0f, 218.0f, 218.0f);
    float greenRange_1 = 25.0f;

    cv::Vec3f targetColorImg_1(0.0f, 2.0f, 65.0f);
    cv::Vec3f targetColorImg_2(22.0f, 33.0f, 150.0f);
    cv::Vec3f targetColorImg_3(48.0f, 64.0f, 186.0f);
    float colorRange_1 = 45.0f;

    cv::Vec3f targetColorImg_4(55.0f, 161.0f, 194.0f);
    cv::Vec3f targetColorImg_5(1.0f, 67.0f, 137.0f);
    float colorRange_2 = 30.0f;

    cv::Vec3f targetColorClusteredImg_1(32.0f, 40.0f, 65.0f);
    float clusterColorRange = 50.0f;

    double correspondenceOnPixel_1 = 0;
    double correspondenceOnPixel_2 = 0;
    double coloredPixelCount = 0;
    double greenPixelCount = 0;
    double blackPixelCount = 0;

    for (int i = 0; i < img.rows; ++i) {
        for (int j = 0; j < img.cols; ++j) {
            cv::Vec3b pixelColor = img.at<cv::Vec3b>(i, j);
            
            int blackThreshold = 5;
            if (pixelColor[0] > blackThreshold || pixelColor[1] > blackThreshold || pixelColor[2] > blackThreshold) {
                coloredPixelCount++;
            }

            cv::Vec3f pixelColorFloat(static_cast<float>(pixelColor[0]), static_cast<float>(pixelColor[1]), static_cast<float>(pixelColor[2]));

            float blackDistance_1 = cv::norm(pixelColorFloat - blackTargetColorImg_1);
            
            float greenDistance_2 = cv::norm(pixelColorFloat - greenTargetColorImg_2);
            float greenDistance_3 = cv::norm(pixelColorFloat - greenTargetColorImg_3);

            float colorDistance_1 = cv::norm(pixelColorFloat - targetColorImg_1);
            float colorDistance_2 = cv::norm(pixelColorFloat - targetColorImg_2);
            float colorDistance_3 = cv::norm(pixelColorFloat - targetColorImg_3);
            float colorDistance_4 = cv::norm(pixelColorFloat - targetColorImg_4);
            float colorDistance_5 = cv::norm(pixelColorFloat - targetColorImg_5);

            if (colorDistance_1 <= colorRange_1 || colorDistance_2 <= colorRange_1 || colorDistance_3 <= colorRange_1) {
                correspondenceOnPixel_1++;
            }
            if (colorDistance_4 <= colorRange_2 || colorDistance_5 <= colorRange_2) {
                correspondenceOnPixel_2++;
            }
            if (greenDistance_2 <= greenRange_1 || greenDistance_3 <= greenRange_1) {
                greenPixelCount++;
            }
            if (blackDistance_1 <= blackRange_1) {
                blackPixelCount++;
            }
        }
    }

    double percentage_1 = (correspondenceOnPixel_1 / coloredPixelCount) * 100.0;
    double roundedPercentage_1 = std::round(percentage_1 * 10.0) / 10.0;

    double percentage_2 = (correspondenceOnPixel_2 / coloredPixelCount) * 100.0;
    double roundedPercentage_2 = std::round(percentage_2 * 10.0) / 10.0;
    
    double totalPercentage = ((correspondenceOnPixel_1 + correspondenceOnPixel_2) / coloredPixelCount) * 100.0;
    double totalRoundedPercentage = std::round(totalPercentage * 10.0) / 10.0;

    double greenPercentage = (greenPixelCount / coloredPixelCount) * 1000.0;
    double greenRoundedPercentage = std::round(greenPercentage * 10.0) / 10.0;

    double blackPercentage = (blackPixelCount / coloredPixelCount) * 1000.0;
    double blackRoundedPercentage = std::round(blackPercentage * 10.0) / 10.0;

    for (const auto& tuple : pixelColorVector) {

        cv::Vec3f color = std::get<0>(tuple);
        double pixelCount = std::get<1>(tuple);
        double coloredPixelCount = std::get<2>(tuple);
        float colorDistance = cv::norm(color - targetColorClusteredImg_1);

        if ((colorDistance <= clusterColorRange) && (totalRoundedPercentage > 10.0) && (roundedPercentage_2 < 2.0) && 
            (greenRoundedPercentage < 1.0) && (blackRoundedPercentage < 1.0)) {
            return true;
        }
    }

    return false;
}


bool basilPotatoes(cv::Mat img, cv::Mat clusteredImg, std::vector<std::tuple<cv::Vec3f, double, double>> pixelColorVector) {

    cv::Vec3f blackTargetColorImg_1(0.0f, 8.0f, 147.0f);
    float blackRange_1 = 20.0f;

    cv::Vec3f greenTargetColorImg_2(40.0f, 145.0f, 128.0f);
    cv::Vec3f greenTargetColorImg_3(168.0f, 218.0f, 218.0f);
    float greenRange_1 = 25.0f;

    cv::Vec3f targetColorImg_1(73.0f, 150.0f, 186.0f);
    cv::Vec3f targetColorImg_2(153.0f, 195.0f, 150.0f);
    cv::Vec3f targetColorImg_3(16.0f, 102.0f, 158.0f);
    float colorRange_1 = 35.0f;

    cv::Vec3f targetColorImg_4(151.0f, 181.0f, 185.0f);
    cv::Vec3f targetColorImg_5(167.0f, 188.0f, 186.0f);
    float colorRange_2 = 15.0f;

    cv::Vec3f targetColorClusteredImg_1(25.0f, 41.0f, 45.0f);
    cv::Vec3f targetColorClusteredImg_2(59.0f, 82.0f, 105.0f);
    cv::Vec3f targetColorClusteredImg_3(74.0f, 119.0f, 134.0f);
    float clusterColorRange_1 = 15.0f;
    float clusterColorRange_2 = 25.0f;
    float clusterColorRange_3 = 20.0f;

    double correspondenceOnPixel_1 = 0;
    double correspondenceOnPixel_2 = 0;
    double coloredPixelCount = 0;
    double greenPixelCount = 0;
    double blackPixelCount = 0;

    for (int i = 0; i < img.rows; ++i) {
        for (int j = 0; j < img.cols; ++j) {
            cv::Vec3b pixelColor = img.at<cv::Vec3b>(i, j);
            
            int blackThreshold = 5;
            if (pixelColor[0] > blackThreshold || pixelColor[1] > blackThreshold || pixelColor[2] > blackThreshold) {
                coloredPixelCount++;
            }

            cv::Vec3f pixelColorFloat(static_cast<float>(pixelColor[0]), static_cast<float>(pixelColor[1]), static_cast<float>(pixelColor[2]));

            float blackDistance_1 = cv::norm(pixelColorFloat - blackTargetColorImg_1);
            
            float greenDistance_2 = cv::norm(pixelColorFloat - greenTargetColorImg_2);
            float greenDistance_3 = cv::norm(pixelColorFloat - greenTargetColorImg_3);

            float colorDistance_1 = cv::norm(pixelColorFloat - targetColorImg_1);
            float colorDistance_2 = cv::norm(pixelColorFloat - targetColorImg_2);
            float colorDistance_3 = cv::norm(pixelColorFloat - targetColorImg_3);
            float colorDistance_4 = cv::norm(pixelColorFloat - targetColorImg_4);
            float colorDistance_5 = cv::norm(pixelColorFloat - targetColorImg_5);

            if (colorDistance_1 <= colorRange_1 || colorDistance_2 <= colorRange_1 || colorDistance_3 <= colorRange_1) {
                correspondenceOnPixel_1++;
            }
            if (colorDistance_4 <= colorRange_2 || colorDistance_5 <= colorRange_2) {
                correspondenceOnPixel_2++;
            }
            if (greenDistance_2 <= greenRange_1 || greenDistance_3 <= greenRange_1) {
                greenPixelCount++;
            }
            if (blackDistance_1 <= blackRange_1) {
                blackPixelCount++;
            }
        }
    }

    double percentage_1 = (correspondenceOnPixel_1 / coloredPixelCount) * 100.0;
    double roundedPercentage_1 = std::round(percentage_1 * 10.0) / 10.0;

    double percentage_2 = (correspondenceOnPixel_2 / coloredPixelCount) * 100.0;
    double roundedPercentage_2 = std::round(percentage_2 * 10.0) / 10.0;
    
    double totalPercentage = ((correspondenceOnPixel_1 + correspondenceOnPixel_2) / coloredPixelCount) * 100.0;
    double totalRoundedPercentage = std::round(totalPercentage * 10.0) / 10.0;

    double greenPercentage = (greenPixelCount / coloredPixelCount) * 1000.0;
    double greenRoundedPercentage = std::round(greenPercentage * 10.0) / 10.0;

    double blackPercentage = (blackPixelCount / coloredPixelCount) * 1000.0;
    double blackRoundedPercentage = std::round(blackPercentage * 10.0) / 10.0;

    for (const auto& tuple : pixelColorVector) {

        cv::Vec3f color = std::get<0>(tuple);
        double pixelCount = std::get<1>(tuple);
        double coloredPixelCount = std::get<2>(tuple);

        float colorDistance_1 = cv::norm(color - targetColorClusteredImg_1);
        float colorDistance_2 = cv::norm(color - targetColorClusteredImg_2);
        float colorDistance_3 = cv::norm(color - targetColorClusteredImg_3);

        if (((colorDistance_1 <= clusterColorRange_1) || (colorDistance_2 <= clusterColorRange_2) || (colorDistance_3 <= clusterColorRange_3)) && 
            (roundedPercentage_1 > 6.0) && (blackRoundedPercentage < 1.0) ) {
            if (greenRoundedPercentage > 15) {
                return true;
                
            } else if (roundedPercentage_2 > 4.0) {
                return true;
            }
        }
    }
    return false;
}


bool salad(cv::Mat img, cv::Mat clusteredImg, std::vector<std::tuple<cv::Vec3f, double, double>> pixelColorVector) {

    cv::Vec3f blackTargetColorImg_1(0.0f, 8.0f, 147.0f);
    float blackRange_1 = 20.0f;

    cv::Vec3f greenTargetColorImg_2(40.0f, 145.0f, 128.0f);
    cv::Vec3f greenTargetColorImg_3(168.0f, 218.0f, 218.0f);
    float greenRange_1 = 25.0f;

    cv::Vec3f targetColorImg_1(0.0f, 2.0f, 65.0f);
    cv::Vec3f targetColorImg_2(22.0f, 33.0f, 150.0f);
    cv::Vec3f targetColorImg_3(48.0f, 64.0f, 186.0f);
    float colorRange_1 = 45.0f;

    cv::Vec3f targetColorClusteredImg_1(35.0f, 79.0f, 135.0f);
    cv::Vec3f targetColorClusteredImg_2(37.0f, 67.0f, 95.0f);
    float clusterColorRange = 25.0f;

    double correspondenceOnPixel_1 = 0;
    double coloredPixelCount = 0;
    double greenPixelCount = 0;
    double blackPixelCount = 0;

    for (int i = 0; i < img.rows; ++i) {
        for (int j = 0; j < img.cols; ++j) {
            cv::Vec3b pixelColor = img.at<cv::Vec3b>(i, j);
            
            int blackThreshold = 5;
            if (pixelColor[0] > blackThreshold || pixelColor[1] > blackThreshold || pixelColor[2] > blackThreshold) {
                coloredPixelCount++;
            }

            cv::Vec3f pixelColorFloat(static_cast<float>(pixelColor[0]), static_cast<float>(pixelColor[1]), static_cast<float>(pixelColor[2]));

            float blackDistance_1 = cv::norm(pixelColorFloat - blackTargetColorImg_1);
            
            float greenDistance_2 = cv::norm(pixelColorFloat - greenTargetColorImg_2);
            float greenDistance_3 = cv::norm(pixelColorFloat - greenTargetColorImg_3);

            float colorDistance_1 = cv::norm(pixelColorFloat - targetColorImg_1);
            float colorDistance_2 = cv::norm(pixelColorFloat - targetColorImg_2);
            float colorDistance_3 = cv::norm(pixelColorFloat - targetColorImg_3);

            if (colorDistance_1 <= colorRange_1 || colorDistance_2 <= colorRange_1 || colorDistance_3 <= colorRange_1) {
                correspondenceOnPixel_1++;
            }
            if (greenDistance_2 <= greenRange_1 || greenDistance_3 <= greenRange_1) {
                greenPixelCount++;
            }
            if (blackDistance_1 <= blackRange_1) {
                blackPixelCount++;
            }
        }
    }

    double percentage_1 = (correspondenceOnPixel_1 / coloredPixelCount) * 100.0;
    double roundedPercentage_1 = std::round(percentage_1 * 10.0) / 10.0;

    double greenPercentage = (greenPixelCount / coloredPixelCount) * 1000.0;
    double greenRoundedPercentage = std::round(greenPercentage * 10.0) / 10.0;

    double blackPercentage = (blackPixelCount / coloredPixelCount) * 1000.0;
    double blackRoundedPercentage = std::round(blackPercentage * 10.0) / 10.0;

    for (const auto& tuple : pixelColorVector) {

        cv::Vec3f color = std::get<0>(tuple);
        double pixelCount = std::get<1>(tuple);
        double coloredPixelCount = std::get<2>(tuple);
        float colorDistance_1 = cv::norm(color - targetColorClusteredImg_1);
        float colorDistance_2 = cv::norm(color - targetColorClusteredImg_2);

        if (((colorDistance_1 <= clusterColorRange) || (colorDistance_2 <= clusterColorRange)) && (greenRoundedPercentage > 5.0) && (roundedPercentage_1 > 5.0)) {
            return true;
        }
    }
    return false;
}


bool bread(cv::Mat img, cv::Mat clusteredImg, std::vector<std::tuple<cv::Vec3f, double, double>> pixelColorVector) {

    cv::Vec3f blackTargetColorImg_1(6.0f, 56.0f, 157.0f);
    float blackRange_1 = 25.0f;

    cv::Vec3f greenTargetColorImg_2(40.0f, 145.0f, 128.0f);
    cv::Vec3f greenTargetColorImg_3(168.0f, 218.0f, 218.0f);
    float greenRange_1 = 25.0f;

    cv::Vec3f targetColorImg_1(26.0f, 36.0f, 76.0f);
    cv::Vec3f targetColorImg_2(0.0f, 2.0f, 3.0f);
    float colorRange_1 = 10.0f;

    cv::Vec3f targetColorImg_4(0.0f, 2.0f, 3.0f);
    float colorRange_2 = 5.0f;

    cv::Vec3f targetColorClusteredImg_1(104.0f, 137.0f, 161.0f);
    cv::Vec3f targetColorClusteredImg_2(66.0f, 106.0f, 112.0f);
    cv::Vec3f targetColorClusteredImg_3(34.0f, 46.0f, 59.0f);
    cv::Vec3f targetColorClusteredImg_4(45.0f, 67.0f, 101.0f);
    float clusterColorRange_1 = 30.0f;
    float clusterColorRange_2 = 10.0f;

    double correspondenceOnPixel_1 = 0;
    double correspondenceOnPixel_2 = 0;
    double coloredPixelCount = 0;
    double greenPixelCount = 0;
    double blackPixelCount = 0;

    for (int i = 0; i < img.rows; ++i) {
        for (int j = 0; j < img.cols; ++j) {
            cv::Vec3b pixelColor = img.at<cv::Vec3b>(i, j);
            
            int blackThreshold = 5;
            if (pixelColor[0] > blackThreshold || pixelColor[1] > blackThreshold || pixelColor[2] > blackThreshold) {
                coloredPixelCount++;
            }

            cv::Vec3f pixelColorFloat(static_cast<float>(pixelColor[0]), static_cast<float>(pixelColor[1]), static_cast<float>(pixelColor[2]));

            float blackDistance_1 = cv::norm(pixelColorFloat - blackTargetColorImg_1);
            
            float greenDistance_2 = cv::norm(pixelColorFloat - greenTargetColorImg_2);
            float greenDistance_3 = cv::norm(pixelColorFloat - greenTargetColorImg_3);

            float colorDistance_1 = cv::norm(pixelColorFloat - targetColorImg_1);
            float colorDistance_2 = cv::norm(pixelColorFloat - targetColorImg_2);

            float colorDistance_4 = cv::norm(pixelColorFloat - targetColorImg_4);

            if (colorDistance_1 <= colorRange_1 || colorDistance_2 <= colorRange_1) {
                correspondenceOnPixel_1++;
            }
            if (colorDistance_4 <= colorRange_2) {
                correspondenceOnPixel_2++;
            }
            if (greenDistance_2 <= greenRange_1 || greenDistance_3 <= greenRange_1) {
                greenPixelCount++;
            }
            if (blackDistance_1 <= blackRange_1) {
                blackPixelCount++;
            }
        }
    }

    double percentage_1 = (correspondenceOnPixel_1 / coloredPixelCount) * 100.0;
    double roundedPercentage_1 = std::round(percentage_1 * 10.0) / 10.0;

    double percentage_2 = (correspondenceOnPixel_2 / coloredPixelCount) * 100.0;
    double roundedPercentage_2 = std::round(percentage_2 * 10.0) / 10.0;
    
    double totalPercentage = ((correspondenceOnPixel_1 + correspondenceOnPixel_2) / coloredPixelCount) * 100.0;
    double totalRoundedPercentage = std::round(totalPercentage * 10.0) / 100.0;

    double greenPercentage = (greenPixelCount / coloredPixelCount) * 1000.0;
    double greenRoundedPercentage = std::round(greenPercentage * 10.0) / 10.0;

    double blackPercentage = (blackPixelCount / coloredPixelCount) * 1000.0;
    double blackRoundedPercentage = std::round(blackPercentage * 10.0) / 10.0;

    for (const auto& tuple : pixelColorVector) {

        cv::Vec3f color = std::get<0>(tuple);
        double pixelCount = std::get<1>(tuple);
        double coloredPixelCount = std::get<2>(tuple);
        float colorDistance_1 = cv::norm(color - targetColorClusteredImg_1);
        float colorDistance_2 = cv::norm(color - targetColorClusteredImg_2);
        float colorDistance_3 = cv::norm(color - targetColorClusteredImg_3);
        float colorDistance_4 = cv::norm(color - targetColorClusteredImg_4);

        if ((colorDistance_1 <= clusterColorRange_1) || (colorDistance_2 <= clusterColorRange_1) || (colorDistance_3 <= clusterColorRange_1) || (colorDistance_4 <= clusterColorRange_2)) {
            if ((greenRoundedPercentage > 25.0) || (totalRoundedPercentage < 705.0)) {
                // DEBUG
                // std::cout << "BlackPercentage: " << blackRoundedPercentage << std::endl;
                return true;
            }
        }
    }
    return false;
}
