#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

using namespace cv;
using namespace std;

void myHoughCerchi(Mat src, Mat &dst, int th, int Rmin, int Rmax)
{
    // passo 1 -> eseguire smooth
    Mat gauss;
    GaussianBlur(src, gauss, Size(3, 3), 0, 0);

    // passo 2 -> ricerca edge con canny
    Mat canny;
    Canny(gauss, canny, 150, 230, 3);

    // passo 3 -> init var-size per matrice voti
    int sizes[] = {canny.rows, canny.cols, Rmax};

    // passo 4 -> dichiarazione matrice voti
    Mat H = Mat::zeros(3, sizes, CV_8U);

    // passo 5 -> calcolo dei voti nella matrice
    for (int x = 0; x < canny.rows; x++)
        for (int y = 0; y < canny.cols; y++)
            if (canny.at<uchar>(x, y) == 255)
                for (int theta = 0; theta < 360; theta++)
                    for (int r = Rmin; r <= Rmax; r++)
                    {
                        double rad = (theta)*CV_PI / 180;
                        double a = x - r * cos(rad);
                        double b = y - r * sin(rad);
                        if ((a >= 0 && a < canny.rows) && (b >= 0 && b < canny.cols))
                            H.at<uchar>(a, b, r)++;
                    }

    // passo 6 -> disegno dei cerchi
    for (int a = 0; a < canny.rows; a++)
        for (int b = 0; b < canny.cols; b++)
            for (int r = Rmin; r < Rmax; r++)
                if (H.at<uchar>(a, b, r) > th)                         // se il valore del voto è > del treshold
                    circle(dst, Point(b, a), r, Scalar(0), 2, 8, 0); // circonferenza
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        cout << "Usage: " << argv[0] << " image_name "
             << " threshold" << endl;
        exit(-1);
    }
    Mat src = imread(argv[1], IMREAD_GRAYSCALE);
    Mat dst = src.clone()-50;
    if (!src.data)
        return -1;
    int th = atoi(argv[2]);
    int Rmin = 40;
    int Rmax = 200;
    myHoughCerchi(src, dst, th, Rmin, Rmax);
    imshow("Input", src);
    imshow("Output", dst);
    waitKey(0);
    return 0;
}