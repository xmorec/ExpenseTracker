#pragma once
#include <QtWidgets>

class User// : QWidget
{
//Q_OBJECT

private:
	QString userName{};
	QString hashPassword{};
public:
	User(const QString& inputName = "") : userName(inputName)
	{
		
	}

	const QString& getUserName()
	{
		return userName;
	}

	const QString& getHashPassword()
	{
		return hashPassword;
	}

};