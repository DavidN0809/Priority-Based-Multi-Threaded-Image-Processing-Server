#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
using namespace cv;

int greyscale_CV( )
{
 

 Mat image;
 image = imread( "image.png", 1 );

 if( !image.data )
 {
   printf( " No image data \n " );
   return -1;
 }

 Mat gray_image;
 cvtColor(image, gray_image, cv::COLOR_BGR2GRAY);

 imwrite( "Gray_Image.png", gray_image );

 return 0;
}
