#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
using namespace std;
using namespace cv;

void myOtsu(Mat &src, Mat &dest)
{

    // 1 - Effettuare uno smoothing 
    GaussianBlur(src, src, Size(5, 5), 0, 0);

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
    double varianzaInterclasse = 0.0, maxVarianza = 0.0;
    int sogliaOttimale1 = 0, sogliaOttimale2 = 0;

    // 5 - for calcolo 1
    double P1K=0, p1k=0, m1k=0, P2K = 0, p2k = 0, m2k = 0, P3K = 0, p3k = 0, m3k = 0;
    for (int i = 0; i < 256; i++)
    {
        P1K += normHistogram[i];        // somma cumulativa P1K
        p1k += i * normHistogram[i];    // media ponderata P1K
        m1k = p1k / P1K;                // intensità media C1 
        
    // 6 - for calcolo 2
        P2K = 0, p2k = 0, m2k = 0, P3K = 0, p3k = 0, m3k = 0;
        for (int j = i + 1; j < 256; j++)
        { 
            P2K += normHistogram[j];        // somma cumulativa P2K
            p2k += j * normHistogram[j];    // media ponderata P1K 
            m2k = p2k / P2K;                // intensità media C2

            P3K = 1 - P2K - P1K;                 // somma cumulativa P3K 
            p3k = mediaGlobale - p2k - p1k;      // media ponderata P3K
            m3k = p3k / P3K;                     // intensità media C3
            
            double varianza =                       // calcolo varianza C1,C2,C3    
            P1K * pow((m1k - mediaGlobale), 2) + 
            P2K * pow((m2k - mediaGlobale), 2) + 
            P3K * pow((m3k - mediaGlobale), 2);

            if (varianza > maxVarianza) //se la varianza all'i-sima iterazione è maggiore vado a modificare il valore
            {
                maxVarianza = varianza;
                sogliaOttimale1 = i; // k1*
                sogliaOttimale2 = j; // k2*
            }
        }
    }
    
    cout << "La 1a soglia ottimale e': " << sogliaOttimale1 << endl;
    cout << "La 2a soglia ottimale e': " << sogliaOttimale2 << endl;

    // 7 - settaggio valori
    for (int i = 0; i < src.rows; i++)
        for (int j = 0; j < src.cols; j++){
            uchar val = src.at<uchar>(i, j);
            if (val > sogliaOttimale2)
                dest.at<uchar>(i, j) = 255;
            else if (val > sogliaOttimale1 && val <= sogliaOttimale2)
                dest.at<uchar>(i, j) = 128; // 255:2
        }

}



int main(int argc, char **argv)
{

    Mat original = imread(argv[1], IMREAD_GRAYSCALE);
    if (original.empty())
    {
        cout << "Select image please! Try again..." << endl;
        exit(-1);
    }
    
    Mat dest = Mat::zeros(original.rows, original.cols, CV_8U);
    myOtsu(original, dest);
    
    imshow("Original", original);
    imshow("Otsu", dest);
    
    waitKey(0);
}