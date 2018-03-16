#include "widget.h"
#include <QApplication>
#include <QtWidgets>

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
    NotMyWindow * subwindow = new NotMyWindow();
    NotMyWindow * res_wind1 = new NotMyWindow();
    NotMyWindow * res_wind2 = new NotMyWindow();
    NotMyWindow * res_wind3 = new NotMyWindow();

    window->setWindowTitle("data");
    subwindow->setWindowTitle("result");
    res_wind1->setWindowTitle("reflection vertical");
    res_wind2->setWindowTitle("reflection horizontal");
    res_wind3->setWindowTitle("transpose");


    //делаем сеточку из отображаемых в окне элементов
    //и говорим что они будут отображатья в окошке
    QGridLayout * qgl = new QGridLayout;
    QGridLayout * SubWindQGL = new QGridLayout;
    QGridLayout * refl_topQGL = new QGridLayout;
    QGridLayout * refl_botQGL = new QGridLayout;
    QGridLayout * transpQGL = new QGridLayout;
    window->setLayout(qgl);
    subwindow->setLayout(SubWindQGL);
    res_wind1->setLayout(refl_topQGL);
    res_wind2->setLayout(refl_botQGL);
    res_wind3->setLayout(transpQGL);

    //грузим картинку
    QImage img1,img2,img3, blend_img, refl_vert_img, refl_horiz_img, transpose_img;
    img1.load("C:/Users/ChUd0/Desktop/workplace/QT projects/pomogiti2/LENNA.jpg");
    img2.load("C:/Users/ChUd0/Desktop/workplace/QT projects/pomogiti2/rose1.jpg");
    img3.load("C:/Users/ChUd0/Desktop/workplace/QT projects/pomogiti2/barb.png");
    blend_img.load("C:/Users/ChUd0/Desktop/workplace/QT projects/pomogiti2/LENNA.jpg");
    refl_vert_img.load("C:/Users/ChUd0/Desktop/workplace/QT projects/pomogiti2/LENNA.jpg");
    refl_horiz_img.load("C:/Users/ChUd0/Desktop/workplace/QT projects/pomogiti2/LENNA.jpg");
    transpose_img.load("C:/Users/ChUd0/Desktop/workplace/QT projects/pomogiti2/LENNA.jpg");

    //rescale all images
    QSize size1 = img1.size();
    img2 = img2.scaled(size1);
    img3 = img3.scaled(size1);

    qDebug() << img1.pixel(0,0);

    // MAIN FUNCTIONAL PART OF KOBEP
    blend_img = makeAlphaBlending(img1,img2,img3);
    reflectImageVerticle(refl_vert_img);
    reflectImageHorizontal(refl_horiz_img);
    transpose(transpose_img);

    QPixmap px1 = QPixmap(QPixmap::fromImage(img1));
    QPixmap px2 = QPixmap(QPixmap::fromImage(img2));
    QPixmap px3 = QPixmap(QPixmap::fromImage(img3));
    QPixmap px_blend = QPixmap(QPixmap::fromImage(blend_img));
    QPixmap px_refl_vert = QPixmap(QPixmap::fromImage(refl_vert_img));
    QPixmap px_refl_horiz = QPixmap(QPixmap::fromImage(refl_horiz_img));
    QPixmap px_transpose = QPixmap(QPixmap::fromImage(transpose_img));
    QLabel ql1,ql2,ql3,ql_blend, ql_refl_vert,ql_refl_horiz, ql_transpose;
    //qDebug() << img1.format();
    ql1.setPixmap(px1);
    ql2.setPixmap(px2);
    ql3.setPixmap(px3);
    ql_blend.setPixmap(px_blend);
    ql_refl_vert.setPixmap(px_refl_vert);
    ql_refl_horiz.setPixmap(px_refl_horiz);
    ql_transpose.setPixmap(px_transpose);

  // здесь заканчивается КОВЁР
  // и начинается ПАЛАС

 // если лень клепать окно чтобы все вывести, то ну можно вообще и вот так глянуть
   //QLabel temp_ql;
   //temp_ql.setPixmap(px_refl_horiz);
   //temp_ql.show();

    //добавляем её в нужное место на сетке
    qgl->addWidget(&ql1,0,0);
    qgl->addWidget(&ql2,0,1);
    qgl->addWidget(&ql3,0,2);

    //тоже для результата
    SubWindQGL->addWidget(&ql_blend,0,0);
    refl_topQGL->addWidget(&ql_refl_vert, 0, 0);
    refl_botQGL->addWidget(&ql_refl_horiz, 0, 0);
    transpQGL->addWidget(&ql_transpose,0,0);

    QPushButton * AlphaBlendButton = new QPushButton("GET ALPHABLENDING RESULT");
    QPushButton * ReflTopButton = new QPushButton("GET VERTICAL REFLECTION RESULT");
    QPushButton * ReflBotButton = new QPushButton("GET HORIZONTAL REFLECTION RESULT");
    QPushButton * TranspButton = new QPushButton("GET TRANSPOSE RESULT");
    QPushButton * BackButton1 = new QPushButton(" <<  BACK");
    QPushButton * BackButton2 = new QPushButton(" <<  BACK");
    QPushButton * BackButton3 = new QPushButton(" <<  BACK");
    QPushButton * BackButton4 = new QPushButton(" <<  BACK");

    qgl->addWidget(AlphaBlendButton, 1, 1);
    qgl->addWidget(ReflTopButton, 2, 1);
    qgl->addWidget(ReflBotButton, 3, 1);
    qgl->addWidget(TranspButton, 4, 1);
    SubWindQGL->addWidget(BackButton1, 1, 0);
    refl_topQGL->addWidget(BackButton2, 1, 0);
    refl_botQGL->addWidget(BackButton3, 1, 0);
    transpQGL->addWidget(BackButton4, 1, 0);



    QObject::connect(AlphaBlendButton, SIGNAL(clicked()), window , SLOT(buttonClick1()));
    QObject::connect(ReflTopButton, SIGNAL(clicked()), window , SLOT(buttonClick3()));
    QObject::connect(ReflBotButton, SIGNAL(clicked()), window , SLOT(buttonClick4()));
    QObject::connect(TranspButton, SIGNAL(clicked()), window , SLOT(buttonClick5()));

    QObject::connect(window, SIGNAL(startSubWindow()), subwindow, SLOT(startWindow()));
    QObject::connect(window, SIGNAL(startReflVWindow()), res_wind1, SLOT(startWindow()));
    QObject::connect(window, SIGNAL(startReflBWindow()), res_wind2, SLOT(startWindow()));
    QObject::connect(window, SIGNAL(startTranspWindow()), res_wind3, SLOT(startWindow()));

    //для всех кнопок Back
    QObject::connect(BackButton1, SIGNAL(clicked()), subwindow, SLOT(buttonClick2()));
    QObject::connect(BackButton2, SIGNAL(clicked()), res_wind1, SLOT(buttonClick2()));
    QObject::connect(BackButton3, SIGNAL(clicked()), res_wind2, SLOT(buttonClick2()));
    QObject::connect(BackButton4, SIGNAL(clicked()), res_wind3, SLOT(buttonClick2()));

    QObject::connect(subwindow, SIGNAL(startMWindow()),window, SLOT(startWindow()));
    QObject::connect(res_wind1, SIGNAL(startMWindow()),window, SLOT(startWindow()));
    QObject::connect(res_wind2, SIGNAL(startMWindow()),window, SLOT(startWindow()));
    QObject::connect(res_wind3, SIGNAL(startMWindow()),window, SLOT(startWindow()));


    window->show();
    return a.exec();
}
