#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
using namespace std;
using namespace cv;
#define L_SIZE 256

void myOtsu(Mat &src, Mat &dest)
{
    /*
    1° Passo:
    Punto 1 - Calcolare l'istogramma normalizzato dell'immagine filtrata
    Punto 4 - Calcolare l'intensità globale media m_G
    */
    double histogram[L_SIZE] = {0};
    double normHistogram[L_SIZE] = {0};
    double MN = src.rows * src.cols;
    double mediaGlobale = 0;
    
    for (int i = 0; i < src.rows; i++)
        for (int j = 0; j < src.cols; j++)
            histogram[src.at<uchar>(i, j)]++; //Calcola istogramma

    for (int i = 0; i < L_SIZE; i++)
    {
        normHistogram[i] = histogram[i] / MN;
        mediaGlobale += i * normHistogram[i];
    }

    /*
    2° Passo:
    Punto 2 - Calcolare le somme cumulative P1(k), P2(k) e P3(k) per k=0,1,...,L-1
    Punto 3 - Calcolare le medie cumulative 𝑚1(k), m2(k), m3(k) per k=0,1,...,L-1
    Punto 5 - Calcolare la varianza interclasse 𝜎(2B)(k) per k=0,1,...,L-1
    Punto 6 - Calcolare la soglia k1*, k2*, ovvero il valore k1, k2 per cui 𝜎(2B)(k1,k2) è massimo
    */
    double P1k = 0, p1k = 0, m1k = 0;
    double maxVarianza = 0;
    int sogliaOttimale1 = 0, sogliaOttimale2 = 0;
    for (int i = 0; i < L_SIZE; i++)
    {
        P1k += normHistogram[i]; // somma cumulativa P1
        p1k += i * normHistogram[i]; //Media ponderata al passo i-esimo  (alla fine ti darà globAVG)
        m1k = p1k / P1k; //valore intensità medio C1
        double P2k = 0, p2k = 0, m2k = 0;
        // Calcoli per la soglia n°2
        for (int j = i + 1; j < L_SIZE; j++)
        { 
            P2k += normHistogram[j]; // somma cumulativa P1
            p2k += j * normHistogram[j]; //Media ponderata al passo i-esimo  (alla fine ti darà globAVG)
            m2k = p2k / P2k; //valore intensità medio C2
            double P3k = 1 - P2k - P1k; // somma cumulativa P3
            double p3k = mediaGlobale - p2k - p1k; //
            double m3k = p3k / P3k; // valore intensità medio C3
            double varianza = P1k * pow((m1k - mediaGlobale), 2) + P2k * pow((m2k - mediaGlobale), 2) + P3k * pow((m3k - mediaGlobale), 2);
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
    dest = Mat::zeros(src.rows, src.cols, CV_8UC1);
    
    for (int i = 0; i < src.rows; i++)
        for (int j = 0; j < src.cols; j++){
            uchar val = src.at<uchar>(i, j);
            if (val > sogliaOttimale2)
                dest.at<uchar>(i, j) = 255;
            else if (val > sogliaOttimale1 && val <= sogliaOttimale2)
                dest.at<uchar>(i, j) = 128;
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

    /*
    0° Passo:
    - Effettuare uno smoothing dell'immagine usando un filtro Laplaciano oppure un filtro Gausiano
    */
    Mat smoothed;
    GaussianBlur(original, smoothed, Size(5, 5), 0, 0);
    
    Mat dest;
    myOtsu(smoothed, dest);
    
    imshow("Original", original);
    imshow("Otsu", dest);
    
    waitKey(0);
}