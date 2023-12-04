#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

void applyGreyScale(const Mat &input, Mat &output) {
    cvtColor(input, output, COLOR_BGR2GRAY);
}

void applyBlur(const Mat &input, Mat &output) {
    GaussianBlur(input, output, Size(5, 5), 0);
}

void applyVerticalFlip(const Mat &input, Mat &output) {
    flip(input, output, 0); // 0 means flipping around the x-axis
}

void applyHorizontalFlip(const Mat &input, Mat &output) {
    flip(input, output, 1); // 1 means flipping around the y-axis
}

int main(int argc, char** argv) {
    if (argc != 4) {
        cout << "Usage: convert <Input_Image_Path> <Output_Image_Path> <Operation>" << endl;
        return -1;
    }

    Mat inputImage = imread(argv[1], IMREAD_COLOR);
    if (!inputImage.data) {
        cout << "No image data" << endl;
        return -1;
    }

    Mat outputImage;
    string operation = argv[3];

    if (operation == "greyscale") {
        applyGreyScale(inputImage, outputImage);
    } else if (operation == "blur") {
        applyBlur(inputImage, outputImage);
    } else if (operation == "vflip") {
        applyVerticalFlip(inputImage, outputImage);
    } else if (operation == "hflip") {
        applyHorizontalFlip(inputImage, outputImage);
    } else {
        cout << "Invalid operation" << endl;
        return -1;
    }

    if (!imwrite(argv[2], outputImage)) {
        cout << "Failed to save the image" << endl;
        return -1;
    }

    return 0;
}

