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

	// Current Logged User
	User* currentUser{};

	// Vector storing all Users
	std::vector<User*> users{};

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


public:

	QSize winSize{};

	confWindow(User* user);

	void saveField(int pos);

	void updateName();

	void updateUserName();

	void updatePassword();

	void editField(int pos);

	void cancelField(int pos);

	void disableFields(bool disable, QLineEdit* fieldEdit);

	//Load Users from DB and store them in 'users' vector
	void loadUsersFromDB();

	void removeUser(int pos);

	void saveManagement();

	void cancelManagement();

	void restartContents();

	~confWindow();

	signals:
		// Emited signal when username is changed
		void userNameMod();	

};



#endif
