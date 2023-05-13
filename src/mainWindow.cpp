#include "mainWindow.h"
#include "tableEdit.h"


mainWindow::mainWindow(const QRect& screen)
{

    setWindowTitle("Expense Tracker");

    QObject::connect(quitButton, SIGNAL(clicked()), qApp, SLOT(quit()));

    QObject::connect(whoAmIButton, &QPushButton::clicked, [=]() {
        textEdit->setText("That's Offensive");
        });

    layout->addWidget(textEdit);
    layout->addWidget(whoAmIButton);
    layout->addWidget(quitButton);

    // Sizing the Main Window
    int width = screen.width() * widthFactor;
    int height = screen.height() * heightFactor;
    resize(width, height);


    /////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////-------------///////////////////////////////////////////////////
    ////////// ---------- EXPENSE/SAVING OVERVIEW  ---------/////////////////////////////////
    /////////////////////////-------------///////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////


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
    
    
    layoutH->addWidget(savingTable);
    layoutH->setAlignment(savingTable, Qt::AlignRight);

    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////

    tableEdit* savingTable2 = new tableEdit(4, 5);
    savingTable2->adaptWidgetToTable();
    layoutH->addWidget(savingTable2);
    layoutH->setAlignment(savingTable2, Qt::AlignLeft);
    layout->addLayout(layoutH);

    
    centralWidget->setLayout(layout);

    setCentralWidget(centralWidget);



    //////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////

    show();
}