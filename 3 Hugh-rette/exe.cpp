#include <opencv2/opencv.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

using namespace cv;
using namespace std;

void hough(Mat src, Mat &dst, int threshold)
{
    double theta, rho, t;
    Mat canny, gauss;

    // passo 1: sfocare
    GaussianBlur(src, gauss, Size(5, 5), 0, 0);

    // passo 2: applicare Canny per edge detection
    Canny(gauss, canny, 50, 200, 3);

    // passo 3: calcola la massima distanza raggiungibile tra 2 punti (lung. cateto 1 e cateto 2)
    int d = hypot(src.rows, src.cols);

    // passo 4: inizializzare la matrice dei "voti" (Condiviso tra i punti che saranno sinusoidi nello spazio dei paramentri)
    Mat H = Mat::zeros(d * 2, 180, CV_8UC1); // sulle righe abbiamo rho (2*lung.massima considera pos+neg), sulle colonne abbiamo theta (0,180° -> 0,2pi);

    // passo 5: fase di voto 
    for (int x = 0; x < canny.rows; x++)
    {
        for (int y = 0; y < canny.cols; y++)
        {
            if (canny.at<uchar>(x, y) == 255) // scansione di ogni pixel di edge (bianco -> 255)
            {// per ogni punto, si controlla se la sinusoide rappresentante nello spazio dei parametri ricade nei punti dello spazio (matrice) dei voti
                for (theta = 0; theta < 180; theta++) // per ogni 0,2pi -> 0,180°
                {
                    double angolo = (theta - 90) * CV_PI / 180; // caolco angolo -> radianti 
                    rho = y * cos(angolo) + x * sin(angolo); // calcolo rho = y*cos(angolo)+x*sin(angolo)
                    H.at<uchar>(rho, theta)++; // incrementa voto rho, theta
                }
            }
        }
    }

    src.copyTo(dst);
    int x, y;
    double sin_t, cos_t;

    // passo 6: si controlla quali valori della matrice dei voti siano considerabili (in base ad un valore in input -> min val da poter considerare i parametri per una "retta plausibile")
    for (int rho = 0; rho < H.rows; rho++)
    {
        for (int theta = 0; theta < H.cols; theta++)
        {
            if (H.at<uchar>(rho, theta) >= threshold) // check treshold
            {
                t = (theta - 90) * CV_PI / 180; // angolo
                x = rho * cos(t); // trova x
                y = rho * sin(t); // trova y
                sin_t = sin(t); // sin
                cos_t = cos(t); // cos
                Point pt1(cvRound(x + d * (-sin_t)), cvRound(y + d * cos_t)); //trova punto sapendo la formula x/y = d * (-sin_t/cos_t)
                Point pt2(cvRound(x - d * (-sin_t)), cvRound(y - d * cos_t)); //trova punto sapendo la formula x/y = d * (-sin_t/cos_t)
                line(dst, pt1, pt2, Scalar(150), 2, 0); // traccia la linea
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
    if (!src.data)
        return -1;
    Mat dst;
    hough(src, dst, atoi(argv[2]));
    imshow("Immagine originale", src);
    imshow("dst", dst);
    waitKey(0);
    destroyAllWindows();
    return 0;
}