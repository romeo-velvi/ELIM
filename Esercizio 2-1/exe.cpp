#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>

using namespace std;
using namespace cv;

void create_mask_CV_32F(Mat &mask, int dim);
float takekernelvalue(Mat &rangeimg, Mat &kernel);
void correlazione(Mat &src, Mat &dst, int tipo_output, Mat &kernel);
void compareBorder(Mat &src, Mat &maks);

int main(int argc, char **argv)
{

    // LE IMMAGINI SONO IMPORTATE ED ELABORATE IN SCALA DI GRIGIO (CV_8UC)
    // IL KERNEL PUÒ ANCHE ASSERE FLOAT

    // Load an image and check if exists
    Mat src = imread(argv[1], IMREAD_GRAYSCALE);
    if (src.empty())
    {
        printf(" Error opening image\n");
        return -1;
    }

    // Lettura dimensione filtro
    int dim, i, j;
    cout << "Inserire dimensione filtro: ";
    cin >> dim;

    // Creazione maschera
    Mat kernel;
    create_mask_CV_32F(kernel, dim);

    // CORRELAZIONE
    Mat dst, mask;
    create_mask_CV_32F(mask, dim);
    correlazione(src, dst, 0, kernel);
    namedWindow("showimg", WINDOW_NORMAL);
    imshow("showimg", dst);
    waitKey(0);

    // COMPARO RISULTATI
    compareBorder(src, mask);

    return 0;
}

void create_mask_CV_32F(Mat &mask, int dim)
{
    mask = Mat(dim, dim, CV_32FC1);
    for (int i = 0; i < dim; i++)
    {
        for (int j = 0; j < dim; j++)
        {
            // cout<<"Inserire valore posizione: "<<i<<" "<<j<<endl;
            // cin>>val;
            mask.at<float>(i, j) = (float)1 / (dim * dim);
        }
    }
}

float takekernelvalue(Mat rangeimg, Mat &kernel)
{
    int n = kernel.rows;
    float out = 0;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            out = (float) out + rangeimg.at<unsigned char>(i, j) * kernel.at<float>(i, j);
        }
    }
    return out;
}

void correlazione(Mat &src, Mat &dst, int tipo_output, Mat &kernel)
{
    int n = kernel.rows;
    // INSERIMENTO BORDO
    // calcolo grandezza del bordo
    int br = (n - 1) / 2;
    cout << br << endl;
    // inserisco bordo di dimensioni calcolate prima
    // Mat src_border;
    Mat dst1;
    copyMakeBorder(src, dst, br, br, br, br, BORDER_CONSTANT, 0);
    copyMakeBorder(src, dst1, br, br, br, br, BORDER_CONSTANT, 0);
    cout << "src r-c: " << src.rows << " " << src.cols << endl;

    for (int i = br; i < src.rows; i++)
    {
        for (int j = br; j < src.cols; j++)
        {
            dst.at<unsigned char>(i, j) = (unsigned char)takekernelvalue(dst1(Rect(j - br, i - br, n, n)), kernel);
        }
    }
    // toglie il padding
    dst = dst(Rect(br, br, src.cols, src.rows));
}

void compareBorder(Mat &src, Mat &mask)
{
    Mat dst1, dst2, tot;
    filter2D(src, dst1, CV_8UC1, mask);
        namedWindow("showimg", WINDOW_NORMAL);
    imshow("showimg", dst1);
    waitKey(0);
    correlazione(src, dst2, 0, mask);

    // show difference
    for (int i = 0; i < dst1.rows; i++)
    {
        for (int j = 0; j < dst1.rows; j++)
        {
            if (dst1.at<unsigned char>(i, j) != dst2.at<unsigned char>(i, j))
            {
                cout << "diversi a: " << i << " " << j << endl;
                return;
            }
        }
    }
    cout << "sono uguali" <<endl;
}