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
    std::vector<int> iteratorVect{};
    int expensesNumber{};

    QPushButton* addExpenseButton{ new QPushButton("Add") };
    QPushButton* saveButton{ new QPushButton("Save") };
    QPushButton* cancelButton{ new QPushButton("Cancel") };

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

        //The Expenses Table is initializated, filled with the user values and Remove Buttons and also made updateable
        fillExpensesTable();

        // Adding an Expense
        QObject::connect(addExpenseButton, &QPushButton::clicked, [=]() {
            addExpense();
            });

        // TBD
        QObject::connect(saveButton, &QPushButton::clicked, [=]() {
            // TBD
            });

        // TBD
        QObject::connect(cancelButton, &QPushButton::clicked, [=]() {
            // TBD
            });
        
        ///// Element visualization and organization in the layout
    
        expensesTable->adaptWidgetToTable();

        QVBoxLayout* vlay { new QVBoxLayout() };
        QHBoxLayout* hlay{ new QHBoxLayout() };

        vlay->addWidget(expensesTable);
        vlay->setAlignment(expensesTable, Qt::AlignRight);

        hlay->addWidget(addExpenseButton);
        hlay->addWidget(saveButton);
        hlay->addWidget(cancelButton);
        int spaceRight{ expensesTable->getTableWidth() - (cancelButton->sizeHint().width() + saveButton->sizeHint().width() + cancelButton->sizeHint().width()) - 11 };
        QSpacerItem* spacerRight = new QSpacerItem(spaceRight, 0);
        hlay->addItem(spacerRight);
        
        vlay->addLayout(hlay);   
        vlay->setAlignment(hlay, Qt::AlignRight);
        addLayout(vlay);
        

        ///////////////////////////////////////////////////////////////////////////////////////
        ////////// ---------- Overview Saving Table  ---------/////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////

        fillSavingTable();

        ///// Element visualization and organization in the layout

        savingTable->adaptWidgetToTable();      
        addWidget(savingTable);
        setAlignment(savingTable, Qt::AlignLeft);

        /////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////
	}

    void addExpense()
    {
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

        expensesNumber = static_cast<int>(expenses.size()) ;
        expensesTable = new tableEdit(expensesNumber, expensesHeaders.length());

        expensesTable->setHorizontalHeaderLabels(expensesHeaders);

        // Adding the expenses values to the table
        for (int row{ 0 }; row < expensesNumber; ++row)
        {
            for (int col{ 0 }; col < expenses[row].length(); ++col)
            {
                QTableWidgetItem* item{ new QTableWidgetItem(expenses[row][col]) };
                
                // The column "Total Amount" should not be editable.
                if (expensesHeaders[col] == totalAmountHeader) {
                    item->setFlags(item->flags() & ~Qt::ItemIsEditable & ~Qt::ItemIsSelectable);
                }
                expensesTable->setItem(row, col, item);
            }
        }

        // Making the table updatable if any cell is modified
        QObject::connect(expensesTable, &QTableWidget::cellChanged, [=](int row, int col) {
            updateTableValues(row, col);
            });

        // The initialization of 'iteratorVect' is useful to get a proper removing of the expenses when a "Remove Button" is clicked on.
        iteratorVect.resize(expensesNumber);
        std::iota(iteratorVect.begin(), iteratorVect.end(), 0);

        // Adding remove buttons to the table
        std::vector<QPushButton*> rmvButtonsVect(expensesNumber);
        for (int row{ 0 }; row < expenses.size(); ++row)
        {
            rmvButtonsVect[row] = new QPushButton("Remove");
            expensesTable->setCellWidget(row, expensesHeaders.length() - 1, rmvButtonsVect[row]);
            QObject::connect(rmvButtonsVect[row], &QPushButton::clicked, [=]() {
                removeExpense(row);
                });
        }

    }

    void updateTableValues(int row, int col)
    {
        // Updating the modified cell in the expenses vector
        auto editedItem{ expensesTable->item(row, col) };
        expenses[row][col] = editedItem->text();

        //If the modified cell was one regarding Expense column
        if (expensesHeaders[col] == expenseHeader)
        {
            // No action needed
        }

        //If the modified cell was one regarding Amount column
        if(expensesHeaders[col] == amountHeader)
        {
            // Getting the frequency item
            auto colFrequency{ expensesHeaders.indexOf(frequencyHeader) };
            auto freqItem{expensesTable->item(row, colFrequency)};
            
            // Getting the new double Total Amount value (amount * frequency)
            double newAmountDouble { editedItem->text().toDouble() };
            double newTotalAmountDouble { newAmountDouble * freqItem->text().toDouble() };            
            
            // Setting the new double Total Amount value to the Table:
            auto colTotalAmount{ expensesHeaders.indexOf(totalAmountHeader) };
            expensesTable->item(row, colTotalAmount)->setText(QString::number(newTotalAmountDouble));

            // Updating the expenses vector with the new Total Amount value
            expenses[row][colTotalAmount] = QString::number(newTotalAmountDouble);

            // Getting the total amount of all Expenses and showing it in the Table
            totalExpense = getTotalExpense();
            auto indexTotalExpenses{ savingHeaders.indexOf(totalExpensesHeader) };
            savingTable->item(0, indexTotalExpenses)->setText(QString::number(totalExpense));

            // Setting the 'Savings' value to the table
            auto indexSavings{ savingHeaders.indexOf(savingsHeader) };
            savings = income - totalExpense;
            savingTable->item(0, indexSavings)->setText(QString::number(savings));
        }

        //If the modified cell was one regarding from Frequency
        if (expensesHeaders[col] == frequencyHeader)
        {
            //Getting the Amount item
            auto colAmount{ expensesHeaders.indexOf(amountHeader) };
            auto amountItem{ expensesTable->item(row, colAmount) };

            // Getting the new double Total Amount value (amount * frequency)
            double newFrequencyDouble{ editedItem->text().toDouble() };
            double newTotalAmountDouble{ newFrequencyDouble * amountItem->text().toDouble() };

            // Setting the new double Total Amount value to the Table:
            auto colTotalAmount{ expensesHeaders.indexOf(totalAmountHeader) };
            expensesTable->item(row, colTotalAmount)->setText(QString::number(newTotalAmountDouble));

            // Updating the expenses vector with the new Total Amount value
            expenses[row][colTotalAmount] = QString::number(newTotalAmountDouble);

            // Getting the total amount of all Expenses and showing it in the Table
            totalExpense = getTotalExpense();
            auto indexTotalExpenses{ savingHeaders.indexOf(totalExpensesHeader) };
            savingTable->item(0, indexTotalExpenses)->setText(QString::number(totalExpense));

            // Setting the 'Savings' value to the table
            auto indexSavings{ savingHeaders.indexOf(savingsHeader) };
            savings = income - totalExpense;
            savingTable->item(0, indexSavings)->setText(QString::number(savings));

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

        // Setting the 'Expenses' value to the table and making it not editable or selectable by the user
        totalExpense = getTotalExpense();        
        auto indexTotalExpenses{ savingHeaders.indexOf(totalExpensesHeader) };
        auto totalExpenseItem = new QTableWidgetItem(QString::number(totalExpense));
        totalExpenseItem->setFlags(totalExpenseItem->flags() & ~Qt::ItemIsEditable & ~Qt::ItemIsSelectable);
        savingTable->setItem(0, indexTotalExpenses, totalExpenseItem);

        // Setting the 'Savings' value to the table
        auto indexSavings{ savingHeaders.indexOf(savingsHeader) };
        savings = income - totalExpense;
        auto savingsItem = new QTableWidgetItem(QString::number(savings));
        savingsItem->setFlags(savingsItem->flags() & ~Qt::ItemIsEditable & ~Qt::ItemIsSelectable);
        savingTable->setItem(0, indexSavings, savingsItem);

    }

    void removeExpense(int row)
    {
        // Detecting the position of the row to be delated from the relative position 'row'
        auto rmvPosIt{ std::find(iteratorVect.begin(), iteratorVect.end(), row) };
        int rmvPos{ static_cast<int>(std::distance(iteratorVect.begin(), rmvPosIt)) };

        // Computing the total amount of the expenses after removing one expense
        auto indexTotalAmount{ expensesHeaders.indexOf(totalAmountHeader) };
        totalExpense -= expenses[rmvPos][indexTotalAmount].toDouble();

        // Setting the 'Expenses' value to the table
        auto indexTotalExpenses{ savingHeaders.indexOf(totalExpensesHeader) };
        savingTable->setItem(0, indexTotalExpenses, new QTableWidgetItem(QString::number(totalExpense)));

        // Setting the 'Savings' value to the table
        auto indexSavings{ savingHeaders.indexOf(savingsHeader) };
        savings = income - totalExpense;
        savingTable->setItem(0, indexSavings, new QTableWidgetItem(QString::number(savings)));

        // Removing the expense from vector and from the table
        expensesTable->removeRow(rmvPos);
        expenses.erase(expenses.begin() + rmvPos);

        // Removing the "remove position" from the 'iteratorVect' (this is useful to get a proper row removing)
        iteratorVect.erase(iteratorVect.begin() + rmvPos);

        // Updating the widget fitting
        expensesTable->adaptWidgetToTable();
    }

    double getTotalExpense()
    {
        // Computing the total amount of the expenses
        auto indexTotalAmount{ expensesHeaders.indexOf(totalAmountHeader) };
        totalExpense = 0;
        for (const auto& expense : expenses)
        {
            totalExpense += expense[indexTotalAmount].toDouble();
        }

        return totalExpense;

    }

};