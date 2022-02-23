#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

using namespace cv;
using namespace std;
RNG rng(12345);

Mat src, dst;



// funione per calcolare il valore medio in un intorno 3x3.
// Se tipo =0 allora si calcola la MEDIANA, altrimenti la MEDIA
unsigned char cvm (int i, int j, int tipo){
	unsigned char res [9];
	/*/c
	res[0] = dst.at<unsigned char>(i,j);
	//n
	res[1] = dst.at<unsigned char>(i-1,j);
	//no
	res[2] = dst.at<unsigned char>(i-1,j-1);
	//ne
	res[3] = dst.at<unsigned char>(i-1,j+1);
	//s
	res[4] = dst.at<unsigned char>(i+1,j);
	//so
	res[5] = dst.at<unsigned char>(i+1,j-1);
	//se
	res[6] = dst.at<unsigned char>(i+1,j+1);
	//o
	res[7] = dst.at<unsigned char>(i,j-1);
	//e
	res[8] = dst.at<unsigned char>(i,j+1);
	//l'esercizio equivalente è quello riportato di sotto (per calcolarsi un intorno generico):
	 */

	int z=0;
	for (int s=-1;s<=1;s++){
		for (int t=-1;t<=1;t++){
			res[z++]=dst.at<unsigned char>(i+s,j+t);
		}
	}

	unsigned char x;
	if(tipo==0){ //mediana
		sort(res,res+(int)(sizeof(res)/sizeof(res[0])));
		x= res[5];
	}else{ // media
		int sum=0;
		for(int k=0 ;k<9;k++){
			//cout<<" "<< (int) res[k];
			sum=sum+res[k];
		}
		x= sum/9;
	}
	return x;
}



int main(int argc, char** argv ) {

	// Declare the variables
	int top, bottom, left, right;
	int borderType1 = BORDER_REPLICATE;

	// Load an image
    src = imread( argv[1], IMREAD_GRAYSCALE); 
   
   // Check if image is loaded fine
    if(src.empty()) {
        printf(" Error opening image\n");
        return -1;
    }
    
    // grandezza del bordo (in pixel)
    int br=1; 
    
    // Initialize arguments for the filter
    top = br; 
    bottom = br;
    left = br; 
    right = br;
    
    //metto le destinazioni di ogni immagine
    copyMakeBorder( src, dst, top, bottom, left, right, borderType1, 0 );
    
    
    //visualizzazioni delle immagini 
    namedWindow( "Padding EXE", WINDOW_NORMAL );
    resizeWindow("Padding EXE",900,900);
    imshow( "Padding EXE", dst ); 
    waitKey( 0 );
   
    
	// Esercizio
	
	Mat m (dst.rows, dst.cols, CV_8UC1);
	for (int i=0;i<dst.rows;i++){
		if(i==0 || i==dst.rows){ // è il bordo
			continue;
		}
		for (int j=0;j<dst.cols;j++){
			if(j==0 || j==dst.cols){ // è il bordo
				continue;
			}
			m.at<unsigned char>(i,j)= cvm(i,j,1);
		}
	}
	

	namedWindow( "EXE", WINDOW_NORMAL );
	resizeWindow("EXE",900,900);
	imshow( "EXE", m ); 
    waitKey( 0 );

    
	return 0;
	// terminaizione del programma
}

