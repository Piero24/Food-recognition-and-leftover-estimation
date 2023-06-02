#include <opencv2/core.hpp> 
#include <opencv2/imgproc.hpp> 
#include <opencv2/highgui.hpp> 
#include <iostream> 


// g++ task.cpp -o task -I/usr/local/include/opencv4 -lopencv_highgui -lopencv_core -lopencv_imgcodecs -lopencv_imgproc -lopencv_features2d

// ./task leftover1.jpg

 
using namespace std; 
using namespace cv; 
 
cv::Mat img; 
cv::Mat resultImage; 
//ceramica 62-71
//vetro 48-54
int minRadius = 62; 
int maxRadius = 71; 
 
void detectCircles(int, void*) { 
    cv::Mat grayImage; 
    cv::cvtColor(img, grayImage, cv::COLOR_BGR2GRAY); 
 
    cv::Mat cannyImg; 
    cv::Canny(grayImage, cannyImg, 44, 264); 
 
    cv::Mat aftGauss; 
    cv::GaussianBlur(cannyImg, aftGauss, cv::Size(5, 5), 0); 
 
    std::vector<cv::Vec3f> circles; 
    cv::HoughCircles(aftGauss, circles, cv::HOUGH_GRADIENT, 1, 1, 100, 20, minRadius, maxRadius); 
 
    std::vector<cv::Vec3f> outerCircles; 
    for (const cv::Vec3f& circ : circles) { 
        bool isInner = false; 
 
        for (const cv::Vec3f& existingCirc : outerCircles) { 
            cv::Point2f center1(circ[0], circ[1]); 
            cv::Point2f center2(existingCirc[0], existingCirc[1]); 
 
            float radius1 = circ[2]; 
            float radius2 = existingCirc[2]; 
 
            // Threshold che permette una leggera sovrapposizione 
            float threshold = 100; 
            float distance = cv::norm(center1 - center2); 
            float sumRadii = radius1 + radius2 - threshold; 
 
            if (distance < sumRadii) { 
                isInner = true; 
                break; 
            } 
        } 
 
        if (!isInner) { 
            outerCircles.push_back(circ); 
        } 
    } 
 
    resultImage = img.clone(); 
    for (const auto& circle : outerCircles) { 
        cv::Point center(cvRound(circle[0]), cvRound(circle[1])); 
        int radius = cvRound(circle[2]); 
        cv::circle(resultImage, center, radius, cv::Scalar(0, 255, 0), 2); 
    } 
 
    cv::imshow("Result", resultImage); 
} 
 
void onMinRadiusChange(int, void*) { 
    detectCircles(0, 0); 
} 
 
void onMaxRadiusChange(int, void*) { 
    detectCircles(0, 0); 
} 
 
int main(int argc, char* argv[]) { 
    string i1 = argv[1]; 
    if ((i1 == "food_image.jpg" || i1 == "leftover1.jpg"  || i1 == "leftover2.jpg" || i1 == "leftover3.jpg")) { 
        cout << "Nice image man!\n"; 
    } else { 
        cout << "Wrong image man!\n"; 
        return 0; 
    } 
 
 //resize a 320 e 240
    Mat src = imread(i1); 
    resize(src, src, cv::Size(320,240));
    img = src.clone(); 
    resultImage = img.clone(); 
 
    cv::namedWindow("Result"); 
    cv::createTrackbar("Min Radius", "Result", &minRadius, 300, onMinRadiusChange); 
    cv::createTrackbar("Max Radius", "Result", &maxRadius, 300, onMaxRadiusChange); 
 
    detectCircles(0, 0); 
 
    cv::waitKey(0); 
    return 0; 
}
