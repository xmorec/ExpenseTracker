#include "mainWindow.h"


mainWindow::mainWindow(User* currentUser)
{
    // Setting the Window Title and Icon
    setWindowIcon(QIcon(icons::expTrackerIcon));
    setWindowTitle("Expense Tracker - " + currentUser->getUserName());

    // Setting Fixed Window size (user cannot resize it) 
    setMaximumWidth(size().width());

    // Declaring the central Widget of the QMainWindow
    QWidget* centralWidget = new QWidget();

    // Declaring the Main Vertical Layout that will contain all content
    QVBoxLayout* mainVLayout = new QVBoxLayout();    


    //////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////  ADDING PREFERENCES ICON  ///////////////////////////////////////

    // Add a clickable Icon related to the Preferences
    QIcon prefIcon(icons::prefIcon);  
    iconButton* iconButt = new iconButton(prefIcon);
    mainVLayout->addWidget(iconButt,0,Qt::AlignTop);

    // Declaring and Initializing the Preferences Window
    confWindow* prefWin{new confWindow(currentUser)};

    // When clicking in the Preferences Button, the Preferences Window is shown
    QObject::connect(iconButt, &QPushButton::clicked, [=]() {
        QTimer::singleShot(80, [=]() {prefWin->winSize = prefWin->size(); });
        prefWin->exec();        
        });

    // When Preferences Window is closed, its size should be restored in case it is opened again
    QObject::connect(prefWin, &QDialog::rejected, [=]() {        
        prefWin->restartContents();
        });

    // When username is updated from Prefernces Window, Main Window title should be updated too   
   QObject::connect(prefWin, &confWindow::userNameMod, [=]() {
        setWindowTitle("Expense Tracker - " + currentUser->getUserName());
        });
        

    //////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////  EXPENSE/SAVING OVERVIEW  ///////////////////////////////////////

    savingOverview* savingTablesLayout = new savingOverview(currentUser);

    mainVLayout->addLayout(savingTablesLayout);
    mainVLayout->setAlignment(savingTablesLayout, Qt::AlignTop);

    // Main Window size is resized after the Expense Table is resized too
    QObject::connect(savingTablesLayout, &savingOverview::tableSizeMod, [=]() {        
        move(pos()); // This line is needed to fix a Qt Bug that does not update the Main Window size if it does not detect a window movement
        setMaximumSize(sizeHint()); // Resizing the Main Window to fit all content inside
        });
    
    ///////////////////////////////////////////////////////////////////////////////////////////

    centralWidget->setLayout(mainVLayout);
    setCentralWidget(centralWidget);
    show();

    //////////////////////////////////////////////////////////////////////////////////////////
}