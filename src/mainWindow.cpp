#include "mainWindow.h"
#include "tableEdit.h"


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

    QHBoxLayout* tableHlayout = new QHBoxLayout();


    ///// OVERVIEW TABLE EXPENSES
    ///////////////////////////////////////

    tableEdit* savingTable2 = new tableEdit(4, 5);
    savingTable2->adaptWidgetToTable();
    tableHlayout->addWidget(savingTable2);
    tableHlayout->setAlignment(savingTable2, Qt::AlignRight);


    ///// OVERVIEW TABLE SAVING
    ///////////////////////////////////////
    QStringList savingParametersList{
        {
            "Salary",
            "Despesa Fixa",
            "Despesa Extra",
            "Ahorro"
        }
    };

    tableEdit* savingTable = new tableEdit(1, savingParametersList.length());

    savingTable->setHorizontalHeaderLabels(savingParametersList);

    //savingTable->resizeColumnsToContents();
    //savingTable->horizontalHeader()->setDefaultSectionSize(100);
    
    savingTable->adaptWidgetToTable();
    

    //// BUTTON TEST INSIDE TABLE

    //QTableWidget* savingTableX = new QTableWidget(2, 2);
    savingTable->setCellWidget(0, 0, quitButton);
    
    QTableWidget* nestedTable = new QTableWidget(1,3);
    nestedTable->horizontalHeader()->setVisible(false);
    nestedTable->verticalHeader()->setVisible(false);
    nestedTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //nestedTable->adaptWidgetToTable();
    nestedTable->setShowGrid(false);

    savingTable->setCellWidget(0,1, nestedTable);
    savingTable->setColumnWidth(1,300);

    tableHlayout->addWidget(savingTable);
    tableHlayout->setAlignment(savingTable, Qt::AlignLeft);

    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////

    mainVLayout->addLayout(tableHlayout);
    
    centralWidget->setLayout(mainVLayout);

    setCentralWidget(centralWidget);

    


    //////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////

    show();
}