#include "widget.h"


void NotMyWindow::buttonClick1()
{
    this->close();
    emit startSubWindow();
}
void NotMyWindow::buttonClick3()
{
    this->close();
    emit startReflVWindow();
}
void NotMyWindow::buttonClick4()
{
    this->close();
    emit startReflBWindow();
}
void NotMyWindow::buttonClick5()
{
    this->close();
    emit startTranspWindow();
}


//для BACK
void NotMyWindow::buttonClick2()
{
    this->close();
    emit startMWindow();
}

void NotMyWindow::startWindow()
{
    this->show();
}

NotMyWindow::NotMyWindow(QWidget *parent)
    : QWidget(parent)
{
}

NotMyWindow::~NotMyWindow()
{

}
