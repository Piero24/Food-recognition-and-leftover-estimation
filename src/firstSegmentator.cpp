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


//ceramica 62-71
//vetro 48-54

int minRadius = 280; 
int maxRadius = 305; 

//int minRadius = 262; 
//int maxRadius = 278; 

//int minRadius = 200; 
//int maxRadius = 220; 

float distance(cv::Vec3f a, cv::Vec3f b);
std::vector<cv::Vec3f> kmeansCircles(std::vector<cv::Vec3f>& circles, float threshold, int thresholdForCluster);


cv::Mat firstSegmentationFunc(cv::Mat img) {

    cv::Mat image = img.clone();
    cv::Mat grayImage;
    cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);

    cv::Mat cannyImg;
    cv::Canny(grayImage, cannyImg, 44, 264);

    cv::Mat aftGauss;
    cv::GaussianBlur(cannyImg, aftGauss, cv::Size(5, 5), 0);

    std::vector<cv::Vec3f> circles;
    cv::HoughCircles(aftGauss, circles, cv::HOUGH_GRADIENT, 1, 1, 100, 20, minRadius, maxRadius);

    std::sort(circles.begin(), circles.end(), compareCircles);
    //std::vector<cv::Vec3f> outerCircles = removeInnerCircles(circles);

    float threshold = 70.0;  // Soglia per considerare i cerchi come simili
    int thresholdForCluster = 100;

    std::vector<cv::Vec3f> clusteredCircles = kmeansCircles(circles, threshold, thresholdForCluster);










    cv::Mat resultImage = image.clone();
    for (const auto& circle : circles) {

        cv::Point center(cvRound(circle[0]), cvRound(circle[1]));
        int radius = cvRound(circle[2]);
        cv::circle(resultImage, center, radius, cv::Scalar(0, 255, 0), 2);
    }

    for (const auto& circle : clusteredCircles) {

        cv::Point center(cvRound(circle[0]), cvRound(circle[1]));
        int radius = cvRound(circle[2]);
        cv::circle(resultImage, center, radius, cv::Scalar(0, 0, 255), 2);
    }

    // Visualizza l'immagine risultante
    cv::imshow("Result", resultImage);
    cv::waitKey(0);

    std::cout << "\U0001F7E1  WARNING: the thirdSegmentationFunc() method isn't complete" << std::endl;

    return resultImage;

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


// Funzione per calcolare la distanza euclidea tra due punti
float distance(cv::Vec3f a, cv::Vec3f b) {
    return std::sqrt(std::pow((a[0] - b[0]), 2) + std::pow((a[1] - b[1]), 2));
}


// Definisci una struttura per rappresentare un cluster insieme alla sua dimensione
struct ClusterInfo {
    cv::Vec3f cluster;
    int clusterSize;
};

// Predicato personalizzato per il sorting dei cluster in base alla dimensione
bool compareClusterSize(const ClusterInfo& c1, const ClusterInfo& c2) {
    return c1.clusterSize > c2.clusterSize; // Ordine decrescente in base alla dimensione
}


std::vector<cv::Vec3f> kmeansCircles(std::vector<cv::Vec3f>& circles, float threshold, int thresholdForCluster) {
    std::vector<cv::Vec3f> result;

    if (circles.empty()) {
        return result;
    }

    cv::Vec3f centroid = circles[0];
    result.push_back(centroid);

    // Raggruppamento dei cerchi simili
    for (int i = 1; i < circles.size(); i++) {
        bool foundCluster = false;

        // Controllo se il cerchio appartiene a un cluster esistente
        for (int j = 0; j < result.size(); j++) {
            float dist = distance(circles[i], result[j]);
            if (dist <= threshold) {
                // Aggiungi il cerchio al cluster
                foundCluster = true;

                // Calcola la media pesata dei centri
                float totalWeight = result[j][2] + circles[i][2];
                // Media pesata delle coordinate X
                result[j][0] = (result[j][0] * result[j][2] + circles[i][0] * circles[i][2]) / totalWeight;
                // Media pesata delle coordinate Y
                result[j][1] = (result[j][1] * result[j][2] + circles[i][1] * circles[i][2]) / totalWeight;

                // Calcola la media pesata dei raggi
                result[j][2] = totalWeight / 2.0;

                break;
            }
        }

        // Se il cerchio non appartiene a nessun cluster esistente, crea un nuovo cluster
        if (!foundCluster) {
            result.push_back(circles[i]);
        }
    }

    // Verifica il numero di cerchi in ciascun cluster e scarta i cluster con pochi cerchi
    std::vector<ClusterInfo> filteredResult;
    for (const auto& cluster : result) {
        int clusterSize = 0;  // Inizializza la dimensione del cluster a 0

        // Conta il numero di cerchi nel cluster
        for (const auto& circle : circles) {
            float dist = distance(circle, cluster);
            if (dist <= threshold) {
                clusterSize++;
            }
        }

        filteredResult.push_back({cluster, clusterSize});
    }

    // Ordina i cluster in base alla dimensione
    std::sort(filteredResult.begin(), filteredResult.end(), compareClusterSize);

    // Estrai solo i cluster ordinati dal vettore di risultato
    std::vector<cv::Vec3f> sortedClusters;
    for (const auto& clusterInfo : filteredResult) {

        if (sortedClusters.size() < 3){

            if (clusterInfo.clusterSize >= thresholdForCluster) {
                std::cout << "\U0001F7E1"  << "Center: (" << clusterInfo.cluster[0] << ", " << clusterInfo.cluster[1] << "), Radius: " << clusterInfo.cluster[2] << " Cluster Size: " << clusterInfo.clusterSize << std::endl;
                sortedClusters.push_back(clusterInfo.cluster);

            } else {
                break;
            }
        } else {
            break;
        }
        
    }

    return sortedClusters;

} 