#ifndef SQLFUNCTIONS_H
#define SQLFUNCTIONS_H

#include <sqlite3.h>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <QDebug>
#include <QString>
#include <QMessageBox>
#include "constants.h"


//////////////////////////////////////////////////////////////////////////////////////////
// Description:                                                                         
// Useful Functions to send Queries to the Database and check its availability 
//////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
/////////////// INITIALIZING DATABASE FUNCTIONS
//////////////////////////////////////////////////////////////////////

//Check if Database file exists or not in its Path
bool isDBExisting();

// Opens the database. Returns true if it is properly open or false otherwise
bool openSQLiteDB(sqlite3*& db);

// Checks if Database exists, opens the database and checks if input tables exist. Generates message errors in case something fails
int checkAndOpenSQLiteDB(sqlite3*& db, QMessageBox* userInfoBox, const std::vector<std::string>& tables);

// Closes the database
void closeSQLiteDB(sqlite3* db);

// Function to debug Database
int testMyDB();


//////////////////////////////////////////////////////////////////////
/////////////// SUPPORTING DATABASE FUNCTIONS
//////////////////////////////////////////////////////////////////////

// Checks if a single Table of database is created or not
int isTableCreated(sqlite3* db, const std::string& tableName);

// Gets the record number from a table depending on the input clause
int getRecordNumber(sqlite3* db, const std::string& tableName, const std::string& clause = "");


//////////////////////////////////////////////////////////////////////
/////////////// SQLITE QUERIES
//////////////////////////////////////////////////////////////////////

// Inserting Record to a table of database. Returns 'true' for a successful inserting, 'false' otherwise
bool insertRecord(sqlite3* db, const std::string& tableName, const std::string& values);

// Gets the record from a table according the input columns and a clause. It returns the selected records.
std::vector<QStringList> getRecords(sqlite3* db, const std::string& tableName, const std::string& columns = "*", const std::string& clause = "");

// Updates Records from a Table of Database
int updateRecords(sqlite3* db, const std::string& tableName, const std::string& columns, const std::string& values, const std::string& condition);

// Deletes Records from a Table of Database
bool deletingRecords(sqlite3* db, const std::string& tableName, const std::string& clause = "");





#endif
