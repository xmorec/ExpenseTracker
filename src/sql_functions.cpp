#include "sql_functions.h"

//////////////////////////////////////////////////////////////////////
/////////////// INITIALIZING DATABASE FUNCTIONS
//////////////////////////////////////////////////////////////////////

//Check if Database file exists or not in its Path
bool isDBExisting()
{
	//returns true if database file exists, false otherwise
	struct stat buffer;
	return (stat(DB::dbFilePath.c_str(), &buffer) == 0);
}

// Opens the database. Returns true if it is properly open or false otherwise
bool openSQLiteDB(sqlite3*& db)
{
	if (sqlite3_open(DB::dbFilePath.c_str(), &db) != SQLITE_OK) // When database could not be open
	{
		sqlite3_close(db);
		return false;
	}

	return true;
}

// Closes the database
void closeSQLiteDB(sqlite3* db)
{
	sqlite3_close(db);
}

// Checks if Database exists, opens the database and checks if input tables exist. Generates message errors in case something fails
int checkAndOpenSQLiteDB(sqlite3*& db, QMessageBox* userInfoBox, const std::vector<std::string>& tables)
{
	//Checking DB availability
	if (isDBExisting())
	{
		// Opening Database
		if (openSQLiteDB(db))
		{
			// Count the number of created tables
			int tableCreated{ 0 };
			for (const std::string& table : tables)
			{
				tableCreated += isTableCreated(db, table);
			}

			// When all input tables are created
			if (tableCreated == tables.size())
			{
				return DB::OPEN_SUCCESS;
			}
			else //When at least one of the input tables is not created
			{
				userInfoBox->setText("Database Tables are not created");
				userInfoBox->setIcon(QMessageBox::Warning);
				userInfoBox->exec();
				closeSQLiteDB(db);
				return DB::NO_TABLES;
			}
		}
		else // When the database could not be opened
		{
			userInfoBox->setText("Database could not be opened");
			userInfoBox->setIcon(QMessageBox::Warning);
			userInfoBox->exec();
			return DB::OPEN_NOT_SUCCESS;
		}
	}
	else // When Database file is not existing
	{
		userInfoBox->setText("Database File is not existing in the expected folder");
		userInfoBox->setIcon(QMessageBox::Warning);
		userInfoBox->exec();
		return DB::NO_DB;
	}
}

// Function to debug Database
int testMyDB()
{
	sqlite3* db{ nullptr };

	//First it is checked if Database file exists

	if (isDBExisting())
	{
		qDebug() << "\n\n\n-----------\nDATABASE FILE EXISTS\n-----------\n\n\n";
	}
	else
	{
		qDebug() << "\n\n\n-----------\nWARNING: DATABASE FILE DOES NOT EXIST\n-----------\n\n\n";
		return -2;
	}

	/* Open database and check whether it could be successfuly open */
	if (openSQLiteDB(db))
	{
		qDebug() << "\n\n\n-----------\nDATABASE SUCCESSFULLY OPENED\n-----------\n\n\n";
	}
	else
	{
		qDebug() << "\n\n\n-----------\nDATABASE NOT SUCCESSFULLY OPENED\n-----------\n\n\n";
		return -1;
	}

	closeSQLiteDB(db);
	return 1;
}

//////////////////////////////////////////////////////////////////////
/////////////// SUPPORTING DATABASE FUNCTIONS
//////////////////////////////////////////////////////////////////////

// Checks if a single Table of database is created or not
int isTableCreated(sqlite3* db, const std::string& tableName)
{
	// Setting query to check whether a table exists or not
	std::string query{ "SELECT count(*) FROM sqlite_master WHERE type = 'table' AND name = '" + tableName + "';" };
	sqlite3_stmt* stmt{};

	// Variable storing the number of tables existing with name 'tableName'
	int isCreated{ -2 };

	// Constructing the query
	int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, 0);

	// Successful query construction
	if (rc == SQLITE_OK)
	{
		// Executing the query and getting the number of tables with name 'tableName' (it should be 0 or 1)
		sqlite3_step(stmt);
		isCreated = sqlite3_column_int(stmt, 0);
	}
	else
	{
		// Error
		isCreated = -1;
	}

	// Destructing statement (stmt) object
	sqlite3_finalize(stmt);

	return isCreated;
}

// Gets the record number from a table depending on the input clause
int getRecordNumber(sqlite3* db, const std::string& tableName, const std::string& clause)
{
	// Setting the SQL Query. 'clause' could be "where column_name = 'value'"
	std::string query{ "SELECT COUNT(*) FROM " + tableName + " " + clause + ";"};
	sqlite3_stmt* stmt;
		
	int RecordNum{ -2 };

	// Constructing the query
	int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, 0);

	if (rc == SQLITE_OK) // Successful query construction
	{
		sqlite3_step(stmt); //Execution of the SQL query
		RecordNum = sqlite3_column_int(stmt, 0); // Getting the Record Number given by the SQL Query
	}
	else
	{
		// Error
		RecordNum = -1;
	}

	sqlite3_finalize(stmt);

	return RecordNum;
}

//////////////////////////////////////////////////////////////////////
/////////////// SQLITE QUERIES
//////////////////////////////////////////////////////////////////////

// Inserting Record to a table of database. Returns 'true' for a successful inserting, 'false' otherwise
bool insertRecord(sqlite3* db, const std::string& tableName, const std::string& values)
{
	// Create SQL statement of "INSERT INTO" filling a table with 'values'
	sqlite3_stmt* stmt{};
	std::string query{ "INSERT INTO " + tableName + " VALUES(" + values + ");" };

	// Constructing the SQL statement
	int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, 0);

	if (rc != SQLITE_OK) // The construction of the statement was not successful
	{
		sqlite3_finalize(stmt);
		qDebug() << "\n****************\nrc = " << rc; //Checking the error code
		const char* errorMessage = sqlite3_errmsg(db);
		qDebug() << "\n\n\nError message: " << errorMessage << "\n\n****************\n\n\n"; // Checking the error message given by the error code

		return false;
	}

	//std::string user{ "userT" };
	//sqlite3_bind_text(stmt, 1, user.c_str(), -1, NULL);
	//sqlite3_bind_text(stmt, 2, std::to_string(numUsers).c_str(), -1, NULL);

	sqlite3_step(stmt);
	sqlite3_reset(stmt);
	sqlite3_finalize(stmt);

	return true;
}

// Gets the record from a table according the input columns and a clause. It returns the selected records.
std::vector<QStringList> getRecords(sqlite3* db, const std::string& tableName, const std::string& columns, const std::string& clause)
{

	// Setting the SQL Query. 'clause' could be "where column_name = 'value'"
	sqlite3_stmt* stmt{};
	std::string query{ "SELECT " + columns + " FROM " + tableName + " " + clause + ";" };

	// The number of records for that SQL query is obtained
	int recordNum{ getRecordNumber(db, tableName, clause) };

	// The vector of Records is created
	std::vector<QStringList> records{};
	records.reserve(recordNum);	

	// Constructing the query
	int rc{ sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, 0) };

	if (rc != SQLITE_OK) // Not Successful query construction
	{
		sqlite3_finalize(stmt);
		return records; // Returns an empty vector for Records
	}

	//bind values to parameters
	//sqlite3_bind_text(stmt, 1, "*", -1, SQLITE_STATIC);
	//sqlite3_bind_text(stmt, 2, "User 1", -1, SQLITE_STATIC);

	// Getting the number of columns related to the SQL Query
	int numColumns = sqlite3_column_count(stmt);

	// Initializing the QStringList elements of the vector with a size of 'numColumns'
	for (int i = 0; i < recordNum; ++i)
	{
		records.push_back(QStringList(numColumns));
	}	

	// QString used to store each value of each column, record of the table
	QString selectedDBData{""};	

	// Running a loop to fill the vector 'records'
	int j{ 0 };
	while (sqlite3_step(stmt) == SQLITE_ROW) //This loops until the execution of the statement does not give a "row" state
	{	
		for (int i = 0; i < numColumns; ++i) // Looping each column
		{
			// When there is 'null' value in the [record, column] position
			if (!sqlite3_column_text(stmt, i)) 
			{
				selectedDBData = "NULL";
			}
			else // When there is a 'non-null' value in the [record, column] position
			{
				selectedDBData = reinterpret_cast<char const*>(sqlite3_column_text(stmt, i));
			}

			// Filling the records vector
			records[j][i] = selectedDBData;		
		}
		++j;
	}

	sqlite3_finalize(stmt);

	return records;
}

// Updates Records from a Table of Database
int updateRecords(sqlite3* db, const std::string& tableName, const std::string& columns, const std::string& values, const std::string& condition)
{
	//Setting the SQL Query where 'condition' could be "column_name = 'value'"
	sqlite3_stmt* stmt{};
	std::string query{ "UPDATE " + tableName + " SET " + columns + " = '" + values + "' WHERE " + condition + ";" };

	//Constructing the SQL query statement
	int rc{ sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, 0) };

	// SQL statement was not successfully constructed
	if (rc != SQLITE_OK) 
	{
		sqlite3_finalize(stmt);
		return -1;
	}
		
	sqlite3_step(stmt); // Executing the SQL Query
	sqlite3_finalize(stmt);

	return 1;

}

// Deletes Records from a Table of Database
int deletingRecords(sqlite3* db, const std::string& tableName, const std::string& clause)
{
	// Setting the SQL Query
	sqlite3_stmt* stmt{};
	std::string query{ "DELETE FROM " + tableName + " " + clause + ";" };

	// Constructs the SQL Query Statement
	int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, 0);

	if (rc != SQLITE_OK) // Construction of the statement was not successful
	{
		sqlite3_finalize(stmt);
		return -1;
	}

	sqlite3_step(stmt); // Executing the SQL Query to the Database
	sqlite3_finalize(stmt);

	return 1;
}
