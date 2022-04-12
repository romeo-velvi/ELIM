#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;

double myDistance(Scalar p1, Scalar p2)
{
    return pow(p1[0] - p2[0], 2) +
           pow(p1[1] - p2[1], 2) +
           pow(p1[2] - p2[2], 2);
}

void myKmeans(Mat &src, Mat &dst, int nClusters)
{
    // passo 0 -> sfocatura
    GaussianBlur(src,src,Size(5,5),0,0);

    vector<Scalar> centers_colors;
    vector<vector<Point>> clusters;

    // Passo 1 -> Calcolo i centri
    for (int k = 0; k < nClusters; k++)
    {
        Point center;
        center.x = (src.cols / nClusters) * k; // K-MEANS++
        center.y = (src.rows / nClusters) * k; // Si potrebbero prendere i centri in maniera diversa
        Scalar center_colors(
            src.at<Vec3b>(center)[0],
            src.at<Vec3b>(center)[1],
            src.at<Vec3b>(center)[2]
        );
        cout<<"Centro "<<k+1<<" in posizione x:"<<center.x << "\ty:"<< center.y << "\t con colore: "<< center_colors<<endl;
        centers_colors.push_back(center_colors);
        vector<Point> t;
        clusters.push_back(t);
    }

    // Passo 2 ->  Assegno i pixel ai cluster, ricalcolo i centri usando le medie, fino a che differenza < 0.1
    double oldCenterSum = 0;
    double difference = INFINITY;
    while (difference > 0.1)
    {
        for (int k = 0; k < nClusters; k++)
        {
            clusters[k].clear();
            cout<<"Centro "<<k+1<<"\t con  NUOVO colore: "<< centers_colors[k] <<endl;
        }
        
        // passo 3 -> controllo quali pixel facciano parte di un determinato cluster calcolando le distanze di intensità
        for (int x = 0; x < src.rows; x++)
        {
            for (int y = 0; y < src.cols; y++)
            {
                // Calcolo le differenze di intensità da pixel ad un centro e posiziono il pixel nel relativo cluster
                double minDistance = INFINITY;
                int clusterIndex = 0;
                Point pixel(y, x);
                // si calcola differenza di intensità del pixel per ogni cluster.
                for (int k = 0; k < nClusters; k++)
                {
                    Scalar center = centers_colors[k];
                    Scalar point(
                        src.at<Vec3b>(x, y)[0], 
                        src.at<Vec3b>(x, y)[1],
                        src.at<Vec3b>(x, y)[2]
                        );
                    double distance = myDistance(center, point);
                    // se la distanza è più piccola di quella trovata fin ora, 
                    if (distance < minDistance)
                    {
                        minDistance = distance;
                        clusterIndex = k; // Salvo il pixel nel cluster di indice k (a cui ha differenza minore)
                    }
                }
                // inserisco il pixel nel cluster
                clusters[clusterIndex].push_back(pixel);
            }
        }
        
        
        // passo 4 -> Calcolo la media di ogni cluster
        double newCenterSum = 0;
        for (int k = 0; k < nClusters; k++)
        {
            vector<Point> clusterPoints = clusters[k];
            double blue = 0, green = 0, red = 0;
            cout << "Cluster points size " << clusterPoints.size() << endl;
            for (int i = 0; i < (int)clusterPoints.size(); i++)
            {
                Point pixel = clusterPoints[i];
                blue += src.at<Vec3b>(pixel)[0];
                green += src.at<Vec3b>(pixel)[1];
                red += src.at<Vec3b>(pixel)[2];
            }
            // Calcolo media dei colori del nuovo centro
            blue /= clusterPoints.size();
            green /= clusterPoints.size();
            red /= clusterPoints.size();
            // Calcolo la distanza del nuovo centro dal vecchio e aggiungo a newCenterSum
            Scalar center = centers_colors[k];
            Scalar newCenter(blue, green, red);
            newCenterSum += myDistance(newCenter, center);
            // Aggiorno il nuovo centro
            centers_colors[k] = newCenter;
        }
        newCenterSum /= nClusters;
        difference = abs(oldCenterSum - newCenterSum);
        cout << "Differenza " << difference << endl;
        oldCenterSum = newCenterSum;
    }
    
    // passo 5 -> Disegno l'immagine di output
    for (int k = 0; k < nClusters; k++)
    {
        vector<Point> clusterPoints = clusters[k];
        Scalar center = centers_colors[k];
        for (int i = 0; i < (int)clusterPoints.size(); i++)
        {
            // Assegno il valore del centro agli altri pixel del cluster
            Point pixel = clusterPoints[i];
            dst.at<Vec3b>(pixel)[0] = center[0];
            dst.at<Vec3b>(pixel)[1] = center[1];
            dst.at<Vec3b>(pixel)[2] = center[2];
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cout << "Usage: " << argv[0] << " image_name and k - value" << endl;
        exit(-1);
    }
    Mat input = imread(argv[1]);
    if (input.empty())
    {
        cout << "Can't find image" << endl;
        exit(-1);
    }
    int nClusters = atoi(argv[2]);

    Mat dst(input.rows, input.cols, input.type());
    myKmeans(input, dst, nClusters);
    imshow("Input image", input);
    imshow("K-Means", dst);
    waitKey(0);
    return 0;
}