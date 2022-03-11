#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

using namespace cv;
using namespace std;

void hough(Mat src, Mat &dst, int th)
{
    // passo 1 -> eseguire smooth
    Mat gauss;
    GaussianBlur(src, gauss, Size(3, 3), 0, 0);

    // passo 2 -> ricerca edge con canny
    Mat canny;
    Canny(gauss, canny, 150, 230, 3);

    // passo 3 -> dichiarazione range massimi/min dei raggi
    int Rmin = 40;
    int Rmax = 200;
    int sizes[] = {canny.rows, canny.cols, Rmax - Rmin + 1};

    // passo 4 -> dichiarazione matrice voti
    Mat H = Mat(3, sizes, CV_8U, Scalar(0));

    // passo 5 -> calcolo dei voti nella matrice
    double r, a, b;
    for (int x = 0; x < canny.rows; x++) 
    {
        for (int y = 0; y < canny.cols; y++)
        {
            if (canny.at<uchar>(x, y) == 255) // se è edge -> bianco
            {
                for (double theta = 0; theta < 360; theta++) // per ogni angolo 0-360
                {
                    for (r = Rmin; r <= Rmax; r++) // nell'intervallo del raggio
                    {
                        a = y - r * cos(theta * CV_PI / 180); // caloclo a
 
                        b = x - r * sin(theta * CV_PI / 180); // calcolo b

                        if ((a >= 0 && a < canny.cols) && (b >= 0 && b < canny.rows)) // se il centro del cerchio risiede nei contorni dell'immagine
                            H.at<uchar>(b, a, r - Rmin)++; // Riporto nell'intervallo [0,Rmax] 
                    }
                }
            }
        }
        cout<< "stato rimanente: "<< canny.rows-x << "row" << endl;
    }

    // passo 6 -> disegno dei cerchi
    dst = src.clone();
    dst= dst + 50; // inbianco immagine
    for (r = Rmin; r < Rmax; r++)
    {
        for (b = 0; b < canny.rows; b++)
        {
            for (a = 0; a < canny.cols; a++)
            {
                if (H.at<uchar>(b, a, r - Rmin) > th) // se il valore del voto è > del treshold
                {
                    circle(dst, Point(a, b), 3, Scalar(0), 2, 8, 0); // centro
                    circle(dst, Point(a, b), r, Scalar(0), 2, 8, 0); // circonferenza 
                }
            }
        }
    }
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
    Mat dst;
    if (!src.data)
        return -1;
    int th = atoi(argv[2]);
    hough(src, dst, th);
    imshow("Input", src);
    imshow("Output", dst);
    waitKey(0);
    return 0;
}