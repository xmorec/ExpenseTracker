#include <QtWidgets>
#include "mainWindow.h"
#include "loggingWindow.h"



int main(int argc, char* argv[])
{


    QApplication app(argc, argv);

    loggingWindow* loggWin{ new loggingWindow() };

    if (loggWin->getLoggingStatus() == true)
    {
        mainWindow* window = new mainWindow(app.primaryScreen()->availableGeometry());
        return app.exec();
    }
    else {
        return 0;
    }
     
    
}