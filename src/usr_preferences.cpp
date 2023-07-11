#include "usr_preferences.h"


confWindow::confWindow(User* user) : currentUser(user)
{
	// Setting Window title and icon
	setWindowIcon(QIcon(icons::prefIcon));
	setWindowTitle("Preferences");

	// Setting Information Window title and icon
	userInfoBox->setWindowTitle("Processing Users Database");
	userInfoBox->setWindowIcon(QIcon(icons::expTrackerIcon));

	/////////////////////////////////////////////////////////////////////
	// DECLARING BUTTONS AND THEIR FUNCTIONS
	/////////////////////////////////////////////////////////////////////

	// Declaring Edit Buttons
	editButtons.resize(numEditBoxes);
	for (int pos = 0; pos < numEditBoxes; ++pos)
	{
		editButtons[pos] = new labelButton("Edit");
		QObject::connect(editButtons[pos], &QPushButton::clicked, [=]() {
			editField(pos);
			});
	}

	// Declaring Save Buttons
	saveButtons.resize(numEditBoxes);
	for (int pos = 0; pos < numEditBoxes; ++pos)
	{
		saveButtons[pos] = new labelButton("Save");
		QObject::connect(saveButtons[pos], &QPushButton::clicked, [=]() {
			saveField(pos);
			});
	}

	// Declaring Cancel Buttons
	cancelButtons.resize(numEditBoxes);
	for (int pos = 0; pos < numEditBoxes; ++pos)
	{
		cancelButtons[pos] = new labelButton("Cancel");
		QObject::connect(cancelButtons[pos], &QPushButton::clicked, [=]() {
			cancelField(pos);
			});
	}


	/////////////////////////////////////////////////////////////////////
	// SETTING THE FIELD AND BUTTONS STRUCTURE OF THE PREFERENCE WINDOWS
	/////////////////////////////////////////////////////////////////////

	// Declaring the QLine Edit Fields
	fieldEdit.resize(numEditBoxes);
	for (QLineEdit*& lineEdit : fieldEdit) { lineEdit = new QLineEdit(); }

	// Declaring the Main Layout for the Preferences Window
	QGridLayout* prefLayout = new QGridLayout();

	//////////////////////////////////
	// User Type
	//////////////////////////////////
	QGroupBox* userTypeBox = new QGroupBox("User Type");
	QVBoxLayout* userTypeLay{ new QVBoxLayout() };
	QLabel* userTypeLab = new QLabel(currentUser->getUserType());
	userTypeLay->addWidget(userTypeLab);

	userTypeBox->setLayout(userTypeLay);
	prefLayout->addWidget(userTypeBox, 0, 0);
	fieldEdit[namePos]->setFocusPolicy(Qt::ClickFocus);

	//////////////////////////////////
	// Name Settings
	//////////////////////////////////
	QGroupBox* nameBox = new QGroupBox("Name");
	QVBoxLayout* nameLay{ new QVBoxLayout() };
	nameLay->addWidget(fieldEdit[namePos]);
	fieldEdit[namePos]->setText(currentUser->getUserRName());
	disableFields(true, fieldEdit[namePos]);

	QHBoxLayout* nameButtonsLay{ new QHBoxLayout() };
	nameButtonsLay->addWidget(editButtons[namePos]);
	nameButtonsLay->addWidget(saveButtons[namePos]);
	nameButtonsLay->addWidget(cancelButtons[namePos]);
	nameLay->addLayout(nameButtonsLay);
	nameButtonsLay->setAlignment(Qt::AlignLeft);
	saveButtons[namePos]->hide();
	cancelButtons[namePos]->hide();

	nameBox->setLayout(nameLay);
	prefLayout->addWidget(nameBox, 1, 0);

	fieldEdit[userNamePos]->setFocusPolicy(Qt::ClickFocus);

	//////////////////////////////////
	// User Name Settings
	//////////////////////////////////
	QGroupBox* userNameBox = new QGroupBox("User Name");
	QVBoxLayout* userNameLay{ new QVBoxLayout() };
	fieldEdit[userNamePos]->setText(currentUser->getUserName());
	userNameLay->addWidget(fieldEdit[userNamePos]);
	disableFields(true, fieldEdit[userNamePos]);

	QHBoxLayout* userNameButtonsLay{ new QHBoxLayout() };
	userNameButtonsLay->addWidget(editButtons[userNamePos]);
	userNameButtonsLay->addWidget(saveButtons[userNamePos]);
	userNameButtonsLay->addWidget(cancelButtons[userNamePos]);
	userNameLay->addLayout(userNameButtonsLay);
	userNameButtonsLay->setAlignment(Qt::AlignLeft);
	saveButtons[userNamePos]->hide();
	cancelButtons[userNamePos]->hide();

	userNameBox->setLayout(userNameLay);
	prefLayout->addWidget(userNameBox, 2, 0);

	//////////////////////////////////
	// Password Settings
	//////////////////////////////////
	QGroupBox* passBox = new QGroupBox("Password");
	QVBoxLayout* passLay{ new QVBoxLayout() };
	oldPassEdit->setEchoMode(QLineEdit::Password);
	passLay->addWidget(oldPassLab);
	passLay->addWidget(oldPassEdit);
	oldPassEdit->hide();
	oldPassLab->hide();

	fieldEdit[passPos]->setEchoMode(QLineEdit::Password);
	fieldEdit[passPos]->setText("XXXXXXXX");
	passLay->addWidget(passLab);
	passLay->addWidget(fieldEdit[passPos]);
	disableFields(true, fieldEdit[passPos]);
	fieldEdit[passPos]->setFocusPolicy(Qt::ClickFocus);

	repeatPassEdit->setEchoMode(QLineEdit::Password);
	passLay->addWidget(repeatPassLab);
	passLay->addWidget(repeatPassEdit);
	repeatPassEdit->hide();
	repeatPassLab->hide();

	QHBoxLayout* passButtonsLay{ new QHBoxLayout() };
	passButtonsLay->addWidget(editButtons[passPos]);
	passButtonsLay->addWidget(saveButtons[passPos]);
	passButtonsLay->addWidget(cancelButtons[passPos]);
	passLay->addLayout(passButtonsLay);
	passButtonsLay->setAlignment(Qt::AlignLeft);
	saveButtons[passPos]->hide();
	cancelButtons[passPos]->hide();

	passBox->setLayout(passLay);
	prefLayout->addWidget(passBox, 3, 0);

	//////////////////////////////////
	// User Management Settings (only for Admin users)
	//////////////////////////////////
	QGroupBox* userManBox = new QGroupBox("User Management");
	QVBoxLayout* userManLay{ new QVBoxLayout() };

	//Load Users from DB and store them in 'users' vector
	loadUsersFromDB();

	// Number of users
	int usersNum{ static_cast<int>(users.size()) };

	// Declaring the Labels and Comboboxes for users
	std::vector<QLabel*> userLabels{};
	userLabels.reserve(usersNum);
	std::vector<QComboBox*> userRoles{};
	userRoles.reserve(usersNum);
	std::vector<QHBoxLayout*> userHLays{};
	userHLays.reserve(usersNum);

	// Declaring the icon buttons for each users
	QIcon rmvIcon(icons::removeIcon);
	std::vector<iconButton*> removeButtons{};
	removeButtons.reserve(usersNum);	

	// Declaring the List for Roles that is set into each Combobox
	QStringList rolesList{ QString::fromStdString(DB::UserType::user), QString::fromStdString(DB::UserType::admin)};

	// Filling the User Management Layout
	for (int pos = 0; pos < usersNum; ++ pos)
	{
		// Getting the Username and User Role (UserType)
		userLabels.push_back(new QLabel(users[pos]->getUserName()));
		userRoles.push_back(new QComboBox());
		userRoles.back()->addItems(rolesList);
		userRoles.back()->setCurrentText(users[pos]->getUserType());

		// Adding removing icon button for each user
		removeButtons.push_back(new iconButton(rmvIcon));
		removeButtons.back()->setIconSize(19);

		QObject::connect(removeButtons.back(), &QPushButton::clicked, [=]() {
			removeUser(pos);
			});

		// Creating the Horizontal Layout for each user and filling it
		userHLays.push_back(new QHBoxLayout());
		userHLays.back()->addWidget(userLabels.back());
		userHLays.back()->addWidget(userRoles.back());
		userHLays.back()->addWidget(removeButtons.back());

		// Adding the HLayout to the VLayout of User Management Settings
		userManLay->addLayout(userHLays.back());
	}	

	QHBoxLayout* useManButtonsLay{ new QHBoxLayout() };
	labelButton* saveManButton{ new labelButton("Save") };
	labelButton* cancelManButton{ new labelButton("Cancel") };
	useManButtonsLay->addWidget(saveManButton);
	useManButtonsLay->addWidget(cancelManButton);
	userManLay->addLayout(useManButtonsLay);
	useManButtonsLay->setAlignment(Qt::AlignLeft);

	userManBox->setLayout(userManLay);
	prefLayout->addWidget(userManBox, 4, 0);


	QObject::connect(saveManButton, &QPushButton::clicked, [=]() {
		saveManagement();
		});

	QObject::connect(cancelManButton, &QPushButton::clicked, [=]() {
		cancelManagement();
		});

	// Setting the Layout into the Dialog (usr_preferences)
	setLayout(prefLayout);

	/////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////

	// Set fixed Dialog size (user cannot resize it)
	setWindowFlag(Qt::MSWindowsFixedSizeDialogHint, true);
}

void confWindow::removeUser(int pos)
{
	QMessageBox msgBox{};
	msgBox.setWindowTitle("Deleting User");
	msgBox.setWindowIcon(QIcon(icons::expTrackerIcon));
	msgBox.setText("Are you sure you want to delete this user?");
	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);	
	msgBox.setIcon(QMessageBox::Question);
	int answer = msgBox.exec();

	if(answer == QMessageBox::Yes)
	{
	
	}

}

void confWindow::saveManagement()
{
	userInfoBox->setText("User Management has been saved!");
	userInfoBox->setIcon(QMessageBox::Information);
	userInfoBox->exec();
}

void confWindow::cancelManagement()
{

}

void confWindow::saveField(int pos)
{
	switch (pos)
	{
	case namePos:
		updateName();
		break;
	case userNamePos:
		updateUserName();
		break;
	case passPos:
		updatePassword();
		break;
	}
}

void confWindow::updateName()
{
	userInfoBox->setWindowTitle("Name modification");

	// Get the input username
	QString newName = fieldEdit[namePos]->text();

	// Checks if there is no change in the edit field
	if (newName == currentUser->getUserRName())
	{
		// The Edit Field and buttons regarding this field are set to an initial state
		cancelField(namePos);		
		saveButtons[namePos]->setDefault(false); // Save button is not pressed by default when clicking Enter
		return;
	}

	// Checks if a user with user permissions tries to set their name to "admin"
	if (!(currentUser->getUserType().toStdString() == DB::UserType::admin) && (newName.toLower() == "admin"))
	{
		userInfoBox->setText("Name cannot be \"admin\"");
		userInfoBox->setIcon(QMessageBox::Warning);
		userInfoBox->exec();
		return;
	}

	// Checks if the input name has a correct Format, and if so, this is updated into Database
	if (isCorrectNameFormat(users, newName, userInfoBox))
	{
		sqlite3* db{};

		// Checks if DB exists and if so, the input parameter is updated in the table
		if (checkAndOpenSQLiteDB(db, userInfoBox, { DB::tableUsers }) == DB::OPEN_SUCCESS)
		{
			// Flag that gets true in case the input parameter is correctly written into Database
			bool updateFlag = updateRecords(db, DB::tableUsers, "name", newName.toStdString(), "username = '" + currentUser->getUserName().toStdString() + "'");

			closeSQLiteDB(db);

			if (updateFlag)
			{
				currentUser->setUserRName(newName);

				// If the input parameter is correctly written into Database, the Edit Field and buttons regarding this field are set to an initial state
				cancelField(namePos);
				saveButtons[namePos]->setDefault(false); // Save button is not pressed by default when clicking Enter
			}
			else
			{
				userInfoBox->setText("Name could not be successfuly updated");
				userInfoBox->setIcon(QMessageBox::Critical);
				userInfoBox->exec();
			}

		}
	}

}

void confWindow::updateUserName()
{
	userInfoBox->setWindowTitle("Username modification");

	// Get the input username
	QString newUsername = fieldEdit[userNamePos]->text();

	// Checks if there is no change in the edit field
	if (newUsername == currentUser->getUserName())
	{
		// The Edit Field and buttons regarding this field are set to an initial state
		cancelField(userNamePos);
		saveButtons[userNamePos]->setDefault(false); // Save button is not pressed by default when clicking Enter
		return;
	}

	// Checks if the input name has a correct Format, and if so, this is updated into Database
	if (isCorrectUserNameFormat(users, newUsername, userInfoBox))
	{
		sqlite3* db{};

		// Checks if DB exists and if so, the input parameter is updated in the table
		if (checkAndOpenSQLiteDB(db, userInfoBox, { DB::tableUsers, DB::tableExpenses, DB::tableIncome }) == DB::OPEN_SUCCESS)
		{
			// Flag that gets true in case the input parameter is correctly written into Database
			bool NameFlag = updateRecords(db, DB::tableUsers, "username", newUsername.toStdString(), "username = '" + currentUser->getUserName().toStdString() + "'");
			bool ExpensesFlag = updateRecords(db, DB::tableExpenses, "username", newUsername.toStdString(), "username = '" + currentUser->getUserName().toStdString() + "'");
			bool IncomeFlag = updateRecords(db, DB::tableIncome, "username", newUsername.toStdString(), "username = '" + currentUser->getUserName().toStdString() + "'");

			closeSQLiteDB(db);

			if (NameFlag && ExpensesFlag && IncomeFlag)
			{
				currentUser->setUserName(newUsername);

				// If the input parameter is correctly written into Database, the Edit Field and buttons regarding this field are set to an initial state
				cancelField(userNamePos);
				saveButtons[userNamePos]->setDefault(false); // Save button is not pressed by default when clicking Enter

				// Since username is updated, other parts of the program should be warned
				emit userNameMod();
			}
			else
			{
				userInfoBox->setText("Username could not be successfuly updated");
				userInfoBox->setIcon(QMessageBox::Critical);
			}
		}
	}
}

void confWindow::updatePassword()
{

	userInfoBox->setWindowTitle("Password modification");

	// Get the input username
	QString oldPass { oldPassEdit->text() };
	QString newPass { fieldEdit[passPos]->text() };
	QString repPass { repeatPassEdit->text() };
	
	QString userSalt{ currentUser->getSalt() };
	QString userPass{ currentUser->getHashPassword() };

	// When Old Password field has not the same Password as the current one
	if(!(hashPassword(oldPass, userSalt) == userPass))
	{
		userInfoBox->setText("Old password is not your current password");
		userInfoBox->setIcon(QMessageBox::Warning);
		userInfoBox->exec();
		return;
	}

	// When the Repeated Password has not the same Password as the New input Password
	if (!(newPass == repPass))
	{
		userInfoBox->setText("New password is not the same as the repeated password");
		userInfoBox->setIcon(QMessageBox::Warning);
		userInfoBox->exec();
		return;
	}
	
	//When the New password is the same current one
	if (hashPassword(newPass, userSalt) == userPass)
	{
		cancelField(passPos);
		saveButtons[passPos]->setDefault(false); // Save button is not pressed by default when clicking Enter
		return;
	}

	sqlite3* db{};

	// Checks if DB exists and if so, the input parameter is updated in the table
	if (checkAndOpenSQLiteDB(db, userInfoBox, { DB::tableUsers }) == DB::OPEN_SUCCESS)
	{		

		QString hashPass{ hashPassword(newPass, userSalt) };		

		std::string hashPassDB{ hashPass.toUtf8().toStdString() };
		std::string condition{ "username = '" + currentUser->getUserName().toStdString() + "'" };		

		std::string userName{ currentUser->getUserName().toStdString() };
		bool hpassFlag{ updateRecords(db, DB::tableUsers, DB::col_hashpass, hashPassDB, DB::col_username + " = '" + userName + "'") };
		bool saltFlag{ true };		

		int it{ 0 };
		int maxTries{ 7 };

		// If hash password or salt are generating troubles to Database, they are generated again
		// in order to trying another User Insertion to Database (max tries: 'maxTries')
		// insertRecord() is the function used to send the SQL Query to database
		while (it < maxTries && (hpassFlag == false || saltFlag == false))
		{

			// Generating Salt
			userSalt = generateSalt(8);

			// Generating Hash Password
			hashPass = hashPassword(newPass, userSalt);

			hashPassDB = hashPass.toUtf8().toStdString();
			condition =  "username = '" + currentUser->getUserName().toStdString() + "'";

			hpassFlag = updateRecords(db, DB::tableUsers, DB::col_hashpass, hashPassDB, DB::col_username + " = '" + userName + "'");
			saltFlag = updateRecords(db, DB::tableUsers, DB::col_salt, userSalt.toUtf8().toStdString(), DB::col_username + " = '" + userName + "'");

			++it;
		}

		// When updating User Password and Salt to the DB was successfuly done
		if (it < maxTries)
		{
			// Store Salt, Hash Password to the current User
			currentUser->setHashPassword(hashPass);
			currentUser->setSalt(userSalt);
			currentUser->setHashPasswordDB(hashPass.toUtf8());
			currentUser->setSaltDB(userSalt.toUtf8());

			cancelField(passPos);
			saveButtons[passPos]->setDefault(false); // Save button is not pressed by default when clicking Enter

			userInfoBox->setText("Password was successfuly changed");
			userInfoBox->setIcon(QMessageBox::Information);
			userInfoBox->exec();

		}
		else
		{
			updateRecords(db, DB::tableUsers, DB::col_hashpass, userPass.toUtf8().toStdString(), DB::col_username + " = '" + userName + "'");
			updateRecords(db, DB::tableUsers, DB::col_salt, userSalt.toUtf8().toStdString(), DB::col_username + " = '" + userName + "'");

			userInfoBox->setText("New password could not be successfuly saved. Please, try again.");
			userInfoBox->setIcon(QMessageBox::Warning);
			userInfoBox->exec();
		}

		closeSQLiteDB(db);
	}
	
}

void confWindow::editField(int pos)
{
	disableFields(false, fieldEdit[pos]);
	editButtons[pos]->hide();
	saveButtons[pos]->show();
	cancelButtons[pos]->show();

	// When pressing Enter, 'Save Button' acts as clicked
	saveButtons[pos]->setDefault(true);

	if (pos == passPos)
	{
		oldPassEdit->show();
		oldPassLab->show();

		repeatPassLab->show();
		repeatPassEdit->show();

		passLab->setText("New Password:");
		fieldEdit[passPos]->setText("");

		setFixedHeight(sizeHint().height());
	}
}

void confWindow::cancelField(int pos)
{
	disableFields(true, fieldEdit[pos]);
	editButtons[pos]->show();
	saveButtons[pos]->hide();
	cancelButtons[pos]->hide();

	if (pos == namePos)
	{
		fieldEdit[pos]->setText(currentUser->getUserRName());
	}

	if (pos == userNamePos)
	{
		fieldEdit[pos]->setText(currentUser->getUserName());
	}

	if (pos == passPos)
	{
		oldPassEdit->hide();
		oldPassLab->hide();

		repeatPassLab->hide();
		repeatPassEdit->hide();

		oldPassEdit->setText("");
		repeatPassEdit->setText("");

		passLab->setText("Password:");
		fieldEdit[pos]->setText("XXXXXXXX");

		setFixedSize(winSize);
	}

}

void confWindow::disableFields(bool disable, QLineEdit* fieldEdit)
{
	fieldEdit->setReadOnly(disable);
	fieldEdit->setStyleSheet(disable ? fieldColor::disable : fieldColor::enable);
}

//Load Users from DB and store them in 'users' vector
void confWindow::loadUsersFromDB()
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

void confWindow::restartContents()
{
	fieldEdit[namePos]->setText(currentUser->getUserRName());
	fieldEdit[userNamePos]->setText(currentUser->getUserName());
	fieldEdit[passPos]->setText("XXXXXXXX");

	passLab->setText("Password:");

	oldPassEdit->hide();
	oldPassLab->hide();

	repeatPassLab->hide();
	repeatPassEdit->hide();

	oldPassEdit->setText("");
	repeatPassEdit->setText("");

	for (int pos = 0; pos < numEditBoxes; ++pos)
	{
		editButtons[pos]->show();
		cancelButtons[pos]->hide();
		saveButtons[pos]->hide();
		disableFields(true, fieldEdit[pos]);
	}

	setFixedSize(winSize);
}

confWindow::~confWindow()
{
	// Free 'User' memory from users vector pointers
	for (User* user : users)
	{
		//if (user != *userIt)
		delete user;
		qDebug() << "\nDeleting user: " << user->getUserName() << "\n";
	}

}