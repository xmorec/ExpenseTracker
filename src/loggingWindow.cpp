#include "loggingWindow.h"

// Generates the main QDialog for the Logging Session
loggingWindow::loggingWindow() : QDialog()
{
	setWindowIcon(QIcon("resources/icons/ExpTrIcon.png"));
	setWindowTitle("Sign In");

	userInfoBox->setWindowIcon(QIcon("resources/icons/logsession.png"));

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
	QString userN1{ "Xavi" };
	QString userN2{ "lara" };
	QString userN3{ "ricard12" };

	User* user1{ new User(userN1) };
	User* user2{ new User(userN2) };
	User* user3{ new User(userN3) };

	users.reserve(20);

	users.push_back(user1);
	users.push_back(user2);
	users.push_back(user3);
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
		User* newUser{ new User(newUsername) };

		// Getting the input plain password from Password Field
		QString plainPassword{ passEdit->text() };

		// Generating Salt
		QString userSalt{ generateSalt(8) };

		// Generating Hash Password
		QString hashPass{ hashPassword(plainPassword, userSalt) };

		// Sotre Hash Password and Salt to the new User
		newUser->setHashPassword(hashPass);
		newUser->setSalt(userSalt);
		newUser->setHashPasswordDB(hashPass.toUtf8());
		newUser->setSaltDB(userSalt.toUtf8());

		// Adding the new user to 'users' vector
		users.push_back(newUser);
		userInfoBox->setText("User Created!");
		userInfoBox->setIcon(QMessageBox::Information);
		userInfoBox->exec();

		// The Logging View is loaded
		loadLoggingView();
	}
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