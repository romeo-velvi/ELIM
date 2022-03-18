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

void compareImage(Mat &src, Mat &maks);

void showimg(String s, Mat &m, int flag = 0, int x = 0, int y = 0)
{
    namedWindow(s, WINDOW_NORMAL);
    imshow(s, m);
    waitKey(0);
    if (flag)
    {
        cout << "--------------- " << s << " --------------- " << endl;
        for (int i = 0; i < x; i++)
        {
            for (int j = 0; j < y; j++)
            {
                cout << "[" << (int)m.at<int>(i, j) << "]"
                     << " ";
            }
            cout << endl;
        }
        // cout<<"---------------"<<endl;
    }
}

// 90∇^2(f(x,y))=f(x+1,y)+f(x-1,y)+f(x,y+1)+f(x,y-1)-4f(x,y)
void createIsotropicLaplacian90(Mat &kernel)
{
    kernel = Mat::zeros(3, 3, CV_32S);
    kernel.at<int>(0, 0) = 0;
    kernel.at<int>(0, 1) = 1;
    kernel.at<int>(0, 2) = 0;
    kernel.at<int>(1, 0) = 1;
    kernel.at<int>(1, 1) = -4;
    kernel.at<int>(1, 2) = 1;
    kernel.at<int>(2, 0) = 0;
    kernel.at<int>(2, 1) = 1;
    kernel.at<int>(2, 2) = 0;
}

// 45∇^2(f(x,y))= 90∇^2(f(x,y))+f(x-1,y-1)+f(x+1,y+1)+f(x-1,y+1)+f(x+1,y-1)-4f(x,y)
void createIsotropicLaplacian45(Mat &kernel)
{
    kernel = Mat::zeros(3, 3, CV_32S);
    kernel.at<int>(0, 0) = 1;
    kernel.at<int>(0, 1) = 1;
    kernel.at<int>(0, 2) = 1;
    kernel.at<int>(1, 0) = 1;
    kernel.at<int>(1, 1) = -8;
    kernel.at<int>(1, 2) = 1;
    kernel.at<int>(2, 0) = 1;
    kernel.at<int>(2, 1) = 1;
    kernel.at<int>(2, 2) = 1;
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
    //src.convertTo(src,CV_32S);
    // src = src/(float)255;

    // Creazione filtro -> MAT DI INTERI
    Mat isolap45, isolap90;
    createIsotropicLaplacian45(isolap45);
    createIsotropicLaplacian90(isolap90);

    // CONVOLUZIONE COMMENTARE CODICE SOTTO
    // rotate(isolap45,isolap45,ROTATE_180);
    // rotate(isolap90,isolap90,ROTATE_180);
    
    //  showimg("90", isolap90, 1, 3, 3);
    //  showimg("45", isolap45, 1, 3, 3);

    Mat dst45,dst45n, dst90,dst90n, dsttot,dsttotn;
    filter2D(src,dst90,CV_8U,isolap90);
    filter2D(src,dst45,CV_8U,isolap45);
    dsttot = dst45|dst90;

    //nel caso il filtro laplaciano dia valori negativi
    normalize(dst90, dst90n, 0, 255, NORM_MINMAX, CV_8U);
    normalize(dst45, dst45n, 0, 255, NORM_MINMAX, CV_8U);
    normalize(dsttot, dsttotn, 0, 255, NORM_MINMAX, CV_8U);

    //mostro le immagini
    showimg("filter90", dst45);
    showimg("filter45", dst90);
    showimg("or 45e90", dsttot);

    //mostro le immagini normalizzate
    showimg("filter90 normalizzate", dst45n);
    showimg("filter45 normalizzate", dst90n);
    showimg("or 45e90 normalizzate", dsttotn);

    // creazione laplaciano con funzione libreria
    Mat lap;
    Laplacian(src,lap,CV_8U,3);
    normalize(lap, lap, 0, 255, NORM_MINMAX, CV_8U);
    showimg("laplacian", lap);

    // paragone immagini
    compareImage(dst90,lap);
    compareImage(dst45,lap);
    compareImage(dsttot,lap);

    // differenza immagine "or" con laplaciano
    dsttot = dsttot-lap;
    showimg("not", dsttot);

    // ESEUGURE SHARPENING IMMAGINE
    Mat sharpimg1 = src+(-1*dst90);
    Mat sharpimg2 = src+(-1*dst45);
    showimg("sharped 90",sharpimg1);
    showimg("sharped 45",sharpimg2);

    // LAPLACIAN BASE PER VEDERE EVIDENZIAZIONE LINEEE IN ABS or 0
    Mat m, ma, m0;
    Laplacian(src,m,CV_32F);
    ma = abs(m);
    showimg("Laplacian abs",ma);
    m0 =m;
    for (int i = 0; i < m.rows; i++)
        for (int j = 0; j<m.cols;j++)
            m0.at<float>(i,j) < 0 ? 0 :  m0.at<float>(i,j);
    showimg("Laplacian 0",m0);
    
    return 0;
}

void compareImage(Mat &m1, Mat &m2)
{

    if(m1.rows != m2.rows || m1.cols!=m2.cols){
            cout << "size diversi" << endl;
            return;
    }

    // show difference
    int cnt=0;
    for (int i = 0; i < m1.rows; i++)
    {
        for (int j = 0; j < m1.cols; j++)
        {
            if ((int)m1.at<unsigned char>(i, j) != (int)m2.at<unsigned char>(i, j))
            {
                //cout << "diversi a: " << i << " " << j << endl;
                cnt++;
                // return;
            }
        }
    }
    cout << "sono uguali :"<<cnt << endl;
}
