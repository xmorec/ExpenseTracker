#include <QtWidgets>
#include "mainWindow.h"
#include "loggingWindow.h"



int main(int argc, char* argv[])
{


QApplication app(argc, argv);

loggingWindow* loggWin{ new loggingWindow() };


mainWindow* window = new mainWindow(app.primaryScreen()->availableGeometry());

     
return app.exec();
}