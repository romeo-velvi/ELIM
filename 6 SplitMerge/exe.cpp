#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

#define MAX_DEV 10
#define MAX_AREA 50
using namespace std;
using namespace cv;
Mat img;

struct Region
{
    vector<Region> child;
    bool valid;
    Scalar label;
    Rect area;
};

bool predicate(Mat src)
{
    Scalar dev;
    meanStdDev(src, Scalar(), dev);
    return (dev[0] < MAX_DEV || src.cols * src.rows < MAX_AREA);
}

// prende in input la porzione dell'immagine e l'aria (rettangolo) di splitting
Region split(Mat src, Rect area)
{
    // passo 0 -> crea nuova Regione
    Region R;
    R.valid = true;
    R.area = area;
     
    // passo 1 -> controllo predicato della regione, vero? rimango, falso? split
    if (predicate(src))
    {// Se il predicato è vero, la regione NON può essere separata
        Scalar mean, stddev;
        meanStdDev(src, mean, stddev); // calcolo media (deviazione standard)
        R.label = mean; // assegno come label la media dev_std.
    }
    else
    {// Se il predicato è falso, la regione DEVE essere separata
        int w = src.cols / 2;
        int h = src.rows / 2;
        Region r1 = split(src(Rect(0, 0, w, h)), Rect(area.x, area.y, w, h)); // eseguo split rettangolo up-left
        Region r2 = split(src(Rect(w, 0, w, h)), Rect(area.x + w, area.y, w, h)); // eseguo split rettangolo up-right
        Region r3 = split(src(Rect(0, h, w, h)), Rect(area.x, area.y + h, w, h)); // eseguo split rettangolo down-left
        Region r4 = split(src(Rect(w, h, w, h)), Rect(area.x + w, area.y + h, w, h)); // eseguo split rettangolo down-right
        // aggiunge al rettangolo di partenza le regioni adiacenti splittate
        R.child.push_back(r1); // imshow("splitted",img(r1.area));    waitKey(0);
        R.child.push_back(r2);// imshow("splitted",img(r2.area));   waitKey(0);
        R.child.push_back(r3);// imshow("splitted",img(r3.area));   waitKey(0);
        R.child.push_back(r4);// imshow("splitted",img(r4.area));   waitKey(0);
    }
    return R;
}

// prende in input la porzione dell'immagine e l'aria (rettangolo) di splitting
void mergeRegion(Mat src, Region &r1, Region &r2)
{
    // passo 1 -> se le aree in considerazione non hanno figli 
    if ((int)r1.child.size() == 0 && (int)r2.child.size() == 0)
    {
        Rect r12 = r1.area | r2.area;
        // passo 2 -> si controlla che le regioni unite rispettino il predicato
        if (predicate(src(r12)))
        { // Se rispettano, le la regione 1 sarà equivalente a reg1 + reg2 
            r1.area = r12;
            r1.label = (r1.label + r2.label) / 2;
            r2.valid = false;
        }
    }
}

// prende in input la porzione dell'immagine e l'aria (rettangolo) di split
void merge(Mat src, Region &r)
{
    // passo 1 -> controlla se ci sono rettangoli figli,
    if ((int)r.child.size() > 0)
    { // Controlla se è possibile unire le regioni
        mergeRegion(src, r.child.at(0), r.child.at(1));
        mergeRegion(src, r.child.at(2), r.child.at(3));
        mergeRegion(src, r.child.at(0), r.child.at(2));
        mergeRegion(src, r.child.at(1), r.child.at(3));
        for (int i = 0; i < (int)r.child.size(); i++)
            merge(src, r.child.at(i)); // Controllare che aver tolto l'if non causi problemi
    }
}

void display(Mat &out, Region R)
{
    // passo 0 -> Se la regione master non ha sotto-regioni e la regione master è valida
    // passo 1 ->  disegna un rettangolo.
    if ((int)R.child.size() == 0 && R.valid)
        rectangle(out, R.area, R.label, FILLED); // Area=Zona di interesse; Label=Colore; FILLED = Tipo di rettangolo
    // Visualizza ricorsivamente le sotto-regioni della regione master
    for (int i = 0; i < (int)R.child.size(); i++)
        display(out, R.child.at(i));
}

int main(int argc, char *argv[])
{
    Mat src = imread(argv[1], IMREAD_ANYCOLOR);
    if (src.empty())
    {
        cout << "Wrong PATH image selected\nPlease, try again..." << endl;
        exit(-1);
    }
    img = src;

    // passo 1: Dividi l'immagine. (Il rettangolo inziale dato come argomento è l'intera immagine).
    Region r = split(src, Rect(0, 0, src.cols, src.rows));
    
    // passo 2: richiama la funzione merge
    merge(src, r);

    Mat out = src.clone();
    display(out, r);

    imshow("Input", src);
    imshow("Output-1", out);
    
    // Mat out2 = Mat::zeros(src.size(), src.type());
    // display(out2, r);
    // imshow("Output-2", out2);

    waitKey(0);
    return 0;
}