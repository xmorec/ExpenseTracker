#pragma once
#include <QtWidgets>
#include "tableEdit.h"
#include <fstream>
#include <iostream>

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

    QPushButton* addExpenseButton{ new QPushButton("Add") };
    QPushButton* saveButton{ new QPushButton("Save") };
    QPushButton* cancelButton{ new QPushButton("Cancel") };
    std::vector<QPushButton*> rmvButtonsVect{};

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

        // Save the values of Expense Table and the Income value to an external source
        QObject::connect(saveButton, &QPushButton::clicked, [=]() {
            saveDataToCSV();
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

    void fillExpensesTable()
    {
        // Update the expenses vector with the content of Database or a extern file
        generateExpensesFromCSV();

        // Create the Expenses Table filled with the values of expenses vector (content from external source)
        int expensesNumber = static_cast<int>(expenses.size()) ;
        expensesTable = new tableEdit(expensesNumber, expensesHeaders.length());

        // Setting the header names of this Table
        expensesTable->setHorizontalHeaderLabels(expensesHeaders);

        // Adding the expenses values to the table
        for (int row{ 0 }; row < expensesNumber; ++row)
        {
            //copying the expense at position 'row' of expenses vector to the table at row 'row'
            copyExpenseToRow(row);
        }

        // Making the table updatable if any cell is modified        
        QObject::connect(expensesTable, &QTableWidget::cellChanged, [=](int row, int col) {
            updateTableValues(row, col, expensesTable);
            });        

        // The initialization of 'iteratorVect' is useful to get a proper removing of the expenses when a "Remove Button" is clicked on.
        iteratorVect.reserve(expensesNumber + 25);
        iteratorVect.resize(expensesNumber);
        std::iota(iteratorVect.begin(), iteratorVect.end(), 0);

        // Adding remove buttons to the table and also its signal behavior
        rmvButtonsVect.reserve(expensesNumber + 25);
        rmvButtonsVect.resize(expensesNumber);
        for (int row{ 0 }; row < expenses.size(); ++row)
        {
            rmvButtonsVect[row] = new QPushButton("Remove");
            expensesTable->setCellWidget(row, expensesHeaders.length() - 1, rmvButtonsVect[row]);
            QObject::connect(rmvButtonsVect[row], &QPushButton::clicked, [=]() {
                removeExpense(row);
                });
        }

    }

    void generateExpensesFromCSV()
    {
        //This function sets the expenses vector with the content of Database or a CSV file

        // CSV File Path is Opened for being read
        static constexpr char expensesFile[]{ "resources/expenses_files/users/demo/expenses_demo.csv" };
        std::fstream expensesFileStream{ expensesFile };        

        if (!expensesFileStream)
        {
            qDebug() << "The file " << expensesFile << " could not be opened\n";
            return;
        }

        // expense vector for storing the CSV data is created:
        std::vector<QStringList> expensesCSV;
        expensesCSV.reserve(25);

        // line by line of CSV is read to store its content in the expensesCSV vector
        std::string line{};
        while (std::getline(expensesFileStream, line))
        {
            QString qline = QString::fromStdString(line);
            qDebug() << qline.split(";") << "\n";
            expensesCSV.push_back(qline.split(";"));
        }

        // closing CSV file
        expensesFileStream.close();
        
        // The positions of the headers of the "Expenses Table" are detected in the CSV
        int expensePos{ static_cast<int>(expensesCSV[0].indexOf(expenseHeader)) };
        int amountPos{ static_cast<int>(expensesCSV[0].indexOf(amountHeader)) };
        int freqPos{ static_cast<int>(expensesCSV[0].indexOf(frequencyHeader)) };
        
        // Memory reservation for the expenses vector
        expenses.reserve(25);
        
        // 'importExpense' is set with every row of 'expensesCSV' whose parameters are sorted according the headers of "Expenses Table".
        QStringList importExpense{};
        importExpense.resize(expensesCSV[0].length() + 1);

        for (int row {1}; row < expensesCSV.size(); ++row)
        {
            for (int col{ 0 }; col < expensesCSV[0].length() + 1 ; ++col)
            {
                if (col == expensesHeaders.indexOf(expenseHeader))
                {
                    importExpense[col] = expensesCSV[row][expensePos];
                }
                if (col == expensesHeaders.indexOf(amountHeader))
                {
                    importExpense[col] = expensesCSV[row][amountPos];
                }
                if (col == expensesHeaders.indexOf(frequencyHeader))
                {
                    importExpense[col] = expensesCSV[row][freqPos];
                }
                if (col == expensesHeaders.indexOf(totalAmountHeader))
                {   
                    double totalAmount = expensesCSV[row][amountPos].toDouble() * expensesCSV[row][freqPos].toDouble();
                    importExpense[col] = QString::number(totalAmount);                    
                } 
            }
            // the sorted row of CSV (expense from CSV) is inserted into 'expenses' vector. 'expenses' vector will be shown in the "Expenses Table".
            expenses.push_back(importExpense);
            qDebug() << expenses[row-1] << "\n";
        }
    }
    
    void saveDataToCSV()
    {
        //This function sets the expenses vector with the content of Database or a CSV file

        // This msgBox will inform the saving status
        QMessageBox* msgBox = new QMessageBox();
        msgBox->setWindowTitle("Saving Expenses");

        // CSV Expenses File Path is Opened for being written
        static constexpr char expensesFile[]{ "resources/expenses_files/users/demo/expenses_demo.csv" };        
        std::fstream expensesFileStream{ expensesFile };

        if (!expensesFileStream)
        {
            msgBox->setText("ERROR: Your expenses management cold not be saved!");
            msgBox->setIcon(QMessageBox::Warning);
            msgBox->exec();
            qDebug() << "The file " << expensesFile << " could not be opened\n";
            return;
        }
        
        static constexpr char expensesFileTemp[]{ "resources/expenses_files/users/demo/expenses_demoTemp.csv" };
        std::ofstream tempExpensesFileStream(expensesFileTemp);

        if (!tempExpensesFileStream)
        {
            msgBox->setText("ERROR: Your expenses management cold not be saved!");
            msgBox->setIcon(QMessageBox::Warning);
            msgBox->exec();
            qDebug() << "The file " << expensesFileTemp << "could not be opened\n";
            return;
        }

        // Exporting the Expense table to the CSV File
        QStringList exportHeadersExpenses{ expensesHeaders };
        exportHeadersExpenses.removeAt(expensesHeaders.indexOf(""));
        exportHeadersExpenses.removeAt(expensesHeaders.indexOf(totalAmountHeader));
        tempExpensesFileStream << exportHeadersExpenses.join(";").toStdString() << '\n';
        
        for (auto expense : expenses)
        {
            expense.removeAt(expensesHeaders.indexOf(totalAmountHeader));
            tempExpensesFileStream << expense.join(";").toStdString() << '\n';
        }        
                
        expensesFileStream.close();
        tempExpensesFileStream.close();

        // Overwrite the Expenses File for the current user
        std::remove(expensesFile);
        std::rename(expensesFileTemp, expensesFile);


        // CSV Savings File Path is Opened for being written
        static constexpr char savingsFile[]{ "resources/expenses_files/users/demo/savings_demo.csv" };
        std::fstream savingsFileStream{ savingsFile };

        if (!savingsFileStream)
        {
            msgBox->setText("ERROR: Your expenses management cold not be saved!");
            msgBox->setIcon(QMessageBox::Warning);
            msgBox->exec();
            qDebug() << "The file " << savingsFile << " could not be opened\n";
            return;
        }

        static constexpr char savingsFileTemp[]{ "resources/expenses_files/users/demo/savings_demoTemp.csv" };
        std::ofstream tempSavingsFileStream(savingsFileTemp);

        if (!tempSavingsFileStream)
        {
            msgBox->setText("ERROR: Your expenses management cold not be saved!");
            msgBox->setIcon(QMessageBox::Warning);
            msgBox->exec();
            qDebug() << "The file " << savingsFileTemp << "could not be opened\n";
            return;
        }

        // Exporting the Expense table to the CSV File
        QStringList exportSavingHeaders{ savingHeaders };
        tempSavingsFileStream << savingHeaders[savingHeaders.indexOf(incomeHeader)].toStdString() << '\n';
        tempSavingsFileStream << income << '\n';

        savingsFileStream.close();
        tempSavingsFileStream.close();

        // Overwrite the Savings File for the current user
        std::remove(savingsFile);
        std::rename(savingsFileTemp, savingsFile);

        msgBox->setText("Your expenses management has been saved!");
        msgBox->setIcon(QMessageBox::Information);
        QIcon icon("resources/icons/saveIcon.png");
        msgBox->setWindowIcon(icon);
        msgBox->exec();

    }

    void copyExpenseToRow(int rowPos)
    {
        //Values of the expense at position 'rowPos' of 'expenses' vector are copied to each column of table at row 'rowPos'
        for (int col{ 0 }; col < expenses[rowPos].length(); ++col)
        {
            QTableWidgetItem* item{ new QTableWidgetItem(expenses[rowPos][col]) };

            // The column "Total Amount" should not be editable.
            if (expensesHeaders[col] == totalAmountHeader) {
                item->setFlags(item->flags() & ~Qt::ItemIsEditable & ~Qt::ItemIsSelectable);
            }
            expensesTable->setItem(rowPos, col, item);
        }
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

    void updateTableValues(int row, int col, tableEdit* inputTable)
    {
        if (inputTable == expensesTable)
        {
            // Updating the expenses vector with the modified cell from expensesTable
            auto editedItem{ expensesTable->item(row, col) };
            expenses[row][col] = editedItem->text();

            //If the modified cell was one regarding Expense column
            if (expensesHeaders[col] == expenseHeader)
            {
                // No action needed
            }

            //If the modified cell was one regarding Amount column
            if (expensesHeaders[col] == amountHeader)
            {
                // Getting the frequency item
                auto colFrequency{ expensesHeaders.indexOf(frequencyHeader) };
                auto freqItem{ expensesTable->item(row, colFrequency) };

                // Getting the new double Total Amount value (amount * frequency)
                double newAmountDouble{ editedItem->text().toDouble() };
                double newTotalAmountDouble{ newAmountDouble * freqItem->text().toDouble() };

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
        if (inputTable == savingTable)
        {
            // Updating the expenses vector with the modified cell from expensesTable
            auto editedItem{ savingTable->item(row, col) };            

            //If the modified cell was the Income cell
            if (savingHeaders[col] == incomeHeader)
            {
                // Computing the new 'income' and 'savings' value
                income = editedItem->text().toDouble();
                savings = income - totalExpense;

                // Updating the value of 'savings' in the "Savings Table"
                auto indexSavings{ savingHeaders.indexOf(savingsHeader) };
                savingTable->item(row, indexSavings)->setText(QString::number(savings));
            }

            // cell = Total Expenses
            if (savingHeaders[col] == totalExpensesHeader)
            {
                // It is not possible to modify this cell because it is disabled for user changes
            }

            // cell = Savings
            if (savingHeaders[col] == savingsHeader)
            {
                // It is not possible to modify this cell because it is disabled for user changes
            }
        }
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

    void addExpense()
    {
        // Disabling capturing signals (Cell Changed Signal emision) in order to update the table with a new expense without getting into a conflict 
        bool emitingSignalState = expensesTable->blockSignals(true);

        // Creating a new expense with empty values:
        QStringList newExpense{};
        newExpense.resize(expensesHeaders.length() - 1);

        for (int i{ 0 }; i < expensesHeaders.length() - 1; ++i)
        {
            newExpense[i] = QString("");
        }
        expenses.push_back(newExpense);

        //Adding a new row to the table and adapting the widget to it
        expensesTable->insertRow(expensesTable->rowCount());
        expensesTable->adaptWidgetToTable();

        // Filling the newest row of the table with the new created expense (empty values)
        int newExpensePos = expenses.size() - 1;
        copyExpenseToRow(newExpensePos);

        // Adding a new Remove Button pointer to the "Remove buttons vector"
        rmvButtonsVect.push_back(new QPushButton("Remove"));

        // Updating the internal 'iteratorVect' useful to remove properly the table rows and expenses
        int rmvSize = rmvButtonsVect.size();
        iteratorVect.push_back(static_cast<int>(rmvSize - 1));

        // Adding remove buttons to the table
        expensesTable->setCellWidget(newExpensePos, expensesHeaders.length() - 1, rmvButtonsVect.back());
        QObject::connect(rmvButtonsVect.back(), &QPushButton::clicked, [=]() {
            removeExpense(rmvSize - 1);
            });

        //Restoring the blocking signal state of the signals of 'expensesTable'
        expensesTable->blockSignals(emitingSignalState);

    }

    void generateSavingsFromCSV()
    {
        //This function sets the Savings data related to the "Savings Table" with the content of Database or a CSV file

        // CSV File Path is Opened for being read
        static constexpr char savingsFile[]{ "resources/expenses_files/users/demo/savings_demo.csv" };
        std::fstream savingsFileStream{ savingsFile };

        if (!savingsFileStream)
        {
            qDebug() << "The file " << savingsFile << " could not be opened\n";
            return;
        }

        // expense vector for storing the CSV data is created:
        std::vector<QStringList> savingsCSV;
        savingsCSV.reserve(25);

        // line by line of CSV is read to store its content in the expensesCSV vector
        std::string line{};
        while (std::getline(savingsFileStream, line))
        {
            QString qline = QString::fromStdString(line);
            qDebug() << qline.split(";") << "\n";
            savingsCSV.push_back(qline.split(";"));
        }

        // closing CSV file
        savingsFileStream.close();

        // The positions of the headers of the "Savings Table" are detected in the CSV
        int incomePos{ static_cast<int>(savingsCSV[0].indexOf(incomeHeader)) };
        //int desiredSavingsPos{ static_cast<int>(savingsCSV[0].indexOf(desiredSavingsHeader)) };

        // 'importSavings' is set with every row of 'savingsCSV' whose parameters are sorted according the headers of "Savings Table".
        QStringList importSavings{};
        importSavings.resize(savingsCSV[0].length());

        for (int row{ 1 }; row < savingsCSV.size(); ++row)
        {
            for (int col{ 0 }; col < savingsCSV[0].length(); ++col)
            {
                if (col == savingHeaders.indexOf(incomeHeader))
                {
                    income = savingsCSV[row][incomePos].toDouble();
                }
                /*
                if (col == savingHeaders.indexOf(desiredSavingsHeader))
                {
                    importSavings[col] = savingsCSV[row][desiredSavingsPos];
                }
               */
            }
        }


    }

    void fillSavingTable()
    {
        //savingTable->resizeColumnsToContents();
        //savingTable->horizontalHeader()->setDefaultSectionSize(100);

        savingTable = new tableEdit(1, savingHeaders.length());

        savingTable->setHorizontalHeaderLabels(savingHeaders);

         // Extracting the income value of the user from external source
        generateSavingsFromCSV();
 
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

        // Making the table updatable if the income is modified        
        QObject::connect(savingTable, &QTableWidget::cellChanged, [=](int row, int col) {
            if (col == indexIncome) 
            {
                updateTableValues(row, col, savingTable);
            }            
            });


    }

};