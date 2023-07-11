#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QString>

namespace DB
{
	inline const std::string dbFilePath{ "resources/database/ExpenseTrackerDB.db" };
	inline const std::string tableUsers{ "Users" };
	inline const std::string tableExpenses{ "Expenses" };
	inline const std::string tableIncome{ "Income" };

	inline const std::string col_username{ "username" };
	inline const std::string col_name{ "name" };
	inline const std::string col_hashpass{ "hash_password" };
	inline const std::string col_salt{ "salt" };
	inline const std::string col_usertype{ "user_type" };


	namespace UserType
	{
		inline const std::string user{ "user" };
		inline const std::string admin{ "admin" };
	}

	inline const int OPEN_SUCCESS{ 100 }; // Return value when Database and tables exists and also Database could be opened
	inline const int OPEN_NOT_SUCCESS{ 101 }; // Return value when Tables are not existing
	inline const int NO_TABLES { 102 }; // Return value when Tables are not existing
	inline const int NO_DB { 103 }; // Return value when Database is not existing
}

namespace icons
{
	inline const QString expTrackerIcon{ "resources/icons/ExpTrIcon.png" };
	inline const QString loggingIcon { "resources/icons/logsession.png" };
	inline const QString saveIcon{ "resources/icons/saveIcon.png" };
	inline const QString prefIcon{ "resources/icons/preferences.png" };
	inline const QString removeIcon{ "resources/icons/remove_user.png" };
}


namespace cellColor
{
	inline static const QColor disableColor{ QColor(Qt::gray) };
	inline static const QColor enableColor{ QColor(Qt::white) };
	inline static const QColor errorColor{ "#EA9791" };
}

namespace fieldColor
{
	inline static const QString disable { "background-color: rgb(236, 236, 236); "};
	inline static const QString enable { "background-color: white; " };
}


#endif