#pragma once
#include <QtWidgets>


class mainWindow : public QMainWindow
{

private:

    double widthFactor{0.7};
    double heightFactor{0.8};

    QTextEdit* textEdit = new QTextEdit();
    QPushButton* quitButton = new QPushButton("Quit");
    QPushButton* whoAmIButton = new QPushButton("Quien soy?");
    QWidget* centralWidget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout();
    QHBoxLayout* layoutH = new QHBoxLayout();

    
public:
    mainWindow(const QRect& screen);

};

