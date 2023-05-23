#include "mainWindow.h"
#include "tableEdit.h"
#include "savingOverview.h"


mainWindow::mainWindow(const QRect& screen)
{
    setWindowIcon(QIcon("resources/icons/ExpTrIcon.png"));
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