#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QRect>
#include "constants.h"
#include "tableEdit.h"
#include "savingOverview.h"

//////////////////////////////////////////////////////////////////////////////////////////
// Description:                                                                         
// It creates the MainWindow were all content of Expense Tracker will be shown
//////////////////////////////////////////////////////////////////////////////////////////

class mainWindow : public QMainWindow
{

private:

    // Screen Resize Factors
    double widthFactor{0.7};
    double heightFactor{0.8};

    
public:

    // Constructor of mainWindow that take as input parameter QRect object that will be used to
    // resize the mainWindow accordingly
    mainWindow(const QRect& screen);

};

#endif