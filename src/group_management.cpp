#include "group_management.h"

//  Adds or removes a user to the requests column (in_requests or out_requests) according the MODE in Database. Returns TRUE (successful Database). False otherwise
bool updateRequestCol(sqlite3* db, User* user, const std::string& col_requests, Group* group, const int MODE)
{

	// Selecting the cell from Groups Table regarding the "out_requests" or "in_requests" column in for the selected "group ID"
	QString requests{ getRecords(db, DB::tableGroups, col_requests, "WHERE " + DB::Groups::col_ID + " = '" + std::to_string(group->ID) + "'")[0][0] };


	if (MODE == ADD_REQUEST) // Add a user to the 'requests' column
	{

		// Adding the user to the QString 'requests' (in_requests or out_requests)
		if (requests.isEmpty()) // When request column has not any other user
		{
			requests = user->getUserName();
		}
		else //When one or more users want were previously added to the request column
		{
			requests += ", " + user->getUserName();
		}

	}
	else if (MODE == REMOVE_REQUEST) // Remove a user from 'requests' column
	{

		if (requests.contains(user->getUserName())) // Checks whether the user exists or not to this requests column
		{
			if (requests.startsWith(user->getUserName() + ", ")) // When user is the first who appears in the requests field
			{
				requests.remove(user->getUserName() + ", ");
			}
			else if (requests.contains(", " + user->getUserName())) // When user is not the first appearing in requests field
			{
				requests.remove(", " + user->getUserName());
			}
			else // This scenario only is possible when the current user is the only appearing in requests field
			{
				requests.clear();
			}
		}

	}
	else
	{
		// When no Mode was selected
		return false;
	}

	// Setting the condition for updating "requests" column (Groups Table)
	std::string conditionGroups{ DB::Groups::col_ID + " = '" + std::to_string(group->ID) + "'" };

	// Updating the column (in_request or out_request) in database and returning true or false according if the Update was successful or not
	bool updateFlag { updateRecords(db, DB::tableGroups, col_requests, requests.toStdString(), conditionGroups) };

	// In case the updating records in database was successful, group is updated as well
	if (updateFlag)
	{
		if (col_requests == DB::Groups::col_inrequests)
		{
			group->in_requests = requests.split(", ");
		}
		else if (col_requests == DB::Groups::col_outrequests)
		{
			group->out_requests = requests.split(", ");
		}
	}

	return updateFlag;



}