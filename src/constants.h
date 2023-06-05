#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QString>


inline const std::string dbFilePath{ "resources/database/ExpenseTrackerDB.db" };

namespace icons
{
	inline const QString expTrackerIcon{ "resources/icons/ExpTrIcon.png" };
	inline const QString loggingIcon { "resources/icons/logsession.png" };
	inline const QString saveIcon{ "resources/icons/saveIcon.png" };	
}


#endif