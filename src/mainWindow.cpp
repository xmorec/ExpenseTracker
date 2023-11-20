#include "mainWindow.h"


mainWindow::mainWindow(User* currentUser, std::vector<User*>& users, std::vector<Group*>& groups)
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
    confWindow* prefWin{new confWindow(currentUser, users, groups)};

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
    groupManWindow* groupManWin{new groupManWindow(currentUser, users, groups)};

    // Depending on the News status, the icon will show a red circle (notification) or not.
    if (groupManWin->getNewsInvStatus() || groupManWin->getNewsReqStatus())
    {
        groupButt->setIcon((QIcon(icons::groupPrefNewsIcon)));
    }
    else
    {
        groupButt->setIcon((QIcon(icons::groupPrefIcon)));
    }

    // When clicking in the Preferences Button, the Preferences Window is shown
    QObject::connect(groupButt, &QPushButton::clicked, [=]() {
        QTimer::singleShot(80, [=]() {
            groupManWin->setFixedSize(groupManWin->sizeHint());
            });
        groupManWin->exec();
        });

   // When Preferences Window is closed, its size should be restored in case it is opened again
    QObject::connect(groupManWin, &QDialog::rejected, [=]() {
        groupManWin->selectView();
        });

    // When there are no input requests or invitations
    QObject::connect(groupManWin, &groupManWindow::clearedNews, [=]() {
        groupButt->setIcon((QIcon(icons::groupPrefIcon)));
        });

    // When there are new input requests as user joined new group with input requests
    QObject::connect(groupManWin, &groupManWindow::raisedNews, [=]() {
        groupButt->setIcon((QIcon(icons::groupPrefNewsIcon)));
        });       

    
    // When user is removed from Preferences Window, the Group Management should be updated accordingly
    QObject::connect(prefWin, &confWindow::userRemoved, [=]() {
        if (groupManWin->getGroupDialogsStatus())
        {
            groupManWin->updateInvitationWindow();
            groupManWin->updateMembersWindow();
            groupManWin->updateHandleInvReqWindow();
        }

        if (groupManWin->getNewsInvStatus() || groupManWin->getNewsReqStatus())
        {
            groupButt->setIcon((QIcon(icons::groupPrefNewsIcon)));
        }
        else
        {
            groupButt->setIcon((QIcon(icons::groupPrefIcon)));
        }
        
        groupManWin->selectView();

        });

    // When group is removed from Preferences Window, the Group Management should be updated accordingly
    QObject::connect(prefWin, &confWindow::groupRemoved, [=]() {

        groupManWin->updateReceivedInvitationsWindow();
        groupManWin->updateSendRequestsWindow();

        if (groupManWin->getNewsInvStatus() || groupManWin->getNewsReqStatus())
        {
            groupButt->setIcon((QIcon(icons::groupPrefNewsIcon)));
        }
        else
        {
            groupButt->setIcon((QIcon(icons::groupPrefIcon)));
        }

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
    

    // Group name has been changed
    QObject::connect(groupManWin, &groupManWindow::renamedGroup, [=]() {
       // Actions to be developed here

        });

    ///////////////////////////////////////////////////////////////////////////////////////////

    centralWidget->setLayout(mainVLayout);
    setCentralWidget(centralWidget);
    show();

    //////////////////////////////////////////////////////////////////////////////////////////
}