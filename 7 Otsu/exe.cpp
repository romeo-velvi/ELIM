#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
using namespace std;
using namespace cv;

#define H_SIZE 256

void myOtsu(Mat &src, Mat &dest)
{
    /*
    1° Passo:
    - Calcolare l'istogramma normalizzato dell'immagine filtrata
    - Calcolare l'intensità globale media m_G
    */
    double histogram[H_SIZE] = {0};
    double normHistogram[H_SIZE] = {0};
    double MN = src.rows * src.cols;
    double globAVG = 0;
    for (int i = 0; i < src.rows; i++)
    {
        for (int j = 0; j < src.cols; j++)
        {
            histogram[src.at<uchar>(i, j)]++;
        }
    }
    for (int i = 0; i < H_SIZE; i++)
    {
        normHistogram[i] = histogram[i] / MN;
        globAVG += i * normHistogram[i];
    }
    /*
    2° Passo:
    - Calcolare le somme cumulative P1(k) per k=0,1,...,L-1
    - Calcolare le medie cumulative 𝑚(k) per k=0,1,...,L-1
    - Calcolare la varianza interclasse 𝜎(2B)(k) per k=0,1,...,L-1
    - Calcolare la soglia k*, ovvero il valore k per cui 𝜎(2B)(k) è
    massimo
    */
    double P1k = 0 , p1k = 0, m1k = 0, m2k = 0, P2k = 0, p2k = 0;
    double varianzaInterclasse = 0.0, maxVarianza = 0.0;
    int sogliaOttimale = 0;
    for (int i = 0; i < H_SIZE; i++)
    {
        P1k += normHistogram[i];
        p1k += i * normHistogram[i];
        P2k = 1 - P1k;
        p2k = globAVG - p1k;
        m1k = p1k / P1k;
        m2k = p2k / P2k;
        varianzaInterclasse = P1k * P2k * pow((m1k - m2k), 2);
        if (varianzaInterclasse > maxVarianza)
        {
            maxVarianza = varianzaInterclasse;
            sogliaOttimale = i;
        }
    }
    cout << "La soglia ottimale e': " << sogliaOttimale << endl;
    cout << "La max varianza interclasse e': " << maxVarianza << endl;
    dest = Mat::zeros(src.rows, src.cols, CV_8UC1);
    
    for (int i = 0; i < src.rows; i++)
    {
        for (int j = 0; j < src.cols; j++)
        {
            if (src.at<uchar>(i, j) > sogliaOttimale)
            {
                dest.at<uchar>(i, j) = 255;
            }
        }
    }
}


int main(int argc, char **argv)
{
    if (argc != 2)
    {
        cout << "Usage: " << argv[0] << " image_name" << endl;
        exit(-1);
    }
    Mat original = imread(argv[1], IMREAD_GRAYSCALE);
    if (original.empty())
    {
        cout << "Wrong image selected! Try again..." << endl;
        exit(-1);
    }
    /*
    0° Passo:
    - Effettuare uno smoothing dell'immagine usando un filtro Gaussiano
    */
    Mat smoothed;
    GaussianBlur(original, smoothed, Size(3, 3), 0, 0);
    Mat dest;
    myOtsu(smoothed, dest);
    imshow("Original", original);
    imshow("Otsu", dest);
    waitKey(0);
}