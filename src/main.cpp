#include <QApplication>
#include <QScreen>
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
        // Getting the logged User
        User* currentUser{loggWin->getCurrentUser()};

        // Getting the Users vector
        std::vector<User*> users{ std::move(loggWin->getUsers()) };

        // Getting the Groups vector
        std::vector<Group*> groups{ std::move(loggWin->getGroups()) };

        // deleting loggWin as it is no longer needed
        delete loggWin;
        loggWin = nullptr;

        // Executing the main window program (screen geometry parameters are set as input parameters to resize the main window)
        mainWindow* window = new mainWindow(currentUser, users, groups);
        
        return app.exec();
    }
    else // The logging Window is closed
    { 
        return 0;
    }

}