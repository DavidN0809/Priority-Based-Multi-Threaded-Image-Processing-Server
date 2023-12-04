#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

int main(int argc, char **argv) {
    if (argc != 5) {
        std::cerr << "Usage: display <Image_Path1> <Title1> <Image_Path2> <Title2>" << std::endl;
        return 1;
    }

    // Load the first image
    cv::Mat image1 = cv::imread(argv[1], cv::IMREAD_COLOR);
    if (image1.empty()) {
        std::cerr << "Could not open or find the first image" << std::endl;
        return 1;
    }

    // Load the second image
    cv::Mat image2 = cv::imread(argv[3], cv::IMREAD_COLOR);
    if (image2.empty()) {
        std::cerr << "Could not open or find the second image" << std::endl;
        return 1;
    }

    // Create two windows
    cv::namedWindow(argv[2], cv::WINDOW_AUTOSIZE);
    cv::namedWindow(argv[4], cv::WINDOW_AUTOSIZE);

    // Display images
    cv::imshow(argv[2], image1);
    cv::imshow(argv[4], image2);

    // Wait for a key press
    cv::waitKey(0);
    return 0;
}

