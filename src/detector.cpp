#include <iostream>

#include "opencv2/core.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/ximgproc/segmentation.hpp"

#include "../include/detector.h"
#include "../include/imagePreprocessing.h"//VERIFICARE SE POSSIBILE

// Method returning the image with printed bounding boxes and contours of the food in the dishes
const cv::Mat Detector::getImage() const {
    return img;
}

// Method returning the vector containing all the bounding boxes
const std::vector<cv::Rect> Detector::getRectanglesVector() const {
    return rectanglesVector;
}

// Method returning the vector of contours of the food in the dishes
const std::vector<std::vector<std::vector<cv::Point>>> Detector::getContours() const {
    return contours;
}

// Method for finding the rectangles out of the dishes
std::vector<cv::Rect> Detector::findRectanglesOutsideCircles(const std::vector<cv::Rect>& rectanglesVector, const std::vector<cv::Vec3f>& circlesVector)
{
    std::vector<cv::Rect> result;

    for (const auto& rectangle : rectanglesVector)
    {
        bool isOutsideCircles = true;

        for (const auto& circle : circlesVector)
        {
            cv::Point2f circleCenter(circle[0], circle[1]);
            float circleRadius = circle[2];

            // Compute the center of the rectangle
            cv::Point2f rectangleCenter(rectangle.x + rectangle.width / 2, rectangle.y + rectangle.height / 2);

            // Check if the center of the rectangle is inside one of the circles
            if (cv::norm(rectangleCenter - circleCenter) <= circleRadius)
            {
                isOutsideCircles = false;
                break;
            }
        }

        // Only add the rectangle to the result if it is outside all circles
        if (isOutsideCircles)
        {
            result.push_back(rectangle);
        }
    }

    return result;
}

// Method for eliminating the white or light gray pixels
cv::Mat Detector::noWhiteOnImg(cv::Mat img, int value1, int value2)
{

    cv::Mat tempImg = img.clone();

    cv::Mat noWhiteImg;
    cv::inRange(tempImg , cv::Scalar(value1, value1, value1), cv::Scalar(value2, value2, value2), noWhiteImg);
    cv::Mat postMask;
    tempImg.copyTo(postMask, 255 - noWhiteImg);

    return postMask;
}

//Test 2 method
cv::Mat Detector::test_2(cv::Mat img)
{
	// Mean shift algorithm used for reducing the nuances
	cv::Mat meanShiftImg;
  cv::pyrMeanShiftFiltering(img, meanShiftImg, 25, 45);
  cv::Mat cannyImg;
  cv::Canny(meanShiftImg, cannyImg, 100, 50);
           
  cv::Mat afterKernel1;
  cv::Mat kernel1 = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(21, 21));
  cv::dilate(cannyImg, afterKernel1, kernel1);
            
  cv::Mat colored;
  img.copyTo(colored, afterKernel1);
            
  cv::Mat result1(colored.size(), CV_8UC3);
            
  for(int i=0; i<colored.rows; i++)
  {
  	for(int j=0; j<colored.cols; j++)
    {
  		cv::Vec3b pixel = colored.at<cv::Vec3b>(i, j);
      uchar blue = pixel[0];
      uchar green = pixel[1];
      uchar red = pixel[2];
               
      // Calculate the differences between color channels
			int diffBR = std::abs(blue - red);
			int diffRG = std::abs(red - green);
      int diffBG = std::abs(blue - green);
               
      // Check if the differences are greater than or equal to 45
      if (diffBR >= 45 || diffRG >= 45 || diffBG >= 45)
      {
      	// If the differences are sufficient, keep the pixel in the resulting image
      	result1.at<cv::Vec3b>(i, j) = pixel;
      }
      else
      {
      	result1.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 0);
      }
    }
	}
						
	cv::Mat grayImg;
	cv::cvtColor(result1, grayImg, cv::COLOR_BGR2GRAY);
	// Here an opening is performed by eroding and dilating the found mask by a 9 by 9 square kernel
	cv::Mat afterErode_2, afterDilate_2;
  cv::Mat kernel_2 = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(9, 9));
	cv::erode(grayImg, afterErode_2, kernel_2);
	
	cv::dilate(afterErode_2, afterDilate_2, kernel_2);
	
	cv::Mat colored_2;
	img.copyTo(colored_2, afterDilate_2);
  return colored_2;

}

// Trova i rettangoli dentro i piatti
cv::Rect Detector::rightRectangles(cv::Mat img)
{
	cv::Mat grayasImage;
  cv::cvtColor(img, grayasImage, cv::COLOR_BGR2GRAY);
  // Applica la soglia binaria per creare la maschera
  cv::Mat finalMask;
  cv::threshold(grayasImage, finalMask, 1, 255, cv::THRESH_BINARY);
           
  // Trova tutte le forme presenti nella maschera
  std::vector<std::vector<cv::Point>> contours;
  
  cv::findContours(finalMask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
            
            
  std::vector<cv::Rect> selectedRects;
            
	for (const auto& contour : contours)
	{
		cv::Rect currentRect = cv::boundingRect(contour);

		// Calcola l'area della forma
		double area = cv::contourArea(contour);
		int areaInt = static_cast<int>(area);

		// Se l'area è piccola salta l'area
		if (currentRect.area() > 400){
		   selectedRects.push_back(currentRect);
			//std::cout << "\n\n###### Area del contorno: " << areaInt << std::endl;
		}
	}
									
	// Crea un rettangolo intorno alla prima forma più estesa
	cv::Rect boundingRect;
	for(int i=0; i<selectedRects.size(); ++i)
	{
		double area = selectedRects.at(i).area();
		int areaInt = static_cast<int>(area);
		
		if(area > boundingRect.area())
			{boundingRect = selectedRects.at(i);}
	}
  //std::cout << "\n\n Area del contorno massimo: " << static_cast<int>(boundingRect.area()) << std::endl;
  // Calcola il centro del bounding rectangle principale
	cv::Point mainRectCenter = cv::Point(boundingRect.x + boundingRect.width / 2, boundingRect.y + boundingRect.height / 2);

	// Definisci una soglia di distanza
	double maxDistance = 240.0; // Soglia di distanza massima tra il centro del bounding rectangle principale e un'area per considerarla parte del corpo principale
	for (const auto& contour : selectedRects) {
						
		// Calcola il centro dell'area corrente
		cv::Point currentRectCenter = cv::Point(contour.x + contour.width / 2, contour.y + contour.height / 2);

	// Calcola la distanza tra il centro del bounding rectangle principale e il centro dell'area corrente
		double distance = cv::norm(mainRectCenter - currentRectCenter);
    //std::cout << "Distance: "<< distance << std::endl;	
                
		// Se la distanza è inferiore o uguale alla soglia maxDistance, aggiungi il contorno al vettore finale
		if (distance <= maxDistance) {
			boundingRect = boundingRect | contour;
		}
	}

  return boundingRect;

}

//Dati i giusti bounding boxes, trova i giusti contorni nella maschera
std::vector<std::vector<cv::Point>> Detector::rightContours(cv::Rect boundingRect, cv::Mat img)
{
	Detector detector;
	cv::Mat cloned = cv::Mat::zeros(img.size(), img.type());
	int x = boundingRect.x;
	int y = boundingRect.y;
	int width = boundingRect.width;
	int height = boundingRect.height;
						
	for(int row = y; row < y + height; ++row)
	{
		for(int col = x; col < x + width; ++col)
		{
		  cloned.at<cv::Vec3b>(row, col) = img.at<cv::Vec3b>(row, col);
		}
	}
	cv::Mat finalIMG = detector.test_2(cloned);
	cv::Mat result(img.size(), CV_8UC3);
	cv::Mat kernelDil0 = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(21, 21));
					
	cv::dilate(finalIMG, result, kernelDil0);
					
	img.copyTo(result, result);
	cv::pyrMeanShiftFiltering(result, result, 25, 45);
	
	cv::Mat result0 = detector.noWhiteOnImg(result, 75, 255);
	cv::Mat kernelDil1 = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(15,15));
	cv::dilate(result0, result0, kernelDil1);
	img.copyTo(result0, result0);							
							
	cv::Mat grayasImage;
	cv::cvtColor(result0, grayasImage, cv::COLOR_BGR2GRAY);
	// Applica la soglia binaria per creare la maschera
	cv::Mat finalMask;
	cv::threshold(grayasImage, finalMask, 1, 255, cv::THRESH_BINARY);
										 
	// Trova tutte le forme presenti nella maschera
	std::vector<std::vector<cv::Point>> contours;
							
	cv::findContours(finalMask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
	
	//Tutte le prossime operazioni sono fatte per eliminare il problema di avere contorni che sono formati da più aree separate (nel caso del piatto vuoto salta l'operazione e passa direttamente a dare il vettore dei contorni vuoto)
	if (contours.size()>0)
	{
		std::vector<std::vector<cv::Point>> selectedContours;
		cv::Rect biggestRect = cv::boundingRect(contours[0]);
		
		selectedContours.push_back(contours[0]);
		for (const auto& contour : contours)
		{
			cv::Rect currentRect = cv::boundingRect(contour);

			// Calcola l'area della forma
			double area = cv::contourArea(contour);
			int areaInt = static_cast<int>(area);
			
			// Se l'area è più grande della prima incontrata, allora la si mette al suo posto, altrimenti tiene quella già nel vettore selectedContours
			if (currentRect.area() > biggestRect.area()){
				selectedContours.clear();
				selectedContours.push_back(contour);
				
			}
		}
		return selectedContours;
	}
	else{return contours;}
  
}

Detector Detector::subjectIsolator(cv::Mat img, std::vector<cv::Vec3f>& circlesVector, std::vector<cv::Rect>& rectanglesVector) {
    
    cv::Mat clonedImg = img.clone();
    cv::Mat rectanglesMask = cv::Mat::zeros(clonedImg.size(), CV_8UC1);
    cv::Mat circlesMask = cv::Mat::zeros(clonedImg.size(), CV_8UC1);
    Detector detector;

    std::vector<cv::Rect> boundingRectangles;
    std::vector<cv::Rect> bestRectangles;
   	std::vector<std::vector<std::vector<cv::Point>>> bestContours;
    std::vector<std::vector<cv::Point>> foodsContours;

    // Crea la maschera dell'immagine con tutti i rettangoli
    for (const cv::Rect& rectangle : rectanglesVector) {
        cv::rectangle(rectanglesMask, rectangle, cv::Scalar(255), -1);
    }
		//cv::imshow("Rects", rectanglesMask);
    
    //Crea la maschera  dell'immagine con tutti i cerchi
    for (const cv::Vec3f& circle : circlesVector) {
        cv::Point center(circle[0], circle[1]);
        int radius = circle[2];
        cv::circle(circlesMask, center, radius, cv::Scalar(255), -1);
    }
    //cv::imshow("Cerchi", circlesMask);
    
    /*--------------------------------------
    	LA PARTE SEGUENTE SERVE PER SEGMENTARE IL PANE BENE... SI FANNO MASCHERE SU MASCHERE (METODO UN PO' ROZZO)*/
    
    std::vector<cv::Rect> rectanglesOutsideCircles = detector.findRectanglesOutsideCircles(rectanglesVector, circlesVector);
    
        // Disegna i rettangoli sull'immagine
    cv::Mat outImgF = img.clone();
    for (const auto& rect : rectanglesOutsideCircles) {
        cv::rectangle(outImgF, rect, cv::Scalar(0, 0, 255), 10);
    }

		//Disegna i contorni del cibo dentro il rettangolo nel rettangolo corrente fuori dal cerchio del piatto
		cv::Mat clonedImgF = clonedImg;
		
    
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

            cv::Mat outputImage;
            cv::bitwise_and(clonedImg, clonedImg, outputImage, intersectionMask);



            // Different detecting approach utilized (test_2)
            cv::Mat finalMask_1 = detector.test_2(clonedImg);
            
            cv::Mat clearImg;
            cv::bitwise_and(finalMask_1, finalMask_1, clearImg, intersectionMask);
            //cv::imshow("FinalInter", clearImg);
            
           	cv::Rect boundingRect = detector.rightRectangles(clearImg);
           	
						cv::Mat cloned = cv::Mat::zeros(img.size(), img.type());
						int x = boundingRect.x;
						int y = boundingRect.y;
						int width = boundingRect.width;
						int height = boundingRect.height;
											
						for(int row = y; row < y + height; ++row)
						{
							for(int col = x; col < x + width; ++col)
							{
								cloned.at<cv::Vec3b>(row, col) = img.at<cv::Vec3b>(row, col);
							}
						}
						
						cv::imshow("BoundingRect", cloned);		 
						// Trova tutte le forme presenti nella maschera
						
						std::vector<std::vector<cv::Point>> contours = detector.rightContours(boundingRect, clonedImg);
						
						cv::Mat clonedImg1 = clonedImg;
						cv::drawContours(clonedImg1, contours, -1, cv::Scalar(0, 0, 255), 2);
           	
           	/*cv::Mat newVoid = cv::Mat::zeros(clonedImg.size(), CV_8UC1);
           	cv::fillPoly(newVoid, contours, cv::Scalar(255,255,255));
           	cv::imshow("Maschera", newVoid);
           	cv::Mat newVoid1 = cv::Mat::zeros(clonedImg.size(), CV_8UC1);
           	cv::bitwise_and(clonedImg1, clonedImg1, newVoid1, newVoid);
           	cv::imshow("Contorni++++++++", newVoid1);*/
           	
           	
		        bestRectangles.push_back(boundingRect);
		        bestContours.push_back(contours);
		        
		        cv::waitKey(0);
        }

    }
			std::vector<cv::Rect> allDishes;//Questo vettore conterrà i rettangoli del cibo nei piatti e quelli vuoti
     // Prende i cerchi non sovrapposti ad altri rettangoli
     // per ognuno crea il rettangolo che lo contiene
    for (const auto& circle : circlesVector) {

        cv::Point center(circle[0], circle[1]);
        int radius = circle[2];

        uchar valorePixel = rectanglesMask.at<uchar>(center);
				// Costruisci un rettangolo centrato nel centro del cerchio
        int sideLength = radius; // Puoi impostare la lunghezza del lato come preferisci
        cv::Rect rectangle(center.x - sideLength / 2, center.y - sideLength / 2, sideLength, sideLength);
				bool full = false;
				
        // Verifica le intersezioni del rettangolo con gli altri rettangoli all'interno del cerchio
        for (const auto& rect : bestRectangles)
        {
            // Verifica se il rettangolo ha un'intersezione con il rettangolo corrente
            if (rectangle.x < rect.x + rect.width &&
                rectangle.x + rectangle.width > rect.x &&
                rectangle.y < rect.y + rect.height &&
                rectangle.y + rectangle.height > rect.y)
            {
                full = true;
                allDishes.push_back(rect);
                break;
            }
        }
        if (full == false) {

            cv::Mat tmpMask = cv::Mat::zeros(clonedImg.size(), CV_8UC1);
            cv::circle(tmpMask, center, radius, cv::Scalar(255), -1);

            std::vector<std::vector<cv::Point>> contours;
            cv::findContours(tmpMask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
            
            for (const auto& contour : contours) {
                cv::Rect boundingRect = cv::boundingRect(contour);
                allDishes.push_back(boundingRect);
            }
        }
    }
    
    //Se ha saltato qualche bounding box questo ciclo la aggiunge
		for (const auto& rect : bestRectangles)
    	{
      	auto it = std::find(allDishes.begin(), allDishes.end(), rect);
      	bool present= (it != allDishes.end());
        if(present==false){allDishes.push_back(rect);}
    	}
    // Disegna i rettangoli sull'immagine
    cv::Mat outImg = img.clone();
    
    for (const auto& rect : allDishes) {
        cv::rectangle(outImg, rect, cv::Scalar(0, 0, 255), 10);
    }
		
		
		// Disegna i rettangoli fuori dai piatti sull'immagine
    for (const auto& rect : rectanglesOutsideCircles) {
        cv::rectangle(outImg, rect, cv::Scalar(0, 0, 255), 10);
    }
		cv::imshow("FinalI", outImg);
    for (int i = 0; i < bestContours.size(); i++)
    {
			for (const auto& contour : bestContours)
		  {
		      cv::drawContours(outImg, contour, -1, cv::Scalar(0, 255, 0), 2);
		  }
		}
		
		allDishes.insert(allDishes.end(), rectanglesOutsideCircles.begin(), rectanglesOutsideCircles.end());
		
    cv::imshow("FinalII", outImg);
    Detector finalDetector(outImg, allDishes, bestContours);
    return finalDetector;
}
