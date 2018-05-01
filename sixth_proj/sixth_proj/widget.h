#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

class NotMyWindow : public QWidget
{
    Q_OBJECT

public:
    NotMyWindow(QWidget *parent = 0);
    ~NotMyWindow();

signals:
    void startMWindow();
    void startSubWindow();
    void startReflVWindow();
    void startReflBWindow();
    void startTranspWindow();

public slots:
    void buttonClick1();
    void buttonClick2();
    void buttonClick3();
    void buttonClick4();
    void buttonClick5();
    void startWindow();
};

#endif // WIDGET_H
