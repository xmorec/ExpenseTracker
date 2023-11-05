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
	QString name{};
	QStringList users{};
	QStringList in_requests{};
	QStringList out_requests{};
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
	labelButton* saveButt{ new labelButton("Save") };
	labelButton* cancelButt{ new labelButton("Cancel") };

	// Vertical Layout containing all widgets
	QVBoxLayout* mainVLay{ new QVBoxLayout() };

	// Dialog section used to create new group
	QLineEdit* newNameLineEdit{ new QLineEdit()};
	QHBoxLayout* newNameLay{ new QHBoxLayout() };
	QHBoxLayout* saveCancelLay{ new QHBoxLayout() };
	QLabel* newNameLabel { new QLabel("Set a group name:")};

	// Create a Dialog useful to join to any created group
	//std::unique_ptr<QDialog> join2GroupWin{ std::make_unique<QDialog>() };
	QDialog* join2GroupWin {new QDialog ()};

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
		newNameLay->addWidget(newNameLabel, 0, Qt::AlignRight);
		newNameLay->addWidget(newNameLineEdit, 0, Qt::AlignLeft);
		newNameLineEdit->setFixedWidth(130);
		mainVLay->addLayout(newNameLay);
		newNameLay->setAlignment(Qt::AlignCenter);
		mainVLay->addWidget(joinGroupButt, 0, Qt::AlignCenter | Qt::AlignTop);
		mainVLay->addWidget(removeReqButt, 0, Qt::AlignCenter | Qt::AlignTop);
		mainVLay->addWidget(inviteUserButt, 0, Qt::AlignCenter | Qt::AlignTop);
		mainVLay->addWidget(requestsButt, 0, Qt::AlignCenter | Qt::AlignTop);
		mainVLay->addWidget(leaveGroupButt, 0, Qt::AlignCenter | Qt::AlignTop);
		mainVLay->addWidget(renameGroupButt, 0, Qt::AlignCenter | Qt::AlignTop);
		mainVLay->addWidget(removeGroupButt, 0, Qt::AlignCenter | Qt::AlignTop);
		saveCancelLay->addWidget(saveButt, 0, Qt::AlignRight);
		saveCancelLay->addWidget(cancelButt, 0, Qt::AlignLeft);		
		mainVLay->addLayout(saveCancelLay);
		saveCancelLay->setAlignment(Qt::AlignCenter);

		// Select and load proper view according the group status of logged user
		selectView();

		setLayout(mainVLay);


		// Load all groups from DB
		loadGroupsFromDB();

		/* ****************************************************************  */
		//Create a Dialog useful to join to any created group
		join2GroupWin->setWindowTitle("Join to a group");
		join2GroupWin->setWindowIcon(QIcon(icons::groupPrefIcon));
		
		std::vector<QLabel*> nameGroup{};
		std::vector<QHBoxLayout*> hLayGroups{};

		auto vLayGroups{ new QVBoxLayout() };
		for (Group* group : groups)
		{
			nameGroup.push_back(new QLabel(group->name));
			hLayGroups.push_back(new QHBoxLayout());
			hLayGroups.back()->addWidget(nameGroup.back());
			vLayGroups->addLayout(hLayGroups.back());
		}
		
		join2GroupWin->setLayout(vLayGroups);
		

		// Set fixed Dialog size (user cannot resize it)
		join2GroupWin->setWindowFlag(Qt::MSWindowsFixedSizeDialogHint, true);

		/* ****************************************************************  */
		

		QObject::connect(createGroupButt, &QPushButton::clicked, [=]() {
			loadCreateGroupSection();
			 });

		QObject::connect(joinGroupButt, &QPushButton::clicked, [=]() {
			joinToGroup();
			});

		QObject::connect(removeReqButt, &QPushButton::clicked, [=]() {

			});

		QObject::connect(inviteUserButt, &QPushButton::clicked, [=]() {

			});

		QObject::connect(requestsButt, &QPushButton::clicked, [=]() {

			});

		QObject::connect(leaveGroupButt, &QPushButton::clicked, [=]() {
			leaveGroup();
			});

		QObject::connect(renameGroupButt, &QPushButton::clicked, [=]() {

			});

		QObject::connect(removeGroupButt, &QPushButton::clicked, [=]() {

			});

		QObject::connect(saveButt, &QPushButton::clicked, [=]() {
			createGroup();
			});

		QObject::connect(cancelButt, &QPushButton::clicked, [=]() {
			selectView();
			});

	}


	void selectView()
	{
		newNameLineEdit->setText("");

		// In case the User has no group
		if (currentUser->getGroupID().toInt() == DB::NO_GROUP)
		{
			loadNoGroupView();
			return;
		}
		
		// In case the User has a group
		if (currentUser->getGroupID().toInt() != DB::NO_GROUP)
		{
			loadGroupView();
			return;
		}

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
			std::vector<QStringList> records{ getRecords(db, DB::tableGroups, "ID, name, users, in_requests, out_requests") };

			// Load the database groups to the Groups vector 'groups' in case they exist in Database
			if (!records.empty())
			{
				// For every record, a user is read and stored in the Users vector (except from the current User)
				for (const QStringList& record : records)
				{
					//if (!(record[0] == currentUser->getUserName())) // No current user
					//{
					// Creating a new User and setting their parameters from the records of Database

					Group* groupDB{ new Group {
						record[0].toInt(),    // Group ID
						record[1],			  // Group Name	
						record[2].split(", "), // users in the group
						record[3].split(", "), // in requests
						record[4].split(", "), // out requests
					} };
						// Inserting the Group to the groups vector
						groups.push_back(groupDB);

					//}
				}
			}
			closeSQLiteDB(db);
		}
	}	

	void loadCreateGroupSection()
	{
		newNameLabel->show();
		newNameLineEdit->show();
		createGroupButt->setVisible(false);
		joinGroupButt->setVisible(false);
		removeReqButt->setVisible(false);
		inviteUserButt->setVisible(false);
		requestsButt->setVisible(false);
		leaveGroupButt->setVisible(false);
		renameGroupButt->setVisible(false);
		removeGroupButt->setVisible(false);
		saveButt->setVisible(true);
		cancelButt->setVisible(true);
	}

	void loadNoGroupView()
	{
		infoText->setText("You are not a member of a group yet");
		newNameLabel->hide();
		newNameLineEdit->hide();
		createGroupButt->setVisible(true);
		joinGroupButt->setVisible(true);
		removeReqButt->setVisible(false);
		inviteUserButt->setVisible(false);
		requestsButt->setVisible(false);
		leaveGroupButt->setVisible(false);
		renameGroupButt->setVisible(false);
		removeGroupButt->setVisible(false);
		saveButt->setVisible(false);
		cancelButt->setVisible(false);

	}

	void loadGroupView()
	{
		infoText->setText("You are a member of a group");
		newNameLabel->hide();
		newNameLineEdit->hide();
		createGroupButt->setVisible(false);
		joinGroupButt->setVisible(false);
		removeReqButt->setVisible(false);
		inviteUserButt->setVisible(true);
		requestsButt->setVisible(true);
		leaveGroupButt->setVisible(true);
		renameGroupButt->setVisible(true);
		removeGroupButt->setVisible(false);
		saveButt->setVisible(false);
		cancelButt->setVisible(false);

	}

	void createGroup()
	{
		QString newGroupName{ newNameLineEdit->text() };

		if (newGroupName.isEmpty())
		{
			userInfoBox->setText("Group name cannot be empty.");
			userInfoBox->setIcon(QMessageBox::Warning);
			userInfoBox->exec();
			return;
		}

		// Checks if the userName already exists from all other users
		for (Group* group : groups)
		{
			if (newGroupName == group->name)
			{
				userInfoBox->setText("This group name already exists.");
				userInfoBox->setIcon(QMessageBox::Warning);
				userInfoBox->exec();
				return;
			}
		}

		sqlite3* db{};

		if (checkAndOpenSQLiteDB(db, userInfoBox, { DB::tableGroups }) == DB::OPEN_SUCCESS)
		{

			int groupNum{ static_cast<int>(groups.size()) + 1};

			std::vector<std::string> values
			{
				std::to_string(static_cast<int>(groupNum)), //ID INTEGER NOT NULL,
				newGroupName.toStdString(), //name NVARCHAR(50) NOT NULL,
				currentUser->getUserName().toStdString(), //users VARCHAR(500),
				"0", //in_requests VARCHAR(100),
				"0" //out_requests VARCHAR(100)
			};


			// Tries to insert the new User to DB and also adds it to the 'users' vector
			if (insertRecord(db, DB::tableGroups, values))
			{
				userInfoBox->setIcon(QMessageBox::Information);
				userInfoBox->setText("New Group Created!");

				groups.push_back(new Group{ groupNum, newGroupName, {currentUser->getUserName()}, {"0"}, {"0"} });

				currentUser->setGroupID(QString::number(groupNum));

				// Enrolling the user to the group (set the goup_id for the logged user according the new created group into Database)
				std::string condition{ DB::col_username + " = '" + currentUser->getUserName().toStdString() + "'" };
				if (!updateRecords(db, DB::tableUsers, DB::col_groupID, std::to_string(groupNum), condition))
				{
					userInfoBox->setIcon(QMessageBox::Warning);
					userInfoBox->setText("Group was created, however, user could not be enrolled to the group due to unknown issues!");
				}

			}
			else
			{
				userInfoBox->setIcon(QMessageBox::Warning);
				userInfoBox->setText("Group could not be created!");
			}

			closeSQLiteDB(db);

			//Show infoBox
			userInfoBox->exec();
		}

		selectView();
	}

	void joinToGroup()
	{		
		
		join2GroupWin->exec();
	}

	void leaveGroup()
	{

	}

};


#endif