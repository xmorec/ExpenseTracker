#include "mainWindow.h"
#include "tableEdit.h"


mainWindow::mainWindow()
{

    setWindowTitle("Program Test");

    QObject::connect(quitButton, SIGNAL(clicked()), qApp, SLOT(quit()));

    QObject::connect(whoAmIButton, &QPushButton::clicked, [=]() {
        textEdit->setText("That's Offensive");
        });

    layout->addWidget(textEdit);
    layout->addWidget(whoAmIButton);
    layout->addWidget(quitButton);


    tableEdit* table = new tableEdit(3,2);
    table->disableCell(2, 1, true);
    table->disableCell(2, 1, false);


    ////////////////////////////////////////////////////

    layout->addWidget(table);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);



    //////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////

    show();
}