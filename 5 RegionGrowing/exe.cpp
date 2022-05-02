#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <stack>

using namespace cv;
using namespace std;

Mat grow(Mat &src, Mat &dest, Point seed, int th)
{
    // Mat temp su cui ci sarà la regione trovata come seed pixel i,j
    Mat region = Mat::zeros(src.rows, src.cols, CV_8U);

    // si parte da uno stack con il solo punto "seed"
    stack<Point> stack;
    stack.push(seed);

    // Finchè lo stack non è vuoto
    while (!stack.empty())
    {
        Point center = stack.top(); // Considero il primo elemento come centro del 3x3
        stack.pop();                // Lo rimuovo dallo stack
        region.at<uchar>(center) = 1;

        for (int i = -1; i <= 1; i++)
            for (int j = -1; j <= 1; j++)
            {
                Point punto = center + Point(j, i);

                // Se il punto stimato esce fuori dall'immagine non fare nulla
                if (punto.x < 0 || punto.x > src.cols - 1 || punto.y < 0 || punto.y > src.rows - 1 || (i == 0 && j == 0))
                    continue;
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

                    // GRAY SCALE // DELTA = | I-I'|
                    uchar distanza_colore = (uchar)abs(src.at<uchar>(center) - src.at<uchar>(punto));

                    // Verifico varie condizioni
                    if (
                        dest.at<uchar>(punto) == 0 &&   // Non è stato ancora etichettato
                        region.at<uchar>(punto) == 0 && // Non è stato ancora inserito nella regione
                        distanza_colore < th                      // Il delta è minore della soglia
                    )
                    {
                        stack.push(punto); // Così che i pixel adiacenti a questo verranno considerati
                    }
                }
            }
    }
    return region;
}

void myRegionGrowing(Mat &src, Mat &dest, int th)
{
    // passo 0 -> Blur e Se l'immagine è troppo grande (> 500x500) faccio il resize
    // if (src.rows > 500 && src.cols > 500)
    //     resize(src, src, Size(0, 0), 0.5, 0.5);
    GaussianBlur(src, src, Size(5, 5), 0, 0);

    // passo 1 -> assegnazione valori
    double min_region = 0.01;                                      // dimensione minima della regione (1% dell'immagine) not exists region < 1%img_size
    int jumpvalue = 10;                                            // indicatore di step-label tra regione
    int max_regions = int(255 / jumpvalue) - 1;                    // valore massimo della regione  (tutta l'immagine)
    int min_region_area = (int)(min_region * src.rows * src.cols); // numero di pixel minimi per una regione
    uchar label = 1;                                               // dare "id" etichetta a regione
    int cnt_region = 0;                                            // contatore regioni trovate

    // passo 3 -> initializzare matrici
    dest = Mat::zeros(src.rows, src.cols, CV_8U); // dest vi saranno pixel: 0 = non calcolata; 1-254 = regioni presenti; 255 = non vi è regione;

    // passo 3 -> scorriamo la matrice in cerca di pixel NON seed da partire la ricerca (new seed)
    for (int i = 0; i < src.rows; i++)
        for (int j = 0; j < src.cols; j++)
            // passo 4 -> Controllo il pixel dell'immagine a cui ancora non è stato assegnato un valore = non sono in regioni
            if (dest.at<uchar>(i, j) == 0)
            {
                // passo 5 -> calcolo regione in base al nuovo pixel seed i,j
                Mat region_found = grow(src, dest, Point(j, i), th); // si calcola la regione di appartenenza (come nuovo seed)
                int region_area_size = (int)sum(region_found).val[0];        // vede quanti pixel ci sono nella regione
                if (region_area_size > min_region_area)                // se la regione è sufficientemente grande
                {

                    // passo 6 -> se tutto ok, inserisco regione con le altre trovate
                    label += jumpvalue;     // incremento label
                    cnt_region++;           // incremento counter
                    dest += region_found * label; // LA registriamo: Aggiungo all'output la maschera * il label (inserisco l'etichetta)
                    // imshow("region found", region*255); waitKey(0);
                    if (cnt_region > max_regions) // se si è raggiuntoil numero massimo di regione che vogliamo provare
                    {
                        cout << "massime regioni trovate" << endl;
                        return;
                    }
                }
                else // se è troppo piccola la marchio come "trascurabile"
                {
                    dest += region_found * 255;
                }
            }

    cout << "area found :" << cnt_region;
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