#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

using namespace cv;
using namespace std;

#define VR_MEDIANA 0
#define VR_MEDIA 1
#define VR_MAT_FILTER_SIZE 3

// funione per calcolare il valore medio in un intorno 3x3.
unsigned char findMedian(Mat &img, int i, int j, int tipo);
// convoluzione filtro su matrice
void passFilter(Mat &src, Mat &dest, int tipo);

int main(int argc, char **argv)
{

	// Load an image
	Mat src = imread(argv[1], IMREAD_GRAYSCALE);

	// Check if image is loaded fine
	if (src.empty())
	{
		printf(" Error opening image\n");
		return -1;
	}

	// INSERIMENTO BORDO
	// calcolo grandezza del bordo
	int br = (VR_MAT_FILTER_SIZE - 1) / 2; cout<<br<<endl;
	// inserisco bordo di dimensioni calcolate prima
	copyMakeBorder(src, src, br, br, br, br, BORDER_REFLECT, 0);

	// visualizzazioni delle immagini (con bordo)
	namedWindow("showimg", WINDOW_NORMAL);
	imshow("showimg", src);
	waitKey(0);

	Mat dst;
	passFilter(src,dst, VR_MEDIANA);

	namedWindow("showimg", WINDOW_NORMAL);
	imshow("showimg", dst);
	waitKey(0);

	medianBlur(src,dst, 15); //funzione librerira 15X15

	namedWindow("showimg", WINDOW_NORMAL);
	imshow("showimg", dst);
	waitKey(0);

	return 0;
	// terminaizione del programma
}

unsigned char findMedian(Mat &img, int i, int j, int tipo)
{
	unsigned char res[9];
	/*/c
	res[0] = dst.at<unsigned char>(i,j);
	//n
	res[1] = dst.at<unsigned char>(i-1,j);
	//no
	res[2] = dst.at<unsigned char>(i-1,j-1);
	//ne
	res[3] = dst.at<unsigned char>(i-1,j+1);
	//s
	res[4] = dst.at<unsigned char>(i+1,j);
	//so
	res[5] = dst.at<unsigned char>(i+1,j-1);
	//se
	res[6] = dst.at<unsigned char>(i+1,j+1);
	//o
	res[7] = dst.at<unsigned char>(i,j-1);
	//e
	res[8] = dst.at<unsigned char>(i,j+1);
	//l'esercizio equivalente è quello riportato di sotto (per calcolarsi un intorno generico):
	 */
	// Si prende gli indici della matrice 3x3 ove centro è un generico pixel i,j dell'immagine (paddingata)
	int z = 0;
	for (int s = -1; s <= 1; s++)
	{
		for (int t = -1; t <= 1; t++)
		{
			res[z++] = img.at<unsigned char>(i + s, j + t);
		}
	}

	unsigned char x;
	int sum = 0;
	switch (tipo)
	{
	case VR_MEDIA:

		for (int k = 0; k < 9; k++)
		{
			sum = sum + res[k];
		}
		x = sum / 9;

		break;

	case VR_MEDIANA:

		sort(res, res + (int)(sizeof(res) / sizeof(res[0])));
		x = res[5];

		break;

	default:
		break;
	}

	return x;
}

void passFilter(Mat &src, Mat &dst, int tipo)
{
	// Esercizio
	dst = Mat(src.rows, src.cols, CV_8UC1);
	for (int i = 0; i < dst.rows; i++)
	{
		if (i == 0 || i == dst.rows)
		{ // è il bordo
			continue;
		}
		for (int j = 0; j < dst.cols; j++)
		{
			if (j == 0 || j == dst.cols)
			{ // è il bordo
				continue;
			}
			dst.at<unsigned char>(i, j) = findMedian(src, i, j, tipo);
		}
	}
}
