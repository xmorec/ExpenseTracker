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

    QStringList expensesHeaders
    {{
        "Expense",
        "Amount (€)",
        "Frequency",
        "Total Amount (€)",
        "rmv bttn"
    }};

    QString moneyUnit{ "€" };

    QStringList expense0{{ "Rent", "700", "1", QString::number(700*1) }};
    QStringList expense1{{ "Car Gas", "35.31", "4", QString::number(35.31*4) }};
    QStringList expense2{{ "SuperMarket", "65", "5", QString::number(65*5) }};
    std::vector<QStringList> expenses{};

    expenses.push_back(expense0);
    expenses.push_back(expense1);
    expenses.push_back(expense2);

    tableEdit* expensesTable = new tableEdit(expenses.size(), expensesHeaders.length());

    expensesTable->setHorizontalHeaderLabels(expensesHeaders);

    for (int row{0}; row < expenses.size(); ++row)
    {
        for (int col{ 0 }; col < expenses[row].length(); ++col)
        {
            QTableWidgetItem* item{ new QTableWidgetItem(expenses[row][col])};
            expensesTable->setItem(row, col, item);
        }
    }

    std::vector<QPushButton*> rmvButtonsVect(expenses.size());

    for (int row{ 0 }; row < expenses.size(); ++row)
    {
        rmvButtonsVect[row] = new QPushButton("Remove");
        expensesTable->setCellWidget(row, expensesHeaders.length()-1, rmvButtonsVect[row]);
    }


    expensesTable->adaptWidgetToTable();
    tableHlayout->addWidget(expensesTable);
    tableHlayout->setAlignment(expensesTable, Qt::AlignRight);


    ///// OVERVIEW TABLE SAVING
    ///////////////////////////////////////
    QStringList savingParametersHeaders{
        {
            "Salary",
            "Despesa Fixa",
            "Despesa Extra",
            "Ahorro"
        }
    };

    tableEdit* savingTable = new tableEdit(1, savingParametersHeaders.length());

    savingTable->setHorizontalHeaderLabels(savingParametersHeaders);

    //savingTable->resizeColumnsToContents();
    //savingTable->horizontalHeader()->setDefaultSectionSize(100);
    
    savingTable->adaptWidgetToTable();
    

    //// BUTTON TEST INSIDE TABLE

    //QTableWidget* savingTableX = new QTableWidget(2, 2);
    savingTable->setCellWidget(0, 0, quitButton);
    
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