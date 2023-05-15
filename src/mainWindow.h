#pragma once
#include <QtWidgets>


class mainWindow : public QMainWindow
{

private:

    double widthFactor{0.7};
    double heightFactor{0.8};

    
public:
    mainWindow(const QRect& screen);

};

