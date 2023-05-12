#pragma once
#include <QtWidgets>

class tableEdit : public QTableWidget
{
private:
    QColor disableColor{ QColor(Qt::gray) };
    QColor enableColor{ QColor(Qt::white) };
    QColor errorColor{ QColor(Qt::magenta) };

public:

    tableEdit(int rows = 0, int columns = 0) : QTableWidget(rows, columns)
    {

        verticalHeader()->hide();

        //setEditTriggers(QAbstractItemView::NoEditTriggers);
        setSelectionMode(QAbstractItemView::NoSelection);
    }

    void disableCell(int row, int column, bool disable)
    {

        if (row > rowCount() - 1 || row < 0 || column > columnCount() - 1 || column < 0)
        {
            assert(false && "Cannot dis/enable the cell. Index out of range");
        }

        QTableWidgetItem* item = this->item(row, column);

        // Verify if the cell(row,column) has already a QTableWidgetItem assignet to it
        if (!item) {
            item = new QTableWidgetItem();
            this->setItem(row, column, item);
        }

                
        if (disable) //Disabling editing and selecting the cell
        {
            item->setText("disabled");
            item->setFlags(item->flags() & ~Qt::ItemIsEditable & ~Qt::ItemIsSelectable);
            item->setBackground(disableColor);
        }
        else //Enabling editing and selecting the cell
        {
            item->setText("enabled");
            item->setFlags(item->flags() | Qt::ItemIsEditable | Qt::ItemIsSelectable);
            item->setBackground(enableColor);
        }

    //setItem(row, column, item);

    }

};