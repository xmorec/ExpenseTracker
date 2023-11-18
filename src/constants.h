#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QString>

namespace DB
{
	// Database Path
	inline const std::string dbFilePath{ "resources/database/ExpenseTrackerDB.db" };

	// Database Tables
	inline const std::string tableUsers{ "Users" };
	inline const std::string tableExpenses{ "Expenses" };
	inline const std::string tableIncome{ "Income" };
	inline const std::string tableGroups{ "Groups" };

	inline const int NO_GROUP{ 0 }; // "0" is the non-group value (group ID = 0)

	// Return values when accessing to the Database
	inline const int OPEN_SUCCESS{ 100 }; // Return value when Database and tables exists and also Database could be opened
	inline const int OPEN_NOT_SUCCESS{ 101 }; // Return value when Tables are not existing
	inline const int NO_TABLES { 102 }; // Return value when Tables are not existing
	inline const int NO_DB { 103 }; // Return value when Database is not existing


	namespace Users
	{
		// Table Users
		inline const std::string col_username{ "username" };
		inline const std::string col_name{ "name" };
		inline const std::string col_hashpass{ "hash_password" };
		inline const std::string col_salt{ "salt" };
		inline const std::string col_usertype{ "user_type" };
		inline const std::string col_groupID{ "group_ID" };

	}

	namespace Groups
	{
		// Table Groups
		inline const std::string col_ID{ "ID" };
		inline const std::string col_group_name{ "name" };
		inline const std::string col_users{ "users" };
		inline const std::string col_inrequests{ "in_requests" };
		inline const std::string col_outrequests{ "out_requests" };
		inline const std::string col_status{ "status" };

		// Status values
		inline const QString status_active{ "active" }; // status_active is when a group is active
		inline const QString status_inactive{ "inactive" }; // status_inactive is when a group is considered as removed or inactive
	}

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
	inline const QString loggingIcon2 { "resources/icons/signin.png" };
	inline const QString saveIcon{ "resources/icons/saveIcon.png" };
	inline const QString prefIcon{ "resources/icons/preferences.png" };
	inline const QString groupPrefIcon{ "resources/icons/groupPref2.png" };
	inline const QString groupPrefNewsIcon{ "resources/icons/groupPrefNews.png" };
	inline const QString noGroupfIcon{ "resources/icons/no_group.png" };
	inline const QString raisedNews{ "resources/icons/newThing.png" };
	inline const QString reqToGroup{ "resources/icons/reqToGroup.png" };
	inline const QString reqFromGroup{ "resources/icons/reqFromGroup.png" };
	inline const QString removeIcon{ "resources/icons/remove_user.png" };
	inline const QString removeExpense { "resources/icons/remove_expense.png"};
	inline const QString accept{ "resources/icons/accept.png" };
	inline const QString decline{ "resources/icons/decline.png" };
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