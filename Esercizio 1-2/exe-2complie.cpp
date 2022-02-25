#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

void neighbor3x3(Mat src, Mat &dst);
unsigned char valoreMedio3x3(Mat dst);

int main()
{
	Mat imageSrc = imread("messi5.jpg", IMREAD_GRAYSCALE);
	Mat imageDst;

	neighbor3x3(imageSrc, imageDst);
	imshow("ORIGINAL", imageSrc);
	imshow("VALORE MEDIO", imageDst);
	waitKey(0);
}

void neighbor3x3(Mat src, Mat &dst)
{
	src.copyTo(dst);
	copyMakeBorder(src, src, 1, 1, 1, 1, BORDER_CONSTANT, 0);
	for (int i = 0; i < dst.rows; i++)
		for (int j = 0; j < dst.cols; j++)
			if (i != dst.rows - 1 || j != dst.cols - 1)
				dst.at<unsigned char>(i, j) = valoreMedio3x3(src(Rect(j, i, 3, 3)));
			else
				dst.at<unsigned char>(i, j) = valoreMedio3x3(src(Rect(j - 1, i - 1, 3, 3)));
}

unsigned char valoreMedio3x3(Mat mat)
{
	/*
	int sum = 0;
	for (int i=0; i < 3; i++) {
		for (int  j = 0; j < 3 ; j++)
			sum += mat.at<unsigned char>(i, j);
	}
	//return unsigned char(sum/9);
	*/
	Scalar somma = sum(mat);
	return (unsigned char)(somma[0] / 9);
}