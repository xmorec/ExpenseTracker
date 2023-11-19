#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QBoxLayout>
#include <QLineEdit>
#include <QString>
#include <QPushButton>
#include <QLabel>
#include <QDialog>
#include <QMessageBox>
#include <QGroupBox>
#include <QComboBox>
#include <QTimer>
#include "User.h"
#include "constants.h"
#include "labelButton.h"
#include "sql_functions.h"
#include "iconButton.h"

//////////////////////////////////////////////////////////////////////////////////////////
// Description:                                                                         
// It creates the Window (QDialog) for managing the preferences / configuration
//////////////////////////////////////////////////////////////////////////////////////////

class confWindow : public QDialog
{

	Q_OBJECT

private:

	// Windows Size value
	QSize winSize{};

	// Current Logged User
	User* currentUser{};

	// Vector storing all Users
	std::vector<User*>& users;

	// Vector storing all Groups
	std::vector<Group*>& groups;

	// Vector storing all Users except for the current one
	std::vector<User*> usersInit{};

	// Total number of users when initializing preferences window
	int totalInitUsers{};

	// Informing QMessageBox
	QMessageBox* userInfoBox = new QMessageBox();
	
	// Indexes used to point to fields related to some User parameters (Name, Username, Password, ...)
	static constexpr int namePos{ 0 };
	static constexpr int userNamePos{ 1 };
	static constexpr int passPos{ 2 };
	const int numEditBoxes{ 3 };

	// Line Edit field vector related to User parameters
	std::vector<QLineEdit*> fieldEdit{};

	// Line Edit and Labels related to Password Fields
	QLineEdit* repeatPassEdit = new QLineEdit();
	QLineEdit* oldPassEdit = new QLineEdit();
	QLabel* passLab = new QLabel("Password:");
	QLabel* oldPassLab = new QLabel("Old Password:");
	QLabel* repeatPassLab = new QLabel("Repeat New Password:");

	// Declaring Label Buttons to Edit/Save/Cancel operations
	std::vector<labelButton*> editButtons{};
	std::vector<labelButton*> saveButtons{};	
	std::vector<labelButton*> cancelButtons{};

	/*
	User Management Widgets, Layouts and User Vectors
	*/
	// Layouts for user management and buttons
	QVBoxLayout* userManLay{ new QVBoxLayout() };
	std::vector<QHBoxLayout*> userHLays{};
	QHBoxLayout* userManButtonsLay{ new QHBoxLayout() };

	// Vectors for Labels, Comboboxes and Remove Icon buttons for User Management
	std::vector<QLabel*> userLabels{};	
	std::vector<QComboBox*> userRoles{};	
	std::vector<QString*> lastRoles {};
	std::vector<iconButton*> removeButtons{};

	// Vector storing Users going to be removed
	std::vector<int> removedUsers{};

	// Vector storing Users whose properties are going to be changed
	std::vector<int> modifiedUsers{};

	// Information text in Management User section
	QLabel* infoManText{ new QLabel() };

public:

	// Constructs the main Preference Window interface and functiontality
	confWindow(User* currentUser, std::vector<User*>& users, std::vector<Group*>& groups);

	// Getting the current size of the Preference Window
	void adjustWinSize();

	// Updates a user parameter according the "save button" pressed for a specific field (pos)
	void saveField(int pos);

	// Checks the new name of the current User and updates it in the Database
	void updateName();

	// Checks the new Username of the current User and updates it in the Database
	void updateUserName();

	// Checks and updates the password user into database
	void updatePassword();

	// Show or hide elements of Preferences window according the "Edit" button pressed
	void editField(int pos);

	// Show or hide elements of Preferences window according the "Cancel" button pressed
	void cancelField(int pos);

	// Disables / Enables the input field (fieldEdit) according a flag (bool disable)
	void disableFields(bool disable, QLineEdit* fieldEdit);

	// Removing user layout from User Management section
	void removeUser(int pos);

	// Loading User Management content
	void loadUserManagement();

	// Sets the vector containing users which are going to be updated
	void updateRole(int pos);

	// Clears all content from a specific layout
	void deleteLayout(QHBoxLayout*& layout);

	// Saves the changes did in the User Management
	void saveManagement();

	// Restores the the content according database
	void restoreManContent();

	// Reset the content shown in the Preferences Dialog Window to default
	void restartContents();

	signals:
		// Emited signal when username is changed
		void userNameMod();	

};



#endif
