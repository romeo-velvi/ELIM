#include <opencv2/opencv.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

using namespace cv;
using namespace std;

void myHoughCerchi(Mat src, Mat &dst, int threshold)
{
    // passo 1: sfocare
    Mat gauss;
    GaussianBlur(src, gauss, Size(5, 5), 0, 0);

    // passo 2: applicare Canny per edge detection
    Mat canny;
    Canny(gauss, canny, 50, 200, 3);

    // passo 3: calcola la massima distanza raggiungibile tra 2 punti (lung. cateto 1 e cateto 2)
    int ipot = hypot(src.rows, src.cols);

    // passo 4: inizializzare la matrice dei "voti" (Condiviso tra i punti che saranno sinusoidi nello spazio dei paramentri)
    Mat H = Mat::zeros(ipot * 2, 180, CV_8UC1); // sulle righe abbiamo rho (2*lung.massima considera pos+neg), sulle colonne abbiamo theta (0,180° -> 0,2pi);

    // passo 5: fase di voto 
    for (int x = 0; x < canny.rows; x++)
        for (int y = 0; y < canny.cols; y++)
            if (canny.at<uchar>(x, y) == 255) // scansione di ogni pixel di edge (bianco -> 255)
            // per ogni punto, si controlla se la sinusoide rappresentante nello spazio dei parametri ricade nei punti dello spazio (matrice) dei voti
                for (int theta = 0; theta < 180; theta++) // per ogni 0,2pi -> 0,180°
                {
                    double rad = (theta - 90) * CV_PI / 180; // caolco da angolo in radianti da -90 a 90
                    double rho = abs( x * cos(rad) + y * sin(rad) ); // calcolo rho 
                    H.at<uchar>( (int) rho, theta)++; // incrementa voto rho, theta
                }
    
    // passo 6: si controlla quali valori della matrice dei voti siano considerabili (in base ad un valore in input -> min val da poter considerare i parametri per una "retta plausibile")
    for (int rho = 0; rho < H.rows; rho++)
        for (int theta = 0; theta < H.cols; theta++)
            if (H.at<uchar>(rho, theta) > threshold) // check treshold
            {
                double rad = (theta - 90) * CV_PI / 180; // radianti da -90 a 90
                double x = rho * cos(rad); // trova x
                double y = rho * sin(rad); // trova y
                Point pt1(cvRound(y + ipot * cos(rad)),cvRound(x + ipot * (- sin(rad)))); //trova punto sapendo la formula x/y = d * (-sin_t/cos_t)
                Point pt2(cvRound(y - ipot * cos(rad)),cvRound(x - ipot * (- sin(rad)))); //trova punto sapendo la formula x/y = d * (-sin_t/cos_t)
                line(dst, pt1, pt2, Scalar(0), 2, 0); // traccia la linea
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
    Mat dst = src.clone();
    myHoughCerchi(src, dst, atoi(argv[2]));
    imshow("Immagine originale", src);
    imshow("dst", dst);
    waitKey(0);
    destroyAllWindows();
    return 0;
}