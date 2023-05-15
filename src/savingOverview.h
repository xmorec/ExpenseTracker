#pragma once
#include <QtWidgets>
#include "tableEdit.h"

class savingOverview : public QHBoxLayout
{

private:

    inline static const QString moneyUnit{ "€" };

    ////////// ---------- Expenses Table Parameters ---------////////////////////////////////

    inline static const QString expenseHeader{ "Expense" };  
    inline static const QString amountHeader{ "Amount" };
    inline static const QString frequencyHeader{ "Frequency" };
    inline static const QString totalAmountHeader{ "Total Amount" };
 
    inline static const QStringList expensesHeaders
    {{
        expenseHeader,
        amountHeader,
        frequencyHeader,
        totalAmountHeader,
        ""
    }};

    tableEdit* expensesTable{};
    std::vector<QStringList> expenses{};

    ////////// ---------- Savings Table Parameters ---------////////////////////////////////
    inline static const QString incomeHeader{ "Income" };
    inline static const QString totalExpensesHeader{ "Expenses" };
    inline static const QString savingsHeader{ "Savings" };

    inline static const QStringList savingHeaders
    {{
        incomeHeader,
        totalExpensesHeader,
        savingsHeader
    }};

    tableEdit* savingTable{};
    double income{};
    double totalExpense{};
    double savings{};
    

public:

	savingOverview() : QHBoxLayout()
	{
        ////////////////////////////////////////////////////////////////////////////////////////
        ////////// ---------- Overview Expenses Table  ---------////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////

        fillExpensesTable();

        expensesTable->adaptWidgetToTable();
        addWidget(expensesTable);
        setAlignment(expensesTable, Qt::AlignRight);


        ///////////////////////////////////////////////////////////////////////////////////////
        ////////// ---------- Overview Saving Table  ---------/////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////


        fillSavingTable();

        savingTable->adaptWidgetToTable();      
        addWidget(savingTable);
        setAlignment(savingTable, Qt::AlignLeft);
	}

    void fillExpensesTable()
    {

        ////////////////// EXTRACTING INFORMATION FOR FILLING EXPENSES TABLE ///////////////////////

        QStringList expense0{ { "Rent", "700", "1", QString::number(700 * 1) } };
        QStringList expense1{ { "Car Gas", "35.31", "4", QString::number(35.31 * 4) } };
        QStringList expense2{ { "SuperMarket", "65", "5", QString::number(65 * 5) } };


        expenses.push_back(expense0);
        expenses.push_back(expense1);
        expenses.push_back(expense2);

        //////////////////////////////////////////////////////////////////////////////////////////////


        expensesTable = new tableEdit(expenses.size(), expensesHeaders.length());

        expensesTable->setHorizontalHeaderLabels(expensesHeaders);

        // Adding the expenses values to the table
        for (int row{ 0 }; row < expenses.size(); ++row)
        {
            for (int col{ 0 }; col < expenses[row].length(); ++col)
            {
                QTableWidgetItem* item{ new QTableWidgetItem(expenses[row][col]) };
                expensesTable->setItem(row, col, item);
            }
        }

        std::vector<QPushButton*> rmvButtonsVect(expenses.size());

        // Adding remove buttons to the table
        for (int row{ 0 }; row < expenses.size(); ++row)
        {
            rmvButtonsVect[row] = new QPushButton("Remove");
            expensesTable->setCellWidget(row, expensesHeaders.length() - 1, rmvButtonsVect[row]);
            QObject::connect(rmvButtonsVect[row], &QPushButton::clicked, [=]() {
                removeExpense(row);
                });
        }

    }

    void fillSavingTable()
    {
        //savingTable->resizeColumnsToContents();
        //savingTable->horizontalHeader()->setDefaultSectionSize(100);

        savingTable = new tableEdit(1, savingHeaders.length());

        savingTable->setHorizontalHeaderLabels(savingHeaders);

        ////////////////// EXTRACTING INFORMATION FOR FILLING SAVING TABLE ///////////////////////

        income = 1900;

        //////////////////////////////////////////////////////////////////////////////////////////////

        // Setting the 'Income' value to the table
        auto indexIncome{ savingHeaders.indexOf(incomeHeader) };
        savingTable->setItem(0, indexIncome, new QTableWidgetItem(QString::number(income)));

        // Computing the total amount of the expenses
        auto indexTotalAmount{ expensesHeaders.indexOf(totalAmountHeader) };
        totalExpense = 0;
        for (const auto& expense : expenses) {
            totalExpense += expense[indexTotalAmount].toDouble();
        }

        // Setting the 'Expenses' value to the table
        auto indexTotalExpenses{ savingHeaders.indexOf(totalExpensesHeader) };
        savingTable->setItem(0, indexTotalExpenses, new QTableWidgetItem(QString::number(totalExpense)));

        // Setting the 'Savings' value to the table
        auto indexSavings{ savingHeaders.indexOf(savingsHeader) };
        savings = income - totalExpense;
        savingTable->setItem(0, indexSavings, new QTableWidgetItem(QString::number(savings)));

    }

    void removeExpense(int row)
    {
        // Computing the total amount of the expenses after removing one expense
        auto indexTotalAmount{ expensesHeaders.indexOf(totalAmountHeader) };
        totalExpense -= expenses[row][indexTotalAmount].toDouble();

        // Setting the 'Expenses' value to the table
        auto indexTotalExpenses{ savingHeaders.indexOf(totalExpensesHeader) };
        savingTable->setItem(0, indexTotalExpenses, new QTableWidgetItem(QString::number(totalExpense)));

        // Setting the 'Savings' value to the table
        auto indexSavings{ savingHeaders.indexOf(savingsHeader) };
        savings = income - totalExpense;
        savingTable->setItem(0, indexSavings, new QTableWidgetItem(QString::number(savings)));

        // Removing the expense from vector and from the table
        expensesTable->removeRow(row);
        expenses.erase(expenses.begin() + row);
        
        // Updating the widget fiting
        expensesTable->adaptWidgetToTable();
    }

};