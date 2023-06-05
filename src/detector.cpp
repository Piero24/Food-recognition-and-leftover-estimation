#include <iostream>

#include "opencv2/core.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/ximgproc/segmentation.hpp"

#include "../include/detector.h"


cv::Mat noWhiteOnImg(cv::Mat img, int value1, int value2);
cv::Mat foodDetector(cv::Mat img, std::vector<cv::Rect> rectVector);


// Rinominare in boundingBoxMerger quando finito
//
// FORSE MEGLIO CHE QUANDO FINITO RITORNI VETTORE CON FORME VARI CIBI PER FACILITARE PROCESSI SUCCESSIVI
// ANDREMO A RICREARE I RETTANGOLI CHE LI CONTENGONO QUANDO SERVE
cv::Mat subjectIsolator(cv::Mat img, std::vector<cv::Vec3f>& circlesVector, std::vector<cv::Rect>& rectanglesVector) {
    
    cv::Mat clonedImg = img.clone();
    cv::Mat rectanglesMask = cv::Mat::zeros(clonedImg.size(), CV_8UC1);
    cv::Mat circlesMask = cv::Mat::zeros(clonedImg.size(), CV_8UC1);

    std::vector<cv::Rect> boundingRectangles;
    std::vector<cv::Rect> residuals;

    // Crea la maschera dell'immagine con tutti i rettangoli
    for (const cv::Rect& rectangle : rectanglesVector) {
        cv::rectangle(rectanglesMask, rectangle, cv::Scalar(255), -1);
    }

    // Crea la maschera  dell'immagine con tutti i cerchi
    for (const cv::Vec3f& circle : circlesVector) {
        cv::Point center(circle[0], circle[1]);
        int radius = circle[2];
        cv::circle(circlesMask, center, radius, cv::Scalar(255), -1);
    }
    
    // Per ogni cerchio dell'immagine
    for (const auto& circle : circlesVector) {

        cv::Mat circleMask = cv::Mat::zeros(clonedImg.size(), CV_8UC1);

        cv::Point center(circle[0], circle[1]);
        int radius = circle[2];
        cv::circle(circleMask, center, radius, cv::Scalar(255), -1);

        // Trova le parti del cerchio che si intersecano con i rettangoli e crea
        // una maschera per restringere l'area di lavoro
        cv::Mat intersectionMask;
        cv::bitwise_and(circleMask, rectanglesMask, intersectionMask);

        // Calcola la somma di tutti i pixel dell'immagine
        cv::Scalar sum = cv::sum(intersectionMask);

        // Se ci sono pixel bianchi vuol dire che c'è una porzione da controllare
        if (sum[0] > 0) {

            //--------------------------------------------------------------------------------#
            /*
            Questa parte di codice è dedicata al preprocessing della maschera per una determinata portata (es. primo, secondo)
            Qui dentro non arrivano cerchi singoli o rettangoli singoli tipo il pane, ma solo quelli che
            hanno una effettiva intersezione tra un cerchio e uno o più rettangoli.

            Qui sotto ci sono vari metodi mischiati per test.

            OBBIETTIVO: Lavorare la maschera in modo che prenda solo le porzioni di cibo coerenti e non cose extra che ingrandirebbero la bounding box in modo errato.

            POSSIBILI METODI DA IMPLEMENTARE (Aggiungerne se ne vengono in mente altri): 
                - Possibile usare density function? per calcolare area puù densa (centrale) così cose esterne su cluster separato e prendo solo cluster principale.
                - Calcolo il centro della forma completa in base al cerchio. Se c'è un gap importante (tanto nero) tra la figura corretta e la parte extra cancello la parte extra.
                    * ES: Uso distanza dal centro del cerchio per vedere distanza delle figure o dei pixel e vedo se cè un salto importante tra 2 pixel nella stessa traiettoria. se salto importante quello dopo viene coperto.

                - Uso un sistema di cluster (No kmeans non va bene in questo caso) per detectare cosa non fa parte del cibo
                - Blob? Verificare
                - Altre tipologie di preprocessing dell'immagine
                - 
                -
                
            */

            cv::Mat outputImage;
            cv::bitwise_and(clonedImg, clonedImg, outputImage, intersectionMask);



            // TEST 1
            // Rimuove il colore biancora dall'immagine a colori
            cv::Mat noWhite = noWhiteOnImg(outputImage, 188, 255);
            cv::Mat noWhite2 = noWhiteOnImg(noWhite, 50, 185);





            // TEST 2
            cv::Mat grayImage;
            cv::cvtColor(outputImage, grayImage, cv::COLOR_BGR2GRAY);
            // Crea una maschera binaria identificando le aree bianche nell'immagine

            // TEST 2.a
            cv::Mat mask;
            cv::threshold(grayImage, mask, 115, 255, cv::THRESH_BINARY_INV);
            // Applica l'operazione bitwise AND tra l'immagine originale e la maschera invertita
            cv::Mat result;
            cv::bitwise_and(outputImage, outputImage, result, mask);


            // TEST 2.b
            cv::Mat mask2;
            cv::threshold(grayImage, mask2, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);
            cv::Mat result2;
            cv::bitwise_and(outputImage, outputImage, result2, mask2);

            
            // test 2.c
            cv::Mat afterKernel;
            cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(7, 7));
            cv::erode(result, afterKernel, kernel);
            cv::Mat afterKernel2;
            cv::Mat kernel2 = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
            cv::erode(afterKernel, afterKernel2, kernel2);
            cv::Mat afterKernel3;
            cv::Mat kernel3 = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(15, 15));
            cv::dilate(afterKernel2, afterKernel3, kernel3);


            // A questo punto abbiamo 4 immagni diverse da 4 divrse tipologie di lavorazioni sull'immagnie.
            // noWhite2
            // result, result2, afterKernel3


            // cv::imshow("Original", outputImage);
            // cv::imshow("noWhite", noWhite2);
            // cv::imshow("THRESH_BINARY_INV", result);
            //cv::imshow("BINARY_INV_E_OTSU", result2);
            //cv::imshow("BINARY_INV_E_ERODE_DILATE", afterKernel3);
            //cv::waitKey(0);

            
            // DA QUI IN POI SERVE PER RICREARE LA MASCHERA E ANDARE AVANTI CON
            // IL CODICE

            cv::Mat grayasImage;
            cv::cvtColor(afterKernel3, grayasImage, cv::COLOR_BGR2GRAY);
            // Applica la soglia binaria per creare la maschera
            cv::Mat finalMask;
            cv::threshold(grayasImage, finalMask, 1, 255, cv::THRESH_BINARY);
            


            //--------------------------------------------------------------------------------#

            // Trova tutte le forme presenti nella maschera
            std::vector<std::vector<cv::Point>> contours;
            //cv::findContours(intersectionMask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
            cv::findContours(finalMask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
            
            // Crea un rettangolo intorno alla forma
            cv::Rect boundingRect;
            for (const auto& contour : contours) {
                cv::Rect currentRect = cv::boundingRect(contour);

                // Calcola l'area della forma
                double area = cv::contourArea(contour);
                int areaInt = static_cast<int>(area);
                std::cout << "\n\n########################################################################### Area del contorno: " << areaInt << std::endl;
                //std::cout << "\n\n########################################################################### AREA: " << currentRect.area() << std::endl;

                // Se l'area è piccola la saltae la mette nel vettore dei residui
                // altrimenti la unisce alla restante parte come forma unica
                if (currentRect.area() > 300) {
                    boundingRect = boundingRect | currentRect;
                } else {
                    residuals.push_back(currentRect);
                }
            }

            boundingRectangles.push_back(boundingRect);
        }

    }

    // Prende i rettangoli non sovrapposti ad altri cerchi
    for (const auto& rectangle : rectanglesVector) {

        int center_x = rectangle.x + rectangle.width / 2;
        int center_y = rectangle.y + rectangle.height / 2;

        uchar valorePixel = circlesMask.at<uchar>(center_y, center_x);

        if (valorePixel == 0) {
            boundingRectangles.push_back(rectangle);
        }
    }

     // Prende i cerchi non sovrapposti ad altri rettangoli
     // per ogniuno crea il rettangolo che lo contiene
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

    // Disegna i rettangoli sull'immagine
    cv::Mat outImg = img.clone();
    for (const auto& rect : boundingRectangles) {
        cv::rectangle(outImg, rect, cv::Scalar(0, 0, 255), 10);
    }

    // Disegna i residui sull'immagine
    for (const auto& rect : residuals) {
        cv::rectangle(outImg, rect, cv::Scalar(255, 0, 0), 15);
    }

    // Disegna i cerchi sull'immagine
    // RIMUOVERE QUANDO FINITO
    for (const cv::Vec3f& circle : circlesVector) {
        cv::Point center(circle[0], circle[1]);
        int radius = circle[2];
        cv::circle(outImg, center, radius, cv::Scalar(0, 255, 0), 15);
    }


    foodDetector(img, rectanglesVector);
    return outImg;
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

    std::cout << "\U0001F7E3  WARNING: foodDetector() method isn't complete" << std::endl;

    // // Reshape dell'immagine in un vettore di pixel
    // cv::Mat samples = img.reshape(1, img.rows * img.cols);
    // samples.convertTo(samples, CV_32F);

    // // Esegui il K-means
    // int k = 3;
    // cv::TermCriteria criteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 10, 1.0);
    // cv::Mat labels, centers;
    // cv::kmeans(samples, k, labels, criteria, 3, cv::KMEANS_RANDOM_CENTERS, centers);

    // // Ottieni i colori dominanti dai centroidi dei cluster
    // cv::Vec3b dominantColor1 = centers.at<cv::Vec3f>(0);
    // cv::Vec3b dominantColor2 = centers.at<cv::Vec3f>(1);
    // cv::Vec3b dominantColor3 = centers.at<cv::Vec3f>(2);
    // cv::Vec3b dominantColor4 = centers.at<cv::Vec3f>(3);

    // // Segmenta l'immagine utilizzando i colori dominanti
    // cv::Mat segmented = img.clone();
    // cv::Vec3b color1 = dominantColor1;
    // cv::Vec3b color2 = dominantColor2;
    // cv::Vec3b color3 = dominantColor3;
    // cv::Vec3b color4 = dominantColor4;

    // for (int i = 0; i < samples.rows; i++) {
    //     int clusterIdx = labels.at<int>(i);
    //     cv::Vec3b& pixel = segmented.at<cv::Vec3b>(i);
    //     if (clusterIdx == 0)
    //         pixel = color1;
    //     else if (clusterIdx == 1)
    //         pixel = color2;
    //     else if (clusterIdx == 2)
    //         pixel = color3;
    //     else
    //         pixel = color4;
    // }

    // // Visualizza l'immagine segmentata
    // cv::imshow("Segmented Image", segmented);
    // cv::waitKey(0);

    return img;

}