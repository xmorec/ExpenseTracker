#include "mainWindow.h"



mainWindow::mainWindow(const QRect& screen)
{
    setWindowIcon(QIcon(icons::expTrackerIcon));
    setWindowTitle("Expense Tracker");

    // Set Fixed Window size (user cannot resize it)
    //setWindowFlag(Qt::MSWindowsFixedSizeDialogHint, true);

    QWidget* centralWidget = new QWidget();
    QVBoxLayout* mainVLayout = new QVBoxLayout();    

    //QObject::connect(quitButton, SIGNAL(clicked()), qApp, SLOT(quit()));
    //mainVLayout->addWidget(quitButton);

    // Sizing the Main Window
    int width = screen.width() * widthFactor;
    int height = screen.height() * heightFactor;
    //resize(width, height);


    //////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////  EXPENSE/SAVING OVERVIEW  ///////////////////////////////////////

    
    savingOverview* savingTablesLayout = new savingOverview();
    mainVLayout->addLayout(savingTablesLayout);


    //////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////  SETTING THE LAYOUT IN MAINWINDOW  //////////////////////////////        

    centralWidget->setLayout(mainVLayout);
    setCentralWidget(centralWidget);
    show();

    //////////////////////////////////////////////////////////////////////////////////////////
}