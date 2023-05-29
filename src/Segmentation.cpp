//--------------------------------------------------------------------------------------------------
//              AUTHOR: MARTONE TOMMASO     ID: 2016972
//--------------------------------------------------------------------------------------------------

#include "../include/Segmentation.h"
#include <fstream>
#include <iostream>

using namespace cv;
using namespace std;

/*
void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
    if (event == EVENT_LBUTTONDOWN) {
        Mat image = *(Mat*)userdata;
        Mat image_out = image.clone();
        Mat img_hsv;
        Rect rect(x, y, 1, 1);
        Scalar mean = cv::mean(image_out(rect));
        cout << "Blue: " << mean[0] << "  Green: " << mean[1] << "  Red: " << mean[2] << endl;
        cvtColor(image_out, img_hsv, COLOR_BGR2HSV);
        Scalar mean_hsv = cv::mean(img_hsv(rect));
        cout << "H: " << mean_hsv[0] << "  S: " << mean_hsv[1] << "  V: " << mean_hsv[2] << endl;
    }
}
*/

int segmentation(int argc, string argv) {
    
    // Open an image file
    string inputPath, outputFile,outputFileMask, base_filename,base_filename_mask,base_filename_txt,ground_truth;
    Mat src;
    try{

        inputPath = argv;
        inputImg = imread(inputPath);

        // Get the file name from path
        if(inputPath.find_last_of('\\')  != -1) {
            base_filename = inputPath.substr(inputPath.find_last_of('\\') + 1);
            base_filename_txt = inputPath.substr(inputPath.find_last_of('\\') + 1);
            base_filename_mask = inputPath.substr(inputPath.find_last_of('\\') + 1);
            ground_truth = inputPath.substr(inputPath.find_last_of('\\') + 1);
        }
        else {
            base_filename = inputPath.substr(inputPath.find_last_of('/') + 1);
            base_filename_txt = inputPath.substr(inputPath.find_last_of('/') + 1);
            base_filename_mask = inputPath.substr(inputPath.find_last_of('/') + 1);
            ground_truth = inputPath.substr(inputPath.find_last_of('/') + 1);
        }
        // Generate the output file name
        base_filename.replace(base_filename.end()-4, base_filename.end(), "_output.jpg");
        base_filename_mask.replace(base_filename_mask.end()-4, base_filename_mask.end(), "_output_mask.jpg");
        ground_truth.replace(ground_truth.end()-4, ground_truth.end(), ".png");

        // Path of the output directory
        string output_path = "../dataset/test/output_segmentation/";
        // Concatenate the path of the directory with the file name
        outputFile = output_path + base_filename;
        outputFileMask = output_path + base_filename_mask;
    }
    catch(...){
        cout << "Could not open the input image, check the program arguments" << endl;
        return -1;
    }

    //--------------------------------------------------------------------------------------------------
    //                               Reading from file txt
    //--------------------------------------------------------------------------------------------------

    base_filename_txt.replace(base_filename_txt.end()-4, base_filename_txt.end(), "_output.txt");
    string path = "../dataset/test/bounding_boxes/" + base_filename_txt;
    ifstream myfile(fileName.c_str(), ios_base::in);

    if (!myfile.is_open())
    {
        cout << " Failed to read file " << endl;
    }

    vector<vector<int>> window_coord;       // (x, y, width, height)
    vector<int> tmp;

    int num;
    int count = 0;

    while (myfile >> num) {
        tmp.push_back(num);
        count++;
        if (count == 4)
        {
            window_coord.push_back(tmp);
            tmp.clear();
            count = 0;
        }
    }
    myfile.close();

    Mat image_copy = src.clone();                   
    Mat image_windows = image_copy.clone();

    //--------------------------------------------------------------------------------------------------
    //                 Equalize light brightness respect to the hands
    //--------------------------------------------------------------------------------------------------
    int x_angle;
    int y_angle;
    int width;
    int height;
    vector<Mat> image_crop;
    vector<string> name_windows = { "Crop 1", "Crop 2", "Crop 3", "Crop 4", "Crop 5" };

    Mat image_hsv, image_hsv_crop, temp;
    cvtColor(image_copy, image_hsv, COLOR_BGR2HSV);
    Scalar brightness = 0;
    vector<double> alpha;
    vector<cv::Mat> chn_hsv_crop;

    //----------- Image cropping ------------

    for (int i = 0; i < window_coord.size(); i++)
    {
        x_angle = window_coord[i][0];
        y_angle = window_coord[i][1];
        width = window_coord[i][2];
        height = window_coord[i][3];

        Rect crop_window(x_angle, y_angle, width, height);
        image_crop.push_back(image_copy(crop_window));

        cvtColor(image_crop[i], image_hsv_crop, COLOR_BGR2HSV);  

        split(image_hsv_crop, chn_hsv_crop);
        brightness = 0;     // Reinitialize at each iteration
        brightness = cv::mean(chn_hsv_crop[2]);
        alpha.push_back(brightness[0]);
    }

    double alpha_tot = 0, sum = 0;
    for (int n = 0; n < alpha.size(); n++)
    {
        sum += alpha[n];
    }
    alpha_tot = (sum / alpha.size());           

    Mat image_bgr_norm;
    vector<cv::Mat> chn_hsv;
    split(image_hsv, chn_hsv);
    chn_hsv[2].convertTo(chn_hsv[2], -1, 110 / alpha_tot);      // Normalization of brightness
    merge(chn_hsv, image_hsv);
    cvtColor(image_hsv, image_bgr_norm, COLOR_HSV2BGR);    

    //--------------------------------------------------------------------------------------------------
    //              HSV Space pre-filtering
    //--------------------------------------------------------------------------------------------------

    medianBlur(image_hsv, image_hsv, 9);       // Smoothing the image

    Vec3b channels_HSV;
    Vec3b channels_BGR;

    // fill the image with black pixel except for the ones into the ranges
    for (int i = 0; i < image_bgr_norm.rows; i++) {
        for (int j = 0; j < image_bgr_norm.cols; j++) {
            channels_HSV = image_hsv.at<Vec3b>(i, j);
            channels_BGR = image_bgr_norm.at<Vec3b>(i, j);
            image_copy.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 0);     // Paint the mask in black

            if ((0 < channels_HSV[0] && channels_HSV[0] < 50) &&
                (40 < channels_HSV[1] && channels_HSV[1] < 180) &&
                (channels_HSV[2] < 170) &&
                (20 < channels_BGR[0]) && (40 < channels_BGR[1]) && (95 < channels_BGR[2]) &&
                (channels_BGR[1] < channels_BGR[2]) && (channels_BGR[0] < channels_BGR[2]) &&
                (channels_BGR[0] < 240 || channels_BGR[1] < 240 || channels_BGR[2] < 250) &&   // Remove white component
                (15 < abs(channels_BGR[2] - channels_BGR[1])))
            {
                image_copy.at<cv::Vec3b>(i, j) = cv::Vec3b(255, 255, 255);
            }
        }
    }

    //--------------------------------------------------------------------------------------------------
    //              Create the mask
    //--------------------------------------------------------------------------------------------------
    Mat image_mask = image_copy.clone();

    for (int i = 0; i < image_copy.rows; i++) 
    {
        for (int j = 0; j < image_copy.cols; j++) 
        {
            image_mask.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 0);
           
            for (int m = 0; m < window_coord.size(); m++)
            {
                x_angle = window_coord[m][0];
                y_angle = window_coord[m][1];
                width = window_coord[m][2];
                height = window_coord[m][3];

                if ((x_angle < j && j < x_angle + width) && (y_angle < i && i < y_angle + height))
                {
                    //cout << "coordinates: " << x_angle << " " << y_angle << " " << width << " " << height << " " << m << endl;
                    image_mask.at<cv::Vec3b>(i, j) = image_copy.at<cv::Vec3b>(i, j);
                }
            }
        }
    }

    // Erosion and dilation for smoothing the border of the mask and remove small segmentation
    Mat kernel = Mat::ones(7, 7, CV_8U);
    for (int p = 0; p < 5; p++)
    {
        erode(image_mask, image_mask, kernel);
        dilate(image_mask, image_mask, kernel);
    }

    // Saving the biniary mask
    Mat binary_mask;
    cvtColor(image_mask, binary_mask, COLOR_BGR2GRAY);

    // Inverting the mask and giving a color to highlinght the hands
    for (int i = 0; i < image_mask.rows; i++)
    {
        for (int j = 0; j < image_mask.cols; j++)
        {
            if (image_mask.at<Vec3b>(i, j) == Vec3b(0, 0, 0)) {                
                image_mask.at<Vec3b>(i, j) = Vec3b(255, 255, 255);
            }
            else {
                image_mask.at<Vec3b>(i, j) = Vec3b(0, 0, 0);
            }
        }
    }

    // Generate random colors
    vector<Vec3b> colors;
    for (size_t i = 0; i < window_coord.size(); i++)
    {
        int b = 0; // theRNG().uniform(0, 256);
        int g = 0;
        int r = theRNG().uniform(0, 256);
        colors.push_back(Vec3b((uchar)b, (uchar)g, (uchar)r));
    }

    // Give a different color to each window where we detected an hand
    for (int m = 0; m < window_coord.size(); m++)
    {
        x_angle = window_coord[m][0];
        y_angle = window_coord[m][1];
        width = window_coord[m][2];
        height = window_coord[m][3];

        for (int i = 0; i < image_mask.rows; i++)
        {
            for (int j = 0; j < image_mask.cols; j++)
            {
                if ((x_angle < j && j < x_angle + width) && (y_angle < i && i < y_angle + height))
                {
                    if (image_mask.at<Vec3b>(i, j) == Vec3b(0, 0, 0)) {
                        image_mask.at<Vec3b>(i, j) = colors[m];
                    }
                }
            }
        }
    }

    Mat output_mask;  // Result output

    // If we use image_mask with 3 different color channels the output_mask will not show any mask on it
    // setting 1 channel to 0 and the others channels with diffenert color will return the output_mask with 
    // only one color even though the mask has a different color for each hand.
    src.copyTo(output_mask, image_mask);

    
    // Adding the rectangle of the hand detection
    //for (int i = 0; i < window_coord.size(); i++)
    //{
    //    x_angle = window_coord[i][0];
    //    y_angle = window_coord[i][1];
    //    width = window_coord[i][2];
    //    height = window_coord[i][3];
    //    Rect crop_window(x_angle, y_angle, width, height);
    //    rectangle(output_mask, crop_window, Scalar(255, 0, 255), 2);      // Draw it on image
    //}

    //--------------------------------------------------------------------------------------------------
    //              Print images
    //--------------------------------------------------------------------------------------------------

    //namedWindow("mask", WINDOW_NORMAL);
    //resizeWindow("mask", 740, 540);
    //imshow("mask", image_mask);

    namedWindow("Normalized image", WINDOW_NORMAL);
    resizeWindow("Normalized image", 740, 540);
    imshow("Normalized image", image_bgr_norm);
    waitKey(0);
    //setMouseCallback("Normalized image", CallBackFunc, (void*)&image_bgr_norm);


    namedWindow("Hand segmantation", WINDOW_NORMAL);
    resizeWindow("Hand segmantation", 740, 540);
    imshow("Hand segmantation", output_mask);
    waitKey(0);

    //--------------------------------------------------------------------------------------------------
    //              Saving results
    //--------------------------------------------------------------------------------------------------

    imwrite(outputFileMask, binary_mask);
    imwrite(outputFile, output_mask);

    //--------------------------------------------------------------------------------------------------
    //              Pixel accurancy
    //--------------------------------------------------------------------------------------------------
    double IoU;
    
    string path_gt = "../dataset/test/mask/"+ground_truth;
    Mat mask_truth = imread(path_gt, IMREAD_GRAYSCALE);

    IoU = IoU_func(binary_mask, mask_truth);
    cout << "Pixel accurancy (IoU): " << IoU << endl;

    return 0;
}

double IoU_func(Mat mask, Mat ground_truth)    // Mat object must be GRAYSCALE
{
    int area_overlap_hand = 0;
    double area_union_hand = 0;    
    double hand_detected = 0;
    int area_overlap_no_hand = 0;
    double area_union_no_hand = 0;
    double no_hand_detected = 0;
    double IoU;

    if(mask.rows * mask.cols != ground_truth.rows * ground_truth.cols)
    {
        cout << "IMAGES DIMENSION MISMATCH" << endl;
        return 0;
    }                               

    for (int i = 0; i < mask.rows; i++)  {
        for (int j = 0; j < mask.cols; j++)  {
            
            if (((int)mask.at<uchar>(i, j) == 255) && ((int)ground_truth.at<uchar>(i, j) == 255)) {
                area_overlap_hand++;
            }
            if (((int)mask.at<uchar>(i, j) == 255) || ((int)ground_truth.at<uchar>(i, j) == 255)) {
                area_union_hand++;
            }            

            if (((int)mask.at<uchar>(i, j) == 0) && ((int)ground_truth.at<uchar>(i, j) == 0)) {
                area_overlap_no_hand++;
            }
            if (((int)mask.at<uchar>(i, j) == 0) || ((int)ground_truth.at<uchar>(i, j) == 0)) {
                area_union_no_hand++;
            }
        }
    }
    hand_detected = area_overlap_hand / area_union_hand; 
    no_hand_detected = area_overlap_no_hand / area_union_no_hand;

    IoU = 100*(hand_detected + no_hand_detected) / 2;       // percentile
    return IoU;
}