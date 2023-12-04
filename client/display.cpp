#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cerr << "Usage: display <Image_Path> <Title>" << std::endl;
        return 1;
    }

    cv::Mat image = cv::imread(argv[1], cv::IMREAD_COLOR);
    if (image.empty()) {
        std::cerr << "Could not open or find the image" << std::endl;
        return 1;
    }

    cv::namedWindow(argv[2], cv::WINDOW_AUTOSIZE);
    cv::imshow(argv[2], image);

    cv::waitKey(0);
    return 0;
}

