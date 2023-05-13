#pragma once
#include <QtWidgets>

class tableEdit : public QTableWidget
{

    Q_OBJECT

private:
    
    // Code Colors for cells
    inline static QColor disableColor{ QColor(Qt::gray) };
    inline static QColor enableColor{ QColor(Qt::white) };
    inline static QColor errorColor{ QColor(Qt::magenta) };

    // Headers Style:
    inline static QString headerStyle{ 
        "QHeaderView::section{"
        "background-color: qlineargradient(x1 : 0, y1 : 0, x2 : 0, y2 : 1, stop : 0 #616161, stop: 0.5 #505050, stop: 0.6 #434343, stop:1 #656565);"
        "color: white;"
        "padding-left: 4px; "
        "border: 1px solid black;"
        "}"
    };

    // Cell Style:
    inline static QString cellStyle{
        "QTableView {" 
        "gridline-color: black;" 
        "}"
    };


public:

    tableEdit(int rows = 0, int columns = 0) : QTableWidget(rows, columns)
    {

        verticalHeader()->hide();

        //setEditTriggers(QAbstractItemView::NoEditTriggers);
        setSelectionMode(QAbstractItemView::NoSelection);

        // Setting the style of Table (headers and cells)
        horizontalHeader()->setStyleSheet(headerStyle);
        setStyleSheet(cellStyle);

        // Disabling the user resizes columns and headers manually
        horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
        verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    }

    void disableCell(int row, int column, bool disable)
    {

        if (row > rowCount() - 1 || row < 0 || column > columnCount() - 1 || column < 0)
        {
            assert(false && "Cannot dis/enable the cell. Index out of range");
        }

        QTableWidgetItem* item = this->item(row, column);

        // Verify if the cell(row,column) has already a QTableWidgetItem assigned to it
        if (!item) {
            item = new QTableWidgetItem();
            this->setItem(row, column, item);
        }
                        
        if (disable) //Disabling editing and selecting the cell
        {
            item->setFlags(item->flags() & ~Qt::ItemIsEditable & ~Qt::ItemIsSelectable);
            item->setBackground(disableColor);
        }
        else //Enabling editing and selecting the cell
        {
            item->setFlags(item->flags() | Qt::ItemIsEditable | Qt::ItemIsSelectable);
            item->setBackground(enableColor);
        }

    }


    void adaptWidgetToTable()
    {
    //savingTable->resizeColumnsToContents();
    //savingTable->horizontalHeader()->setDefaultSectionSize(100);

    int tableWidth{};

    for (int i{ 0 }; i < columnCount(); ++i)
    {
        tableWidth += columnWidth(i);
    }
    setFixedWidth(tableWidth + 2 * frameWidth());
    
    }

};