// show_image.cpp
// Simple C++ program to display an image using OpenCV2
// Usage: show_image.exe <imagefile>

#include <opencv2/opencv.hpp>
#include <iostream>

int main(int argc, char* argv[]) {
    std::cout << "show_image.exe: OpenCV image viewer started." << std::endl;
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <imagefile>" << std::endl;
        return 1;
    }
    cv::Mat image = cv::imread(argv[1], cv::IMREAD_COLOR);
    if (image.empty()) {
        std::cout << "Could not open or find the image: " << argv[1] << std::endl;
        return 1;
    }
    cv::imshow("Image Viewer", image);
    std::cout << "Press any key in the image window to exit..." << std::endl;
    cv::waitKey(0);
    return 0;
}
