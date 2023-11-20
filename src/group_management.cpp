#include "group_management.h"

//  Adds or removes (MODE) a user to Groups Table of the database column (col_update) according the group. Returns TRUE (successful Database update) or False otherwise
bool updateRequestCol(sqlite3* db, User* user, const std::string& col_update, Group* group, const int MODE)
{

	// Selecting the cell from a Table regarding the 'col_update' for the selected "group ID"
	QString requests{ getRecords(db, DB::tableGroups, col_update, "WHERE " + DB::Groups::col_ID + " = '" + std::to_string(group->ID) + "'")[0][0] };


	if (MODE == ADD_REQUEST) // Add a user to the 'col_update'
	{

		// Adding the user to the QString 'requests' 
		if (requests.isEmpty()) // When 'col_update' column has not any other user
		{
			requests = user->getUserName();
		}
		else //When one or more users want were previously added to the 'col_update' column
		{
			requests += ", " + user->getUserName();
		}

	}
	else if (MODE == REMOVE_REQUEST) // Remove a user from 'col_update' column
	{

		if (requests.contains(user->getUserName())) // Checks whether the user exists or not to this 'col_update' column
		{
			if (requests.startsWith(user->getUserName() + ", ")) // When user is the first who appears in the 'col_update' column
			{
				requests.remove(user->getUserName() + ", ");
			}
			else if (requests.contains(", " + user->getUserName())) // When user is not the first appearing in 'col_update' column
			{
				requests.remove(", " + user->getUserName());
			}
			else // This scenario only is possible when the current user is the only appearing in 'col_update' column
			{
				requests.clear();
			}
		}

	}
	else
	{
		// When no Mode (Adding/Removing) was selected
		return false;
	}

	// Setting the condition for updating 'col_update' column
	std::string conditionGroups{ DB::Groups::col_ID + " = '" + std::to_string(group->ID) + "'" };

	// Updating the 'col_update' column in database and returning true or false according if the Update was successful or not
	bool updateFlag { updateRecords(db, DB::tableGroups, col_update, requests.toStdString(), conditionGroups) };

	// In case the updating records in database was successful, group from 'groups' vector is updated as well
	if (updateFlag)
	{
		if (col_update == DB::Groups::col_inrequests)
		{
			group->in_requests = requests.split(", ");

			// In case there only value in the column is an empty string (""), group->col_update is set to 0
			if (group->in_requests[0] == "") group->in_requests.clear();

		}
		else if (col_update == DB::Groups::col_outrequests)
		{
			group->out_requests = requests.split(", ");

			// In case there only value in the column is an empty string (""), group->col_update is set to 0
			if (group->out_requests[0] == "") group->out_requests.clear();

		}
		else if (col_update == DB::Groups::col_users)
		{
			group->users = requests.split(", ");

			// In case there only value in the column is an empty string (""), group->col_update is set to 0
			if (group->users[0] == "") group->users.clear();
		}
	}

	return updateFlag;



}

// Deletes a group from Database and from 'groups' vector. Returns true for a successful removing, false otherwise
bool deleteGroup(sqlite3* db, Group* group, QMessageBox* userInfoBox, bool dbOpen)
{

	bool deleteFlag{ true };

	if (dbOpen) // In case Database should be opened and checked
	{
		deleteFlag = checkAndOpenSQLiteDB(db, userInfoBox, { DB::tableGroups, DB::tableUsers }) == DB::OPEN_SUCCESS;
	}

	// Setting the group as inactive in Database and removing all In Requests and Out Requests
	std::string condition = DB::Groups::col_ID + " = '" + std::to_string(group->ID) + "'";

	if (deleteFlag &= updateRecords(db, DB::tableGroups, DB::Groups::col_status, DB::Groups::status_inactive.toStdString(), condition)
		&& updateRecords(db, DB::tableGroups, DB::Groups::col_inrequests, "", condition)
		&& updateRecords(db, DB::tableGroups, DB::Groups::col_outrequests, "", condition))
	{

		// Setting the group as inactive in 'groups' vector
		group->status = DB::Groups::status_inactive;

		// Clearing the Users of the group
		group->users.clear();

		// Clearing the In Requests and Out Requests of the group
		group->in_requests.clear();
		group->out_requests.clear();

	}

	if (dbOpen) // In case Database should were opened and check within this function
	{
		closeSQLiteDB(db);
	}

	return deleteFlag;


}