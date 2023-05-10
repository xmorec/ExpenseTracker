#include <QtWidgets>
#include "mainWindow.h"



int main(int argc, char* argv[])
{


QApplication app(argc, argv);


    mainWindow* window = new mainWindow();

     
return app.exec();

// Tret de la windows, els demes QWidgets que siguin punters (new QWidget)
// Usar el QVBoxLayout i QHBoxLayout per organitzar els widgets dins la windows (+ fer us del "addWidget()"). Si es vol afegir un Layout dins d'un Layout, usar "addLayout()"
// A tots els Widgets que els hi passis un parent (p.ex.: un button el qual el seu pare es una finestra), tots els fills del parent (finestra) seran eliminats quan el parent sigui eliminat.
// A l'usar senyals en Buttons (o altres QWidgets), fer ús de Lambda Functions, tal com: QObject::connect(..., ..., LAMBDA FUNCTION)

}