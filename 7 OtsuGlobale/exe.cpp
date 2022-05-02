#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
using namespace std;
using namespace cv;

void myOtsu(Mat& src, Mat& dest)
{
    // 1 smooth
    GaussianBlur(src, src, Size(3, 3), 0, 0);

    // 2 - Calcolare l'istogramma dell'immagine
	double histogram[256] = { 0 };
    for (int i = 0; i < src.rows; i++)
        for (int j = 0; j < src.cols; j++)
            histogram[src.at<uchar>(i, j)]++; //Calcola istogramma

	// 3 - Calcolare istogramma normalizzato e media globale
	double normHistogram[256] = { 0 };
	double mediaGlobale = 0;
    for (int i = 0; i < 256; i++)
    {
        normHistogram[i] = histogram[i] / (src.rows * src.cols); //Calcola istogramma normalizzato
        mediaGlobale += i * normHistogram[i]; //Calcola intensita globale media m_G
    }

    // 4 - init var
    double P1k=0, p1k=0, m1k, m2k, P2k, p2k;
    double varianzaInterclasse = 0.0, maxVarianza = 0.0;
    int sogliaOttimale = 0;

    // 5 for calcolo
    for (int i = 0; i < 256; i++)
    {
        P1k += normHistogram[i];        // somma cumulativa P1
        P2k = 1 - P1k;                  // somma cumulativa P2
        p1k += i * normHistogram[i];    // Media ponderata P1
        p2k = mediaGlobale - p1k;       // Media ponderata P2 
        m1k = p1k / P1k;                // valore intensità medio C1
        m2k = p2k / P2k;                // valore intensità medio C2
        varianzaInterclasse = P1k * P2k * pow((m1k - m2k), 2);
        if (varianzaInterclasse > maxVarianza) //se la varianza all'i-sema iterazione è maggiore vado a modificare il valore
        {
            maxVarianza = varianzaInterclasse; // massima varianza trovata
            sogliaOttimale = i; //k*
        }
    }

    cout << "La soglia ottimale e': " << sogliaOttimale << endl;
    cout << "La max varianza interclasse e': " << maxVarianza << endl;
    dest = Mat::zeros(src.rows, src.cols, CV_8UC1);

    // 6 treshold
    threshold(src,dest,sogliaOttimale,255,THRESH_BINARY);

}


int main(int argc, char** argv)
{

    Mat img = imread(argv[1], IMREAD_GRAYSCALE);
    Mat dest = Mat::zeros(img.rows, img.cols, CV_8U);
    myOtsu(img, dest);
    imshow("Original", img);
    imshow("Otsu", dest);
    waitKey(0);
}