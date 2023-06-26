#ifndef PREFERENCES_H
#define PREFERENCES_H

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
#include <QGroupBox>
#include "User.h"
#include "constants.h"
#include "labelButton.h"
#include "sql_functions.h"


//////////////////////////////////////////////////////////////////////////////////////////
// Description:                                                                         
// It creates the Window (QDialog) for managing the preferences / configuration
//////////////////////////////////////////////////////////////////////////////////////////

class confWindow : public QDialog
{

	Q_OBJECT

private:
	User* currentUser{};
	std::vector<User*> users{};

	// Informing QMessageBox
	QMessageBox* userInfoBox = new QMessageBox();
		
	QLineEdit* NameEdit = new QLineEdit();
	QLineEdit* userNameEdit = new QLineEdit();
	QLineEdit* passEdit = new QLineEdit();
	QLineEdit* repeatPassEdit = new QLineEdit();
	QLineEdit* oldPassEdit = new QLineEdit();

	QLabel* passLab = new QLabel("Password:");
	QLabel* oldPassLab = new QLabel("Old Password:");
	QLabel* repeatPassLab = new QLabel("Repeat New Password:");

	labelButton* editNameButton = new labelButton("Edit");
	labelButton* editUserNameButton = new labelButton("Edit");
	labelButton* editPassButton = new labelButton("Change Password");
	labelButton* cancelPassButton = new labelButton("Cancel");

	QPushButton* saveButton = new QPushButton("Save");
	QPushButton* cancelButton = new QPushButton("Cancel");

public:

	QSize winSize{};

	confWindow(User* user) : currentUser(user)
	{
		// Setting Window title and icon
		setWindowIcon(QIcon(icons::prefIcon));
		setWindowTitle("Preferences");
		
		// Setting Information Window title and icon
		userInfoBox->setWindowTitle("Processing Users Database");
		userInfoBox->setWindowIcon(QIcon(icons::expTrackerIcon));		

		/////////////////////////////////////////////////////////////////////
		// Setting the content in configuration Windows
		/////////////////////////////////////////////////////////////////////

		QGridLayout* prefLayout = new QGridLayout();

		// User Type
		QGroupBox* userTypeBox = new QGroupBox("User Type");
		QVBoxLayout* userTypeLay{ new QVBoxLayout() };
		QLabel* userTypeLab = new QLabel(currentUser->getUserType());
		userTypeLay->addWidget(userTypeLab);

		userTypeBox->setLayout(userTypeLay);
		prefLayout->addWidget(userTypeBox, 0, 0);

		// Name Settings
		QGroupBox* nameBox = new QGroupBox("Name");
		QVBoxLayout* nameLay{ new QVBoxLayout() };
		nameLay->addWidget(NameEdit);
		NameEdit->setText(currentUser->getUserRName());
		disableFields(true, NameEdit);
		nameLay->addWidget(editNameButton);

		nameBox->setLayout(nameLay);		
		prefLayout->addWidget(nameBox, 1, 0);

		// User Name Settings
		QGroupBox* userNameBox = new QGroupBox("User Name");
		QVBoxLayout* userNameLay{ new QVBoxLayout() };		
		userNameEdit->setText(currentUser->getUserName());
		userNameLay->addWidget(userNameEdit);
		disableFields(true, userNameEdit);
		userNameLay->addWidget(editUserNameButton);
		
		userNameBox->setLayout(userNameLay);
		prefLayout->addWidget(userNameBox, 2, 0);

		// Password Settings
		QGroupBox* passBox = new QGroupBox("Password");
		QVBoxLayout* passLay{ new QVBoxLayout() };
		oldPassEdit->setEchoMode(QLineEdit::Password);
		passLay->addWidget(oldPassLab);
		passLay->addWidget(oldPassEdit);
		oldPassEdit->hide();
		oldPassLab->hide();
			
		passEdit->setEchoMode(QLineEdit::Password);
		passEdit->setText("XXXXXXXX");
		passLay->addWidget(passLab);
		passLay->addWidget(passEdit);
		disableFields(true, passEdit);		
				
		repeatPassEdit->setEchoMode(QLineEdit::Password);
		passLay->addWidget(repeatPassLab);
		passLay->addWidget(repeatPassEdit);
		repeatPassEdit->hide();
		repeatPassLab->hide();

		passLay->addWidget(editPassButton);
		passLay->addWidget(cancelPassButton);
		cancelPassButton->hide();

		passBox->setLayout(passLay);
		prefLayout->addWidget(passBox, 3, 0);

		// Adding Buttons
		QHBoxLayout* hLay{ new QHBoxLayout() };
		hLay->addWidget(saveButton);
		hLay->addWidget(cancelButton);
		prefLayout->addLayout(hLay, 4, 0, Qt::AlignCenter);

		setLayout(prefLayout);

		/////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////

		QObject::connect(editNameButton, &QPushButton::clicked, [=]() {
			disableFields(false, NameEdit);
			});

		QObject::connect(editUserNameButton, &QPushButton::clicked, [=]() {
			disableFields(false, userNameEdit);
			});

		QObject::connect(editPassButton, &QPushButton::clicked, [=]() {
			changePassword();			
			});

		QObject::connect(cancelPassButton, &QPushButton::clicked, [=]() {
			cancelPassEdit();
			});		

		QObject::connect(cancelButton, &QPushButton::clicked, [=]() {
			this->close();
			});
		

		/////////////////////////////////////////////////////////////////////

		// Set fixed Dialog size (user cannot resize it)
		setWindowFlag(Qt::MSWindowsFixedSizeDialogHint, true);		

	}

	void disableFields(bool disable, QLineEdit* fieldEdit)
	{
		fieldEdit->setReadOnly(disable);
		fieldEdit->setStyleSheet(disable ? fieldColor::disable : fieldColor::enable);
	}

	void changePassword()
	{
		winSize = size();

		oldPassEdit->show();
		oldPassLab->show();

		repeatPassLab->show();
		repeatPassEdit->show();

		passLab->setText("New Password:");
		passEdit->setText("");

		disableFields(false, passEdit);

		editPassButton->hide();
		cancelPassButton->show();

		setFixedHeight(sizeHint().height());
	}

	void cancelPassEdit()
	{
		oldPassEdit->hide();
		oldPassLab->hide();

		repeatPassLab->hide();
		repeatPassEdit->hide();

		oldPassEdit->setText("");
		repeatPassEdit->setText("");

		disableFields(true, passEdit);

		editPassButton->show();
		cancelPassButton->hide();

		passLab->setText("Password:");
		passEdit->setText("XXXXXXXX");

		setFixedSize(winSize);
	}

	//Load Users from DB and store them in 'users' vector
	void loadUsersFromDB()
	{
		sqlite3* db{};

		if (checkAndOpenSQLiteDB(db, userInfoBox, { DB::tableUsers }) == DB::OPEN_SUCCESS)
		{
			//records gets the output of the SELECT query given by 'getRecords()'
			std::vector<QStringList> records{ getRecords(db, DB::tableUsers, "username, name, salt, hash_password, user_type") };

			// Load the database users to the Users vector 'users'
			if (!records.empty())
			{
				for (const QStringList& record : records)
				{
					if (!(record[0] == currentUser->getUserName()))
					{
						User* userDB{ new User(record[0]) };
						userDB->setUserRName(record[1]);
						userDB->setSalt(record[2]);
						userDB->setHashPassword(record[3]);
						userDB->setSaltDB(record[2].toUtf8());
						userDB->setHashPasswordDB(record[3].toUtf8());
						userDB->setUserType(record[4]);
						users.push_back(userDB);
					}
				}
			}
			else
			{
				userInfoBox->setIcon(QMessageBox::Warning);
				userInfoBox->setText("There are no users in Database");
				userInfoBox->exec();
			}
			closeSQLiteDB(db);
		}

	}

	void restartContents()
	{
		NameEdit->setText(currentUser->getUserRName());
		disableFields(true, NameEdit);

		userNameEdit->setText(currentUser->getUserName());
		disableFields(true, userNameEdit);

		disableFields(true, passEdit);

		oldPassEdit->hide();
		oldPassLab->hide();

		repeatPassLab->hide();
		repeatPassEdit->hide();

		editPassButton->show();
		cancelPassButton->hide();

		passLab->setText("Password:");
		passEdit->setText("XXXXXXXX");
		oldPassEdit->setText("");
		repeatPassEdit->setText("");

		setFixedSize(winSize);		

	}

	~confWindow()
	{
		// Free 'User' memory from users vector pointers
		for (User* user : users)
		{
			//if (user != *userIt)
			delete user;
			qDebug() << "\nDeleting user: " << user->getUserName() << "\n";
		}

		delete editNameButton;
		delete editUserNameButton;
		delete editPassButton;

	}

};



#endif
