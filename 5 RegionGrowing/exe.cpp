#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <stack>

using namespace cv;
using namespace std;

#define NEIGHBORHOOD_SIZE 8

// Valori costanti
const double min_region = 0.01; // dimensione minima della regione (1% dell'immagine) not exists region < 1%img_size
const int jumpvalue = 15;
const int max_regions = int(255/jumpvalue)-1;  // valore massimo della regionen (tutta l'immagine)

// Intorno 8 connesso
const Point Shift2D[NEIGHBORHOOD_SIZE]{
    Point(-1, -1),
    Point(-1, 0),
    Point(-1, 1),
    Point(0, -1),
    Point(0, 1),
    Point(1, -1),
    Point(1, 0),
    Point(1, 1),
};

// funzione
void grow(Mat &src, Mat &dest, Mat &region, Point seed, int th)
{
    // si parte da uno stack con il solo punto "seed"
    stack<Point> stack;
    stack.push(seed);

    // Finchè lo stack non è vuoto
    while (!stack.empty())
    {
        Point center = stack.top(); // Considero il primo elemento come centro dell'8x8
        stack.pop();                // Lo rimuovo dallo stack

        for (int i = 0; i < NEIGHBORHOOD_SIZE; i++) // per ogni pixel nell'8-intorno
        {
            Point punto = center + Shift2D[i]; // prendo posizione pixel i-esimo dell'8-intorno

            // Se il punto stimato esce fuori dall'immagine non fare nulla
            if (punto.x < 0 || punto.x > src.cols - 1 || punto.y < 0 || punto.y > src.rows - 1){
                continue;
            }
            else
            {
                // RGB // -> DELTA = (R-R')^2 + (G-G')^2 + (B-B')^2 (distanza euclidea tra il centro ed il punto stimato)
                /*
                int delta = int(pow(src.at<Vec3b>(center)[0] -
                                        src.at<Vec3b>(punto)[0],
                                    2) +
                                pow(src.at<Vec3b>(center)[1] -
                                        src.at<Vec3b>(punto)[1],
                                    2) +
                                pow(src.at<Vec3b>(center)[2] -
                                        src.at<Vec3b>(punto)[2],
                                    2));
                */

                // GRAY SCALE // DELTA = |I-I'|
                uchar delta = (uchar) abs(src.at<uchar>(center) - src.at<uchar>(punto));

                // Verifico varie condizioni
                if (
                    dest.at<uchar>(punto) == 0 &&   // Non è stato ancora etichettato
                    region.at<uchar>(punto) == 0 && // Non è stato ancora inserito nella regione
                    delta < th                      // Il delta è minore della soglia
                )
                {
                    region.at<uchar>(punto) = 1; // Inserisco il pixel nella regione
                    stack.push(punto);           // Così che i pixel adiacenti a questo verranno considerati
                }
            }
        }
    }
}

void myRegionGrowing(Mat &src, Mat &dest, int th)
{
    // passo 0 -> Se l'immagine è troppo grande (> 500x500) faccio il resize
    if (src.rows > 500 && src.cols > 500)
        resize(src, src, Size(0, 0), 0.5, 0.5);
    GaussianBlur(src,src,Size(5,5),0,0);

    // passo 1 -> Calcolo del numero di pixel minimi per una regione
    int min_region_area = (int)(min_region * src.rows * src.cols);

    // passo 2 -> dare "id" etichetta a regione
    uchar label = 1;
    int cnt = 0 ;

    // passo 3 -> initializzare matrici
    dest = Mat::zeros(src.rows, src.cols, CV_8U);       // dest vi saranno pixel: 0 = non calcolata; 1-254 = regioni presenti; 255 = non vi è regione;
    Mat region = Mat::zeros(src.rows, src.cols, CV_8U); // maschera d'appoggio che serve a trovare le regioni

    // passo 3 -> scorriamo la matrice in cerca di pixel NON seed da partire la ricerca (new seed)
    for (int i = 0; i < src.rows; i++){
        for (int j = 0; j < src.cols; j++){
            // passo 4 -> Controllo il pixel dell'immagine a cui ancora non è stato assegnato un valore = non sono in regioni
            if (dest.at<uchar>(i, j) == 0)
            {
                // passo 5 -> calcolo regione in base al nuovo pixel seed
                grow(src, dest, region, Point(j, i), th);       // si calcola la regione di appartenenza (come nuovo seed)
                int region_area_size = (int)sum(region).val[0]; // vede quanti pixel ci sono nella regione
                if (region_area_size > min_region_area)         // se la regione è sufficientemente grande
                {
                    // passo 6 -> inserisco nel dest e le etichietto altrimenti no
                    label+=jumpvalue; // incremento label
                    cnt ++; // incremento counter
                    dest += region * label; // LA registriamo: Aggiungo all'output la maschera * il label (inserisco l'etichetta)
                    // imshow("region found", region*255); waitKey(0);
                    if (cnt > max_regions) // se si è raggiuntoil numero massimo di regione che vogliamo provare
                    {
                        cout << "massime regioni trovate" << endl;
                        return;
                    }
                }
                else // se è troppo piccola la marchio come "trascurabile"
                {   
                    dest += region*255;
                }
                region -= region; // Azzero la maschera della regione
            
            }
        }
    }

    cout<<"area found :"<<cnt;
}


int main(int argc, char *argv[])
{
    Mat original = imread(argv[1], IMREAD_GRAYSCALE); // OR RGB -> remove 2nd arg.
    if (original.empty())
    {
        cout << "Wrong image selected!" << endl;
        exit(-1);
    }
    int threshold = 4;
    Mat dest;

    myRegionGrowing(original, dest, threshold);

    imshow("Original", original);
    imshow("RegionGrowing", dest);

    waitKey(0);
    return 0;
}