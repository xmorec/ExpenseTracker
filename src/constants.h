#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QString>

namespace DB
{
	inline const std::string dbFilePath{ "resources/database/ExpenseTrackerDB.db" };
	inline const std::string tableUsers{ "Users" };

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
}


#endif