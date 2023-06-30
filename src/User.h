#ifndef USER_H
#define USER_H

#include <QString>
#include <QByteArray>
#include <QCryptographicHash>
#include <QRandomGenerator>
#include <QRegularExpression>
#include <QMessageBox>


//////////////////////////////////////////////////////////////////////////////////////////
// Description:                                                                         
// It manages the information of the User
//////////////////////////////////////////////////////////////////////////////////////////

class User
{

private:
	QString userName{};
	QString userRName{};
	QString salt{};
	QString hashPassword{};
	QString userType{};

	QByteArray saltDB{};
	QByteArray hashPasswordDB{};

	
public:
	// Constructs a User Object getting as an input the username
	User(const QString& inputName);

	// Returns the Username
	const QString& getUserName();

	// Sets the Username
	void setUserName(const QString& userName);

	// Returns the Name of the user
	const QString& getUserRName();

	// Sets the name of the user
	void setUserRName(const QString& userRName);

	// Gets the User Type
	const QString& getUserType();

	// Sets the User Type
	void setUserType(const QString& userType);

	// Returns the Hash Password
	const QString& getHashPassword();

	// Sets the Hash Password
	void setHashPassword(const QString& hashPassword);

	// Returns the Salt
	const QString& getSalt();

	// Sets the Salt
	void setSalt(const QString& salt);

	// Returns the Hash Password in UTF-8
	const QByteArray& getHashPasswordDB();

	// Sets the Hash Password in UTF-8 
	void setHashPasswordDB(const QByteArray& hashPassword);

	// Gets the Salt in UTF-8
	const QByteArray& getSaltDB();

	// Sets the Salt in UTF-8
	void setSaltDB(const QByteArray& salt);

};

// Generating the hashPassword from the plain password and the generated Salt
QString hashPassword(const QString& plainPassword, const QString& userSalt);

// Checks if the input user-name has a correct text format
bool isCorrectUserNameFormat(std::vector<User*>& users, const QString& newUsername, QMessageBox* userInfoBox);

// Checks if the input name has a correct text format
bool isCorrectNameFormat(std::vector<User*>& users, const QString& name, QMessageBox* userInfoBox);

// Salt generating with a size of 'saltSize' bytes
QString generateSalt(int saltSize);

#endif