#pragma once
#include <QtWidgets>
#include "tableEdit.h"
#include <fstream>
#include <iostream>

//////////////////////////////////////////////////////////////////////////////////////////
// Description:                                                                         
// It creates the view of Expenses and Savings Tables, adds the functionality of adding or
// removing Expenses, managing them, and loading values from external sources as well
//////////////////////////////////////////////////////////////////////////////////////////

class savingOverview : public QHBoxLayout
{

private:

    inline static const QString moneyUnit{ "€" };
    bool WRONG_CELL_FLAG{ false };

    ////////// ---------- Expenses Table Parameters --------- //////////////////////////////
    // Horizontal Header Table Titles:
    inline static const QString expenseHeader{ "Expense" };  
    inline static const QString amountHeader{ "Amount" };
    inline static const QString frequencyHeader{ "Frequency" };
    inline static const QString totalAmountHeader{ "Total Amount" };
    
    // Horizontal Header Table Titles (QStringList):
    inline static const QStringList expensesHeaders
    {{
        expenseHeader,
        amountHeader,
        frequencyHeader,
        totalAmountHeader,
        ""
    }};

    tableEdit* expensesTable{ nullptr }; // Expenses Table pointer
    std::vector<QStringList> expenses{}; // Expenses Vector that stores every expense
    std::vector<int> iteratorVect{}; // Useful vector storing iterators (indexs)

    // Button Section
    QPushButton* addExpenseButton{ new QPushButton("Add") };
    QPushButton* saveButton{ new QPushButton("Save") };
    QPushButton* cancelButton{ new QPushButton("Restore") };
    std::vector<QPushButton*> rmvButtonsVect{}; // Vector of "Remove Buttons"

    ////////// ---------- Savings Table Parameters --------- //////////////////////////////
    // Horizontal Header Table Titles:
    inline static const QString incomeHeader{ "Income" };
    inline static const QString totalExpensesHeader{ "Expenses" };
    inline static const QString savingsHeader{ "Savings" };

    // Horizontal Header Table Titles (QStringList):
    inline static const QStringList savingHeaders
    {{
        incomeHeader,
        totalExpensesHeader,
        savingsHeader
    }};

    tableEdit* savingTable{ nullptr }; // Saving Table pointer
    double income{}; // Income value
    double totalExpense{}; // The total amount of ALL expenses
    double savings{}; // The Savings value
    

public:

    // Constructor of the View with two tables "Expenses Table" and "Savings Table"
    savingOverview(); //: QHBoxLayout();

    // Function that generates the Expenses Table
    void fillExpensesTable();

    // Function that generates the Savings Table
    void fillSavingTable();

    // Expenses vector generation from external source
    void generateExpensesFromCSV();
    
    // Income generation from external source
    void generateSavingsFromCSV();

    // Copying an Expense to a Table Row
    void copyExpenseToRow(int rowPos, bool moneyUnitFlag = true);
    
    // Updating Table values from a user-modification of any cell
    void updateTableValues(int row, int col, tableEdit* inputTable);

    // Checking cell format and returning the double cell value according its format checking
    double setCellAccordingToTextFormat(int row, int col, tableEdit* inputTable, bool moneyUnitFlag = false);

    // Total Expense computing from all "Total Amount expenses"
    double getTotalExpense();

    // Action from button: Remove Button
    void removeExpense(int row);

    // Action from button: Add Button
    void addExpense();

    // Action from button: Save Button
    void saveDataToCSV();

    // Action from button: Restore Button
    void restoreTableValues();

};