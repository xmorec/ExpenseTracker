#pragma once
#include <QtWidgets>


class mainWindow : public QMainWindow
{

private:

    QTextEdit* textEdit = new QTextEdit();
    QPushButton* quitButton = new QPushButton("Quit");
    QPushButton* whoAmIButton = new QPushButton("Quien soy?");
    QWidget* centralWidget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout();

    
public:
    mainWindow();

};

