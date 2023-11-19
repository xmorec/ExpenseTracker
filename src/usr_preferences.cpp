#include "usr_preferences.h"

// Constructs the main Preference Window interface and functiontality
confWindow::confWindow(User* currentUser, std::vector<User*>& users, std::vector<Group*>& groups) 
	: currentUser(currentUser), users (users), groups (groups)
{
	// Setting Window title and icon
	setWindowIcon(QIcon(icons::prefIcon));
	setWindowTitle("Preferences");

	// Setting Information Window title and icon
	userInfoBox->setWindowTitle("Preferences");
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
	QGridLayout* prefLayout = new QGridLayout(this);

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

	// Adding Buttons Edit, Save and Cancel and its layout
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
	QGroupBox* userNameBox = new QGroupBox("Username");	
	QVBoxLayout* userNameLay{ new QVBoxLayout() };
	fieldEdit[userNamePos]->setText(currentUser->getUserName());
	userNameLay->addWidget(fieldEdit[userNamePos]);
	disableFields(true, fieldEdit[userNamePos]);

	// Adding Buttons Edit, Save and Cancel and its layout
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

	// Adding Buttons Edit, Save and Cancel and its layout
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

	// Adding the Information Message Widget to User Management section
	userManLay->addWidget(infoManText);	
	infoManText->setAlignment(Qt::AlignCenter);
	infoManText->hide();

	// Setting the Users Init vector with the users vector when initializing the preferences window
	usersInit = users;
	usersInit.erase(std::find(usersInit.begin(), usersInit.end(), currentUser));

	// The User Management section is filled with each user information
	loadUserManagement();

	// Updating the total users number (when intializing preferences window)
	totalInitUsers = usersInit.size();

	// Adding buttons for Saving or Canceling actions made durin User Management 
	if (usersInit.size() > 0) // If there are other users in Database
	{
		labelButton* saveManButton{ new labelButton("Save") };
		labelButton* cancelManButton{ new labelButton("Cancel") };
		userManButtonsLay->addWidget(saveManButton);
		userManButtonsLay->addWidget(cancelManButton);
		userManLay->addLayout(userManButtonsLay);
		userManButtonsLay->setAlignment(Qt::AlignLeft);

		// Action when pressing in the Save Button from User Management
		QObject::connect(saveManButton, &QPushButton::clicked, [=]() {
			saveManagement();
			});

		// Action when pressing in the Cancel Button from User Management
		QObject::connect(cancelManButton, &QPushButton::clicked, [=]() {
			restoreManContent();
			// Setting the windows size to current size
			QTimer::singleShot(10, [=]() {
				setFixedHeight(sizeHint().height());
				});
			});
	}
	else // If there are no Users to show (no users in Database appart from the current one)
	{
		infoManText->setText("There are no other\nexisting users to show.");
		infoManText->show();
	}	

	// Setting the layout for User Management
	userManBox->setLayout(userManLay);

	// User Management is only shown to Admin users
	if (currentUser->getUserType().toStdString() == DB::UserType::admin)
	{
		prefLayout->addWidget(userManBox, 4, 0);
	}

	// Setting the Layout into the Dialog (usr_preferences)
	setLayout(prefLayout);

	/////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////

	// Set fixed Dialog size (user cannot resize it)
	setWindowFlag(Qt::MSWindowsFixedSizeDialogHint, true);
}

// Getting the current size of the Preference Window
void confWindow::adjustWinSize()
{
	winSize = size();
}

// Loading User Management content
void confWindow::loadUserManagement()
{
	// Declaring the QIcon used for the icon button for removing Users
	static const QIcon rmvIcon(icons::removeIcon);

	// Clearing the widgets and layouts regarding each user for this section
	userLabels.clear();
	userRoles.clear();
	userHLays.clear();
	removeButtons.clear();

	// Clearing the users that were going to be removed or modified
	removedUsers.clear();
	modifiedUsers.clear();

	// In case there are other users in Database, no need for showing information message
	if (usersInit.size() > 0)
	{
		infoManText->hide();
	}

	// Number of users
	int usersNum{ static_cast<int>(usersInit.size()) };

	// Reserving memory for the Labels and Comboboxes for users
	userLabels.reserve(usersNum);
	userRoles.reserve(usersNum);

	// Reserving memory for the Horizontal Layout for each user
	userHLays.reserve(usersNum);

	// Reserving memory for icon buttons for each users	
	removeButtons.reserve(usersNum);

	// Reserving memory for vectors containing users that are going to be removed or modified
	removedUsers.reserve(usersNum);
	modifiedUsers.reserve(usersNum);

	// Reserving memory for a vector that stores the last user role before it changes in the ComboBox 'userRoles'	
	lastRoles.reserve(usersNum);

	// Declaring the List for Roles that is set into each Combobox
	static const QStringList rolesList{ QString::fromStdString(DB::UserType::user), QString::fromStdString(DB::UserType::admin) };

	// Filling the User Management Layout
	for (int pos = 0; pos < usersNum; ++pos)
	{
		// Getting the Username and User Role (UserType)
		userLabels.push_back(new QLabel(usersInit[pos]->getUserName()));
		userRoles.push_back(new QComboBox());
		userRoles.back()->addItems(rolesList);
		userRoles.back()->setCurrentText(usersInit[pos]->getUserType());
		lastRoles.push_back(new QString(userRoles.back()->currentText()));

		// Adding removing icon button for each user
		removeButtons.push_back(new iconButton(rmvIcon));
		removeButtons.back()->setToolTip("Remove user");
		removeButtons.back()->setIconSize(19);

		// Creating the Horizontal Layout for each user and filling it
		userHLays.push_back(new QHBoxLayout());
		userHLays.back()->addWidget(userLabels.back());
		userHLays.back()->addWidget(userRoles.back());
		userHLays.back()->addWidget(removeButtons.back());

		// Adding the HLayout to the VLayout of User Management Settings
		userManLay->addLayout(userHLays.back());

		// Action executed when changing the user role
		QObject::connect(userRoles[pos], &QComboBox::currentTextChanged, [=]() {
			updateRole(pos);
			});

		// Action executed when pressing on "Remove User" Icon Button
		QObject::connect(removeButtons[pos], &QPushButton::clicked, [=]() {
			removeUser(pos);
			});
	}
}

// Removing user layout from User Management section
void confWindow::removeUser(int pos)
{
	// A Message Dialog Box is opened asking to delete the user
	QMessageBox msgBox{};
	msgBox.setWindowTitle("Deleting User");
	msgBox.setWindowIcon(QIcon(icons::expTrackerIcon));
	msgBox.setText("Are you sure you want to delete this user?");
	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	msgBox.setIcon(QMessageBox::Question);
	int answer = msgBox.exec();	

	// When Pressing "Yes" button (deleting user)
	if (answer == QMessageBox::Yes)
	{
		// Deleting the layout content
		deleteLayout(userHLays[pos]);

		// A reference to the user going to be deleted is saved in the vector 'removedUsers'
		removedUsers.push_back(pos);
	}

	// rmvUserFlag is used to know if all users - except the logged one - are desired to be removed
	bool rmvUsersFlag{ true };
	for (const auto& layout : userHLays)
	{
		if (layout) //If there is at least one layout of userHLays that is not a nullptr (which means it is not deleted), flag is set to true.
		{
			rmvUsersFlag = false;
		}
	}

	// If all users - except the logged one - are desired to be removed (when rmvUsersFlag is true)
	if (rmvUsersFlag)
	{
		infoManText->setText("All users are deleted.\nSave or Cancel the changes.");
		infoManText->show();
	}

	// Updating the windows size
	QTimer::singleShot(10, [=]() {
		setFixedHeight(sizeHint().height());
		});	
}

// Sets the vector containing users which are going to be updated
void confWindow::updateRole(int pos)
{
	// Disabling capturing signals (ComboBox Text changed) in order to update the combobox without getting into this function again 
	bool emitingSignalState = userRoles[pos]->blockSignals(true);

	// Declaring the newRole variable as the selected role
	QString newRole{ userRoles[pos]->currentText() };

	// A Message Dialog Box is opened asking to modify the user Role
	QMessageBox msgBox{};
	msgBox.setWindowTitle("Changing User Role");
	msgBox.setWindowIcon(QIcon(icons::expTrackerIcon));
	msgBox.setText("Are you sure you want change this user role from '" + *lastRoles[pos] + "' to '" + newRole + "?");
	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	msgBox.setIcon(QMessageBox::Question);
	int answer = msgBox.exec();

	// When Pressing "Yes" button (changing user role)
	if (answer == QMessageBox::Yes)
	{
		// A reference to the user going to be modified is saved in the vector 'modifiedUsers'
		modifiedUsers.push_back(pos);

		// Update the new last role (current one)
		*lastRoles[pos] = userRoles[pos]->currentText();
	}
	else if (answer == QMessageBox::No) // When Pressing "No" button (Not changing user role)
	{
		// Restarting the user role as it was before
		userRoles[pos]->setCurrentText(*lastRoles[pos]);
	}

	//Restoring the blocking signal state of the signals of 'userRoles' ComboBox
	userRoles[pos]->blockSignals(emitingSignalState);
}

// Saves the changes did in the User Management
void confWindow::saveManagement()
{	
	userInfoBox->setWindowTitle("Saving User Management");

	// No changes were done in the user fields of User Management
	if (removedUsers.size() == 0 && modifiedUsers.size() == 0)
	{
		userInfoBox->setText("There were no changes made in User Management");
		userInfoBox->setIcon(QMessageBox::Information);
		userInfoBox->exec();
		return;
	}	

	sqlite3* db{};

	// Checking and opening the SQLite Database
	if (checkAndOpenSQLiteDB(db, userInfoBox, { DB::tableUsers, DB::tableExpenses, DB::tableIncome }) == DB::OPEN_SUCCESS)
	{

		// Initializing a flag to check if the deleting process in DataBase was propperly conducted or not (true = correct saving, false = error occured)
		bool deletingStatusFlag{ true };

		if (removedUsers.size() > 0)
		{
			// Sorting the RemovedUsers vector in order to avoid making errors related to exceeding User vector size when executing the loop below
			std::sort(removedUsers.begin(), removedUsers.end());

			// Loop that removes the desired useres beginning with the last user User vector
			for (int i = removedUsers.size() - 1; i >= 0; --i)
			{
				// Setting the clause for Deleting the desired user from Database
				std::string userName{ usersInit.at(removedUsers[i])->getUserName().toStdString() };
				std::string clause{ "WHERE username = '" + userName + "'" };

				// If deleting User, Income and Expenses of desired User from Database was successful
				if (deletingRecords(db, DB::tableUsers, clause) == true
					&& deletingRecords(db, DB::tableExpenses, clause) == true
					&& deletingRecords(db, DB::tableIncome, clause) == true)
				{
					// Refreshing the Deleting status Flag
					deletingStatusFlag &= true;

					// Removing user from 'users' vector
					auto rmvUser_it{ std::find(users.begin(), users.end(), usersInit[removedUsers[i]]) };
					users.erase(rmvUser_it);

					// Detecting if a modified User was also removed
					if (modifiedUsers.size() > 0)
					{
						auto modUserPos{ std::find(modifiedUsers.begin(), modifiedUsers.end(), removedUsers[i]) };

						// If a modified user is also going to be removed, it is erased from 'modifiedUsers' vector
						if (modUserPos != modifiedUsers.end())
						{ 
							modifiedUsers.erase(modUserPos);
						}
					}
				}
				else // Deleting process in Database was not successful
				{
					deletingStatusFlag = false;
				}
			}
		}

		// Initializing a flag to check if the modifying process in DataBase was propperly conducted or not (true = correct saving, false = error occured)
		bool modifyStatusFlag{ true };

		// If any user is modified
		if (modifiedUsers.size() > 0)
		{
			// Loop that modifies the desired users in the database
			for (int i = 0; i < modifiedUsers.size(); ++i)
			{
				// Preparig the clause for Modifiying the desired user from Database
				std::string userName{ usersInit.at(modifiedUsers[i])->getUserName().toStdString() };
				std::string condition{ "username = '" + userName + "'" };
				std::string role {userRoles[modifiedUsers[i]]->currentText().toStdString()};

				// modifying Status Flag is updated according the result of the updating Database process
				if (modifyStatusFlag &= updateRecords(db, DB::tableUsers, DB::Users::col_usertype, role, condition))
				{
					// Modifying user at users vector according his new Role
					auto modUser_it{ std::find(users.begin(), users.end(), usersInit[modifiedUsers[i]]) };
					(*modUser_it)->setUserType(userRoles[modifiedUsers[i]]->currentText());					
				}
			}
		}

		closeSQLiteDB(db);

		// When deleting process in Database was successfuly done
		if (deletingStatusFlag && modifyStatusFlag)
		{
			// After all users were removed according removedUsers vector, this vector is cleared
			removedUsers.clear();

			// After all users were modified according modifiedUsers vector, this vector is cleared
			modifiedUsers.clear();

			userInfoBox->setText("User Management has been saved!");
			userInfoBox->setIcon(QMessageBox::Information);
			userInfoBox->exec();

		}
		else // When there was an error deleting 
		{
			userInfoBox->setText("Deleting users could not be succesfully done");
			userInfoBox->setIcon(QMessageBox::Critical);

			userInfoBox->exec();
		}

		
		if(users.size() == 1) // If there are no Users to show (no users in Database appart from the current one)
		{
			infoManText->setText("There are no other\nexisting users to show.");			
			infoManText->show();

			// The button layout is removed since no more changes could be done with no users
			deleteLayout(userManButtonsLay);			
			userManLay->removeItem(userManButtonsLay);
		}

	}

	// Setting the default winSize to the current size
	QTimer::singleShot(50, [=]() {
		winSize = size();
		});	
}

// Restores the the content according database
void confWindow::restoreManContent()
{
	// The current horizontal layouts for all users are removed
	for (QHBoxLayout*& hlay : userHLays)
	{
		deleteLayout(hlay);
	}

	// The button layout is removed in order to put it in the proper place in the User Management Layout
	userManLay->removeItem(userManButtonsLay);

	// Setting the Users Init vector with the users vector when initializing the preferences window
	usersInit = users;
	usersInit.erase(std::find(usersInit.begin(), usersInit.end(), currentUser));

	// Data from Database is read and loaded again to the User Management field
	loadUserManagement();

	// Button layout is added again to the User Management Layout, just below the users layout
	userManLay->addLayout(userManButtonsLay);
}

// Updates a user parameter according the "save button" pressed for a specific field (pos)
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

// Clears all content from a specific layout
void confWindow::deleteLayout(QHBoxLayout*& layout)
{
	// Check if layout exists
	if (layout != nullptr)
	{
		// Delete all widgets from the layout
		while (QLayoutItem* item = layout->takeAt(0))
		{
			if (QWidget* widget = item->widget())
			{
				delete widget;
			}
			delete item;
		}

		// Free memory
		delete layout;

		// Set the layout pointer to nullptr
		layout = nullptr;
	}
}

// Checks the new name of the current User and updates it in the Database
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

// Checks the new Username of the current User and updates it in the Database
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

// Checks and updates the password user into database
void confWindow::updatePassword()
{
	userInfoBox->setWindowTitle("Password modification");

	// Gets the password from password fields
	QString oldPass { oldPassEdit->text() };
	QString newPass { fieldEdit[passPos]->text() };
	QString repPass { repeatPassEdit->text() };
	
	// Gets the user Salt and HashPassword
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

		// Sets the values that are going to be used to SQL Query
		std::string hashPassDB{ hashPass.toUtf8().toStdString() };
		std::string condition{ "username = '" + currentUser->getUserName().toStdString() + "'" };		
		std::string userName{ currentUser->getUserName().toStdString() };

		// Sets a flag with the result (true/false) of updating data in database with new Password
		bool hpassFlag{ updateRecords(db, DB::tableUsers, DB::Users::col_hashpass, hashPassDB, DB::Users::col_username + " = '" + userName + "'") };
		bool saltFlag{ true };		

		//'it' and 'maxTries' are used to assure a proper hash password to Database is generated
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

			// New Hash Password and Salt are inserted to Database
			hpassFlag = updateRecords(db, DB::tableUsers, DB::Users::col_hashpass, hashPassDB, DB::Users::col_username + " = '" + userName + "'");
			saltFlag = updateRecords(db, DB::tableUsers, DB::Users::col_salt, userSalt.toUtf8().toStdString(), DB::Users::col_username + " = '" + userName + "'");

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

			// The password section view of Prefences window is reset
			cancelField(passPos);
			saveButtons[passPos]->setDefault(false); // Save button is not pressed by default when clicking Enter

			userInfoBox->setText("Password was successfuly changed");
			userInfoBox->setIcon(QMessageBox::Information);
			userInfoBox->exec();

		}
		else //When updating User Password and Salt to the DB was not successful
		{
			// Previous values of HashPassword and Salt are inserted again to the Database
			updateRecords(db, DB::tableUsers, DB::Users::col_hashpass, userPass.toUtf8().toStdString(), DB::Users::col_username + " = '" + userName + "'");
			updateRecords(db, DB::tableUsers, DB::Users::col_salt, userSalt.toUtf8().toStdString(), DB::Users::col_username + " = '" + userName + "'");

			userInfoBox->setText("New password could not be successfuly saved. Please, try again.");
			userInfoBox->setIcon(QMessageBox::Warning);
			userInfoBox->exec();
		}

		closeSQLiteDB(db);
	}	
}

// Show or hide elements of Preferences window according the "Edit" button pressed
void confWindow::editField(int pos)
{
	// Edit Fields are enabled
	disableFields(false, fieldEdit[pos]);

	// Showing / Hiding buttons
	editButtons[pos]->hide();
	saveButtons[pos]->show();
	cancelButtons[pos]->show();

	// When pressing Enter, 'Save Button' acts as clicked
	saveButtons[pos]->setDefault(true);

	// When Password "Edit" button is pressed
	if (pos == passPos)
	{
		oldPassEdit->show();
		oldPassLab->show();

		repeatPassLab->show();
		repeatPassEdit->show();

		passLab->setText("New Password:");
		fieldEdit[passPos]->setText("");

		// Assures the windows has a propper size
		setFixedHeight(sizeHint().height());
	}
}

// Show or hide elements of Preferences window according the "Cancel" button pressed
void confWindow::cancelField(int pos)
{
	// Edit Fields are disabled
	disableFields(true, fieldEdit[pos]);

	// Showing / Hiding buttons
	editButtons[pos]->show();
	saveButtons[pos]->hide();
	cancelButtons[pos]->hide();

	// The values in the Edit fields are restored
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

		// Assures the windows has a propper size
		setFixedSize(winSize);
	}
}

// Disables / Enables the input field (fieldEdit) according a flag (bool disable)
void confWindow::disableFields(bool disable, QLineEdit* fieldEdit)
{
	// Makes the Edit Field only readable (writting it is not allowed)
	fieldEdit->setReadOnly(disable);

	// Sets the color of the Field according if it is disabled or enabled
	fieldEdit->setStyleSheet(disable ? fieldColor::disable : fieldColor::enable);
}

// Reset the content shown in the Preferences Dialog Window to default
void confWindow::restartContents()
{
	// Setting the Edit fields according default settings
	fieldEdit[namePos]->setText(currentUser->getUserRName());
	fieldEdit[userNamePos]->setText(currentUser->getUserName());
	fieldEdit[passPos]->setText("XXXXXXXX");

	// Setting the layout to "Password:"
	passLab->setText("Password:");

	// Hiding the Old Password and Repeat Password fields
	oldPassEdit->hide();
	oldPassLab->hide();

	repeatPassLab->hide();
	repeatPassEdit->hide();

	// Setting the Edit fields of Old Password and Repeat Password empty
	oldPassEdit->setText("");
	repeatPassEdit->setText("");

	// Showing/Hiding buttons according default settings
	for (int pos = 0; pos < numEditBoxes; ++pos)
	{
		editButtons[pos]->show();
		cancelButtons[pos]->hide();
		saveButtons[pos]->hide();
		disableFields(true, fieldEdit[pos]);
	}

	// Restore content shown in the User Management section
	restoreManContent();

	// Set the fixed windows size
	setFixedSize(winSize);
}