#include "mainWindow.h"
#include "tableEdit.h"
#include "savingOverview.h"


mainWindow::mainWindow(const QRect& screen)
{

    setWindowTitle("Expense Tracker");


    QTextEdit* textEdit = new QTextEdit();
    textEdit->setFixedHeight(300);
    QPushButton* quitButton = new QPushButton("Quit");
    QPushButton* whoAmIButton = new QPushButton("Quien soy?");
    QWidget* centralWidget = new QWidget();
    QVBoxLayout* mainVLayout = new QVBoxLayout();
    

    QObject::connect(quitButton, SIGNAL(clicked()), qApp, SLOT(quit()));

    QObject::connect(whoAmIButton, &QPushButton::clicked, [=]() {
        textEdit->setText("That's Offensive");
        });

    mainVLayout->addWidget(textEdit);
    mainVLayout->addWidget(whoAmIButton);
    //mainVLayout->addWidget(quitButton);

    // Sizing the Main Window
    int width = screen.width() * widthFactor;
    int height = screen.height() * heightFactor;
    //resize(width, height);


    /////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////-------------///////////////////////////////////////////////////
    ////////// ---------- EXPENSE/SAVING OVERVIEW  ---------/////////////////////////////////
    /////////////////////////-------------///////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////
    
    savingOverview* tableHlayout = new savingOverview();

    mainVLayout->addLayout(tableHlayout);
    //mainVLayout->addLayout(tableHlayout->savingTablesLayout());

    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
        
    centralWidget->setLayout(mainVLayout);

    setCentralWidget(centralWidget);

    


    //////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////

    show();
}