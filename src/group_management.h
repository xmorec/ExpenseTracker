#ifndef GROUPMAN_H
#define GROUPMAN_H

#include <QBoxLayout>
#include <QLineEdit>
#include <QString>
#include <QPushButton>
#include <QLabel>
#include <QDialog>
#include <QMessageBox>
#include <QGroupBox>
#include <QComboBox>
#include <QTimer>
#include "User.h"
#include "constants.h"
#include "labelButton.h"
#include "sql_functions.h"
#include "iconButton.h"


struct Group
{
	int ID{};
	std::string name{};
	//std::vector<std::string> users{};
	//std::vector<std::string> in_requests{};
	//std::vector<std::string> out_requests{};
	std::string users{};
	std::string in_requests{};
	std::string out_requests{};
};

class groupManWindow : public QDialog
{

	Q_OBJECT

private:

	// Windows Size value
	QSize winSize{};

	// Current Logged User
	User* currentUser{};

	// Vector storing all Users
	std::vector<User*> users{};

	// Vector storing all Groups
	std::vector<Group*> groups{};

	// Informing QMessageBox
	QMessageBox* userInfoBox = new QMessageBox();

	// Information text in Management User section
	QLabel* infoText{ new QLabel() };

	// Buttons
	labelButton* createGroupButt{ new labelButton("Create Group")};
	labelButton* joinGroupButt{ new labelButton("Join to a Group") };
	labelButton* removeReqButt{ new labelButton("Remove Request") };
	labelButton* inviteUserButt{ new labelButton("Invite a user to the group") };
	labelButton* requestsButt{ new labelButton("Requests") };
	labelButton* leaveGroupButt{ new labelButton("Leave the group") };
	labelButton* renameGroupButt{ new labelButton("Rename group") };
	labelButton* removeGroupButt{ new labelButton("Remove group") };

	// Vertical Layout containing all widgets
	QVBoxLayout* mainVLay{ new QVBoxLayout() };

	// Dialog section used to create new group
	QLineEdit* newNameLineEdit{ new QLineEdit()};
	QHBoxLayout* newNameLay{ new QHBoxLayout() };
	QLabel* newNameLabel { new QLabel("Set a group name")};

public:

	groupManWindow(User* currentUser) : currentUser(currentUser)
	{
		setWindowIcon(QIcon(icons::groupPrefIcon));
		setWindowTitle("Group Management");
		userInfoBox->setWindowIcon(QIcon(icons::expTrackerIcon));

		// Set fixed Dialog size (user cannot resize it)
		setWindowFlag(Qt::MSWindowsFixedSizeDialogHint, true);

		// Giving a font style to the informating text
		QFont font(infoText->font().family(), 11, QFont::Bold);
		infoText->setFont(font);

		mainVLay->addWidget(infoText, 0, Qt::AlignCenter | Qt::AlignTop);


		// Adding the Logging Image
		QLabel* imageLabel = new QLabel();
		QPixmap noGroupImage(icons::noGroupfIcon);
		imageLabel->setPixmap(noGroupImage.scaled(QSize(127, 100)));
		mainVLay->addWidget(imageLabel, 0, Qt::AlignCenter);

		mainVLay->addWidget(createGroupButt, 0, Qt::AlignCenter | Qt::AlignTop);		
		newNameLay->addWidget(newNameLabel);
		newNameLay->addWidget(newNameLineEdit);
		mainVLay->addLayout(newNameLay);
		mainVLay->addWidget(joinGroupButt, 0, Qt::AlignCenter | Qt::AlignTop);
		mainVLay->addWidget(removeReqButt, 0, Qt::AlignCenter | Qt::AlignTop);
		mainVLay->addWidget(inviteUserButt, 0, Qt::AlignCenter | Qt::AlignTop);
		mainVLay->addWidget(requestsButt, 0, Qt::AlignCenter | Qt::AlignTop);
		mainVLay->addWidget(leaveGroupButt, 0, Qt::AlignCenter | Qt::AlignTop);
		mainVLay->addWidget(renameGroupButt, 0, Qt::AlignCenter | Qt::AlignTop);
		mainVLay->addWidget(removeGroupButt, 0, Qt::AlignCenter | Qt::AlignTop);

		// In case the current User has no group
		if (currentUser->getGroupID() == DB::NO_GROUP )
		{
			loadNoGroupView();
		}

		setLayout(mainVLay);



		QObject::connect(createGroupButt, &QPushButton::clicked, [=]() {
			loadCreateGroupSection();
			 });

		QObject::connect(joinGroupButt, &QPushButton::clicked, [=]() {

			});

		QObject::connect(removeReqButt, &QPushButton::clicked, [=]() {

			});

		QObject::connect(inviteUserButt, &QPushButton::clicked, [=]() {

			});

		QObject::connect(requestsButt, &QPushButton::clicked, [=]() {

			});

		QObject::connect(leaveGroupButt, &QPushButton::clicked, [=]() {

			});

		QObject::connect(renameGroupButt, &QPushButton::clicked, [=]() {

			});

		QObject::connect(removeGroupButt, &QPushButton::clicked, [=]() {

			});

	}

	
	void loadUsersFromDB()
	{
		// Clearing Users vector in order to get better results
		users.clear();

		sqlite3* db{};

		// Checks and Open the Database
		if (checkAndOpenSQLiteDB(db, userInfoBox, { DB::tableUsers }) == DB::OPEN_SUCCESS)
		{
			//records gets the output of the SELECT query given by 'getRecords()'
			std::vector<QStringList> records{ getRecords(db, DB::tableUsers, "username, name, salt, hash_password, group_ID, user_type") };

			// Load the database users to the Users vector 'users' in case they exist in Database
			if (!records.empty())
			{
				// For every record, a user is read and stored in the Users vector (except from the current User)
				for (const QStringList& record : records)
				{
					if (!(record[0] == currentUser->getUserName())) // No current user
					{
						// Creating a new User and setting their parameters from the records of Database
						User* userDB{ new User(record[0]) };
						userDB->setUserRName(record[1]);
						userDB->setSalt(record[2]);
						userDB->setHashPassword(record[3]);
						userDB->setSaltDB(record[2].toUtf8());
						userDB->setHashPasswordDB(record[3].toUtf8());
						userDB->setGroupID(record[4]);
						userDB->setUserType(record[5]);

						// Inserting the new User to the Users vector
						users.push_back(userDB);
					}
				}
			}
			else // When there are no users in Database (even not the current user)
			{
				userInfoBox->setIcon(QMessageBox::Warning);
				userInfoBox->setText("There are no users in Database");
				userInfoBox->exec();
			}
			closeSQLiteDB(db);
		}
	}


	void loadGroupsFromDB()
	{
		// Clearing Users vector in order to get better results
		groups.clear();

		sqlite3* db{};

		// Checks and Open the Database
		if (checkAndOpenSQLiteDB(db, userInfoBox, { DB::tableGroups }) == DB::OPEN_SUCCESS)
		{
			//records gets the output of the SELECT query given by 'getRecords()'
			std::vector<QStringList> records{ getRecords(db, DB::tableUsers, "ID, name, users, in_requests, out_requests") };

			// Load the database groups to the Users vector 'users' in case they exist in Database
			if (!records.empty())
			{
				// For every record, a user is read and stored in the Users vector (except from the current User)
				for (const QStringList& record : records)
				{
					//if (!(record[0] == currentUser->getUserName())) // No current user
					//{
					// Creating a new User and setting their parameters from the records of Database
					Group* groupDB{ new Group {
						record[0].toInt(),
						record[1].toStdString(),
						record[2].toStdString(),
						record[3].toStdString(),
						record[4].toStdString(),
					} };
						// Inserting the Group to the groups vector
						groups.push_back(groupDB);

					//}
				}
			}
			else // When there are no users in Database (even not the current user)
			{
				userInfoBox->setIcon(QMessageBox::Warning);
				userInfoBox->setText("There are no groups in Database");
				userInfoBox->exec();
			}
			closeSQLiteDB(db);
		}
	}	

	void loadCreateGroupSection()
	{
		newNameLabel->show();
		newNameLineEdit->show();
		createGroupButt->setVisible(true);
		joinGroupButt->setVisible(false);
		removeReqButt->setVisible(false);
		inviteUserButt->setVisible(false);
		requestsButt->setVisible(false);
		leaveGroupButt->setVisible(false);
		renameGroupButt->setVisible(false);
		removeGroupButt->setVisible(false);
	}

	void loadNoGroupView()
	{
		infoText->setText("You are not a member of a group yet");
		createGroupButt->setVisible(true);
		joinGroupButt->setVisible(true);
		removeReqButt->setVisible(false);
		inviteUserButt->setVisible(false);
		requestsButt->setVisible(false);
		leaveGroupButt->setVisible(false);
		renameGroupButt->setVisible(false);
		removeGroupButt->setVisible(false);
		newNameLabel->hide();
		newNameLineEdit->hide();
	}

};


#endif