#include "widget.h"
#include <QApplication>
#include <QtWidgets>

#include <vector>
#include <math.h>
#include <iostream>
#include <stdio.h>

using namespace std;


//################################ SECOND PROJ PART

QRgb setPixel(unsigned int red, unsigned int green, unsigned int blue )
{

    QRgb result =  qRgba(red, green ,blue, 255);

    return result;

}

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

//############################# THIRD  PROJ PART

unsigned char getNormalPixVal(QRgb pixel)
{
    return (unsigned char)pixel;
}

QRgb setUCharToQRGB(unsigned char val)
{
    QRgb result = (255 << 24)  + (val << 16) + (val << 8) + val;
    return result;
}

template< typename T>
vector<T> mult_vec_on_matr(vector<T> vec, double ** matr, int size1 , int size2)
{
    vector<T> result;
    result.resize(vec.size());
    for (int ix = 0; ix < size1; ix ++)
    {
        result[ix] = 0;
        for (int iy = 0; iy < size2; iy++)
            result[ix] += vec[iy]  * matr[iy][ix];
    }
    return result;
}
#define EPS 0.00000000001

double toZero(double value)
{
    if(value < EPS && value > - EPS)
        return 0.0;
    else
        return value;
}

void toZero(double ** matr, int s1, int s2)
{
    for(int i = 0 ; i < s1; i++)
        for(int j = 0 ; j < s2; j++)
            matr[i][j] =  toZero(matr[i][j]);
}

template<typename T>
void toZero(vector<T> v)
{
    for(auto i: v)
    {
        i = toZero(i);
    }
}

double max_val(vector<double> v )
{
    double max = v[0];
    for( auto i : v)
        if( i >  max)
            max = i;
    return max;
}

double min_val(vector<double> v )
{
    double min = v[0];
    for( auto i : v)
        if( i <  min)
            min = i;
    return min;
}

vector<int> findanglesXY(QImage im,double ** matr, int size1 , int size2 )
{
    vector<double> XV;
    vector<double> YV;
    vector<double> tempV;
    vector<double> result;
    tempV.resize(3);

    //левый верхний
    //x
    tempV[0] = 0.0;
    //y
    tempV[1] = im.size().height();
    tempV[2] = 1.0;

    result = mult_vec_on_matr(tempV, matr, size1, size2);
    XV.push_back(result[0]);
    YV.push_back(result[1]);

    //левый нижний
    //x
    tempV[0] = 0.0;
    //y
    tempV[1] = 0.0;
    tempV[2] = 1.0;

    result = mult_vec_on_matr(tempV, matr, size1, size2);
    XV.push_back(result[0]);
    YV.push_back(result[1]);

    //правый верхний
    //x
    tempV[0] = im.size().width();
    //y
    tempV[1] = im.size().height();
    tempV[2] = 1.0;

    result = mult_vec_on_matr(tempV, matr, size1, size2);
    XV.push_back(result[0]);
    YV.push_back(result[1]);

    //правый нижний
    //x
    tempV[0] = im.size().width();
    //y
    tempV[1] = 0.0;
    tempV[2] = 1.0;

    result = mult_vec_on_matr(tempV, matr, size1, size2);
    XV.push_back(result[0]);
    YV.push_back(result[1]);

    vector<int> res;
    res.resize(4);
    double left_bot_X = min_val(XV);
    double left_bot_Y = min_val(YV);
    double right_top_X = max_val(XV);
    double right_top_Y = max_val(YV);
    res[0] = (int)left_bot_X;
    res[1] = (int)left_bot_Y;
    res[2] = (int)right_top_X;
    res[3] = (int)right_top_Y;
    return res;
}


vector<double> calcOldCoords(int Y, int X , double ** matr, int Msize1 ,int Msize2)
{
    vector<double> tempV;
    vector<double> result;
    tempV.resize(3);

    //x
    tempV[0] = (double)X;
    //y
    tempV[1] = (double)Y;
    tempV[2] = (double)1;

    result = mult_vec_on_matr(tempV, matr, Msize1, Msize2);
    return result;

}


double K_func(double delta)
{
    double a = -1.0;
    double absDelta = fabs(delta);
    if(absDelta > 2.0)
        return 0.0;
    if(absDelta > 1.0 && absDelta <= 2.0)
        return a * absDelta * absDelta * absDelta + 5.0 * absDelta * absDelta + 8.0 * a *absDelta - 4.0 * a;
    if(absDelta >= 0 && absDelta <= 1.0)
        return (a + 2.0) * absDelta * absDelta * absDelta - (a + 3.0) * absDelta * absDelta + 1.0;

}


QRgb  bicubicInterp(QImage img1, double x , double y)
{
    if(x > 130  && x < 131  && y > 130  && y < 131)
        x = x + 1.0 - 1.0;
    int size1 = img1.size().height();
    int size2 = img1.size().width();
    int ceilX = ceil(x);
    int ceilY = ceil(y);
    int floorX = floor(x);
    int floorY = floor(y);
    if(ceilX < 0 || ceilX >= size2)
    {
        QRgb temp = setPixel(0, 0, 0);
        return temp;
    }
    if(ceilY < 0 || ceilY >= size1)
    {
        QRgb temp = setPixel(0, 0, 0);
        return temp;
    }
    if(ceilX - 2 < 0  || ceilY - 2 < 0)
    {
        //QRgb temp = setPixel(0, 0, 0);
        //return temp;
        return img1.pixel(ceilX, ceilY);
    }
    if(ceilX + 1 >= size2 || ceilY + 1 >= size1)
    {
        //QRgb temp = setPixel(0, 0, 0);
        //return temp;
        return img1.pixel(ceilX, ceilY);
    }

    vector<double> values;
    //         x - 2               x + 1
    for(int i = floorY - 1; i <= ceilY + 1; i++)
    {
        double value = 0.0;
        for(int j = floorX - 1; j <= ceilX + 1; j++)
        {
            unsigned char S = getNormalPixVal(img1.pixel(j, i));
            double temp = K_func(x - (double)j);
            value += temp * (double)S;
        }
        values.push_back(fabs(value));
    }
    double result = 0.0;
    int counter = 0;
    for(int i = floorY - 1; i <= ceilY + 1; i++)
    {
        double S = values[counter++];
        double temp = K_func(y - (double)i);
        result += S * temp;

    }
    result = fabs(result);
    return setUCharToQRGB((unsigned char)result);

}


//###################################### PROJECT 4 #######################


unsigned char add_pix_to_pix( unsigned char a,  double b)
{
    int x = a;
    int y = ceil(b);
    if(x + y  > 255)
    {
         return 255;
    }
    if(x +  y < 0)
    {
        return 0;
    }
    return x + y;
}


//############################### PROJECT 5 #################


double ** get_kernel(char filename[256], int * size1, int * size2 )
{
    int s1, s2;
    int multiplier;

    double** result = NULL;

    FILE * f = fopen(filename, "r");
    fscanf(f, "%d", &s1);
    fscanf(f, "%d", &s2);
    fscanf(f, "%d", &multiplier);

    result = new double*[s1];
    for(int i = 0; i < s1; i++)
        result[i] = new double[s2];

    for(int i = 0; i < s1; i++)
        for(int j = 0 ; j < s2; j++)
        {
            int temp;
            fscanf(f, "%d",&temp );
            result[i][j] = ((double)temp) / ((double)multiplier);
        }

    fclose(f);

    *size1 = s1;
    *size2 = s2;
    return result;
}



QImage& makeConvoluton( QImage &img1, QImage &img2, double ** krnl , int ksize1, int ksize2)
{
    int halfY = ksize1 / 2;
    int halfX = ksize2 / 2;
    int oddX =  ksize2 % 2;
    int oddY =  ksize1 % 2;



    for(int i = 0; i < img1.height(); i++)
        for(int j = 0 ; j < img1.width(); j++)
        {
            double sum = 0.0;
            for(int k = 0; k < ksize1; k++)
                for(int l = 0; l < ksize2; l++)
                {
                    int real_indxX = j + l - halfX;
                    int real_indxY = i + k - halfY;
                    unsigned char pix_val;
                    if(real_indxX >= img1.width() || real_indxX < 0 || real_indxY < 0 || real_indxY >= img1.height())
                        continue;
                    else
                    {
                        QRgb cur_pix = img1.pixel(real_indxX, real_indxY);
                        pix_val = getNormalPixVal(cur_pix);
                        sum += pix_val * krnl[k][l];
                    }
                }
            if(sum > 255)
                sum = 255;
            if(sum < 0)
                sum = 0;
            unsigned char result = sum;
            img2.setPixel(j, i, setUCharToQRGB(result));

        }




    return img2;
}



//########################## PROJECT 6 #####################
int ** get_aperture(char filename[256], int * size1, int * size2 )
{

    int s1, s2;

    int** result = NULL;

    FILE * f = fopen(filename, "r");
    fscanf(f, "%d", &s1);
    fscanf(f, "%d", &s2);

    result = new int*[s1];
    for(int i = 0; i < s1; i++)
        result[i] = new int[s2];

    for(int i = 0; i < s1; i++)
        for(int j = 0 ; j < s2; j++)
        {
            int temp;
            fscanf(f, "%d",&temp );
            result[i][j] = temp;
        }

    fclose(f);

    *size1 = s1;
    *size2 = s2;
    return result;
}


QImage& makeRankFiltration( QImage &img1, QImage &img2, int ** aperture , int asize1, int asize2, int rank)
{
    int halfY = asize1 / 2;
    int halfX = asize2 / 2;
    int oddX =  asize2 % 2;
    int oddY =  asize1 % 2;

    vector<int> v;

    img2 = img2.scaled(img1.width() - 2 * halfX, img1.height() - 2 *halfY);


    for(int i = 0; i < img1.height() - 2 * halfY ; i++)
        for(int j = 0 ; j < img1.width() - 2 * halfX; j++)
        {
            v.clear();
            for(int k = 0; k < asize1; k++)
                for(int l = 0; l < asize2; l++)
                {
                    int real_indxX = j + l;
                    int real_indxY = i + k;


                    if(aperture[k][l] != 0)
                    {
                        QRgb cur_pix = img1.pixel(real_indxX,real_indxY);
                        unsigned char pix_val = getNormalPixVal(cur_pix);
                        v.push_back( pix_val);
                    }
                }
            std::sort(v.begin(), v.end());
            unsigned char result = v[rank - 1];
            img2.setPixel(j, i, setUCharToQRGB(result));
        }

    return img2;



}


#define PI 3.141592653589793238462643

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    NotMyWindow * window = new NotMyWindow();
    window->setWindowTitle("data");


    //делаем сеточку из отображаемых в окне элементов
    //и говорим что они будут отображатья в окошке
    QGridLayout * qgl = new QGridLayout;
    window->setLayout(qgl);

    char filename[256] = "../../lena.png";
    char apertureFilename[256] = "../../apert.txt";

    //грузим картинку
    QImage img1;
    QImage newImg;
    //img1.load("../../rose1.jpg");
    //img1.load("../../barb.png");

    img1.load(filename);
    newImg.load(filename);

    int ** aperture = NULL;
    int asize1, asize2;
    int RANK = 5;

    aperture = get_aperture(apertureFilename, &asize1, &asize2);
    newImg = makeRankFiltration(img1, newImg, aperture, asize1, asize2, RANK );




   // img1 = img1.scaled(200, 200);
    QPixmap px1 = QPixmap(QPixmap::fromImage(img1));
    QPixmap px2 = QPixmap(QPixmap::fromImage(newImg));
    QLabel ql1, ql2;
    ql1.setPixmap(px1);
    ql2.setPixmap(px2);
    //добавляем её в нужное место на сетке
    qgl->addWidget(&ql1, 0, 0);
    qgl->addWidget(&ql2, 0, 1);


    window->show();
    return a.exec();
}
