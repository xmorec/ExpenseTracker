#ifndef SAVINGOVERVIEW_H
#define SAVINGOVERVIEW_H

#include <QHBoxLayout>
#include <QString>
#include <QMessageBox>
#include <QPushButton>
#include <QTimer>
#include <fstream>
#include "constants.h"
#include "sql_functions.h"
#include "User.h"
#include "tableEdit.h"


//////////////////////////////////////////////////////////////////////////////////////////
// Description:                                                                         
// It creates the view of Expenses and Savings Tables, adds the functionality of adding or
// removing Expenses, managing them, and loading values from external sources as well
//////////////////////////////////////////////////////////////////////////////////////////

class savingOverview : public QHBoxLayout
{

    Q_OBJECT

private:

    // Logged User
    User* currentUser{nullptr};
    
    inline static const QString moneyUnit{ "€" };
    bool wrongCellFlag{ false };

    // Informing QMessageBox
    QMessageBox* userInfoBox = new QMessageBox();

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
    double income{-1}; // Income value
    double totalExpense{-1}; // The total amount of ALL expenses
    double savings{-1}; // The Savings value
    

public:

    // Constructor of the View with two tables "Expenses Table" and "Savings Table"
    savingOverview(User* loggedUser); //: QHBoxLayout();

    // Function that generates the Expenses Table
    void fillExpensesTable();

    // Function that generates the Savings Table
    void fillSavingTable();

    // Expenses vector generation from external source
    void generateExpensesFromCSV();
    
    // Income generation from external source
    void generateSavingsFromCSV();

    // Update the expenses vector and Income with the content of SQLite Database
    void getExpensesAndIncomeFromDB();

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

    // Save Expenses to a external file
    void saveDataToCSV();

    // Save Expenses to a SQLite Database
    void saveDataToDB();

    // Action from button: Restore Button
    void restoreTableValues();


    signals:
        // Emited signal when table changes its size from removing or restoring expenses
        void tableSizeMod(); 

};

#endif