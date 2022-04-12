#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
using namespace std;
using namespace cv;

#define L_SIZE 256

void myOtsu(Mat& src, Mat& dest)
{
    /*
    1° Passo:
    Punto 1 - Calcolare l'istogramma normalizzato dell'immagine filtrata
    Punto 4 - Calcolare l'intensità globale media m_G
    */
    double histogram[L_SIZE] = { 0 };
    double normHistogram[L_SIZE] = { 0 };
    double MN = src.rows * src.cols;
    double mediaGlobale = 0;

    for (int i = 0; i < src.rows; i++)
        for (int j = 0; j < src.cols; j++)
            histogram[src.at<uchar>(i, j)]++; //Calcola istogramma


    for (int i = 0; i < L_SIZE; i++)
    {
        normHistogram[i] = histogram[i] / MN; //Calcola istogramma normalizzato
        mediaGlobale += i * normHistogram[i]; //Calcola intensita globale media m_G
    }

    /*
    2° Passo:
    Punto 2 - Calcolare le somme cumulative P1(k) per k=0,1,...,L-1
    Punto 3 - Calcolare le medie cumulative 𝑚(k) per k=0,1,...,L-1
    Punto 5 - Calcolare la varianza interclasse 𝜎(2B)(k) per k=0,1,...,L-1
    Punto 6 - Calcolare la soglia k*, ovvero il valore k per cui 𝜎(2B)(k) è massimo
    */
    double P1k=0, p1k=0, m1k, m2k, P2k, p2k;
    double varianzaInterclasse = 0.0, maxVarianza = 0.0;
    int sogliaOttimale = 0;
    for (int i = 0; i < L_SIZE; i++)
    {
        P1k += normHistogram[i]; //somma cumulativa P1
        p1k += i * normHistogram[i]; //Media ponderata al passo i-esimo  (alla fine ti darà globAVG)
        P2k = 1 - P1k; // somma cumulativa P2
        p2k = mediaGlobale - p1k; //Differenza tra media ponderata su tutti i valori di intesita e media ponderata al passo i-esimo
        m1k = p1k / P1k; //valore intensità medio C1
        m2k = p2k / P2k; //valore intensità medio C2
        varianzaInterclasse = P1k * P2k * pow((m1k - m2k), 2);
        if (varianzaInterclasse > maxVarianza) //se la varianza all'i-sema iterazione è maggiore vado a modificare il valore
        {
            maxVarianza = varianzaInterclasse;
            sogliaOttimale = i; //k*
        }
    }

    cout << "La soglia ottimale e': " << sogliaOttimale << endl;
    cout << "La max varianza interclasse e': " << maxVarianza << endl;
    dest = Mat::zeros(src.rows, src.cols, CV_8UC1);

    // for (int i = 0; i < src.rows; i++)
    //     for (int j = 0; j < src.cols; j++)
    //         if (src.at<uchar>(i, j) > sogliaOttimale) //se ha un valore maggiore del k*
    //             dest.at<uchar>(i, j) = 255; //poni a bianco

    threshold(src,dest,sogliaOttimale,255,THRESH_BINARY);

}


int main(int argc, char** argv)
{

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