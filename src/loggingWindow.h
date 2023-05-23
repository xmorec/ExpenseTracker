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
	static constexpr int lineEditWidth{ 200 };

	QLabel* titlePopup{ new QLabel("Sign In") };
	QLabel* passLabel{ new QLabel("Password") };
	QLabel* userLabel{ new QLabel("User") };
	QVBoxLayout* vLay{ new QVBoxLayout() };

	QPushButton* signInButton{ new QPushButton("LOGIN") };
	QPushButton* cancelButton{ new QPushButton("CANCEL") };
	labelButton* createUserButton{ new labelButton("Create User") };
	labelButton* submitUserButton{ new labelButton("Submit") };

	QMessageBox* userInfoBox = new QMessageBox();
	std::vector<User*> users{};	

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
		vLay->addWidget(submitUserButton, 0, Qt::AlignCenter);
		submitUserButton->hide();
		vLay->addWidget(cancelButton, 0, Qt::AlignCenter);
		cancelButton->hide();

		setLayout(vLay);

		// When pressing Enter, signInButton acts as clicked
		signInButton->setDefault(true);

		// Hiding the plain password
		passEdit->setEchoMode(QLineEdit::Password);
		

		QObject::connect(createUserButton, &QPushButton::clicked, [=]() {
			uploadCreateUserView();
			});

		QObject::connect(signInButton, &QPushButton::clicked, [=]() {
			loggingIn();
			});

		QObject::connect(submitUserButton, &QPushButton::clicked, [=]() {
			createUser();
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
		QString userN3{ "ricard12" };

		User* user1{ new User(userN1) };
		User* user2{ new User(userN2) };
		User* user3{ new User(userN3) };

		users.reserve(20);

		users.push_back(user1);
		users.push_back(user2);
		users.push_back(user3);
	}

	void loggingIn()
	{
		//QIcon icon("resources/icons/saveIcon.png");
		//userInfoBox->setWindowIcon(icon);
		userInfoBox->setWindowTitle("Loggin In");
		QString inputUserName = userEdit->text();		
		bool userFound{ false };
		bool correctPassword{ false };

		for (auto user : users)
		{
			if (inputUserName.toLower() == user->getUserName().toLower())
			{				
				userFound = true;
				QString inputPass{ passEdit->text() };
				//QString userSalt{user->getSalt()};
				QString userSalt{ QString::fromUtf8(user->getSaltDB())};
				if (hashPassword(inputPass, userSalt) == QString::fromUtf8(user->getHashPasswordDB()))
				{
					correctPassword = true;
				}

				break;
			}
		}

		if (userFound)
		{
			if (correctPassword)
			{
				loggingStatus = true;
				this->close();
			}
			else // User found but the input password is wrong
			{
				userInfoBox->setText("Incorrect Password");
				userInfoBox->setIcon(QMessageBox::Warning);
				userInfoBox->exec();
			}
		}
		else //User not found
		{
			userInfoBox->setText("User not found.");
			userInfoBox->setIcon(QMessageBox::Warning);
			userInfoBox->exec();

		}

	}

	void createUser()
	{
		userInfoBox->setWindowTitle("Create User");

		QString newUsername = userEdit->text();
		if (isCorrectNameFormat(newUsername))
		{
			User* newUser{ new User(newUsername) };
			QString plainPassword{ passEdit->text() };
			QString userSalt{ generateSalt()};
			QString hashPass{ hashPassword(plainPassword, userSalt) };
			newUser->setHashPassword(hashPass);
			newUser->setSalt(userSalt);
			newUser->setHashPasswordDB(hashPass.toUtf8());
			newUser->setSaltDB(userSalt.toUtf8());
			users.push_back(newUser);
			userInfoBox->setText("User Created!");
			userInfoBox->setIcon(QMessageBox::Information);
			userInfoBox->exec();
			uploadLoggingView();
		}
	}

	QString hashPassword(const QString& plainPassword, const QString& userSalt)
	{
		QCryptographicHash hash{ QCryptographicHash (QCryptographicHash::Sha256)};
		QByteArray inputData{ userSalt.toLatin1() + plainPassword.toUtf8()};

		hash.addData(inputData);
		QByteArray hashPassword_ByteArr{ hash.result() };

		//From every byte of 'hashPassword_ByteArr' it is converted to 0-255 value of QString
		QString hashPassword{ QString::fromLatin1(hashPassword_ByteArr) };

		return hashPassword;
	}

	QString generateSalt()
	{
		static const int saltSize = 8; // Salt size (bytes)

		
		QByteArray saltByteArray{};
		saltByteArray.resize(saltSize);

		for (int i = 0; i < saltSize; ++i) {
			saltByteArray[i] = static_cast<char>(QRandomGenerator::system()->generate());;
			qDebug() << "saltByteArray[i] is: " << saltByteArray[i];
		}

		return QString::fromLatin1(saltByteArray);
	}



	bool isCorrectNameFormat(const QString& newUsername)
	{		

		if (newUsername.isEmpty())
		{
			userInfoBox->setText("Username cannot be empty.");
			userInfoBox->setIcon(QMessageBox::Warning);
			userInfoBox->exec();			
			return false;
		}

		QRegularExpression validUserNameValues("^[A-Za-z0-9_\\.]+$");

		if (!validUserNameValues.match(newUsername).hasMatch())
		{
			userInfoBox->setText("Username should be made of simple letters, numbers, \"_\", \".\"\n and without spaces.");
			userInfoBox->setIcon(QMessageBox::Warning);
			userInfoBox->exec();
			return false;
		}

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
	
	void uploadCreateUserView()
	{
		userEdit->setFocus();
		setWindowTitle("Create User");
		titlePopup->setText("Create User");	
		passLabel->setText("Set Password");
		userLabel->setText("Set Username");
		userEdit->setText("");
		passEdit->setText("");
		submitUserButton->setDefault(true);
		createUserButton->hide();
		signInButton->hide();
		submitUserButton->show();
		cancelButton->show();
	}

	void uploadLoggingView()
	{
		userEdit->setFocus();
		setWindowTitle("Sign In");
		titlePopup->setText("Sign In");
		passLabel->setText("Password");
		userLabel->setText("User");
		userEdit->setText("");
		passEdit->setText("");
		signInButton->setDefault(true);
		submitUserButton->hide();
		cancelButton->hide();
		createUserButton->show();
		signInButton->show();
	}

	bool getLoggingStatus()
	{
		return loggingStatus;
	}
	
};