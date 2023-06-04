#ifndef CIRCULARBOUNDINGBOXES_H
#define CIRCULARBOUNDINGBOXES_H

#include <iostream>
#include <locale>
#include <cmath>
#include <iterator>

#include "opencv2/core.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/ximgproc/segmentation.hpp"

// Yellow dot:
//     Unicode code: \U0001F7E1
//     RGB value: (255, 255, 0)
//     BGR value: (0, 255, 255)


struct ClusterInfo {
    cv::Vec3f cluster;
    int clusterSize;
};


/**
* Trova le bounding box circolari nell'immagine utilizzando l'algoritmo Hough 
* Circle e il clustering k-means.
* @param img L'immagine di input in cui cercare le bounding box.
* @return Un vettore contenente le bounding box circolari trovate.
*/
std::vector<cv::Vec3f> findCircularBoundingBoxes(cv::Mat img);
/**
* Esegue il preprocessing dell'immagine per individuare cerchi di dimensioni maggiori.
* @param img L'immagine di input da elaborare.
* @return L'immagine preprocessata con le operazioni di conversione in scala di grigi, 
* edge detection e smoothing.
*/
cv::Mat biggerCirclesPreprocessing(cv::Mat img);
/**
* Esegue il preprocessing dell'immagine per individuare cerchi di dimensioni minori, 
* tenendo conto dei cerchi di dimensioni maggiori precedentemente individuati.
* @param img L'immagine di input da elaborare.
* @param clusteredCircles Un vettore contenente le informazioni dei cerchi di dimensioni maggiori.
* @return L'immagine preprocessata con le operazioni di mascheramento, 
* conversione in scala di grigi, edge detection e smoothing.
*/
cv::Mat smallerCirclesPreprocessing(cv::Mat img, std::vector<cv::Vec3f>& clusteredCircles);
/**
* Confronta due cerchi in base all'area.
* @param circ1 Il primo cerchio da confrontare.
* @param circ2 Il secondo cerchio da confrontare.
* @return True se l'area del primo cerchio è maggiore dell'area del secondo cerchio, altrimenti false.
*/
bool compareCircles(const cv::Vec3f& circ1, const cv::Vec3f& circ2);
/**
* Verifica se un cerchio interno è contenuto completamente all'interno di un cerchio esterno, escludendo eventuali cerchi specificati.
* @param innerCircle Il cerchio interno da verificare.
* @param externalCircle Il cerchio esterno di riferimento.
* @param clustersExcluded Un vettore contenente cerchi da escludere dalla verifica.
* @return True se il cerchio interno è completamente contenuto nel cerchio esterno, altrimenti false.
*/
bool isInnerCircle(const cv::Vec3f& innerCircle, const cv::Vec3f& externalCircle, std::vector<cv::Vec3f>& clustersExcluded);
/**
* Calcola la distanza euclidea tra due punti rappresentati come vettori tridimensionali.
* @param a Il primo punto.
* @param b Il secondo punto.
* @return La distanza euclidea tra i due punti.
*/
float distance(cv::Vec3f a, cv::Vec3f b);
/**
* Confronta due oggetti ClusterInfo in base alla dimensione del cluster, restituendo true se la dimensione del primo cluster è maggiore della dimensione del secondo cluster.
* @param c1 Il primo oggetto ClusterInfo da confrontare.
* @param c2 Il secondo oggetto ClusterInfo da confrontare.
* @return True se la dimensione del primo cluster è maggiore della dimensione del secondo cluster, altrimenti false.
*/
//bool compareClusterSize(const ClusterInfo& c1, const ClusterInfo& c2);
/**
* Esegue l'algoritmo di k-means per raggruppare i cerchi in cluster in base alla loro distanza.
* @param circles I cerchi da raggruppare.
* @param threshold La soglia di distanza per considerare due cerchi come parte dello stesso cluster.
* @param thresholdForCluster La soglia di dimensione minima per considerare un cluster valido.
* @param numberToReturn Il numero massimo di cluster da restituire.
* @param biggerCircles Un puntatore a un vettore di cerchi di dimensione maggiore (opzionale).
* @return I cerchi raggruppati in cluster.
*/
std::vector<cv::Vec3f> kmeansCircles(std::vector<cv::Vec3f>& circles, float threshold, int thresholdForCluster, int numberToReturn, std::vector<cv::Vec3f>* biggerCircles = nullptr);


#endif


