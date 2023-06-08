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
	QLabel* userLabel{ new QLabel("User") };
	QVBoxLayout* vLay{ new QVBoxLayout() };

	QPushButton* signInButton{ new QPushButton("LOGIN") };
	QPushButton* cancelButton{ new QPushButton("CANCEL") };
	labelButton* createUserButton{ new labelButton("Create User") };
	labelButton* submitUserButton{ new labelButton("Submit") };

	QMessageBox* userInfoBox = new QMessageBox();

	// Users vector that contains all users in the program-system
	std::vector<User*> users{};	

public:

	// Generates the main QDialog for the Logging Session
	loggingWindow();

	//Load Users from DB and store them in 'users' vector
	void loadUsersFromDB();

	// Execute the Logging In process and Checking
	void loggingIn();

	// Executes the Creation User Process and Checking
	void createUser();

	// Insert the created user to DB
	bool insertUserToDB(sqlite3* db, const QString& newUsername);

	// Generating the hashPassword from the plain password and the generated Salt
	QString hashPassword(const QString& plainPassword, const QString& userSalt);

	// Salt generating with a size of 'saltSize' bytes
	QString generateSalt(int saltSize = 8);

	// Checks if the input user name (newUsername) has a correct text format
	bool isCorrectNameFormat(const QString& newUsername);
	
	// Load Create User View in the QDialog
	void loadCreateUserView();

	// Load Loggin View in the QDialog
	void loadLoggingView();

	// Returns the logging status flag
	bool getLoggingStatus();
	
};

#endif