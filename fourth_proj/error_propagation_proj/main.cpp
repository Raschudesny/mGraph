#include "widget.h"
#include <QApplication>
#include <QtWidgets>

#include <vector>
#include <math.h>
#include <iostream>

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
    //грузим картинку
    QImage img1;
    QImage newImg;
    //img1.load("../../rose1.jpg");
    //img1.load("../../barb.png");

    img1.load(filename);
    newImg.load(filename);

    int N = 2;


    for(int i = 0 ;  i < img1.height(); i++)
        for(int j = 0; j < img1.width(); j++)
        {
            QRgb cur_pix = img1.pixel(j, i);
            unsigned char pix_val = getNormalPixVal(cur_pix);
            unsigned char threshhold = (255 / N) ;
            unsigned char result;
            if(pix_val > threshhold)
                result = 255;
            else
                result = 0;
            int err =  pix_val - result;
            img1.setPixel(j, i, setUCharToQRGB(result));

            if(j + 1 < img1.width())
            {
                QRgb temp_pix = img1.pixel(j  + 1, i);
                unsigned char temp_val = getNormalPixVal(temp_pix);
                double temp_res = (7.0/16.0) * err;
                unsigned char res_pix_val = add_pix_to_pix(temp_val ,  (double)temp_res);
                img1.setPixel( j + 1 , i , setUCharToQRGB(res_pix_val));
            }
            if( j -  1 > 0 && i + 1 < img1.height())
            {
                QRgb temp_pix = img1.pixel(j  - 1 , i + 1);
                unsigned char temp_val = getNormalPixVal(temp_pix);
                double temp_res = (3.0/16.0) * err;
                unsigned char res_pix_val = add_pix_to_pix(temp_val ,  (double)temp_res);
                img1.setPixel( j - 1 , i + 1, setUCharToQRGB(res_pix_val));
            }
            if( i + 1  < img1.height())
            {
                QRgb temp_pix = img1.pixel(j, i + 1);
                unsigned char temp_val = getNormalPixVal(temp_pix);
                double temp_res = (5.0/16.0) * err;
                unsigned char res_pix_val = add_pix_to_pix(temp_val ,  (double)temp_res);
                img1.setPixel( j , i + 1, setUCharToQRGB(res_pix_val));

            }
            if(i + 1  < img1.height() && j + 1  < img1.width())
            {
                QRgb temp_pix = img1.pixel(j  + 1, i + 1);
                unsigned char temp_val = getNormalPixVal(temp_pix);
                //double temp_res = (1.0 / 16.0 ) * err;
                double temp_res = err - (7.0/16.0) * err  -  (3.0/16.0) * err - (5.0/16.0) * err;
                unsigned char res_pix_val = add_pix_to_pix(temp_val ,  (double)temp_res);
                img1.setPixel( j + 1 , i + 1, setUCharToQRGB(res_pix_val));
            }



        }



   // img1 = img1.scaled(200, 200);
    QPixmap px1 = QPixmap(QPixmap::fromImage(newImg));
    QPixmap px2 = QPixmap(QPixmap::fromImage(img1));
    QLabel ql1, ql2;
    ql1.setPixmap(px1);
    ql2.setPixmap(px2);
    //добавляем её в нужное место на сетке
    qgl->addWidget(&ql1, 0, 0);
    qgl->addWidget(&ql2, 0, 1);


    window->show();
    return a.exec();
}
