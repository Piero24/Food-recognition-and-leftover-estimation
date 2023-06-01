
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
using namespace std;
using namespace cv;
int main(int argc, char *argv[])
{

    string i1=argv[1];
    if((i1=="food_image.jpg"||i1=="leftover1.jpg"||i1=="leftover2.jpg"||i1=="leftover3.jpg"))
    {
    	cout <<" Nice image man!\n";
    }
    else
    {
    	cout <<"Wrong image man!\n";
    	return 0;
    }
    Mat src = imread(i1);
    

    src.convertTo(src, -1, 1.0, 40);
    //imshow("Source Image", src);
    //GaussianBlur(src, src, cv::Size(3,3),0);
    cv::pyrMeanShiftFiltering(src,src,14,20); 

  // Reshape the image to a 2D matrix of pixels 
    cv::Mat reshapedImage = src.reshape(0, 1); 
 
    // Convert the image data to float for K-means algorithm 
    cv::Mat data; 
    reshapedImage.convertTo(data, CV_32F); 
 
    // Define the number of clusters 
    int k = 6; 
 
    // Run the K-means algorithm 
    cv::Mat labels, centers; 
    cv::TermCriteria criteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 10, 1.0); 
    cv::kmeans(data, k, labels, criteria, 3, cv::KMEANS_RANDOM_CENTERS, centers); 
 
// Reshape the labels to match the image dimensions 
cv::Mat reshapedLabels = labels.reshape(1, src.rows); 
 
// Replace each label value with the corresponding cluster center value 
cv::Mat clusteredImage = cv::Mat::zeros(src.size(), src.type()); 
for (int i = 0; i < src.rows; i++) { 
    for (int j = 0; j < src.cols; j++) { 
        int label = reshapedLabels.at<int>(i, j); 
        clusteredImage.at<cv::Vec3b>(i, j) = centers.at<cv::Vec3f>(label, 0); 
    } 
}
    // Convert the clustered image back to 8-bit unsigned integer 
    clusteredImage.convertTo(clusteredImage, CV_8U); 
 
    src= clusteredImage;
 
 
     // better results during the use of Distance Transform
    Mat mask;
    inRange(src, Scalar(255, 255, 255), Scalar(255, 255, 255), mask);
    src.setTo(Scalar(0, 0, 0), mask);
    // Show output image
//    imshow("Black Background Image", src);
     
    // Create a kernel that we will use to sharpen our image
    Mat kernel = (Mat_<float>(3,3) <<
                  1,  1, 1,
                  1, -8, 1,
                  1,  1, 1);
    Mat imgLaplacian;
    filter2D(src, imgLaplacian, CV_32F, kernel);
    Mat sharp;
    src.convertTo(sharp, CV_32F);
    Mat imgResult = sharp - imgLaplacian;
    
    // convert back to 8bits gray scale
    imgResult.convertTo(imgResult, CV_8UC3);
    imgLaplacian.convertTo(imgLaplacian, CV_8UC3);
    
    // imshow( "Laplace Filtered Image", imgLaplacian );
//    imshow( "New Sharped Image", imgResult );
    
    // Create binary image from source image
    Mat bw;
    cvtColor(imgResult, bw, COLOR_BGR2GRAY);
    threshold(bw, bw, 40, 255, THRESH_BINARY | THRESH_OTSU);
//    imshow("Binary Image", bw);
    
    // Perform the distance transform algorithm
    Mat dist;
    distanceTransform(bw, dist, DIST_L2, 3);//3
    // Normalize the distance image for range = {0.0, 1.0}
    // so we can visualize and threshold it
    normalize(dist, dist, 0 , 1.0, NORM_MINMAX);
//    imshow("Distance Transform Image", dist);
    
    // Threshold to obtain the peaks
    // This will be the markers for the foreground objects
    threshold(dist, dist, 0.02, 1.0, THRESH_BINARY);                //da modificare i 0.02
    // Dilate a bit the dist image
    Mat kernel1 = Mat::ones(3, 3, CV_8UC1);
    dilate(dist, dist, kernel1);
 //   imshow("Peaks", dist);
    
    
    // Create the CV_8U version of the distance image
    // It is needed for findContours()
    Mat dist_8u;
    dist.convertTo(dist_8u, CV_8U);
    // Find total markers
    vector<vector<Point> > contours;
    findContours(dist_8u, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    // Create the marker image for the watershed algorithm
    Mat markers = Mat::zeros(dist.size(), CV_32SC1);
    // Draw the foreground markers
    for (size_t i = 0; i < contours.size(); i++)
    {
        drawContours(markers, contours, static_cast<int>(i), Scalar(static_cast<int>(i)+1), -1);
    }
    // Draw the background marker
    circle(markers, Point(5,5), 3, Scalar(255), -1);
    Mat markers8u;
    markers.convertTo(markers8u, CV_8U, 10);
//    imshow("Markers", markers8u);
   
   // Perform the watershed algorithm
    watershed(imgResult, markers);
    Mat mark;
    markers.convertTo(mark, CV_8U);
    bitwise_not(mark, mark);
    //    imshow("Markers_v2", mark); // uncomment this if you want to see how the mark
    // image looks like at that point
    // Generate random colors
    vector<Vec3b> colors;
    for (size_t i = 0; i < contours.size(); i++)
    {
        int b = theRNG().uniform(0, 256);
        int g = theRNG().uniform(0, 256);
        int r = theRNG().uniform(0, 256);
        colors.push_back(Vec3b((uchar)b, (uchar)g, (uchar)r));
    }
    // Create the result image
    Mat dst = Mat::zeros(markers.size(), CV_8UC3);
    // Fill labeled objects with random colors
    for (int i = 0; i < markers.rows; i++)
    {
        for (int j = 0; j < markers.cols; j++)
        {
            int index = markers.at<int>(i,j);
            if (index > 0 && index <= static_cast<int>(contours.size()))
            {
                dst.at<Vec3b>(i,j) = colors[index-1];
            }
        }
    }
    
   
    imshow("Final Result", dst);
    waitKey();
 
 
 
 
 
 
 
 
 
 
 
 
    // Display the original and clustered images 
    //cv::imshow("Original Image", src); 
    //cv::imshow("Clustered Image", clusteredImage); 
    //cv::waitKey(0); 
    waitKey();
    return 0;
}

