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
RNG rng(12345);


Mat generator(int dim){
	Mat m(dim,dim,CV_32F);
	srand(time(NULL));
	int i,j;
	for(i=0;i<dim;i++){
    	for(j=0;j<dim;j++){
    		if(j==0){
    			m.at<float>(i,j)=-i;
    		} else if(j==dim-1){
    				m.at<float>(i,j)=i;
    		}else{
    		m.at<float>(i,j)=0;
    		}
    	}
    } 
    return m;
}


int main( int argc, char** argv ){

	int dim;
	int i,j;
	
	if( argc < 2){
        cout<<"usage: "<<argv[0]<<" image_name"<<endl;
        exit(0);
    }
    String imageName = argv[1];

	//Lettura immagine    
    Mat image = imread( samples::findFile( imageName ), IMREAD_GRAYSCALE );
    if( image.empty() ){ cout <<  "Could not open or find the image" << std::endl;
        return -1;
    }

    //Lettura dimensione filtro
    cout<<"Inserire dimensione filtro: ";
    cin>>dim;

    //Allocazione filtro
    Mat average_filter = generator(dim);
    cout<< "stampa matrice filtro normale"<<endl;
    for(i=0;i<dim;i++){
    	for(j=0;j<dim;j++){
    		cout<<" "<<average_filter.at<float>(i,j);
    	}
    	cout<<endl;
    }cout<<endl;
    
    // correlazione
    Mat output1;
    filter2D(image,output1,image.type(),average_filter);
	    
    
    //Per la convoluzione ruotare il filtro di 180°
    Mat r_filter; 
    rotate(average_filter,r_filter,ROTATE_180);
    cout<< "stampa matrice filtro ruotato"<<endl;
    for(int i=0;i<dim;i++){
    	for(int j=0;j<dim;j++){
    		cout<<" "<<r_filter.at<float>(i,j);
    	}
    	cout<<endl;
    }
    
    // convoluzione
    Mat output2;
    filter2D(image,output2,image.type(),r_filter);  
    
    
    //mostro differenze
    Mat combine(image.rows*4,image.cols*4, CV_8UC1);
    Mat left_roi(combine, Rect(0, 0, output1.rows, output1.cols));
	output1.copyTo(left_roi);
	Mat right_roi(combine, Rect(output2.rows, 0, output2.rows, output2.cols));
	output2.copyTo(right_roi);
	imshow("differenze",combine);
    waitKey(0);    


    imshow("CORRELAZIONE",output1); 
    imshow("CONVOLUZIONE",output2);
    waitKey(0);    
	
	return 0;
}
