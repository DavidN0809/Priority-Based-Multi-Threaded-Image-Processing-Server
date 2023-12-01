#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Usage: display <Image_Path>" << std::endl;
        return -1;
    }

    // Read the image
    Mat image = imread(argv[1], IMREAD_COLOR);

    if (!image.data) {
        std::cout << "No image data" << std::endl;
        return -1;
    }

    // Display the image
    namedWindow("Display Image", WINDOW_AUTOSIZE );
    imshow("Display Image", image);

    // Wait for a keystroke in the window
    waitKey(0);

    // Optionally save the image
    // std::string savePath = "~/Documents/GitHub/Socket-Programming-with-C/opencv/savepath/processed_image.jpg";
    // if (!imwrite(savePath, image)) {
    //     std::cout << "Failed to save the image" << std::endl;
    //     return -1;
    // }
    // std::cout << "Image is successfully saved to " << savePath << std::endl;

    return 0;
}

