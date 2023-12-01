#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, char** argv) {
    if (argc != 3) {
        cout << "Usage: convert <Input_Image_Path> <Output_Image_Path>" << endl;
        return -1;
    }

    Mat colorImage = imread(argv[1], IMREAD_COLOR);
    if (!colorImage.data) {
        cout << "No image data" << endl;
        return -1;
    }

    Mat greyImage;
    cvtColor(colorImage, greyImage, COLOR_BGR2GRAY);

    if (!imwrite(argv[2], greyImage)) {
        cout << "Failed to save the image" << endl;
        return -1;
    }

    return 0;
}

