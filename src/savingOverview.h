#pragma once
#include <QtWidgets>
#include "tableEdit.h"

class savingOverview : public QHBoxLayout
{

private:
    inline static const QString moneyUnit{ "€" };
    tableEdit* expensesTable{};

public:

	savingOverview() : QHBoxLayout()
	{
        /////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////-------------///////////////////////////////////////////////////
        ////////// ---------- Overview Expenses Table  ---------/////////////////////////////////
        /////////////////////////-------------///////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////

        QStringList expensesHeaders
        { {
            "Expense",
            "Amount (€)",
            "Frequency",
            "Total Amount (€)",
            "rmv bttn"
        } };

    

        QStringList expense0{ { "Rent", "700", "1", QString::number(700 * 1) } };
        QStringList expense1{ { "Car Gas", "35.31", "4", QString::number(35.31 * 4) } };
        QStringList expense2{ { "SuperMarket", "65", "5", QString::number(65 * 5) } };
        std::vector<QStringList> expenses{};

        expenses.push_back(expense0);
        expenses.push_back(expense1);
        expenses.push_back(expense2);

        expensesTable = new tableEdit(expenses.size(), expensesHeaders.length());

        expensesTable->setHorizontalHeaderLabels(expensesHeaders);

        for (int row{ 0 }; row < expenses.size(); ++row)
        {
            for (int col{ 0 }; col < expenses[row].length(); ++col)
            {
                QTableWidgetItem* item{ new QTableWidgetItem(expenses[row][col]) };
                expensesTable->setItem(row, col, item);
            }
        }

        std::vector<QPushButton*> rmvButtonsVect(expenses.size());

        for (int row{ 0 }; row < expenses.size(); ++row)
        {
            rmvButtonsVect[row] = new QPushButton("Remove");
            expensesTable->setCellWidget(row, expensesHeaders.length() - 1, rmvButtonsVect[row]);
            QObject::connect(rmvButtonsVect[row], &QPushButton::clicked, [=]() {
                removeExpense(row);
                });
        }


        expensesTable->adaptWidgetToTable();
        addWidget(expensesTable);
        setAlignment(expensesTable, Qt::AlignRight);


        /////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////-------------///////////////////////////////////////////////////
        ////////// ---------- Overview Saving Table  ---------/////////////////////////////////
        /////////////////////////-------------///////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////
        QStringList savingParametersHeaders{
            {
                "Salary",
                "Despesa Fixa",
                "Despesa Extra",
                "Ahorro"
            }
        };

        tableEdit* savingTable = new tableEdit(1, savingParametersHeaders.length());

        savingTable->setHorizontalHeaderLabels(savingParametersHeaders);

        //savingTable->resizeColumnsToContents();
        //savingTable->horizontalHeader()->setDefaultSectionSize(100);

        savingTable->adaptWidgetToTable();

      
        addWidget(savingTable);
        setAlignment(savingTable, Qt::AlignLeft);
	}

    void removeExpense(int row) 
    {
        expensesTable->disableCell(row, 0, true);
    }

};