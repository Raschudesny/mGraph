#include "widget.h"
#include <QApplication>
#include <QtWidgets>

#include <vector>
#include <math.h>
#include <iostream>

using namespace std;




QRgb calculateNewPixel(QRgb first, QRgb second, QRgb alpha)
{
    unsigned char f = first;
    unsigned char s = second;
    unsigned char a = alpha;
    unsigned int res = ((unsigned int)f * a + (255 - a)*(unsigned int)s)/255 ;
    QRgb result = (255 << 24)  + (res << 16) + (res << 8) + res;

    //QRgb result = ((long)first * alpha + (255 - alpha)*(long)second)/255;
    return result;
}

void getRGB(int* r, int * g, int * b, QRgb pix)
{
    *b = 255 & pix;
    pix >>= 8;
    *g = 255 & pix;
    pix >>= 8;
    *r = 255 & pix;
}

int getBrightness(QRgb pix)
{
    int pix_r, pix_g, pix_b;
    getRGB(&pix_r, &pix_g, &pix_b, pix);
    int result = 0.299 * pix_r + 0.587 * pix_g + 0.114 * pix_b;
    return result;
}


vector<double> makeDistr(vector<int> v , int NumPixels )
{
    vector<double> distrib;
    for(int i : v)
        distrib.push_back((double)i / (double)NumPixels);
    return distrib;
}


int ** calculateMatrix(int c , int r, QImage img)
{
    int** matrix = new int*[256];
    for(int i = 0 ; i < 256; i++)
        matrix[i] = new int[256];
    for(int i = 0 ; i < 256; i ++)
        for(int j = 0 ; j < 256; j ++)
            matrix[i][j] = 0;

    for(int k = 0 ; k + r < img.size().height(); k++)
        for(int l = 0; l  + c < img.size().width(); l++)
        {
            int pixa = getBrightness(img.pixel(l, k));
            int pixb = getBrightness(img.pixel(l + c, k + r) );
            matrix[pixa][pixb]++;
        }



    return matrix;
}






vector<int> makeHisto(QImage img)
{
    vector<int> v ;
    v.resize(256);
    std::fill(v.begin(), v.end(), 0);

    for(int i = 0 ; i < img.size().height(); i++)
        for(int j = 0 ; j < img.size().width(); j++)
        {
            int bright = getBrightness(img.pixel(j, i));
            v[bright]++;
        }
    return v;
}

QImage makeAlphaBlending(QImage img1, QImage img2, QImage img3)
{
    QImage* retImg = new QImage(img1);


    for(int i = 0; i < img1.size().height(); i++)
        for(int j = 0; j < img2.size().width(); j++)
        {
            retImg->setPixel(j,i, calculateNewPixel(img1.pixel(j,i),img2.pixel(j, i), img3.pixel(j,i)));
        }
    return *retImg;
}

void reflectImageVerticle(QImage& img)
{
    for(int i = 0; i < img.size().height(); i++)
        for(int j = 0; j < img.size().width() / 2; j++)
        {
            QRgb temp = img.pixel(j,i);
            img.setPixel(j, i ,img.pixel(img.size().width() - j - 1, i));
            img.setPixel(img.size().width() - j - 1 , i , temp);
        }
}

void reflectImageHorizontal(QImage& img)
{
    for(int i = 0; i < img.size().height() / 2; i++)
        for(int j = 0; j < img.size().width(); j++)
        {
            QRgb temp = img.pixel(j,i);
            img.setPixel(j, i ,img.pixel(j, img.size().height() - i - 1));
            img.setPixel(j , img.size().height() - i - 1 , temp);
        }
}

void transpose(QImage& img)
{
    for(int i = 0 ; i < img.size().height(); i++)
        for(int j = 0 ; j < i; j++)
        {
            QRgb temp = img.pixel(j,i);
            img.setPixel(j, i, img.pixel(i,j));
            img.setPixel(i, j, temp);
        }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    NotMyWindow * window = new NotMyWindow();

    window->setWindowTitle("data");


    //делаем сеточку из отображаемых в окне элементов
    //и говорим что они будут отображатья в окошке
    QGridLayout * qgl = new QGridLayout;
    window->setLayout(qgl);

    //грузим картинку
    QImage img1;
    //img1.load("../../rose1.jpg");
    img1.load("../../barb.png");

    //img1.load("../../lena.png");

    //rescale all images
    QSize size1 = img1.size();

    vector<int> hist = makeHisto(img1);
    for(int  i: hist)
        cout << i << ", ";
    cout << endl;

    vector<double> distrib = makeDistr(hist, img1.size().width() * img1.size().height());
    for(double  i: distrib)
        cout << i << ", ";
    cout << endl;

    double Average = 0.0;
    for(int i = 0; i < distrib.size(); i++)
    {
        Average += distrib[i] * (double)i;
    }

    double Disp = 0.0,  Eks = 0.0, Assim = 0.0, Energy = 0.0 , Entropia = 0.0;

    for(int i = 0; i < distrib.size(); i++)
    {
        Disp += distrib[i] * ((double)i - Average)* ((double)i - Average);
        Eks += distrib[i] * ((double)i - Average)* ((double)i - Average) * ((double)i - Average)* ((double)i - Average);
        Eks -= 3.0;
        Assim += distrib[i] * ((double)i - Average)* ((double)i - Average)* ((double)i - Average);
        Energy += distrib[i] * distrib[i];
        if(distrib[i] < 0.000000001 && distrib[i] > -0.0000000001)
            continue;
        Entropia -= distrib[i] * log(distrib[i]) / log(2);
    }

    Eks *= 1.0 / (Disp * Disp);
    Assim *= 1.0 / (Disp * sqrt(Disp));


    cout << "Average = " << Average << endl;
    cout << "Disp = " << Disp << endl;
    cout << "S K O  = " << sqrt(Disp) << endl;
    cout <<" Assim = " << Assim << endl;
    cout <<" Eks = " << Eks  << endl;
    cout << "Energy = " << Energy << endl;
    cout << "Entropia = " << Entropia << endl;


    const int R = 1;
    const int C = 0;
    int ** matr = calculateMatrix(C, R, img1);


    for(int i = 0 ; i < 256; i++)
    {
        for(int j = 0; j < 256; j++)
            cout << matr[i][j] << " ";
        cout << endl;
    }




    double MATRIXENERGY = 0.0;

    double Pmatr[256][256];
    int AMOUNT = (img1.size().height() - R) * (img1.size().width() - C);
    for(int i = 0 ; i < 256; i++ )
        for(int j = 0 ; j < 256; j++)
        {
            Pmatr[i][j] = (double)matr[i][j] /(double) AMOUNT;
            MATRIXENERGY += Pmatr[i][j] *Pmatr[i][j];
        }
    cout << "Matr Energy = " << MATRIXENERGY << endl;

    QPixmap px1 = QPixmap(QPixmap::fromImage(img1));
    QLabel ql1;
    ql1.setPixmap(px1);
    //добавляем её в нужное место на сетке
    qgl->addWidget(&ql1,0,0);


//    QPushButton * AlphaBlendButton = new QPushButton("GET ALPHABLENDING RESULT");

//    qgl->addWidget(AlphaBlendButton, 1, 1);

//    QObject::connect(AlphaBlendButton, SIGNAL(clicked()), window , SLOT(buttonClick1()));

    //связать клик по кнопке и функцию расчета матрицы с гистограмой
//    QObject::connect(window, SIGNAL(startSubWindow()), window, SLOT(startWindow()));

    window->show();
    return a.exec();
}
