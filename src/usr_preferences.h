#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QBoxLayout>
#include <QLineEdit>
#include <QString>
#include <QPushButton>
#include <QLabel>
#include <QDialog>
#include <QMessageBox>
#include <QCryptographicHash>
#include <QRandomGenerator>
#include <QRegularExpression>
#include <QGroupBox>
#include "User.h"
#include "constants.h"
#include "labelButton.h"
#include "sql_functions.h"

//////////////////////////////////////////////////////////////////////////////////////////
// Description:                                                                         
// It creates the Window (QDialog) for managing the preferences / configuration
//////////////////////////////////////////////////////////////////////////////////////////

class confWindow : public QDialog
{

private:
	User* currentUser{};
	QVBoxLayout* vLay{ new QVBoxLayout() };

public:
	confWindow(User* user) : currentUser(user)
	{
		setWindowIcon(QIcon(icons::prefIcon));
		setWindowTitle("Preferences");
		
		// Setting the content in configuration Windows
		QGroupBox* userInfo = new QGroupBox("User Information");

		QLabel* NameLab = new QLabel("Name");
		QLineEdit* NameEdit = new QLineEdit();
		//userNameEdit->setPlaceholderText("Placeholder Text");
		//userNameEdit->setFocus();
		vLay->addWidget(NameLab);
		vLay->addWidget(NameEdit);

		QLabel* userNameLab = new QLabel("User Name:");
		QLineEdit* userNameEdit = new QLineEdit();
		vLay->addWidget(userNameLab);
		vLay->addWidget(userNameEdit);

		QLabel* passLab = new QLabel("Password:");
		QLineEdit* passEdit = new QLineEdit();
		vLay->addWidget(passLab);
		vLay->addWidget(passEdit);

		QLabel* userTypeLab = new QLabel("User Type: user");
		vLay->addWidget(userTypeLab);

		userInfo->setLayout(vLay);
		QGridLayout* layout = new QGridLayout();
		layout->addWidget(userInfo, 0, 0);
		setLayout(layout);

		

		// Set fixed Dialog size (user cannot resize it)
		setWindowFlag(Qt::MSWindowsFixedSizeDialogHint, true);

	}

};

#endif
