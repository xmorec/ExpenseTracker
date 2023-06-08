#include "loggingWindow.h"

// Generates the main QDialog for the Logging Session
loggingWindow::loggingWindow() : QDialog()
{
	testMyDB();

	setWindowIcon(QIcon(icons::expTrackerIcon));
	setWindowTitle("Sign In");

	userInfoBox->setWindowIcon(QIcon(icons::loggingIcon));

	// Set fixed Dialog size (user cannot resize it)
	setWindowFlag(Qt::MSWindowsFixedSizeDialogHint, true);

	userEdit->setFixedWidth(lineEditWidth);
	passEdit->setFixedWidth(lineEditWidth);

	// Giving a onw style to the Title
	QFont font(titlePopup->font().family(), 11, QFont::Bold);
	titlePopup->setFont(font);

	// Filling Dialog Content (Labels, User/Password Fields, Buttons)
	vLay->addWidget(titlePopup, 0, Qt::AlignCenter);
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

	// Checking if Database file exists
	if (isDBExisting())
	{
		sqlite3* db{};

		//Opening database
		if (openSQLiteDB(db))
		{
			// Check if tableUsers is created
			if (isTableCreated(db, DB::tableUsers) == 1)
			{
				//records gets the output of the SELECT query given by 'getRecords()'
				std::vector<QStringList> records{ getRecords(db, DB::tableUsers, "username, salt, hash_password") };

				//Get the number of users with admin privileges
				int adminUsersNum = getRecordNumber(db, DB::tableUsers, "WHERE user_type = '" + DB::UserType::admin + "'");

				// If there is no admin in database, one is created
				if (adminUsersNum == 0)
				{
					// Tries to insert the new admin to DB and also adds it to the 'users' vector
					if (insertUserToDB(db, "admin"))
					{
						//Show infoBox
						userInfoBox->setWindowTitle("Admin Creation");
						userInfoBox->setIcon(QMessageBox::Information);
						userInfoBox->setText("Admin User is created\nUser: \"admin\"\nPass: \"admin\"");
					}
					else
					{
						userInfoBox->setIcon(QMessageBox::Warning);
						userInfoBox->setText("An admin should be created but it was not possible.\nRestart the application");
					}

					userInfoBox->exec();
				}

				// Load the database users to the Users vector 'users'
				if(!records.empty())
				{				
					for (const QStringList& record : records)
					{
						User* userDB{ new User(record[0])};
						userDB->setSalt(record[1]);
						userDB->setHashPassword(record[2]);						
						userDB->setSaltDB(record[1].toUtf8());
						userDB->setHashPasswordDB(record[2].toUtf8());
						users.push_back(userDB);
					}
				}
			}
			else // If Database tableUsers is not found
			{
				userInfoBox->setWindowTitle("Warning");
				userInfoBox->setIcon(QMessageBox::Warning);
				userInfoBox->setText("Users could not be loaded (no database table found)!");

				//Show infoBox
				userInfoBox->exec();
			}
			closeSQLiteDB(db);
		}
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
	if (isCorrectNameFormat(newUsername))
	{
		// Adding new user to Database: Checking its availability
		if (isDBExisting())
		{
			sqlite3* db{};

			// Opening Database
			if (openSQLiteDB(db))
			{
				// Checking if Table is created
				if (isTableCreated(db, DB::tableUsers) == 1)
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
				}
				else // When Users Table is not found
				{
					userInfoBox->setIcon(QMessageBox::Warning);
					userInfoBox->setText("User could not be created (no database table found)!");
				}				
				closeSQLiteDB(db);
			}			
		}
		else
		{
			userInfoBox->setIcon(QMessageBox::Warning);
			userInfoBox->setText("No Database found!");
		}

		//Show infoBox
		userInfoBox->exec();

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

	std::string userType{ newUsername == "admin" ? DB::UserType::admin : DB::UserType::user };

	// Setting the parameters to be sent to the SQL Query
	std::string newUsernameStr{ newUsername.toStdString() };
	std::string values{ "'" + newUsernameStr + "','" + newUsernameStr + "','" + userSalt.toUtf8().toStdString() + "','" + hashPass.toUtf8().toStdString() + "','" + userType + "'" };

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

		// Updating the string values to be sent to the SQL query
		values = "'" + newUsernameStr + "','" + newUsernameStr + "','" + userSalt.toUtf8().toStdString() + "','" + hashPass.toUtf8().toStdString() + "','" + userType + "'";

		++it;
	}

	// Inserting User to the DB was successfuly done
	if (it < maxTries)
	{
		// Adding the new user to 'users' vector
		users.push_back(newUser);

		// Sotre Hash Password and Salt to the new User
		newUser->setHashPassword(hashPass);
		newUser->setSalt(userSalt);
		newUser->setHashPasswordDB(hashPass.toUtf8());
		newUser->setSaltDB(userSalt.toUtf8());

		return true;
	}
	else //The user could not be inserted to the database
	{
		return false;
	}

}

// Salt generating with a size of 'saltSize' bytes
QString loggingWindow::generateSalt(int saltSize)
{
	// Initializing Salt Byte Array of size 'saltSize'
	QByteArray saltByteArray{};
	saltByteArray.resize(saltSize);

	// Filling each byte of Salt with a random unsigned character (int value rank 0-255 (1 byte))
	for (int i = 0; i < saltSize; ++i)
	{
		saltByteArray[i] = static_cast<unsigned char>(QRandomGenerator::system()->generate());
	}

	return QString::fromLatin1(saltByteArray);
}

// Generating the hashPassword from the plain password and the generated Salt
QString loggingWindow::hashPassword(const QString& plainPassword, const QString& userSalt)
{
	// Creating the type of Cryptographic Hash Operation (SHA-256)
	QCryptographicHash hash{ QCryptographicHash(QCryptographicHash::Sha256) };

	// Generating the input Data of Hash (Salt + Plain Password)
	QByteArray inputData{ userSalt.toLatin1() + plainPassword.toUtf8() };

	// Generating the Hash result from the input
	hash.addData(inputData);
	QByteArray hashPassword_ByteArr{ hash.result() };

	//From every byte of 'hashPassword_ByteArr' it is converted to 0-255 value of QString
	QString hashPassword{ QString::fromLatin1(hashPassword_ByteArr) };

	return hashPassword;
}

// Checks if the input user name (newUsername) has a correct text format
bool loggingWindow::isCorrectNameFormat(const QString& newUsername)
{
	// Checks the input user name is empty
	if (newUsername.isEmpty())
	{
		userInfoBox->setText("Username cannot be empty.");
		userInfoBox->setIcon(QMessageBox::Warning);
		userInfoBox->exec();
		return false;
	}

	// Checks if user tries to set their name to "admin"
	if (newUsername.toLower() == "admin")
	{
		userInfoBox->setText("Username cannot be \"admin\"");
		userInfoBox->setIcon(QMessageBox::Warning);
		userInfoBox->exec();
		return false;
	}

	//Initializing pattern useful to detect only correct input character values: "A-Z, a-z, 0-9, '.', '_'
	QRegularExpression validUserNameValues("^[A-Za-z0-9_\\.]+$");

	// Checks if the input user name (newUsername) does not match de correct text format
	if (!validUserNameValues.match(newUsername).hasMatch())
	{
		userInfoBox->setText("Username should be made of simple letters, numbers, \"_\", \".\"\n and without spaces.");
		userInfoBox->setIcon(QMessageBox::Warning);
		userInfoBox->exec();
		return false;
	}

	// Checks if the newUsername already exists from all other users
	for (auto user : users)
	{
		if (newUsername.toLower() == user->getUserName().toLower())
		{
			userInfoBox->setText("This user name already exists.");
			userInfoBox->setIcon(QMessageBox::Warning);
			userInfoBox->exec();
			return false;
		}
	}

	return true;
}

// Load Create User View in the QDialog
void loggingWindow::loadCreateUserView()
{
	// Giving the proper widget apperance to the view
	setWindowTitle("Create User");
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
	setWindowTitle("Sign In");
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