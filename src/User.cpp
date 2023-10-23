#include "User.h"


// Constructs a User Object getting as an input the username
User::User(const QString& inputName) : userName(inputName)
{	}

// Returns the Username
const QString& User::getUserName()
{
	return userName;
}

// Sets the Username
void User::setUserName(const QString& userName)
{
	this->userName = userName;
}

// Returns the Name of the user
const QString& User::getUserRName()
{
	return userRName;
}

// Sets the name of the user
void User::setUserRName(const QString& userRName)
{
	this->userRName = userRName;
}

// Gets the User Type
const QString& User::getUserType()
{
	return userType;
}

// Sets the User Type
void User::setUserType(const QString& userType)
{
	this->userType = userType;
}

// Returns the Hash Password
const QString& User::getHashPassword()
{
	return hashPassword;
}

// Sets the Hash Password
void User::setHashPassword(const QString& hashPassword)
{
	this->hashPassword = hashPassword;
}

// Returns the Salt
const QString& User::getSalt()
{
	return salt;
}

// Sets the Salt
void User::setSalt(const QString& salt)
{
	this->salt = salt;
}

// Returns the Hash Password in UTF-8
const QByteArray& User::getHashPasswordDB()
{
	return hashPasswordDB;
}

// Sets the Hash Password in UTF-8 
void User::setHashPasswordDB(const QByteArray& hashPassword)
{
	this->hashPasswordDB = hashPassword;
}

// Gets the Salt in UTF-8
const QByteArray& User::getSaltDB()
{
	return saltDB;
}

// Sets the Salt in UTF-8
void User::setSaltDB(const QByteArray& salt)
{
	this->saltDB = salt;
}

// Gets the Group ID
const QString& User::getGroupID()
{
	return groupID;
}

// Sets the Group ID
void User::setGroupID(const QString& ID)
{
	this->groupID = ID;
}

////////////////////////////////////////////////////////////////////////
// SUPPORTING FUNCTIONS TO CREATE PASSWORD AND CHECK USER NAME FORMAT //
////////////////////////////////////////////////////////////////////////

// Generating the hashPassword from the plain password and the generated Salt
QString hashPassword(const QString& plainPassword, const QString& userSalt)
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

// Checks if the input user-name has a correct text format
bool isCorrectUserNameFormat(std::vector<User*>& users, const QString& userName, QMessageBox* userInfoBox)
{
	// Checks the input user name is empty
	if (userName.isEmpty())
	{
		userInfoBox->setText("Username cannot be empty.");
		userInfoBox->setIcon(QMessageBox::Warning);
		userInfoBox->exec();
		return false;
	}

	// Checks if user tries to set their name to "admin"
	if (userName.toLower() == "admin")
	{
		userInfoBox->setText("Username cannot be \"admin\"");
		userInfoBox->setIcon(QMessageBox::Warning);
		userInfoBox->exec();
		return false;
	}

	//Initializing pattern useful to detect only correct input character values: "A-Z, a-z, 0-9, '.', '_'
	QRegularExpression validUserNameValues("^[A-Za-z0-9_\\.]+$");

	// Checks if the input user name (userName) does not match de correct text format
	if (!validUserNameValues.match(userName).hasMatch())
	{
		userInfoBox->setText("Username should be made of simple letters, numbers, \"_\", \".\"\n and without spaces.");
		userInfoBox->setIcon(QMessageBox::Warning);
		userInfoBox->exec();
		return false;
	}

	// Checks if the userName already exists from all other users
	for (auto user : users)
	{
		if (userName.toLower() == user->getUserName().toLower())
		{
			userInfoBox->setText("This user name already exists.");
			userInfoBox->setIcon(QMessageBox::Warning);
			userInfoBox->exec();
			return false;
		}
	}

	return true;
}

// Checks if the input name has a correct text format
bool isCorrectNameFormat(std::vector<User*>& users, const QString& name, QMessageBox* userInfoBox)
{
	// Checks the input user name is empty
	if (name.isEmpty())
	{
		userInfoBox->setText("Name cannot be empty.");
		userInfoBox->setIcon(QMessageBox::Warning);
		userInfoBox->exec();
		return false;
	}

	//Initializing pattern useful to detect only correct input character values: "A-Z, a-z, 0-9, '.', '_'
	QRegularExpression validUserNameValues("^[A-Za-z]+$");

	// Checks if the input user name (userName) does not match de correct text format
	if (!validUserNameValues.match(name).hasMatch())
	{
		userInfoBox->setText("Username should be made of simple letters and without spaces.");
		userInfoBox->setIcon(QMessageBox::Warning);
		userInfoBox->exec();
		return false;
	}

	return true;
}

// Salt generating with a size of 'saltSize' bytes
QString generateSalt(int saltSize)
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