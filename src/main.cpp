#include <QtWidgets>
#include "mainWindow.h"



int main(int argc, char* argv[])
{


QApplication app(argc, argv);


mainWindow* window = new mainWindow(app.primaryScreen()->availableGeometry());

     
return app.exec();
}