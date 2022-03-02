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

void compareImage(Mat &m1, Mat &m2, String s = " ")
{

    if (m1.rows != m2.rows || m1.cols != m2.cols)
    {
        cout << "size diversi" << endl;
        return;
    }

    // show difference
    int cnt=0;
    for (int i = 0; i < m1.rows; i++)
        for (int j = 0; j < m1.cols; j++)
            if ((int)m1.at<float>(i, j) != (int)m2.at<float>(i, j))
                cnt++;
    cout << s << " divergno di : " << cnt << " pixel" << endl;
}

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
                cout << "[" << (int)m.at<float>(i, j) << "]"
                     << " ";
            }
            cout << endl;
        }
        // cout<<"---------------"<<endl;
    }
}

// GX
void createGX(Mat &kernel)
{
    kernel = Mat::zeros(3, 3, CV_32F);
    kernel.at<float>(0, 0) = (float)-1;
    kernel.at<float>(0, 1) = (float)0;
    kernel.at<float>(0, 2) = (float)1;
    kernel.at<float>(1, 0) = (float)-2;
    kernel.at<float>(1, 1) = (float)0;
    kernel.at<float>(1, 2) = (float)2;
    kernel.at<float>(2, 0) = (float)-1;
    kernel.at<float>(2, 1) = (float)0;
    kernel.at<float>(2, 2) = (float)1;
}

// GY
void createGY(Mat &kernel)
{
    kernel = Mat::zeros(3, 3, CV_32F);
    kernel.at<float>(0, 0) = (float)-1;
    kernel.at<float>(0, 1) = (float)-2;
    kernel.at<float>(0, 2) = (float)-1;
    kernel.at<float>(1, 0) = (float)0;
    kernel.at<float>(1, 1) = (float)0;
    kernel.at<float>(1, 2) = (float)0;
    kernel.at<float>(2, 0) = (float)1;
    kernel.at<float>(2, 1) = (float)2;
    kernel.at<float>(2, 2) = (float)1;
}

// make sobel
void makeSobel(Mat &mat, int type, Mat &gx, Mat &gy, Mat &magsqrt, Mat &magabs)
{
    // Creazione filtro -> MAT DI INTERI
    Mat sobel_x, sobel_y;
    createGX(sobel_x);
    createGY(sobel_y);
    // applicazione filtri
    Mat imgx, ix, imgy, iy;
    filter2D(mat, imgx, type, sobel_x);
    filter2D(mat, imgy, type, sobel_y);
    // magnitudo
    Mat magnitudo1, magnitudo2;
    // sqrt(gx^2+gy^2)
    pow(imgx, 2, ix);
    pow(imgy, 2, iy);
    Mat sum = ix + iy;
    sqrt(sum, magnitudo1);
    // abs(gc)+abs(gy)
    magnitudo2 = abs(ix) + abs(iy);
    // return result
    gx = imgx;
    gy = imgy;
    magsqrt = magnitudo1;
    magabs = magnitudo2;
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
    src.convertTo(src, CV_32F);
    src = src / (float)255;

    // apply filter
    Mat mat, gx, gy, magsqrt, magabs;
    makeSobel(src, src.type(), gx, gy, magsqrt, magabs);
    // show result
    showimg("gx", gx);
    showimg("gy", gy);
    showimg("xy mag sqrt", magsqrt);
    showimg("xy mag abs", magabs);

    // make sobel from lib
    Mat sobx, sx, soby, sy, sobxy, sobmagabs, sobmagroot;
    Sobel(src, sobx, src.type(), 1, 0);
    Sobel(src, soby, src.type(), 0, 1);
    // sobel mag root
    pow(sobx, 2, sx);
    pow(soby, 2, sy);
    Mat sum = sx + sy;
    sqrt(sum, sobmagroot);
    // sobel mag abs
    sobmagabs = abs(sobx) + abs(soby);
    //show result
    showimg("sob gx", sobx);
    showimg("sob gy", soby);
    showimg("sob mag root", sobmagroot);
    showimg("sob mag abs", sobmagabs);

    // paragone immagini
    compareImage(sobx, gx);
    compareImage(soby, gy);
    compareImage(magsqrt, sobmagroot);
    compareImage(magabs, sobmagabs);

    Mat grad,sxy;
    addWeighted(sx, 0.5, sy, 0.5, 0, grad);
    showimg("sob grad", grad);
    Sobel(src, sxy, src.type(), 1, 0);
    showimg("sob xy", sxy);

    return 0;
}
