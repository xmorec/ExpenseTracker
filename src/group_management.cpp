#include "group_management.h"

//  Adds or removes a user to the requests column (in_requests or out_requests) according the MODE in Database. Returns TRUE (successful Database). False otherwise
bool updateRequestCol(sqlite3* db, User* user, const std::string& col_requests, int groupID, int MODE)
{

	if (MODE == ADD_REQUEST) // Add a user to the 'requests' column
	{

		// Selecting the cell from Groups Table regarding the "out_requests" or "in_requests" column in for the selected "group ID"
		QString requests{ getRecords(db, DB::tableGroups, col_requests, "WHERE " + DB::Groups::col_ID + " = '" + std::to_string(groupID) + "'")[0][0] };

		// Adding the user to the QString 'requests' (in_requests or out_requests)
		if (requests.isEmpty()) // When request column has not any other user
		{
			requests = user->getUserName();
		}
		else //When one or more users want were previously added to the request column
		{
			requests += ", " + user->getUserName();
		}

		// Setting the condition for updating "requests" column (Groups Table)
		std::string conditionGroups{ DB::Groups::col_ID + " = '" + std::to_string(groupID) + "'" };

		// Updating the column (in_request or out_request) in database and returning true or false according if the Update was successful or not
		return updateRecords(db, DB::tableGroups, col_requests, requests.toStdString(), conditionGroups);


	}
	else if (MODE == REMOVE_REQUEST) // Remove a user from 'requests' column
	{
		// Selecting the cell from Groups Table regarding the "requests" column in for the selected "group ID"
		QString inRequests{ getRecords(db, DB::tableGroups, col_requests, "WHERE " + DB::Groups::col_ID + " = '" + std::to_string(groupID) + "'")[0][0] };


		if (inRequests.contains(user->getUserName())) // Checks whether the user exists or not to this requests column
		{
			if (inRequests.startsWith(user->getUserName() + ", ")) // When user is the first who appears in the requests field
			{
				inRequests.remove(user->getUserName() + ", ");
			}
			else if (inRequests.contains(", " + user->getUserName())) // When user is not the first appearing in requests field
			{
				inRequests.remove(", " + user->getUserName());
			}
			else // This scenario only is possible when the current user is the only appearing in requests field
			{
				inRequests.clear();
			}
		}

		// Setting the condition for updating "in_requests" column (Groups Table)
		std::string conditionGroups{ DB::Groups::col_ID + " = '" + std::to_string(groupID) + "'" };

		// Removing a user from the 'requests' column. Returns true for a successful Database update. False otherwise
		return updateRecords(db, DB::tableGroups, col_requests, inRequests.toStdString(), conditionGroups);
	}
	else
	{
		// When no Mode was selected
		return false;
	}



}