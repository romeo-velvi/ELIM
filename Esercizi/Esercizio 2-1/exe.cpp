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

void create_mask(Mat &mask, int dim);
float takekernelvalue(Mat &rangeimg, Mat &kernel);
void correlazione(Mat &src, Mat &dst, int tipo_output, Mat &kernel);
void compareImage(Mat &src, Mat &maks);
void showimg(Mat m){
    namedWindow("showimg", WINDOW_NORMAL);
    imshow("showimg", m);
    waitKey(0);
}


int main(int argc, char **argv)
{

    // LE IMMAGINI SONO IMPORTATE ED ELABORATE TUTTE IN FLOAT (32)
    
    // Load an image and check if exists
    Mat src = imread(argv[1], IMREAD_GRAYSCALE);
    if (src.empty())
    {
        printf(" Error opening image\n");
        return -1;
    }
    src.convertTo(src,CV_32F);
    src = src/(float)255;

    // Lettura dimensione filtro
    int dim, i, j;
    cout << "Inserire dimensione filtro: ";
    cin >> dim;

    // Creazione maschera
    Mat kernel;
    create_mask(kernel, dim);

    // CORRELAZIONE
    Mat dst, mask;
    create_mask(mask, dim);
    correlazione(src, dst, 0, kernel);
    showimg(dst);

    // COMPARO RISULTATI
    compareImage(src, mask);

    return 0;
}

void create_mask(Mat &mask, int dim)
{
    mask = Mat(dim, dim, CV_32F);
    for (int i = 0; i < dim; i++)
    {
        for (int j = 0; j < dim; j++)
        {
            mask.at<float>(i, j) = (float)1 / (dim * dim);
        }
    }
}

float takekernelvalue(Mat rangeimg, Mat &kernel)
{
    int n = kernel.rows;
    float out = 0, k;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            out = (float) out + (rangeimg.at<float>(i, j)* kernel.at<float>(i, j));
        }
    }
    //cout<<"float: "<<out<<" uc:"<<(int)out<<endl;
    return (float) out;
}

void correlazione(Mat &src1, Mat &dst, int tipo_output, Mat &kernel)
{
    int n = kernel.rows;
    
    int br = (n - 1) / 2;
    Mat src = src1;
    dst = Mat(src.rows,src.cols,CV_32F);
    copyMakeBorder(src, src, br, br, br, br, BORDER_CONSTANT, 0);

    for (int i = 0; i < dst.rows; i++)
    {
        for (int j = 0; j < dst.cols; j++)
        {
            dst.at<float>(i, j) = takekernelvalue(src(Rect(j, i, n, n)), kernel);
        }
    }
    
}

void compareImage(Mat &src, Mat &mask)
{
    Mat dst1, dst2, tot;
    correlazione(src, dst2, 0, mask);
    filter2D(src, dst1, src.type(), mask);
    showimg(dst1);

    // show difference
    for (int i = 0; i < dst1.rows; i++)
    {
        for (int j = 0; j < dst1.rows; j++)
        {
            if ((int)dst1.at<float>(i, j) != (int)dst2.at<float>(i, j))
            {
                cout << "diversi a: " << i << " " << j << endl;
                return;
            }
        }
    }
    cout << "sono uguali" << endl;
}
