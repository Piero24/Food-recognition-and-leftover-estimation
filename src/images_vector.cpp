#include <iostream>
#include <opencv2/opencv.hpp>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

int main(int argc, char** argv)
{
	
	if(! argv[1])
	{
		std::cout <<  "Cannot open or find the folder" << std::endl;
		cv::waitKey(3000);
		return -1;
	}

	std::string folderPath = argv[1];
	std::vector<fs::directory_entry> imageFiles;

	for (const auto& entry : fs::directory_iterator(folderPath))
	{
		if (entry.is_regular_file())
		{
			std::string filePath = entry.path().string();
			cv::Mat image = cv::imread(filePath);
			std::string extension = entry.path().extension().string();
            if (extension == ".png" || extension == ".bmp" || extension == ".jpg")
            {
                imageFiles.push_back(entry);
            }
            else{std::cout <<  "This is not an image file" << std::endl;}
            
			if (!image.empty())
			{
				cv::namedWindow("Image");
				cv::imshow("Image", image);
				std::cout << "Images number: " << imageFiles.size() << std::endl;
				cv::waitKey(0);
			}
			else
			{
				std::cout << "Failed to load image: " << filePath << std::endl;
			}
		}
		
	}
	
	
	return 0;
}
