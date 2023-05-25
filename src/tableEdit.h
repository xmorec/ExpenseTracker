#ifndef TABLEEDIT_H
#define TABLEEDIT_H

#include <QTableWidget>
#include <QHeaderView>
#include <QString>

//////////////////////////////////////////////////////////////////////////////////////////
// Description:                                                                         
// It creates a custom table with own style to create Tables in the Expense Tracker.
// It also has the functionality to enable/disable cells with specific parameters. 
//////////////////////////////////////////////////////////////////////////////////////////

class tableEdit : public QTableWidget
{

    Q_OBJECT

private:
    
    // Code Colors for cells
    inline static const QColor disableColor{ QColor(Qt::gray) };
    inline static const QColor enableColor{ QColor(Qt::white) };
    inline static const QColor errorColor{ "#EA9791"};

    // Headers Style:
    inline static const QString headerStyle{ 
        "QHeaderView::section{"
        "background-color: qlineargradient(x1 : 0, y1 : 0, x2 : 0, y2 : 1, stop : 0 #616161, stop: 0.5 #505050, stop: 0.6 #434343, stop:1 #656565);"
        "color: white;"
        "padding-left: 4px; "
        "border: 1px solid black;"
        "}"
    };

    // Cell Style:
    inline static const QString cellStyle{
        "QTableView {" 
        "gridline-color: black;" 
        "}"
    };

    int tableWidth{};
    int tableHeight{};

public:
    
    //enum values to set the fitting type of the widget to the table (width, height, whole: both height and width)
    const enum widgetFit {
        WidthFit,
        HeightFit,
        WholeFit
    };

public:

    // Creates the QTableWidget with own customization
    tableEdit(int rows = 0, int columns = 0);

    // Disable cell with own customizaition
    void disableCell(int row, int column, bool disable = true);

    // Set a style for a cell whose content is assumed as wrong
    void setWrongCell(int row, int column, bool wrong = true);

    // Adapts the Widget to the table (width-height option)
    void adaptWidgetToTable(widgetFit wfit = widgetFit::WholeFit);

    // Returns the whole Table Width
    int getTableWidth();

    // Returns the whole Table Height
    int getTableHeight();

};

#endif
