#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

using namespace cv;
using namespace std;

void myHarris(Mat src, Mat &dst, int k_size, float k, int th)
{

    // passo 1 -> calcolare derivare parziali secondo x e secondo y
    Mat sob_x, sob_y, sob_2x, sob_2y, sob_xy;
    Sobel(src, sob_x, CV_32F, 1, 0, k_size);
    Sobel(src, sob_y, CV_32F, 0, 1, k_size);

    // passo 2 -> elevare alla 2a le derivate parziali e moltiplicare le derivate parziali (no ^2) tra di loro
    pow(sob_x, 2, sob_2x);          // -> C00
    pow(sob_y, 2, sob_2y);          // -> C11
    multiply(sob_x, sob_y, sob_xy); // C01, C10 (stessi)

    // passo 3 -> eseguire un blur (7x7) delle derivate
    GaussianBlur(sob_2x, sob_2x, Size(7, 7), 2.0, 0, BORDER_DEFAULT);   // solo su x
    GaussianBlur(sob_2y, sob_2y, Size(7, 7), 0, 2.0, BORDER_DEFAULT);   // solo su y
    GaussianBlur(sob_xy, sob_xy, Size(7, 7), 2.0, 2.0, BORDER_DEFAULT); // su tutti e 2

    // passo 4 -> si ottengono C00, C01, C10, C11
    /*
        C00 =   sob_2x
        C11 =   sob_2y
        C10 =   sob_xy
        C01 =   sob_xy
    */

    // passo 5 -> calcolare indice R
    Mat det1, det2, trace, determinante, R;
    multiply(sob_2x, sob_2y, det1);
    multiply(sob_xy, sob_xy, det2);
    determinante = (det1 - det2);       // ottengo determinante [(C00*C11)-(C10*C01)]
    pow((sob_2x + sob_2y), 2.0, trace); // ottengo trace [(C00+C11)^2]
    R = (determinante - k * trace);     // calcolo R [determinante+k*trace]

    // punto 6 -> normalizzare indice R [0,255]
    normalize(R, R, 0, 255, NORM_MINMAX, CV_32FC1);
    Mat R_int = R/(float)255; imshow("R:",R_int); // stampo R 

    // passo 7 -> sogliare R
    dst = src.clone();
    for (int i = 0; i < src.rows; i++)
        for (int j = 0; j < src.cols; j++)
            if ((int)R.at<float>(i, j) > th) // quando
                circle(dst, Point(j, i), 6, Scalar(0), 1, 8, 0); // disegno cerchio
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        cout << "Usage: image_name, threshold" << endl;
        exit(-1);
    }
    
    int kernel_size = 3;

    Mat src = imread(argv[1], IMREAD_GRAYSCALE), dst;
    if (!src.data)
        return -1;
    int th = atoi(argv[2]);

    myHarris(src, dst, kernel_size, 0.04, th);
    imshow("Input", src);
    imshow("Output", dst);
    waitKey(0);
    return 0;
}
