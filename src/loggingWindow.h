#ifndef LOGGINGWINDOW_H
#define LOGGINGWINDOW_H

#include <QBoxLayout>
#include <QLineEdit>
#include <QString>
#include <QPushButton>
#include <QLabel>
#include <QDialog>
#include <QMessageBox>
#include <QCryptographicHash>
#include <QRandomGenerator>
#include <QRegularExpression>
#include "User.h"
#include "constants.h"
#include "labelButton.h"
#include "sql_functions.h"

//////////////////////////////////////////////////////////////////////////////////////////
// Description:                                                                         
// It creates the Window (QDialog) for managing the logging session and user creation
//////////////////////////////////////////////////////////////////////////////////////////


class loggingWindow : public QDialog
{

	Q_OBJECT

private:

	// logging status flag. true : successful logging, false : not successful logging (Dialog closed)
	bool loggingStatus{ false }; 

	QLineEdit* userEdit{ new QLineEdit() };
	QLineEdit* passEdit{ new QLineEdit() };
	static constexpr int lineEditWidth{ 200 };

	QLabel* titlePopup{ new QLabel("Sign In") };
	QLabel* passLabel{ new QLabel("Password") };
	QLabel* userLabel{ new QLabel("Username") };
	QVBoxLayout* vLay{ new QVBoxLayout() };

	QPushButton* signInButton{ new QPushButton("LOGIN") };
	QPushButton* cancelButton{ new QPushButton("CANCEL") };
	labelButton* createUserButton{ new labelButton("Create User") };
	labelButton* submitUserButton{ new labelButton("Submit") };

	QMessageBox* userInfoBox = new QMessageBox();

	// Users vector that contains all users in the program-system
	std::vector<User*> users{};	

	// Vector storing all Groups
	std::vector<Group*> groups{};

	// Logged User
	User* loggedUser{nullptr};

public:

	// Generates the main QDialog for the Logging Session
	loggingWindow();

	//Load Users from DB and store them in 'users' vector
	void loadUsersFromDB();

	// Load the groups from Database and load them into 'groups' vector
	void loadGroupsFromDB();

	// Execute the Logging In process and Checking
	void loggingIn();

	// Executes the Creation User Process and Checking
	void createUser();

	// Insert the created user to DB
	bool insertUserToDB(sqlite3* db, const QString& newUsername);

	// Load Create User View in the QDialog
	void loadCreateUserView();

	// Load Loggin View in the QDialog
	void loadLoggingView();

	// Returns the logging status flag
	bool getLoggingStatus();
	
	// Return the users vector
	std::vector<User*>& getUsers();

	// Returns the groups vector
	std::vector<Group*>& getGroups();

	// Returns the logged User
	User* getCurrentUser();

};

#endif