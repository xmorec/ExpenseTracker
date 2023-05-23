#pragma once
#include <QtWidgets>

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

    tableEdit(int rows = 0, int columns = 0) : QTableWidget(rows, columns)
    {
        // Vertical Headers are hidden
        verticalHeader()->hide();

        //setEditTriggers(QAbstractItemView::DoubleClicked);
        setSelectionMode(QAbstractItemView::NoSelection);

        // Setting the style of Table (headers and cells)
        horizontalHeader()->setStyleSheet(headerStyle);
        setStyleSheet(cellStyle);

        // Disabling the user resizing columns and headers manually
        horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
        verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    }

    void disableCell(int row, int column, bool disable = true)
    {

        // Assert code to assure no index is out of range
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


    void setWrongCell(int row, int column, bool wrong = true)
    {

        // Assert code to assure no index is out of range
        if (row > rowCount() - 1 || row < 0 || column > columnCount() - 1 || column < 0)
        {
            assert(false && "Cannot change the cell status. Index out of range");
        }

        // Verify if the cell(row,column) has already a QTableWidgetItem assigned to it
        if (!this->item(row, column)) {
            this->setItem(row, column, new QTableWidgetItem());
        }
        
        QTableWidgetItem* item{ this->item(row, column)};

        if (wrong) //Setting wrong cell
        {
            item->setBackground(errorColor);
        }
        else //Not wrong cell: Setting normal color
        {
            item->setBackground(enableColor);
        }

    }

    void adaptWidgetToTable(widgetFit wfit = widgetFit::WholeFit)
    {
        // Adapt widget to table from width
        if (wfit == widgetFit::WholeFit || wfit == widgetFit::WidthFit)
        {
            int tableWidth{ verticalHeader()->width() };

            // Set the widget width to the table width plus table border width
            setFixedWidth(getTableWidth());

            // Disable the horizontal scroll bar
            setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        }

        // Adapt widget to table from height
        if (wfit == widgetFit::WholeFit || wfit == widgetFit::HeightFit)
        {
            
            // Set the widget height to the table height plus table border width
            setFixedHeight(getTableHeight());

            // Disable the vertical scroll bar
            setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        }
    }

    int getTableWidth()
    {
        // Computing the vertical header width
        tableWidth = verticalHeader()->width();

        // Computing table width (without borders)
        for (int i{ 0 }; i < columnCount(); ++i)
        {
            tableWidth += columnWidth(i);
        }

        // Add table borders width
        tableWidth += 2 * frameWidth();

        return tableWidth;
    }

    int getTableHeight()
    {
        // Computing the horizontal header height
        tableHeight = horizontalHeader()->height();

        // Adding table rows height
        for (int i{ 0 }; i < rowCount(); ++i)
        {
            tableHeight += rowHeight(i);
        }

        // Adding table borders 
        tableHeight += 2 * frameWidth();

        return tableHeight;
    }

};

