#include <iostream>

#include "opencv2/core.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/ximgproc/segmentation.hpp"

#include "../include/detector.h"
#include "../include/imagePreprocessing.h"//VERIFICARE SE POSSIBILE

std::vector<cv::Rect> findRectanglesOutsideCircles(const std::vector<cv::Rect>& rectanglesVector, const std::vector<cv::Vec3f>& circlesVector);
cv::Scalar findAverageColor(const cv::Mat& img, const cv::Rect& roi);
std::vector<cv::Rect> filterRectanglesByColor(const std::vector<cv::Rect>& rectanglesOutsideCircles, const cv::Mat& img);
cv::Mat noWhiteOnImg(cv::Mat img, int value1, int value2);
cv::Mat foodDetector(cv::Mat img, std::vector<cv::Rect> rectVector);
cv::Mat test_3(cv::Mat img);
cv::Rect rightRectangles(cv::Mat img);
std::vector<std::vector<cv::Point>> rightContours(cv::Rect boundingRect, cv::Mat img);

// Rinominare in boundingBoxMerger quando finito
//
// FORSE MEGLIO CHE QUANDO FINITO RITORNI VETTORE CON FORME VARI CIBI PER FACILITARE PROCESSI SUCCESSIVI
// ANDREMO A RICREARE I RETTANGOLI CHE LI CONTENGONO QUANDO SERVE

//Togliere stuff e commentare


cv::Mat subjectIsolator(cv::Mat img, std::vector<cv::Vec3f>& circlesVector, std::vector<cv::Rect>& rectanglesVector) {
    
    cv::Mat clonedImg = img.clone();
    cv::Mat clonedImg2 = img.clone();
    cv::Mat rectanglesMask = cv::Mat::zeros(clonedImg.size(), CV_8UC1);
    cv::Mat circlesMask = cv::Mat::zeros(clonedImg.size(), CV_8UC1);

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
    
    std::vector<cv::Rect> rectanglesOutsideCircles = findRectanglesOutsideCircles(rectanglesVector, circlesVector);

    // Stampa i rettangoli trovati
    for (const auto& rectangle : rectanglesOutsideCircles)
    {
      std::cout << "Rectangle: x=" << rectangle.x << ", y=" << rectangle.y << ", width=" << rectangle.width << ", height=" << rectangle.height << std::endl;
      // Trova il colore medio della porzione di immagine nella ROI
      cv::Mat clonedImg0 = img.clone();
      cv::pyrMeanShiftFiltering(clonedImg0, clonedImg0, 20, 40);
      //cv::imshow("imm",clonedImg0);
		  cv::Scalar averageColor = findAverageColor(clonedImg0, rectangle);

		  // Stampa il colore medio //B=78.0344, G=101.049, R=136.987
		  std::cout << "Average Color: B=" << averageColor[0] << ", G=" << averageColor[1] << ", R=" << averageColor[2] << std::endl;
    }
    
    std::vector<cv::Rect> filteredRectangles = filterRectanglesByColor(rectanglesOutsideCircles, clonedImg);
    //return clonedImg2;
    for (const auto& rectangle : filteredRectangles)
    {
        std::cout << "Rectangle: x=" << rectangle.x << ", y=" << rectangle.y << ", width=" << rectangle.width << ", height=" << rectangle.height << std::endl;
    }
    
        // Disegna i rettangoli sull'immagine
    cv::Mat outImgF = img.clone();
    for (const auto& rect : rectanglesOutsideCircles) {
        cv::rectangle(outImgF, rect, cv::Scalar(0, 0, 255), 10);
    }

		//Disegna i contorni del cibo dentro il rettangolo nel rettangolo corrente fuori dal cerchio del piatto
		cv::Mat clonedImgF = clonedImg;
		/*for(int i=0; i < rectanglesOutsideCircles.size(); ++i)
		{
			cv::Mat inImg1 = img.clone();
			cv::Mat outImg1 = cv::Mat::zeros(inImg1.size(), inImg1.type());
		  int x = rectanglesOutsideCircles[i].x;
		  int y = rectanglesOutsideCircles[i].y;
		  int width = rectanglesOutsideCircles[i].width;
		  int height = rectanglesOutsideCircles[i].height;
		  
				for(int row = y; row < y + height; ++row)
				{
				    for(int col = x; col < x + width; ++col)
				    {
				        outImg1.at<cv::Vec3b>(row, col) = inImg1.at<cv::Vec3b>(row, col);
				    }
				}
				cv::Mat blurredOutImg1;
				cv::pyrMeanShiftFiltering(outImg1, blurredOutImg1, 10, 20);
				cv::imshow("Mean", blurredOutImg1);
				//outImg1 = removeBlue(blurredOutImg1);
				blurredOutImg1 = removeBlue(blurredOutImg1);
				cv::imshow("Mean1", outImg1);
				cv::Mat noTicketImg1;
				cv::inRange(blurredOutImg1, cv::Scalar(9, 46, 120), cv::Scalar(35, 70, 160), noTicketImg1);
				cv::Mat noTrayImg1;
				cv::inRange(blurredOutImg1, cv::Scalar(44, 53, 83), cv::Scalar(105, 130, 125), noTrayImg1);
				cv::Mat noTrayImg2;
				cv::inRange(blurredOutImg1, cv::Scalar(0, 0, 51), cv::Scalar(38, 54, 77), noTrayImg2);
				cv::Mat noTrayImg3;
				cv::inRange(blurredOutImg1, cv::Scalar(35, 50, 75), cv::Scalar(55, 70, 90), noTrayImg3);
				cv::Mat noTrayImg4;
				cv::inRange(blurredOutImg1, cv::Scalar(120, 135, 159), cv::Scalar(213, 215, 220), noTrayImg4);
				cv::Mat noWhiteImg1;
				cv::inRange(blurredOutImg1, cv::Scalar(92, 108, 120), cv::Scalar(135, 150, 170), noWhiteImg1);
				cv::Mat noWhiteImg2;
				cv::inRange(blurredOutImg1, cv::Scalar(220, 220, 220), cv::Scalar(245, 245, 245), noWhiteImg2);
				cv::Mat noWhiteImg3;
				cv::inRange(blurredOutImg1, cv::Scalar(195, 195, 190), cv::Scalar(231, 230, 230), noWhiteImg3);
				cv::Mat noWhiteImg4;
				cv::inRange(blurredOutImg1, cv::Scalar(110, 110, 110), cv::Scalar(150, 150, 150), noWhiteImg4);
				cv::Mat noWhiteImg5;
				cv::inRange(blurredOutImg1, cv::Scalar(170, 170, 170), cv::Scalar(190, 190, 190), noWhiteImg5);
				cv::Mat noTicketImg2;
				cv::inRange(blurredOutImg1, cv::Scalar(30, 70, 165), cv::Scalar(63, 111, 203), noTicketImg2);

				cv::Mat postMask0, postMask1, postMask2, postMask3, postMask4, postMask5, postMask6, postMask7, postMask8, postMask9, postMask10;
				
				outImg1.copyTo(postMask0, 255 - noTicketImg1);
				postMask0.copyTo(postMask1, 255 - noTrayImg1);
				postMask1.copyTo(postMask2, 255 - noTrayImg2);
				postMask2.copyTo(postMask3, 255 - noTrayImg3);
				postMask3.copyTo(postMask4, 255 - noTrayImg4);
				postMask4.copyTo(postMask5, 255 - noWhiteImg1);
				postMask5.copyTo(postMask6, 255 - noWhiteImg2);
				postMask6.copyTo(postMask7, 255 - noWhiteImg3);
				postMask7.copyTo(postMask8, 255 - noWhiteImg4);
				postMask8.copyTo(postMask9, 255 - noWhiteImg5);
				postMask9.copyTo(postMask10, 255 - noTicketImg2);
				
				postMask10 = removeBlue(postMask10);
				
				cv::imshow("Test_3", postMask10);
				cv::Mat result2(postMask10.size(), CV_8UC3);
				cv::Mat result3(postMask10.size(), CV_8UC3);
				cv::Mat kernel0 = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5,5));
				cv::erode(postMask10,result2,kernel0);
				cv::imshow("Erosione", result2);
				cv::Mat kernel1 = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(19, 19));
				cv::dilate(result2, result3, kernel1);
				//cv::imshow("Dilatazione1", result3);
				cv::dilate(result3, result3, kernel1);
				cv::imshow("Dilatazione", result3);
				//cv::erode(result3, result3, kernel0);
				//cv::imshow("Dilatazione2", result3);
				clonedImg.copyTo(result3, result3);
				
				cv::Mat grayasImage;
				cv::cvtColor(result3, grayasImage, cv::COLOR_BGR2GRAY);
				// Applica la soglia binaria per creare la maschera
				cv::Mat finalMask;
				cv::threshold(grayasImage, finalMask, 1, 255, cv::THRESH_BINARY);
						     
				// Trova tutte le forme presenti nella maschera
				std::vector<std::vector<cv::Point>> contours;
				//cv::findContours(intersectionMask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
				cv::findContours(finalMask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
				
				// Crea un'immagine vuota per disegnare i contorni
				//cv::Mat contourImage = cv::Mat::zeros(result4.size(), CV_8UC3);
				
				// Cerca la forma più estesa
				cv::Rect boundingRect;
				std::vector<std::vector<cv::Point>> rightContours;
				for (const auto& contour : contours)
				{
					cv::Rect currentRect = cv::boundingRect(contour);

					// Calcola l'area della forma
					double area = cv::contourArea(contour);
					rightContours.push_back(contour);
					if(area > boundingRect.area())
						{boundingRect = currentRect;
        rightContours.clear();
        rightContours.push_back(contour);}
					else{rightContours.pop_back();}
				}
				cv::drawContours(clonedImgF, rightContours, -1, cv::Scalar(0, 0, 255), 2);
				//std::cout<<"Size of right: "<<rightContours.size()<<std::endl;
				
				// Mostra l'immagine dei contorni
				cv::imshow("Contorni", clonedImgF);
				cv::waitKey(0);
				foodsContours.push_back(rightContours.at(0));
				
		}
		
		//cv::imshow("Solo rettangoli", outImg1);
		          
		cv::imshow("finallllllll", clonedImgF);
		cv::waitKey(0);
    return clonedImgF;*/
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



            // test 3
            cv::Mat finalMask_1 = test_3(clonedImg);
            cv::Mat clearImg;
            cv::bitwise_and(finalMask_1, finalMask_1, clearImg, intersectionMask);
            cv::imshow("FinalInter", clearImg);
            
           	cv::Rect boundingRect = rightRectangles(clearImg);
           	
						
											 
						// Trova tutte le forme presenti nella maschera
						std::vector<std::vector<cv::Point>> contours = rightContours(boundingRect, clonedImg);
						
						cv::Mat clonedImg1 = clonedImg;
						cv::drawContours(clonedImg1, contours, -1, cv::Scalar(0, 0, 255), 2);
           		
           	//cv::imshow("Contorni++++++++", clonedImg1);
           	
           	
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
		cv::imshow("FinalI", outImg);
		
		// Disegna i rettangoli fuori dai piatti sull'immagine
    for (const auto& rect : rectanglesOutsideCircles) {
        cv::rectangle(outImg, rect, cv::Scalar(0, 0, 255), 10);
    }
		
    for (int i = 0; i < bestContours.size(); i++)
    {
			for (const auto& contour : bestContours)
		  {
		      cv::drawContours(outImg, contour, -1, cv::Scalar(0, 255, 0), 2);
		  }
		}
    cv::imshow("FinalII", outImg);
		return outImg;
}

//Metodo per trovare i rettangoli fuori dai piatti
std::vector<cv::Rect> findRectanglesOutsideCircles(const std::vector<cv::Rect>& rectanglesVector, const std::vector<cv::Vec3f>& circlesVector)
{
    std::vector<cv::Rect> result;

    for (const auto& rectangle : rectanglesVector)
    {
        bool isOutsideCircles = true;

        for (const auto& circle : circlesVector)
        {
            cv::Point2f circleCenter(circle[0], circle[1]);
            float circleRadius = circle[2];

            // Calcola il centro del rettangolo
            cv::Point2f rectangleCenter(rectangle.x + rectangle.width / 2, rectangle.y + rectangle.height / 2);

            // Verifica se il centro del rettangolo è all'interno di uno dei cerchi
            if (cv::norm(rectangleCenter - circleCenter) <= circleRadius)
            {
                isOutsideCircles = false;
                break;
            }
        }

        // Aggiungi il rettangolo al risultato solo se è al di fuori di tutti i cerchi
        if (isOutsideCircles)
        {
            result.push_back(rectangle);
        }
    }

    return result;
}

//Metodo per calcolare la media del colore dei pixel in un Rect

cv::Scalar findAverageColor(const cv::Mat& img, const cv::Rect& roi)
{
    // Estrai la regione di interesse (ROI) dall'immagine
    cv::Mat roiImage = img(roi);

    // Calcola il colore medio della ROI
    cv::Scalar averageColor = cv::mean(roiImage);

    return averageColor;
}

std::vector<cv::Rect> filterRectanglesByColor(const std::vector<cv::Rect>& rectanglesOutsideCircles, const cv::Mat& img)
{
    std::vector<cv::Rect> filteredRectangles;

    cv::Scalar referenceColor(178, 154, 129); // BGR order

    const double threshold = 100.0;

    for (const auto& rectangle : rectanglesOutsideCircles)
    {
        // Calcola il colore medio della porzione di immagine nel rettangolo
        cv::Scalar averageColor = findAverageColor(img, rectangle);

        // Calcola la distanza euclidea tra il colore medio e il colore di riferimento
        double distance = cv::norm(averageColor, referenceColor, cv::NORM_L2);
				std::cout <<"Distanza: "<<distance<<std::endl;
        // Controlla se la distanza supera il threshold
        if (distance <= threshold)
        {
            filteredRectangles.push_back(rectangle);
        }
    }

    return filteredRectangles;
}

cv::Mat noWhiteOnImg(cv::Mat img, int value1, int value2) {

    cv::Mat tempImg = img.clone();

    cv::Mat noWhiteImg;
    cv::inRange(tempImg , cv::Scalar(value1, value1, value1), cv::Scalar(value2, value2, value2), noWhiteImg);
    cv::Mat postMask;
    tempImg.copyTo(postMask, 255 - noWhiteImg);

    return postMask;
}

//Test 3 method
cv::Mat test_3(cv::Mat img)

{
	cv::Mat meanShiftImg;
  cv::pyrMeanShiftFiltering(img, meanShiftImg, 25, 45);
  cv::Mat cannyImg;
  cv::Canny(meanShiftImg, cannyImg, 100, 50);
           
  cv::Mat afterKernel1;
  cv::Mat kernel1 = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(21, 21));
  cv::dilate(cannyImg, afterKernel1, kernel1);
  //cv::imshow("After dilatation", afterKernel1);
            
  cv::Mat colored;
  img.copyTo(colored, afterKernel1);
  //cv::imshow("Colored dilatation", colored);
            
  cv::Mat result1(colored.size(), CV_8UC3);
            
  for(int i=0; i<colored.rows; i++)
  {
  	for(int j=0; j<colored.cols; j++)
    {
  		cv::Vec3b pixel = colored.at<cv::Vec3b>(i, j);
      uchar blue = pixel[0];
      uchar green = pixel[1];
      uchar red = pixel[2];
               
      // Calcola le differenze tra i canali di colore
			int diffBR = std::abs(blue - red);
			int diffRG = std::abs(red - green);
      int diffBG = std::abs(blue - green);
               
      // Verifica se le differenze sono maggiori o uguali a 45
      if (diffBR >= 45 || diffRG >= 45 || diffBG >= 45)
      {
      	// Se le differenze sono sufficienti, mantieni il pixel nell'immagine risultante
      	result1.at<cv::Vec3b>(i, j) = pixel;
      }
      else
      {
      	result1.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 0);
      }
    }
	}
            
  cv::Mat meanShiftImg_2;
  cv::pyrMeanShiftFiltering(result1, meanShiftImg_2, 25, 45);
  //Toglie la gamma dei colori del biglietto del dessert (sfumature di arancione)
	cv::Mat noTicketImg;
	cv::inRange(meanShiftImg_2, cv::Scalar(75, 130, 195), cv::Scalar(135, 170, 233), noTicketImg);
	cv::Mat postMask0;
	result1.copyTo(postMask0, 255 - noTicketImg);
						
	cv::Mat grayImg;
	cv::cvtColor(postMask0, grayImg, cv::COLOR_BGR2GRAY);
						      
	cv::Mat afterErode_2, afterDilate_2;
  cv::Mat kernelEr_2 = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(9, 9));
	cv::Mat kernelDil_2 = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(9, 9));
	cv::erode(grayImg, afterErode_2, kernelEr_2);
	
	cv::dilate(afterErode_2, afterDilate_2, kernelDil_2);
	
	cv::Mat afterBlur;
	cv::GaussianBlur(afterDilate_2, afterBlur, cv::Size(7,7),0);
	
	cv::Mat colored_2;
	img.copyTo(colored_2, afterBlur);
  return colored_2;

}

cv::Rect rightRectangles(cv::Mat img)
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
		//std::cout << "\n\n!!!!!!!!!!!!!!!!!! Area del contorno: " << areaInt << std::endl;
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
std::vector<std::vector<cv::Point>> rightContours(cv::Rect boundingRect, cv::Mat img)
{
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
	cv::Mat finalIMG = test_3(cloned);
	cv::Mat result(img.size(), CV_8UC3);
	cv::Mat kernelDil0 = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(21, 21));
					
	cv::dilate(finalIMG, result, kernelDil0);
					
	img.copyTo(result, result);
	cv::pyrMeanShiftFiltering(result, result, 25, 45);
	
	cv::Mat result0 = noWhiteOnImg(result, 75, 255);
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
