#include "tableEdit.h"


tableEdit::tableEdit(int rows, int columns) : QTableWidget(rows, columns)
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

void tableEdit::disableCell(int row, int column, bool disable)
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

void tableEdit::setWrongCell(int row, int column, bool wrong)
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

    QTableWidgetItem* item{ this->item(row, column) };

    if (wrong) //Setting wrong cell
    {
        item->setBackground(errorColor);
    }
    else //Not wrong cell: Setting normal color
    {
        item->setBackground(enableColor);
    }

}

void tableEdit::adaptWidgetToTable(widgetFit wfit)
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

int tableEdit::getTableWidth()
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

int tableEdit::getTableHeight()
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