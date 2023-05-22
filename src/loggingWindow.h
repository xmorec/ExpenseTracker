#pragma once
#include <QtWidgets>
#include "labelButton.h"
#include "User.h"

class loggingWindow : public QDialog
{

	Q_OBJECT

private:
	bool loggingStatus{ false };

	QLineEdit* userEdit{ new QLineEdit() };
	QLineEdit* passEdit{ new QLineEdit() };
	QLabel* titlePopup{ new QLabel("Sign In") };
	QLabel* passLabel{ new QLabel("Password") };
	QLabel* userLabel{ new QLabel("User") };
	QVBoxLayout* vLay{ new QVBoxLayout() };
	std::vector<User*> users{};

	QPushButton* signInButton{ new QPushButton("LOGIN") };
	QPushButton* cancelButton{ new QPushButton("CANCEL") };
	labelButton* createUserButton{ new labelButton("Create User") };
	labelButton* saveUserButton{ new labelButton("Submit") };
	static constexpr int lineEditWidth{ 200 };

public:

	loggingWindow() : QDialog()
	{
		setWindowIcon(QIcon("resources/icons/ExpTrIcon.png"));
		setWindowTitle("Sign In");
		setWindowFlag(Qt::MSWindowsFixedSizeDialogHint, true);

		userEdit->setFixedWidth(lineEditWidth);
		passEdit->setFixedWidth(lineEditWidth);
				
		QFont font(titlePopup->font().family(), 11, QFont::Bold);
		titlePopup->setFont(font);
				
		vLay->addWidget(titlePopup, 0, Qt::AlignCenter);
		vLay->addWidget(userLabel);
		vLay->addWidget(userEdit);
		vLay->addWidget(passLabel);
		vLay->addWidget(passEdit);		
		vLay->addWidget(createUserButton, 0, Qt::AlignCenter);
		vLay->addWidget(signInButton, 0, Qt::AlignCenter);
		vLay->addWidget(saveUserButton, 0, Qt::AlignCenter);
		saveUserButton->hide();
		vLay->addWidget(cancelButton, 0, Qt::AlignCenter);
		cancelButton->hide();

		setLayout(vLay);

		getUsersFromDB();

		QObject::connect(createUserButton, &QPushButton::clicked, [=]() {
			uploadCreateUserView();
			});

		QObject::connect(signInButton, &QPushButton::clicked, [=]() {
			loggingStatus = true;
			this->close();
			});

		QObject::connect(saveUserButton, &QPushButton::clicked, [=]() {
			checkUserCreation();
			});

		QObject::connect(cancelButton, &QPushButton::clicked, [=]() {
			uploadLoggingView();
			});

		
		
		exec();
	}

	void getUsersFromDB()
	{
		QString userN1{ "Xavi" };
		QString userN2{ "lara" };

		User* user1{ new User(userN1) };
		User* user2{ new User(userN2) };

		users.reserve(20);

		users.push_back(user1);
		users.push_back(user2);
	}

	void checkUserCreation()
	{
		QString newUsername = userEdit->text();
		QMessageBox* userInfoBox = new QMessageBox();		
		userInfoBox->setWindowTitle("Create User");
		//QIcon icon("resources/icons/saveIcon.png");
		//userInfoBox->setWindowIcon(icon);

		if (newUsername.isEmpty())
		{
			userInfoBox->setText("Username cannot be empty.");
			userInfoBox->setIcon(QMessageBox::Warning);
			userInfoBox->exec();			
			return;
		}

		for (auto user : users)
		{
			if (newUsername == user->getUserName())
			{
				userInfoBox->setText("This user name already exists.");
				userInfoBox->setIcon(QMessageBox::Warning);
				userInfoBox->exec();
				return;
			}
		}

	}
	
	void uploadCreateUserView()
	{
		setWindowTitle("Create User");
		titlePopup->setText("Create User");	
		passLabel->setText("Set Password");
		userLabel->setText("Set Username");
		createUserButton->hide();
		signInButton->hide();
		saveUserButton->show();
		cancelButton->show();
	}

	void uploadLoggingView()
	{
		setWindowTitle("Sign In");
		titlePopup->setText("Sign In");
		passLabel->setText("Password");
		userLabel->setText("User");
		saveUserButton->hide();
		cancelButton->hide();
		createUserButton->show();
		signInButton->show();
	}

	bool getLoggingStatus()
	{
		return loggingStatus;
	}
	
};