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
	QString userRName{};
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

	void setUserName(const QString& userName)
	{
		this->userName = userName;
	}

	const QString& getUserRName()
	{
		return userRName;
	}

	void setUserRName(const QString& userRName)
	{
		this->userRName = userRName;
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

	void setHashPassword(const QString& hashPassword)
	{
		this->hashPassword = hashPassword;
	}

	const QString& getSalt()
	{
		return salt;
	}

	void setSalt(const QString& salt)
	{
		this->salt = salt;
	}

	const QByteArray& getHashPasswordDB()
	{
		return hashPasswordDB;
	}

	void setHashPasswordDB(const QByteArray& hashPassword)
	{
		this->hashPasswordDB = hashPassword;
	}

	const QByteArray& getSaltDB()
	{
		return saltDB;
	}

	void setSaltDB(const QByteArray& salt)
	{
		this->saltDB = salt;
	}


};

#endif