#include <QtWidgets>
#include "mainWindow.h"
#include "loggingWindow.h"



int main(int argc, char* argv[])
{


    QApplication app(argc, argv);

    // Creating a QDialog, which will conduct the Logging Session from the users
    loggingWindow* loggWin{ new loggingWindow() };

    // If Logging In was successful (all credentials are the correct ones for the logged user)
    if (loggWin->getLoggingStatus() == true)
    {
        // Executing the main window program (screen geometry parameters are set as input parameters to resize the main window)
        mainWindow* window = new mainWindow(app.primaryScreen()->availableGeometry());
        return app.exec();
    }
    else // The logging Window is closed
    { 
        return 0;
    }
     
    
}