#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include "iconButton.h"

#include "constants.h"
#include "tableEdit.h"
#include "savingOverview.h"
#include "User.h"
#include "usr_preferences.h"
#include "group_management.h"

//////////////////////////////////////////////////////////////////////////////////////////
// Description:                                                                         
// It creates the MainWindow were all content of Expense Tracker will be shown
//////////////////////////////////////////////////////////////////////////////////////////

class mainWindow : public QMainWindow
{

    Q_OBJECT
            
public:

    // Constructor of mainWindow 
    mainWindow(User* currentUser);

};

#endif