#ifndef USER_H
#define USER_H

#include <QString>
#include <QByteArray>

//////////////////////////////////////////////////////////////////////////////////////////
// Description:                                                                         
// It manages the information of the User
//////////////////////////////////////////////////////////////////////////////////////////

class User
{

private:
	QString userName{};
	QString salt{};
	QString hashPassword{};
	QString userType{};

	QByteArray saltDB{};
	QByteArray hashPasswordDB{};

	
public:
	User(const QString& inputName) : userName(inputName)
	{	}

	const QString& getUserName()
	{
		return userName;
	}

	const QString& getUserType()
	{
		return userType;
	}

	void setUserType(const QString& userType)
	{
		this->userType = userType;
	}

	const QString& getHashPassword()
	{
		return hashPassword;
	}

	const QString& getSalt()
	{
		return salt;
	}

	const QByteArray& getHashPasswordDB()
	{
		return hashPasswordDB;
	}

	const QByteArray& getSaltDB()
	{
		return saltDB;
	}

	void setSalt(const QString& salt)
	{
		this->salt = salt;
	}

	void setHashPassword(const QString& hashPassword)
	{
		this->hashPassword = hashPassword;
	}

	void setSaltDB(const QByteArray& salt)
	{
		this->saltDB = salt;
	}

	void setHashPasswordDB(const QByteArray& hashPassword)
	{
		this->hashPasswordDB = hashPassword;
	}

};

#endif