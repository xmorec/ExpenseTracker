#include "loggingWindow.h"

// Generates the main QDialog for the Logging Session
loggingWindow::loggingWindow() : QDialog()
{
	setWindowIcon(QIcon(icons::expTrackerIcon));
	setWindowTitle("Expense Tracker");

	userInfoBox->setWindowIcon(QIcon(icons::loggingIcon));

	// Set fixed Dialog size (user cannot resize it)
	setWindowFlag(Qt::MSWindowsFixedSizeDialogHint, true);

	userEdit->setFixedWidth(lineEditWidth);
	passEdit->setFixedWidth(lineEditWidth);

	// Giving a own style to the Title
	QFont font(titlePopup->font().family(), 11, QFont::Bold);
	titlePopup->setFont(font);

	// Filling Dialog Content (Labels, User/Password Fields, Buttons)
	vLay->addWidget(titlePopup, 0, Qt::AlignCenter);

	// Adding the Logging Image
	QLabel* imageLabel = new QLabel();
	QPixmap image(icons::loggingIcon2);
	imageLabel->setPixmap(image.scaled(QSize(100, 100)));
	vLay->addWidget(imageLabel, 0, Qt::AlignCenter);

	// Adding Filed Edits and Labels regarding User Credentials
	vLay->addWidget(userLabel);
	vLay->addWidget(userEdit);
	vLay->addWidget(passLabel);
	vLay->addWidget(passEdit);
	vLay->addWidget(createUserButton, 0, Qt::AlignCenter);
	vLay->addWidget(signInButton, 0, Qt::AlignCenter);
	vLay->addWidget(submitUserButton, 0, Qt::AlignCenter);
	submitUserButton->hide();
	vLay->addWidget(cancelButton, 0, Qt::AlignCenter);
	cancelButton->hide();
	setLayout(vLay);

	// When pressing Enter, signInButton acts as clicked
	signInButton->setDefault(true);

	// Hiding the plain password
	passEdit->setEchoMode(QLineEdit::Password);

	// Load Users from DB and store them in 'users' vector
	loadUsersFromDB();

	// Load Create User View
	QObject::connect(createUserButton, &QPushButton::clicked, [=]() {
		loadCreateUserView();
		});

	// Execute the Logging In process and Checking
	QObject::connect(signInButton, &QPushButton::clicked, [=]() {
		loggingIn();
		});

	// Executes the Creation User Process and Checking
	QObject::connect(submitUserButton, &QPushButton::clicked, [=]() {
		createUser();
		});

	// Load the Signing In View
	QObject::connect(cancelButton, &QPushButton::clicked, [=]() {
		loadLoggingView();
		});

	// Executes the QDialog
	exec();
}

//Load Users from DB and store them in 'users' vector
void loggingWindow::loadUsersFromDB()
{

	sqlite3* db{};

	//Check if DB exists, and if so, Users table is open
	if (checkAndOpenSQLiteDB(db, userInfoBox, { DB::tableUsers }) == DB::OPEN_SUCCESS)
	{
		//Get the number of users with admin privileges
		int adminUsersNum = getRecordNumber(db, DB::tableUsers, "WHERE user_type = '" + DB::UserType::admin + "'");

		// If there is no admin in database, one is created
		if (adminUsersNum == 0)
		{
			// Tries to insert the new admin to DB and also adds it to the 'users' vector
			if (insertUserToDB(db, "admin"))
			{
				//Show infoBox	
				userInfoBox->setWindowIcon(QIcon(icons::expTrackerIcon));
				userInfoBox->setWindowTitle("Expense Tracker");
				userInfoBox->setIcon(QMessageBox::Information);
				userInfoBox->setText("Admin User is created\nUser: \"admin\"\nPass: \"admin\"");
			}
			else
			{
				userInfoBox->setWindowIcon(QIcon(icons::expTrackerIcon));
				userInfoBox->setIcon(QMessageBox::Warning);
				userInfoBox->setText("An admin should be created but it was not possible.\nRestart the application");
			}			
			userInfoBox->exec();
		}

		//records gets the output of the SELECT query given by 'getRecords()'
		std::vector<QStringList> records{ getRecords(db, DB::tableUsers, "username, name, salt, hash_password, group_ID, user_type") };

		// Load the database users to the Users vector 'users'
		if (!records.empty())
		{
			for (const QStringList& record : records)
			{
				User* userDB{ new User(record[0]) };
				userDB->setUserRName(record[1]);
				userDB->setSalt(record[2]);
				userDB->setHashPassword(record[3]);
				userDB->setSaltDB(record[2].toUtf8());
				userDB->setHashPasswordDB(record[3].toUtf8());
				userDB->setGroupID(record[4]);
				userDB->setUserType(record[5]);
				users.push_back(userDB);
			}
		}

		closeSQLiteDB(db);
	}
}

// Execute the Logging In process and Checking
void loggingWindow::loggingIn()
{
	userInfoBox->setWindowTitle("Sign In");

	// Getting the input user name from the User Field
	QString inputUserName = userEdit->text();

	// Getting the iterator pointing at that user from 'users' vector that has the same user name as the one from the input (in case there is a match)
	auto userIt = std::find_if(users.begin(), users.end(), [=](User* userx) { return inputUserName.toLower() == userx->getUserName().toLower(); });

	// User exists with the input user name
	if (!(userIt == users.end()))
	{
		// Getting the input plain password from the Password Field
		QString inputPass{ passEdit->text() };

		// Getting the 'Salt' from DB
		QString userSalt{ QString::fromUtf8((*userIt)->getSaltDB()) };

		// Checks if the input password is the correct one for the selected user
		if (hashPassword(inputPass, userSalt) == QString::fromUtf8((*userIt)->getHashPasswordDB()))
		{
			// In case the password is correct, the Logged User is set accordingly
			loggedUser = (*userIt);

			// Free 'User' memory from users vector pointers
			for (User* user : users)
			{
				if (user != *userIt)
					delete user;
			}

			// In case the password is correct, the logging status is set to "true" and QDialog is closed
			loggingStatus = true;
			this->close();
		}
		else // Incorrect password
		{
			userInfoBox->setText("Incorrect Password.");
			userInfoBox->setIcon(QMessageBox::Warning);
			userInfoBox->exec();
		}
	}
	else // User does not exist with the input user name
	{
		userInfoBox->setText("User not found.");
		userInfoBox->setIcon(QMessageBox::Warning);
		userInfoBox->exec();
	}
}

// Executes the Creation User Process and Checking
void loggingWindow::createUser()
{
	userInfoBox->setWindowTitle("Create User");

	// Get the input username
	QString newUsername = userEdit->text();

	//Checks if input user name has a correct format and, if so, hash password and salt is generated for this user
	if (isCorrectUserNameFormat(users, newUsername, userInfoBox))
	{

		sqlite3* db{};

		if (checkAndOpenSQLiteDB(db, userInfoBox, { DB::tableUsers }) == DB::OPEN_SUCCESS)
		{

			// Tries to insert the new User to DB and also adds it to the 'users' vector
			if (insertUserToDB(db, newUsername))
			{
				userInfoBox->setIcon(QMessageBox::Information);
				userInfoBox->setText("User Created!");
			}
			else
			{
				userInfoBox->setIcon(QMessageBox::Warning);
				userInfoBox->setText("User could not be created!");
			}

			closeSQLiteDB(db);

			//Show infoBox
			userInfoBox->exec();
		}
		
		// The Logging View is loaded
		loadLoggingView();
	}
}

// This function tries to insert the new User to DB and also adds it to the 'users' vector. Returns 'true' for a successful insertion and 'false' otherwise
bool loggingWindow::insertUserToDB(sqlite3* db, const QString& newUsername)
{
	//Creating new User
	User* newUser{ new User(newUsername) };

	// Getting the input plain password from Password Field
	QString plainPassword{ newUsername == "admin" ? "admin" : passEdit->text()};

	// Generating Salt
	QString userSalt{ generateSalt(8) };

	// Generating Hash Password
	QString hashPass{ hashPassword(plainPassword, userSalt) };

	// Setting the User Type
	std::string userType{ newUsername == "admin" ? DB::UserType::admin : DB::UserType::user };

	// Setting the parameters to be sent to the SQL Query
	std::string newUsernameStr{ newUsername.toStdString() };

	// Creating the 'values sentence' for the INSERT INTO query for the Database (" INSERT INTO table VALUES ('val1', 'val2', ...) ")
	std::vector<std::string> values
	{
		newUsernameStr,
		newUsernameStr,
		userSalt.toUtf8().toStdString(),
		hashPass.toUtf8().toStdString(),
		std::to_string(DB::NO_GROUP),
		userType
	};

	int it{ 0 };
	int maxTries{ 7 };

	// If hash password or salt are generating troubles to Database, they are generated again
	// in order to trying another User Insertion to Database (max tries: 'maxTries')
	// insertRecord() is the function used to send the SQL Query to database
	while (it < maxTries && (insertRecord(db, DB::tableUsers, values) != true))
	{
		// Generating Salt
		userSalt = generateSalt(8);

		// Generating Hash Password
		hashPass = hashPassword(plainPassword, userSalt);

		// Updating the values vector to be sent to the SQL query
		values.clear();
		values.push_back(newUsernameStr);
		values.push_back(newUsernameStr);
		values.push_back(userSalt.toUtf8().toStdString());
		values.push_back(hashPass.toUtf8().toStdString());
		values.push_back(std::to_string(DB::NO_GROUP));
		values.push_back(userType);

		++it;
	}

	// When inserting User to the DB was successfuly done
	if (it < maxTries)
	{
		// Adding the new user to 'users' vector
		users.push_back(newUser);

		// Store Salt, Hash Password and User Type to the new User
		newUser->setUserRName(newUsername);
		newUser->setHashPassword(hashPass);
		newUser->setSalt(userSalt);
		newUser->setHashPasswordDB(hashPass.toUtf8());
		newUser->setSaltDB(userSalt.toUtf8());
		newUser->setGroupID(QString::number(DB::NO_GROUP));
		newUser->setUserType(QString::fromStdString(userType));

		return true;
	}
	else //The user could not be inserted to the database
	{
		return false;
	}

}

// Load Create User View in the QDialog
void loggingWindow::loadCreateUserView()
{
	// Giving the proper widget apperance to the view
	titlePopup->setText("Create User");
	passLabel->setText("Set Password");
	userLabel->setText("Set Username");
	userEdit->setText("");
	passEdit->setText("");
	createUserButton->hide();
	signInButton->hide();
	submitUserButton->show();
	cancelButton->show();

	// Every time this view is loaded, userEdit Line has the focus
	userEdit->setFocus();

	// In case of pressing the ENTER key, the Submit labelButton will emit the clicked() signal
	submitUserButton->setDefault(true);
}

// Load Loggin View in the QDialog
void loggingWindow::loadLoggingView()
{
	// Giving the proper widget apperance to the view
	titlePopup->setText("Sign In");
	passLabel->setText("Password");
	userLabel->setText("User");
	userEdit->setText("");
	passEdit->setText("");
	submitUserButton->hide();
	cancelButton->hide();
	createUserButton->show();
	signInButton->show();

	// Every time this view is loaded, userEdit Line has the focus
	userEdit->setFocus();

	// In case of pressing the ENTER key, the SIGN IN Button will emit the clicked() signal
	signInButton->setDefault(true);
}

// Returns the logging status flag
bool loggingWindow::getLoggingStatus()
{
	return loggingStatus;
}

// Returns the logged User
User* loggingWindow::getCurrentUser()
{
	return loggedUser;
}