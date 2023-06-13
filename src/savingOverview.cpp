#include "savingOverview.h"

// Constructor of the View with two tables "Expenses Table" and "Savings Table"
savingOverview::savingOverview(User* loggedUser) : QHBoxLayout()
{
    ////////////////////////////////////////////////////////////////////////////////////////
    ////////// ---------- Overview Expenses Table  ---------////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////

    // Setting the current User as the logged User
    currentUser = loggedUser;

    // Update the expenses vector and Income with the content of SQLite Database
    getExpensesAndIncomeFromDB();

    //The Expenses Table is initializated, filled with the user values and Remove Buttons and also made updateable
    fillExpensesTable();

    // Adding an Expense
    QObject::connect(addExpenseButton, &QPushButton::clicked, [=]() {
        addExpense();
        });

    // Save the values of Expense Table and the Income value to an external source
    QObject::connect(saveButton, &QPushButton::clicked, [=]() {
        saveDataToDB();
        });

    // Restore the table values existing in an external Source
    QObject::connect(cancelButton, &QPushButton::clicked, [=]() {
        restoreTableValues();
        });

    ///// Element visualization and organization in the layout        

    QVBoxLayout* vlay{ new QVBoxLayout() };
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

    addWidget(savingTable);
    setAlignment(savingTable, Qt::AlignLeft);

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////
}

// Function that generates the Expenses Table
void savingOverview::fillExpensesTable()
{
    // Create the Expenses Table filled with the values of expenses vector (content from external source)
    int expensesNumber = static_cast<int>(expenses.size());
    if (!expensesTable)
    {
        expensesTable = new tableEdit();
    }
    expensesTable->setRowCount(expensesNumber);
    expensesTable->setColumnCount(expensesHeaders.length());


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

    expensesTable->adaptWidgetToTable();

}

// Function that generates the Savings Table
void savingOverview::fillSavingTable()
{
    //savingTable->resizeColumnsToContents();
    //savingTable->horizontalHeader()->setDefaultSectionSize(100);
    if (!savingTable)
    {
        savingTable = new tableEdit();
    }
    savingTable->setRowCount(1);
    savingTable->setColumnCount(savingHeaders.length());

    savingTable->setHorizontalHeaderLabels(savingHeaders);

    // Setting the 'Income' value to the table
    auto indexIncome{ savingHeaders.indexOf(incomeHeader) };
    savingTable->setItem(0, indexIncome, new QTableWidgetItem(QString::number(income) + " " + moneyUnit));

    // Setting the 'Expenses' value to the table and making it not editable or selectable by the user
    totalExpense = getTotalExpense();
    auto indexTotalExpenses{ savingHeaders.indexOf(totalExpensesHeader) };
    auto totalExpenseItem = new QTableWidgetItem(QString::number(totalExpense) + " " + moneyUnit);
    totalExpenseItem->setFlags(totalExpenseItem->flags() & ~Qt::ItemIsEditable & ~Qt::ItemIsSelectable);
    savingTable->setItem(0, indexTotalExpenses, totalExpenseItem);

    // Setting the 'Savings' value to the table
    auto indexSavings{ savingHeaders.indexOf(savingsHeader) };
    savings = income - totalExpense;
    auto savingsItem = new QTableWidgetItem(QString::number(savings) + " " + moneyUnit);
    savingsItem->setFlags(savingsItem->flags() & ~Qt::ItemIsEditable & ~Qt::ItemIsSelectable);
    savingTable->setItem(0, indexSavings, savingsItem);

    // Making the table updatable if the income is modified        
    QObject::connect(savingTable, &QTableWidget::cellChanged, [=](int row, int col) {
        if (col == indexIncome)
        {
            updateTableValues(row, col, savingTable);
        }
        });

    savingTable->adaptWidgetToTable();
}

// Expenses vector generation from external source
void savingOverview::generateExpensesFromCSV()
{
    //This function sets the expenses vector with the content of Database or a CSV file

    // CSV File Path is Opened for being read
    static constexpr char expensesFile[]{ "resources/expenses_files/users/demo/expenses_demo.csv" };
    std::fstream expensesFileStream{ expensesFile };

    if (!expensesFileStream)
    {
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

    for (int row{ 1 }; row < expensesCSV.size(); ++row)
    {
        for (int col{ 0 }; col < expensesCSV[0].length() + 1; ++col)
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
    }
}

// Expenses vector and Income generation from Database SQLite
void savingOverview::getExpensesAndIncomeFromDB()
{
    userInfoBox->setWindowTitle("Loading User Expenses");
    userInfoBox->setWindowIcon(QIcon(icons::expTrackerIcon));

    sqlite3* db{};

    if (checkAndOpenSQLiteDB(db, userInfoBox, { DB::tableExpenses, DB::tableIncome }) == DB::OPEN_SUCCESS)
    {
        std::string clause{ "WHERE username = '" + currentUser->getUserName().toStdString() + "'"};
        std::string columns{"expense, amount, frequency"};
        std::vector<QStringList> expensesDB{ getRecords(db, DB::tableExpenses, columns, clause) };        

        // Load the database expenses to the Expense vector
        if (!expensesDB.empty()) 
        {
            expenses.reserve(expensesDB.size() + 5);
            for (QStringList& expense : expensesDB)
            {
                // Adding the totalAmount = Expense Amount * Expense Frequency
                QString totalAmount{ QString::number(expense[1].toDouble() * expense[2].toDouble())};
                expense.append(totalAmount);
                expenses.push_back(expense);
            }
        }
        else // There are no expenses for this user in the database
        {
            userInfoBox->setIcon(QMessageBox::Information);
            userInfoBox->setText("For the moment, there are no expenses for this user.");
            userInfoBox->exec();            
        }

        // Loading Income from Database
        clause = "WHERE username = '" + currentUser->getUserName().toStdString() + "'" ;
        columns = "income_amount, frequency";
        std::vector<QStringList> incomeDB{ getRecords(db, DB::tableIncome, columns, clause) };        

        // Load the database Income 
        if (!incomeDB.empty())
        {
            double totalIncome{ 0 };
            std::vector<QStringList> incomeVect{};
            for (QStringList& income : incomeDB)
            {
                // Adding the totalAmount = Expense Amount * Expense Frequency
                QString totalAmount{ QString::number(income[0].toDouble() * income[1].toDouble()) };
                income.append(totalAmount);
                incomeVect.push_back(income);
                totalIncome += totalAmount.toDouble();
            }
            income = totalIncome;
        }
        else // There are no income for this user in the database
        {
            income = 0;
        }

        closeSQLiteDB(db);
    }
}

// Income generation from external source
void savingOverview::generateSavingsFromCSV()
{
    //This function sets the Savings data related to the "Savings Table" with the content of Database or a CSV file

    // CSV File Path is Opened for being read
    static constexpr char savingsFile[]{ "resources/expenses_files/users/demo/savings_demo.csv" };
    std::fstream savingsFileStream{ savingsFile };

    if (!savingsFileStream)
    {
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

// Copying an Expense to a Table Row
void savingOverview::copyExpenseToRow(int rowPos, bool moneyUnitFlag)
{
    //Values of the expense at position 'rowPos' of 'expenses' vector are copied to each column of table at row 'rowPos'
    for (int col{ 0 }; col < expenses[rowPos].length(); ++col)
    {
        QTableWidgetItem* item{ new QTableWidgetItem(expenses[rowPos][col]) };

        // The column "Total Amount" should not be editable.
        if (expensesHeaders[col] == totalAmountHeader) {
            item->setFlags(item->flags() & ~Qt::ItemIsEditable & ~Qt::ItemIsSelectable);

            // Add the symbol of 'moneyUnit' for the Total Amount column
            if (expensesHeaders[col] == totalAmountHeader && moneyUnitFlag)
            {
                item->setText(item->text() + " " + moneyUnit);
            }
        }
        // Add the symbol of 'moneyUnit' for the amount column
        if (expensesHeaders[col] == amountHeader && moneyUnitFlag)
        {
            item->setText(item->text() + " " + moneyUnit);
        }
        expensesTable->setItem(rowPos, col, item);

    }
}

// Updating Table values from a user-modification of any cell
void savingOverview::updateTableValues(int row, int col, tableEdit* inputTable)
{
    if (inputTable == expensesTable)
    {
        // Updating the expenses vector with the modified cell from expensesTable
        auto editedItem{ expensesTable->item(row, col) };
        auto editedItemText = editedItem->text();
        expenses[row][col] = editedItemText;

        //If the modified cell was one regarding Expense column
        if (expensesHeaders[col] == expenseHeader)
        {
            // No action needed
        }

        //If the modified cell was one regarding Amount column
        if (expensesHeaders[col] == amountHeader)
        {
            // Checking the format of the cell text. If it is not OK the cell will be background painted and return value will be set to 0.0
            // Function returns the cell double value
            double newAmountDouble = setCellAccordingToTextFormat(row, col, expensesTable, true);
            expenses[row][col] = QString::number(newAmountDouble);

            // Getting the frequency item
            auto colFrequency{ expensesHeaders.indexOf(frequencyHeader) };
            auto freqItem{ expensesTable->item(row, colFrequency) };

            // Getting the new double Total Amount value (amount * frequency)
            //double newAmountDouble{ expenses[row][col].toDouble() };
            double newTotalAmountDouble{ newAmountDouble * freqItem->text().toDouble() };

            // Setting the new double Total Amount value to the Table:
            auto colTotalAmount{ expensesHeaders.indexOf(totalAmountHeader) };
            expensesTable->item(row, colTotalAmount)->setText(QString::number(newTotalAmountDouble) + " " + moneyUnit);

            // Updating the expenses vector with the new Total Amount value
            expenses[row][colTotalAmount] = QString::number(newTotalAmountDouble);

            // Getting the total amount of all Expenses and showing it in the Table
            totalExpense = getTotalExpense();
            auto indexTotalExpenses{ savingHeaders.indexOf(totalExpensesHeader) };
            savingTable->item(0, indexTotalExpenses)->setText(QString::number(totalExpense) + " " + moneyUnit);

            // Setting the 'Savings' value to the table
            auto indexSavings{ savingHeaders.indexOf(savingsHeader) };
            savings = income - totalExpense;
            savingTable->item(0, indexSavings)->setText(QString::number(savings) + " " + moneyUnit);
        }

        //If the modified cell was one regarding from Frequency        
        if (expensesHeaders[col] == frequencyHeader)
        {
            // Checking the format of the cell text. If it is not OK the cell will be background painted and return value will be set to 0.0
            // Function returns the cell double value
            double newFrequencyDouble = setCellAccordingToTextFormat(row, col, expensesTable, false);
            expenses[row][col] = QString::number(newFrequencyDouble);

            //Getting the Amount item
            auto colAmount{ expensesHeaders.indexOf(amountHeader) };
            auto amountItem{ expenses[row][colAmount].toDouble() };

            // Getting the new double Total Amount value (amount * frequency)
            double newTotalAmountDouble{ newFrequencyDouble * amountItem };

            // Setting the new double Total Amount value to the Table:
            auto colTotalAmount{ expensesHeaders.indexOf(totalAmountHeader) };
            expensesTable->item(row, colTotalAmount)->setText(QString::number(newTotalAmountDouble) + " " + moneyUnit);

            // Updating the expenses vector with the new Total Amount value
            expenses[row][colTotalAmount] = QString::number(newTotalAmountDouble);

            // Getting the total amount of all Expenses and showing it in the Table
            totalExpense = getTotalExpense();
            auto indexTotalExpenses{ savingHeaders.indexOf(totalExpensesHeader) };
            savingTable->item(0, indexTotalExpenses)->setText(QString::number(totalExpense) + " " + moneyUnit);

            // Setting the 'Savings' value to the table
            auto indexSavings{ savingHeaders.indexOf(savingsHeader) };
            savings = income - totalExpense;
            savingTable->item(0, indexSavings)->setText(QString::number(savings) + " " + moneyUnit);

        }
    }
    if (inputTable == savingTable)
    {
        // Updating the expenses vector with the modified cell from expensesTable
        auto editedItem{ savingTable->item(row, col) };

        //If the modified cell was the Income cell
        if (savingHeaders[col] == incomeHeader)
        {
            // Checking the format of the 'income' cell text. If it is not OK the cell will be background painted and income will be set to 0.0
            income = setCellAccordingToTextFormat(row, col, savingTable, true);

            // Computing the new 'income' and 'savings' value
            savings = income - totalExpense;

            // Updating the value of 'savings' in the "Savings Table"
            auto indexSavings{ savingHeaders.indexOf(savingsHeader) };
            savingTable->item(row, indexSavings)->setText(QString::number(savings) + " " + moneyUnit);
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

// Checking cell format and returning the double cell value according its format checking
double savingOverview::setCellAccordingToTextFormat(int row, int col, tableEdit* inputTable, bool moneyUnitFlag)
{
    // This function checks the text format of the cell [row, col] from the table 'inputTable'. 
    // If this text format is not OK the cell will be background painted and return value will be set to 0.0
    // If text format is OK the cell will be backgroung painted as a cell with correct format. In this case, the return value is the cell double value.
    // The function considers to add into the cell the symbol of 'moneyUnit' depending an input flag 'moneyUnitFlag'

    // Getting the item (row,col) from inputTable
    auto item = inputTable->item(row, col);
    auto editedItemText = item->text();

    // Removing the input money symbol shwon in the table
    if (moneyUnitFlag)
    {
        editedItemText.remove(moneyUnit);
    }

    // 'isDoubleVal' is used to check if the cell value has a double format or not
    bool isDoubleVal;
    double cellVal = editedItemText.toDouble(&isDoubleVal);

    // Setting the background color in case 'cellVal' has not a propper double format
    // Setting the Wrong_flag to "1" in order to avoid saving the data
    if (!isDoubleVal) {
        inputTable->setWrongCell(row, col);
        wrongCellFlag = true;
    }
    // 'cellVal' has a proper double format and its backgroud is painted in the correct-cell-format style
    else {
        inputTable->setWrongCell(row, col, false);

        // Adding or not the moneyUnit symbol to table according the flag 'moneyUnitFlag'
        if (moneyUnitFlag)
        {
            item->setText(QString::number(cellVal) + " " + moneyUnit);
        }
        else
        {
            item->setText(QString::number(cellVal));
        }
        wrongCellFlag = false;
    }

    // cellVal is returned, and it could be 0.0 in case the format was wrong or the cell double value
    return cellVal;
}

// Total Expense computing from all "Total Amount expenses"
double savingOverview::getTotalExpense()
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

// Action from button: Remove Button
void savingOverview::removeExpense(int row)
{
    // Detecting the position of the row to be delated from the relative position 'row'
    auto rmvPosIt{ std::find(iteratorVect.begin(), iteratorVect.end(), row) };
    int rmvPos{ static_cast<int>(std::distance(iteratorVect.begin(), rmvPosIt)) };

    // Computing the total amount of the expenses after removing one expense
    auto indexTotalAmount{ expensesHeaders.indexOf(totalAmountHeader) };
    totalExpense -= expenses[rmvPos][indexTotalAmount].toDouble();

    // Setting the 'Expenses' value to the table
    auto indexTotalExpenses{ savingHeaders.indexOf(totalExpensesHeader) };
    savingTable->item(0, indexTotalExpenses)->setText(QString::number(totalExpense) + " " + moneyUnit);

    // Setting the 'Savings' value to the table
    auto indexSavings{ savingHeaders.indexOf(savingsHeader) };
    savings = income - totalExpense;
    savingTable->item(0, indexSavings)->setText(QString::number(savings) + " " + moneyUnit);

    // Removing the expense from vector and from the table
    expensesTable->removeRow(rmvPos);
    expenses.erase(expenses.begin() + rmvPos);

    // Removing the "remove position" from the 'iteratorVect' (this is useful to get a proper row removing)
    iteratorVect.erase(iteratorVect.begin() + rmvPos);

    // Updating the widget fitting
    expensesTable->adaptWidgetToTable();
}

// Action from button: Add Button
void savingOverview::addExpense()
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
    copyExpenseToRow(newExpensePos, false);

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

// Save Expenses and Income to SQLite Database
void savingOverview::saveDataToDB()
{
    // This function stores the table data values (Saving and Expenses Tables) to Database.
    // To do it, first the Expenses and Income data from Database is removed. Afterwards, the data from tables is inserted to the Database.

    sqlite3* db{};

    // This msgBox will inform the saving status
    userInfoBox->setWindowTitle("Saving Expenses");
    userInfoBox->setWindowIcon(QIcon(icons::saveIcon));

    // If the cell format is not correct
    if (wrongCellFlag)
    {
        userInfoBox->setText("You cannot save your expense management until they have a proper format.");
        userInfoBox->setIcon(QMessageBox::Warning);
        userInfoBox->exec();
        return;
    }

    // Checking and opening the SQLite Database
    if (checkAndOpenSQLiteDB(db, userInfoBox, { DB::tableExpenses, DB::tableIncome }) == DB::OPEN_SUCCESS)
    {
        // Preparig the clause for Deleting and Inserting values into the Database
        std::string userName{ currentUser->getUserName().toStdString() };
        std::string clause{ "WHERE username = '" + userName + "'" };

        // Initializing a flag to check if the saving process was propperly conducted or not (true = correct saving, false = error occured)
        bool savingStatusFlag{ false };

        // If deleting Income and Expenses from Database was successful conducted
        if (deletingRecords(db, DB::tableExpenses, clause) == true && deletingRecords(db, DB::tableIncome, clause) == true)
        {
            // Setting the Saving flag to 'true'
            savingStatusFlag = true;

            // Getting the indexes of "Expense name, Expense amount and Expense Frequency"
            auto indexExpense{ expensesHeaders.indexOf(expenseHeader) };
            auto indexAmount{ expensesHeaders.indexOf(amountHeader) };
            auto indexFreq{ expensesHeaders.indexOf(frequencyHeader) };

            // Executing the "INSERT INTO" SQLite Query for each Expense
            std::string values{};
            for (const QStringList& expense : expenses)
            {
                values = "'" + userName + "','" + expense[indexExpense].toStdString() + "','" + expense[indexAmount].toStdString() + "','" + expense[indexFreq].toStdString() + "'";

                // Refreshing the Saving Status Flag according the result of SQL Insertion
                savingStatusFlag &= insertRecord(db, DB::tableExpenses, values);
            }

            // Preparing the values of "INSERT INTO" SQLite Query for the Income
            values = "'" + userName + "','" + "no_name" + "','" + std::to_string(income) + "','" + "1" + "'";

            // Refreshing the Saving Status Flag according the result of SQL Insertion
            savingStatusFlag &= insertRecord(db, DB::tableIncome, values);
        }

        // Checking the Status of Saving Process
        if (savingStatusFlag)
        {
            userInfoBox->setText("Your expenses management has been saved!");
            userInfoBox->setIcon(QMessageBox::Information);
            userInfoBox->exec();
        }
        else
        {
            userInfoBox->setText("Your expenses management could not be saved due to an unknown error.");
            userInfoBox->setIcon(QMessageBox::Critical);
            userInfoBox->exec();
            return;

        }

        closeSQLiteDB(db);
    }       

}

// Action from button: Save Button
void savingOverview::saveDataToCSV()
{
    //This function sets the expenses vector and Income with the content of Database or a CSV file

    // This msgBox will inform the saving status
    userInfoBox->setWindowTitle("Saving Expenses");
    userInfoBox->setWindowIcon(QIcon(icons::saveIcon));

    if (wrongCellFlag)
    {
        userInfoBox->setText("You cannot save your expense management until they have a proper format.");
        userInfoBox->setIcon(QMessageBox::Warning);
        userInfoBox->exec();
        return;
    }

    // CSV Expenses File Path is Opened for being written
    static constexpr char expensesFile[]{ "resources/expenses_files/users/demo/expenses_demo.csv" };
    std::fstream expensesFileStream{ expensesFile };

    if (!expensesFileStream)
    {
        userInfoBox->setText("ERROR: Your expenses management cold not be saved!");
        userInfoBox->setIcon(QMessageBox::Warning);
        userInfoBox->exec();
        return;
    }

    static constexpr char expensesFileTemp[]{ "resources/expenses_files/users/demo/expenses_demoTemp.csv" };
    std::ofstream tempExpensesFileStream(expensesFileTemp);

    if (!tempExpensesFileStream)
    {
        userInfoBox->setText("ERROR: Your expenses management cold not be saved!");
        userInfoBox->setIcon(QMessageBox::Warning);
        userInfoBox->exec();
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
        userInfoBox->setText("ERROR: Your expenses management cold not be saved!");
        userInfoBox->setIcon(QMessageBox::Warning);
        userInfoBox->exec();
        return;
    }

    static constexpr char savingsFileTemp[]{ "resources/expenses_files/users/demo/savings_demoTemp.csv" };
    std::ofstream tempSavingsFileStream(savingsFileTemp);

    if (!tempSavingsFileStream)
    {
        userInfoBox->setText("ERROR: Your expenses management cold not be saved!");
        userInfoBox->setIcon(QMessageBox::Warning);
        userInfoBox->exec();
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

    userInfoBox->setText("Your expenses management has been saved!");
    userInfoBox->setIcon(QMessageBox::Information);
    userInfoBox->exec();

}

// Action from button: Restore Button
void savingOverview::restoreTableValues()
{
    // Disabling capturing signals (Cell Changed Signal emision) in order to update the table with previous saved values 
    bool emitingExpSignalState = expensesTable->blockSignals(true);
    bool emitingSavSignalState = savingTable->blockSignals(true);

    // Cleaning the storing vectors and expense table
    iteratorVect.clear();
    rmvButtonsVect.clear();
    expenses.clear();
    expensesTable->setRowCount(0);

    // Restore the last stored data (Expenses and Income) from Database
    getExpensesAndIncomeFromDB();

    //Fill again the Expense Table with the last saved values
    fillExpensesTable();

    // Cleaning the saving table and filling it again with the last saved values
    savingTable->setRowCount(0);
    fillSavingTable();

    //Restoring the blocking signal state of the signals of 'expensesTable'
    expensesTable->blockSignals(emitingExpSignalState);
    savingTable->blockSignals(emitingSavSignalState);
}