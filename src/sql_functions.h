#ifndef SQLFUNCTIONS_H
#define SQLFUNCTIONS_H

#include <sqlite3.h>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <QDebug>
#include "constants.h"


//////////////////////////////////////////////////////////////////////////////////////////
// Description:                                                                         
// Useful Functions to send Queries to the Database and check its availability 
//////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
/////////////// INITIALIZING DATABASE FUNCTIONS
//////////////////////////////////////////////////////////////////////

bool isDBExisting();

bool openSQLiteDB(sqlite3*& db);

void closeSQLiteDB(sqlite3* db);

int testMyDB();


//////////////////////////////////////////////////////////////////////
/////////////// SUPPORTING DATABASE FUNCTIONS
//////////////////////////////////////////////////////////////////////

int isTableCreated(sqlite3* db, const std::string& tableName);

int getRecordNumber(sqlite3* db, const std::string& tableName);


//////////////////////////////////////////////////////////////////////
/////////////// SQLITE QUERIES
//////////////////////////////////////////////////////////////////////

bool insertRecord(sqlite3* db, const std::string& tableName, const std::string& values);

int getRecords(sqlite3* db, const std::string& tableName, const std::string& columns = "*", const std::string& clause = "");

int updateRecords(sqlite3* db, const std::string& tableName, const std::string& columns, const std::string& values, const std::string& condition);

int deletingRecords(sqlite3* db, const std::string& tableName, const std::string& clause = "");





#endif
