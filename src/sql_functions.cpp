#include "sql_functions.h"

//////////////////////////////////////////////////////////////////////
/////////////// INITIALIZING DATABASE FUNCTIONS
//////////////////////////////////////////////////////////////////////

bool isDBExisting()
{
	struct stat buffer;
	return (stat(dbFilePath.c_str(), &buffer) == 0);
}

bool openSQLiteDB(sqlite3*& db)
{
	if (sqlite3_open(dbFilePath.c_str(), &db) != SQLITE_OK)
	{
		sqlite3_close(db);
		return false;
	}

	return true;
}

void closeSQLiteDB(sqlite3* db)
{
	sqlite3_close(db);
}

int testMyDB()
{
	sqlite3* db{ nullptr };

	if (isDBExisting())
	{
		qDebug() << "\n\n\n-----------\nDATABASE FILE EXISTS\n-----------\n\n\n";
	}
	else
	{
		qDebug() << "\n\n\n-----------\nWARNING: DATABASE FILE DOES NOT EXIST\n-----------\n\n\n";
		return -2;
	}

	/* Open database */
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

int getRecordNumber(sqlite3* db, const std::string& tableName)
{
	std::string query{ "SELECT COUNT(*) FROM " + tableName + ";" };
	sqlite3_stmt* stmt;
	int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, 0);

	int RecordNum{ -2 };

	if (rc == SQLITE_OK)
	{
		sqlite3_step(stmt);
		RecordNum = sqlite3_column_int(stmt, 0);
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

bool insertRecord(sqlite3* db, const std::string& tableName, const std::string& values)
{
	/* Create SQL statement */
	sqlite3_stmt* stmt{};
	std::string query{ "INSERT INTO " + tableName + " VALUES(" + values + ");" };

	int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, 0);
	if (rc != SQLITE_OK)
	{
		sqlite3_finalize(stmt);
		qDebug() << "\n---------- rc = " << rc;
		const char* errorMessage = sqlite3_errmsg(db);
		qDebug() << "\n\n****************\n\n\nError message: " << errorMessage << "\n\n\n\n\n";

		return false;
	}

	//std::string user{ "userT" };
	//sqlite3_bind_text(stmt, 1, user.c_str(), -1, NULL);
	//sqlite3_bind_text(stmt, 2, std::to_string(numUsers).c_str(), -1, NULL);

	sqlite3_step(stmt);
	sqlite3_finalize(stmt);

	return true;
}

int getRecords(sqlite3* db, const std::string& tableName, const std::string& columns, const std::string& clause)
{
	//create prepared statement
	sqlite3_stmt* stmt{};
	std::string query{ "SELECT " + columns + " FROM " + tableName + " " + clause + ";" };
	int rc{ sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, 0) };

	if (rc != SQLITE_OK)
	{
		std::cout << "Failure in Selecting\n";
		sqlite3_finalize(stmt);
		return -1;
	}

	//bind values to parameters
	//sqlite3_bind_text(stmt, 1, "*", -1, SQLITE_STATIC);
	//sqlite3_bind_text(stmt, 2, "User 1", -1, SQLITE_STATIC);

	int numColumns = sqlite3_column_count(stmt);

	// run the SQL
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		for (int i = 0; i < numColumns; ++i)
		{
			//std::cout << reinterpret_cast<const char*>(sqlite3_column_text(stmt, i)) << ' ';
			if (!sqlite3_column_text(stmt, i))
			{
				std::cout << "NULL ";
			}
			else
			{
				std::cout << sqlite3_column_text(stmt, i) << ' ';
			}
		}
		std::cout << '\n';
	}

	sqlite3_finalize(stmt);

	return 1;

}

int updateRecords(sqlite3* db, const std::string& tableName, const std::string& columns, const std::string& values, const std::string& condition)
{
	//create prepared statement
	sqlite3_stmt* stmt{};
	//UPDATE table SET column1 = value1, column2 = value2...., columnN = valueN, WHERE condition.
	std::string query{ "UPDATE " + tableName + " SET " + columns + " = '" + values + "' WHERE " + condition + ";" };

	int rc{ sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, 0) };

	if (rc != SQLITE_OK)
	{
		std::cout << "Failure in Modifying\n";
		sqlite3_finalize(stmt);
		return -1;
	}

	sqlite3_step(stmt);
	sqlite3_finalize(stmt);

	return 1;

}

int deletingRecords(sqlite3* db, const std::string& tableName, const std::string& clause)
{

	sqlite3_stmt* stmt{};
	std::string query{ "DELETE FROM " + tableName + " " + clause + ";" };

	int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, 0);
	if (rc != SQLITE_OK)
	{
		std::cout << "Failure in Deleting\n";
		sqlite3_finalize(stmt);
		return -1;
	}

	sqlite3_step(stmt);
	sqlite3_finalize(stmt);

	return 1;
}
