#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"
#include <iostream>

using namespace cv;
using namespace std;
// RNG rng(12345);

#define VR_BORDER_REPLICATE 1
#define VR_BORDER_REFLECT 2
#define VR_BORDER_WRAP 3
#define VR_BORDER_CONSTANT 4

void vr_copyMakeBorder(Mat &src, Mat &dst, int top, int bottom, int left, int right, int type);
void showmaxmat(Mat);
void compareBorder(Mat &src, int top, int bottom, int left, int right);

int main(int argc, char **argv)
{

	// Declare the variables
	Mat src;
	Mat dst, dst1, dst2, dst3;

	// Load an image -> NO BORDER
	src = imread(argv[1], IMREAD_GRAYSCALE);

	// Check if image is loaded fine
	if (src.empty())
	{
		printf(" Error opening image\n");
		return -1;
	}

	int top = 10, bottom = 20, left = 30, right = 40;

	vr_copyMakeBorder(src, dst, top, bottom, left, right, VR_BORDER_CONSTANT);
	vr_copyMakeBorder(src, dst1, top, bottom, left, right, VR_BORDER_REFLECT);
	vr_copyMakeBorder(src, dst2, top, bottom, left, right, VR_BORDER_REPLICATE);
	vr_copyMakeBorder(src, dst3, top, bottom, left, right, VR_BORDER_WRAP);

	namedWindow("showimg", WINDOW_NORMAL);
	imshow("showimg", dst);
	waitKey(0);
	namedWindow("showimg", WINDOW_NORMAL);
	imshow("showimg", dst1);
	waitKey(0);
	namedWindow("showimg", WINDOW_NORMAL);
	imshow("showimg", dst2);
	waitKey(0);
	namedWindow("showimg", WINDOW_NORMAL);
	imshow("showimg", dst3);
	waitKey(0);

	compareBorder(src, top, bottom, left, right);

	/*
	// Initialize arguments for the filter
	top = (int) (0.05*src.rows);
	bottom = top;
	left = (int) (0.05*src.cols);
	right = left;

	//metto le destinazioni di ogni immagine
	Mat dst1, dst2, dst3, dst4;
	copyMakeBorder( src, dst1, top, bottom, left, right, borderType1, 0 );
	copyMakeBorder( src, dst2, top, bottom, left, right, borderType2, 0 );
	copyMakeBorder( src, dst3, top, bottom, left, right, borderType3, 0 );
	copyMakeBorder( src, dst4, top, bottom, left, right, borderType4, (rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255)) );


	// visualizzazioni delle immagini
	//namedWindow( "Padding EXE", WINDOW_NORMAL );resizeWindow("Padding EXE",300,300);imshow( "Padding EXE", src ); waitKey( 0 );


	// creazione dell'immagine canvas ove verranno inseriti gli elaborati
	Mat canvas (src.rows, src.cols, src.flags, (rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255)));

	// ridimensionamento degli elaborati in modo da farli tutti entrare (precisamente) nel canvas
	Mat dst1_1, dst2_1, dst3_1, dst4_1;
	resize(dst1, dst1_1, Size(src.rows/2,src.cols/2));
	resize(dst2, dst2_1, Size(src.rows/2,src.cols/2));
	resize(dst3, dst3_1, Size(src.rows/2,src.cols/2));
	resize(dst4, dst4_1, Size(src.rows/2,src.cols/2));

	// creazione della porzione di immagine in cui entraranno gli elaborati un elaborato
	Mat insetImage1 (canvas, Rect(0,0,src.rows/2,src.cols/2));
	Mat insetImage2 (canvas, Rect(0,(src.cols/2),src.rows/2,src.cols/2));
	Mat insetImage3 (canvas, Rect((src.rows/2),0,src.rows/2,src.cols/2));
	Mat insetImage4 (canvas, Rect((src.rows/2),(src.cols/2),src.rows/2,src.cols/2));

	// copia degli elaborati nell'intera immagine
	dst1_1.copyTo(insetImage1);
	dst2_1.copyTo(insetImage2);
	dst3_1.copyTo(insetImage3);
	dst4_1.copyTo(insetImage4);

	imshow("Overlay Image", canvas);
	waitKey( 0 );

	// gli ultimi tre blocchi possono essere riassunti in questo modo:

	// dst1_1.copyTo(Mat(canvas, Rect(0,0,src.rows/2,src.cols/2)));
	// dst2_1.copyTo(Mat(canvas, Rect(0,(src.cols/2),src.rows/2,src.cols/2)));
	// dst3_1.copyTo(Mat(canvas, Rect((src.rows/2),0,src.rows/2,src.cols/2)));
	// dst4_1.copyTo(Mat(canvas, Rect((src.rows/2),(src.cols/2),src.rows/2,src.cols/2)));
	// imshow("Overlay Image Copy", canvas);
	// waitKey( 0 );


   */

	return 0;
	// terminaizione del programma
}

void showmaxmat(Mat m)
{
	double min, max;
	cv::minMaxLoc(m, &min, &max);
	cout << max << endl;
}

void vr_copyMakeBorder(Mat &src, Mat &dst, int top, int bottom, int left, int right, int type)
{

	int img_row = src.rows;
	int img_col = src.cols;
	Mat mb, mt, mr, ml;

	// initializzo la matrice
	dst = Mat::zeros(img_row + top + bottom, img_col + left + right, CV_8UC1);
	// copio prima l'immagine
	src.copyTo(dst(Rect(left, top, img_col, img_row)));

	switch (type)
	{

	case VR_BORDER_CONSTANT:

		// future implementazioni per colore bordo...

		break;

	case VR_BORDER_REFLECT:

		// PRENDO I RETTANGOLI NELL'IMMAGINE E LI RUOTO (prima top e bottom)
		flip(src(Rect(0, 0, img_col, top)), mt, 0);
		flip(src(Rect(0, img_row - bottom, img_col, bottom)), mb, 0);

		// prendo i rettangoli (top e bottom) e li inserisco ell'immagine dest
		mt.copyTo(dst(Rect(left, 0, img_col, top)));
		mb.copyTo(dst(Rect(left, img_row + top, img_col, bottom)));

		// stesso con l'immagine dst mi ricavo i lati
		flip(dst(Rect(left, 0, left, dst.rows)), ml, 1);
		flip(dst(Rect(dst.cols - 2 * right, 0, right, dst.rows)), mr, 1);

		// prendo i nuovi rettangoli (left e right) e li inserisco nell'immagine dest
		ml.copyTo(dst(Rect(0, 0, left, dst.rows)));
		mr.copyTo(dst(Rect(dst.cols - right, 0, right, dst.rows)));

		break;

	case VR_BORDER_REPLICATE:

		// inizializzo i mt,mb,mr,ml
		mt = Mat(1, dst.cols, CV_8UC1);
		mb = Mat(1, dst.cols, CV_8UC1);
		mr = Mat(dst.rows, 1, CV_8UC1);
		ml = Mat(dst.rows, 1, CV_8UC1);

		// PRENDO I RETTANGOLI NELL'IMMAGINE E LI RUOTO (prima top e bottom)
		mt = dst(Rect(0, top, dst.cols, 1));
		mb = dst(Rect(0, top + img_row - 1, dst.cols, 1));

		for (int i = 0; i < top; i++)
		{
			mt.copyTo(dst(Rect(0, top - i, dst.cols, 1)));
		}

		for (int i = 0; i < bottom; i++)
		{
			mb.copyTo(dst(Rect(0, top + img_row + i, dst.cols, 1)));
		}

		// prendo i nuovi rettangoli (left e right) e li inserisco nell'immagine dest
		ml = dst(Rect(left, 0, 1, dst.rows));
		mr = dst(Rect(left + img_col - 1, 0, 1, dst.rows));

		for (int i = 0; i < left; i++)
		{
			ml.copyTo(dst(Rect(left - i, 0, 1, dst.rows)));
		}

		for (int i = 0; i < right; i++)
		{
			mr.copyTo(dst(Rect(left + img_row + i, 0, 1, dst.rows)));
		}

		break;

	case VR_BORDER_WRAP:

		// TOP-CENTER
		src(Rect(0, src.rows - top, src.cols, top)).copyTo(dst(Rect(left, 0, src.cols, top)));
		// TOP-LEFT
		src(Rect(src.cols - left, src.rows - top, left, top)).copyTo(dst(Rect(0, 0, left, top)));
		// LEFT-CENTER
		src(Rect(src.cols - left, 0, left, src.rows)).copyTo(dst(Rect(0, top, left, src.rows)));
		// LEFT-BOTTOM
		src(Rect(src.cols - left, 0, left, bottom)).copyTo(dst(Rect(0, dst.rows - bottom, left, bottom)));
		// BOTTOM-CENTER
		src(Rect(0, 0, src.cols, bottom)).copyTo(dst(Rect(left, dst.rows - bottom, src.cols, bottom)));
		// BOTTOM-RIGHT
		src(Rect(0, 0, right, bottom)).copyTo(dst(Rect(dst.cols - right, dst.rows - bottom, right, bottom)));
		// RIGHT-CENTER
		src(Rect(0, 0, right, src.rows)).copyTo(dst(Rect(dst.cols - right, top, right, src.rows)));
		// TOP-RIGHT
		src(Rect(0, src.rows - top, right, top)).copyTo(dst(Rect(dst.cols - right, 0, right, top)));

		break;

	default:
		break;
	}
};

void compareBorder(Mat &src, int top, int bottom, int left, int right)
{
	Mat dst, dst1, dst2, dst3;
	vr_copyMakeBorder(src, dst, top, bottom, left, right, VR_BORDER_CONSTANT);
	vr_copyMakeBorder(src, dst1, top, bottom, left, right, VR_BORDER_REFLECT);
	vr_copyMakeBorder(src, dst2, top, bottom, left, right, VR_BORDER_REPLICATE);
	vr_copyMakeBorder(src, dst3, top, bottom, left, right, VR_BORDER_WRAP);

	Mat adst, adst1, adst2, adst3;
	copyMakeBorder(src, adst, top, bottom, left, right, VR_BORDER_CONSTANT, 0);
	copyMakeBorder(src, adst1, top, bottom, left, right, VR_BORDER_REFLECT, 0);
	copyMakeBorder(src, adst2, top, bottom, left, right, VR_BORDER_REPLICATE, 0);
	copyMakeBorder(src, adst3, top, bottom, left, right, VR_BORDER_WRAP, 0);

	Mat a;
	Mat mask = Mat();
	int dtype = -1;

	// show difference
	subtract(dst, adst, a, mask, dtype);
	showmaxmat(a);
	subtract(dst1, adst1, a, mask, dtype);
	showmaxmat(a);
	subtract(dst2, adst2, a, mask, dtype);
	showmaxmat(a);
	subtract(dst3, adst3, a, mask, dtype);
	showmaxmat(a);
}