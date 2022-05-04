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

Scalar color_at(Mat src, Point p)
{
    return Scalar(
        src.at<Vec3b>(p)[0],
        src.at<Vec3b>(p)[1],
        src.at<Vec3b>(p)[2]);
}

struct Pivot
{
    Scalar color;
    vector<Point> cluster;
};

void myKmeans(Mat &src, Mat &dst, int nClusters)
{
    // passo 0 -> sfocatura
    GaussianBlur(src, src, Size(5, 5), 0, 0);

    vector<Pivot> pivot;

    // Passo 1 -> Calcolo i centri
    for (int k = 0; k < nClusters; k++)
    {
        // in questo caso i centri sono proporzionalemnte distribuiti sulla diagonale dell'immagine ->"[\]"
        // Si potrebbero prendere i centri in maniera diversa
        Pivot e;
        int x = (src.rows / nClusters) * k;
        int y = (src.cols / nClusters) * k;
        e.color = color_at(src, Point(y,x));
        cout << "Centro " << k + 1 << " in posizione x:" << x << "\ty:" << y << "\t con colore: " << e.color << endl;
        e.cluster = vector<Point>();
        pivot.push_back(e);
    }

    // Passo 2 ->  Assegno i pixel ai cluster, ricalcolo i centri usando le medie, fino a che differenza <= 0.1
    double old_clusters_difference = 0, new_clusters_difference = 0;
    double difference = INFINITY;
    while (difference > 0.1)
    {
        for (int k = 0; k < nClusters; k++)
        {
            pivot[k].cluster.clear();
            cout << "Centro " << k + 1 << "\t con  NUOVO colore: " << pivot[k].color << endl;
        }

        // passo 3 -> controllo quali pixel facciano parte di un determinato cluster calcolando le distanze di intensità
        for (int x = 0; x < src.rows; x++)
        {
            for (int y = 0; y < src.cols; y++)
            {
                // Calcolo le differenze di intensità da pixel ad un centro e posiziono il pixel nel relativo cluster
                double min_distance = INFINITY;
                int cluster_index = 0;
                // si calcola differenza di intensità del pixel per ogni cluster.
                for (int k = 0; k < nClusters; k++)
                {
                    Scalar pixel_color = color_at(src,Point(y,x));
                    double distance = myDistance(pivot[k].color, pixel_color);
                    if (distance < min_distance)// se la distanza è più piccola di quella trovata fin ora,
                    {
                        min_distance = distance;
                        cluster_index = k; // Salvo il pixel nel cluster di indice k (a cui ha differenza minore)
                    }
                }
                // inserisco il pixel nel cluster
                pivot[cluster_index].cluster.push_back(Point(y, x));
            }
        }

        // passo 4 -> Calcolo la media colore di ogni cluster
        new_clusters_difference = 0;
        for (int k = 0; k < nClusters; k++)
        {
            vector<Point> clusterK = pivot[k].cluster; // vettore di appoggio
            int dim_cluster = (int)clusterK.size();
            cout << "Cluster points size " << dim_cluster << endl;
            Scalar mean_cluster_color = Scalar(0, 0, 0);
            for (int i = 0; i < dim_cluster; i++)
            {
                Scalar pixel_color = color_at(src,clusterK[i]);
                mean_cluster_color += pixel_color;
            }
            mean_cluster_color /= dim_cluster;
            pivot[k].color = mean_cluster_color; // Aggiorno il nuovo centro
            // Calcolo la distanza del nuovo centro dal vecchio e aggiungo a new_clusters_difference
            new_clusters_difference += myDistance(mean_cluster_color, pivot[k].color);
        }

        new_clusters_difference /= nClusters;
        difference = abs(old_clusters_difference - new_clusters_difference);
        cout << "Differenza " << difference << endl;
        old_clusters_difference = new_clusters_difference;
    }

    // passo 5 -> Disegno l'immagine di output
    for (int k = 0; k < nClusters; k++)
    {
        vector<Point> clusterK = pivot[k].cluster;
        Scalar pivot_color = pivot[k].color;
        for (int i = 0; i < (int)clusterK.size(); i++)
        {
            // Assegno il valore del centro agli altri pixel del cluster
            Point pixel = clusterK[i];
            dst.at<Vec3b>(pixel)[0] = pivot_color[0];
            dst.at<Vec3b>(pixel)[1] = pivot_color[1];
            dst.at<Vec3b>(pixel)[2] = pivot_color[2];
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