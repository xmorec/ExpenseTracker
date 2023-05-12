#include <QtWidgets>
#include "mainWindow.h"



int main(int argc, char* argv[])
{


QApplication app(argc, argv);


mainWindow* window = new mainWindow();

     
return app.exec();
}