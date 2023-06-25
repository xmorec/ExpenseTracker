#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QString>

namespace DB
{
	inline const std::string dbFilePath{ "resources/database/ExpenseTrackerDB.db" };
	inline const std::string tableUsers{ "Users" };
	inline const std::string tableExpenses{ "Expenses" };
	inline const std::string tableIncome{ "Income" };

	inline const int OPEN_SUCCESS{ 100 }; // Return value when Database and tables exists and also Database could be opened
	inline const int OPEN_NOT_SUCCESS{ 101 }; // Return value when Tables are not existing
	inline const int NO_TABLES { 102 }; // Return value when Tables are not existing
	inline const int NO_DB { 103 }; // Return value when Database is not existing

	namespace UserType
	{
		inline const std::string user{ "user" };
		inline const std::string admin{ "admin" };
	}

}

namespace icons
{
	inline const QString expTrackerIcon{ "resources/icons/ExpTrIcon.png" };
	inline const QString loggingIcon { "resources/icons/logsession.png" };
	inline const QString saveIcon{ "resources/icons/saveIcon.png" };
	inline const QString prefIcon{ "resources/icons/preferences.png" };
}


#endif