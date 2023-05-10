#include "mainWindow.h"


mainWindow::mainWindow()
{

    setWindowTitle("Expense Tracker");

    QObject::connect(quitButton, SIGNAL(clicked()), qApp, SLOT(quit()));

    QObject::connect(whoAmIButton, &QPushButton::clicked, [=]() {
        textEdit->setText("That's offensive");
        });

    layout->addWidget(textEdit);
    layout->addWidget(whoAmIButton);
    layout->addWidget(quitButton);

    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    show();

}