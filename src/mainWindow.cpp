#include "mainWindow.h"


mainWindow::mainWindow(User* currentUser)
{

    // Set fixed Window size (user cannot resize it)
    setWindowFlag(Qt::MSWindowsFixedSizeDialogHint, true);

    // Setting the Window Title and Icon
    setWindowIcon(QIcon(icons::expTrackerIcon));
    setWindowTitle("Expense Tracker - " + currentUser->getUserName());
 
    // Declaring the central Widget of the QMainWindow
    QWidget* centralWidget = new QWidget();

    // Declaring the Main Vertical Layout that will contain all content
    QVBoxLayout* mainVLayout = new QVBoxLayout();    

    // Declaring the layout which contains top buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();

    //////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////  ADDING PREFERENCES ICON  ///////////////////////////////////////

    // Add a clickable Icon related to the Preferences
    QIcon prefIcon(icons::prefIcon);  
    iconButton* prefButt = new iconButton(prefIcon);
    prefButt->setToolTip("Open Preferences menu");
    buttonLayout->addWidget(prefButt);

    // Declaring and Initializing the Preferences Window
    confWindow* prefWin{new confWindow(currentUser)};

    // When clicking in the Preferences Button, the Preferences Window is shown
    QObject::connect(prefButt, &QPushButton::clicked, [=]() {
        QTimer::singleShot(80, [=]() {
            prefWin->adjustWinSize();
            });
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
   ////////////////////////  ADDING GROUP PREFERENCES ICON  /////////////////////////////////

// Add a clickable Icon related to the Preferences
   iconButton* groupButt = new iconButton(QIcon(icons::groupPrefIcon));
   groupButt->setToolTip("Open Group Management menu");
   groupButt->setIconSize(26);
   buttonLayout->addWidget(groupButt);

   // Declaring and Initializing the Preferences Window
   groupManWindow* groupManWin{new groupManWindow(currentUser)};

   // When clicking in the Preferences Button, the Preferences Window is shown
   QObject::connect(groupButt, &QPushButton::clicked, [=]() {
       QTimer::singleShot(80, [=]() {
           //prefWin->adjustWinSize();
           });
       groupManWin->exec();
       });

   // When Preferences Window is closed, its size should be restored in case it is opened again
   QObject::connect(groupManWin, &QDialog::rejected, [=]() {
       groupManWin->selectView();
       });
       

   mainVLayout->addLayout(buttonLayout);
   mainVLayout->setAlignment(buttonLayout, Qt::AlignLeft);

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