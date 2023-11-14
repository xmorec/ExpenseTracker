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


/////////////////////////////////////////////////////////////////////////////////////////////
// Description:                                                                         
// It manages Group Management (Requests to joinin groups, own group management and edition,
//  inivitation to users and Group Creation)
////////////////////////////////////////////////////////////////////////////////////////////

// This struct contains the main parameters of a Group
struct Group
{
	int ID{};
	QString name{};
	QStringList users{};
	QStringList in_requests{};
	QStringList out_requests{};
	QString status{};
};

// imageSize is useful to give the main pictures of the QDialog the correct size
namespace imageSize
{
	inline const QSize groupMember { 127, 132 };
	inline const QSize noGroup { 127, 95 };
	inline const QSize requestToGroup { 130, 100 };
	inline const QSize requestFromGroup{ 130, 120 };
}

// These constants are useful to add or remove requests/invitations in the Groups Table (database)
constexpr int ADD_REQUEST{ 0 };
constexpr int REMOVE_REQUEST{ 1 };


//  Adds or removes a user to the requests column (in_requests or out_requests) according the MODE in Database. Returns TRUE (successful Database). False otherwise
//bool updateRequestCol(sqlite3* db, User* user, const std::string& col_requests, int groupID, int MODE);


//  Adds or removes a user to the requests column (in_requests or out_requests) according the MODE in Database. Returns TRUE (successful Database). False otherwise
bool updateRequestCol(sqlite3* db, User* user, const std::string& col_requests, int groupID, int MODE);


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

	// Image shown in the middle of Dialog
	QLabel* imageLabel = new QLabel();
	QPixmap GroupImage{};

	// Buttons
	labelButton* createGroupButt{ new labelButton("Create Group")};
	labelButton* joinGroupButt{ new labelButton("Join to a Group") };
	labelButton* removeReqButt{ new labelButton("Remove Request") };
	labelButton* inviteUserButt{ new labelButton("Invite a user to the group") };
	labelButton* requestsButt{ new labelButton("Handle invitations") };
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
	QDialog* join2GroupWin {new QDialog ()};

	// Create a Dialog useful to send invitation to a user
	QDialog* sendInvitationWin{ new QDialog() };

public:

	// Constructs the main QDialog window used to manage the group preferences (join, create, leave, or edit a group. 
	// Also, requesting to be part of a group or invite someone to be part of your group could be performed here)
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
		infoText->setAlignment(Qt::AlignCenter);

		mainVLay->addWidget(infoText, 0, Qt::AlignCenter | Qt::AlignTop);

		// Adding the Logging Image		
		GroupImage = icons::noGroupfIcon;
		imageLabel->setPixmap(GroupImage.scaled(imageSize::noGroup));
		mainVLay->addWidget(imageLabel, 0, Qt::AlignCenter);

		// Adding all labels and buttons and layouts into the main QDialog
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

		// Setting the layout shown in the main QDialog window
		setLayout(mainVLay);

		// Load all users from DB
		loadUsersFromDB();

		// Load all groups from DB
		loadGroupsFromDB();

		//Create a Dialog useful to join to any created group
		createSendRequestsWindow();

		//Create a Dialog useful to send invitations to users (in case the current user is member of a group)
		if (!currentUser->getGroupID().startsWith(DB::REQTO_TAG)) // In case the User has not requested to be in a group
		{
			if (currentUser->getGroupID().toInt() > DB::NO_GROUP) // In case the user is member of a group
			{
				sqlite3* db{}; // Create database object useful to read the invited users to the group
				createInvitationWindow(db, true);
			}
		}		

		// Select and load proper view according the group status of logged user
		selectView();


		// Generate the lambda functions when pressing buttons
		QObject::connect(createGroupButt, &QPushButton::clicked, [=]() {
			loadCreateGroupSection();
			 });

		QObject::connect(joinGroupButt, &QPushButton::clicked, [=]() {
			join2GroupWin->exec();
			});

		QObject::connect(removeReqButt, &QPushButton::clicked, [=]() {
			removeRequest();
			});

		QObject::connect(inviteUserButt, &QPushButton::clicked, [=]() {
			sendInvitationWin->exec();
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


	// Selects a view according whether the user is member of a group or not, and taking into account if he/she sent or received a request
	// to join to a group.
	void selectView()
	{		

		if (currentUser->getGroupID().startsWith(DB::REQTO_TAG)) // In case the User has requested to be in a group
		{
			loadRequestSentView();			
		}		
		else if (currentUser->getGroupID().startsWith(DB::REQFROM_TAG)) // In case the User has been invited to be part of a group
		{
			//loadRequestReceivedView();			
		}
		else if (currentUser->getGroupID().toInt() == DB::NO_GROUP) // In case the User has no group
		{
			loadNoGroupView();			
		}
		else if (currentUser->getGroupID().toInt() != DB::NO_GROUP) // In case the User has a group
		{
			loadGroupView();
		}

		// Update the Dialog size according the current content
		QTimer::singleShot(20, [=]() {
			setFixedSize(sizeHint());
		});		

	}

	//Create a Dialog useful to send invitations to users
	void createSendRequestsWindow()
	{

		join2GroupWin->setWindowTitle("Join to a group");
		join2GroupWin->setWindowIcon(QIcon(icons::groupPrefIcon));

		QGroupBox* Join2GroupBox = new QGroupBox("Join to a Group");

		// A vector that contains each group as a labelButton is created
		std::vector<labelButton*> nameGroup{};

		auto vLayGroups{ new QVBoxLayout() };

		// For each active group, a labelButton is created and added to the layout
		for (Group* group : groups)
		{
			if (group->status == DB::Groups::status_active) // just adding the active groups
			{
				nameGroup.push_back(new labelButton(group->name));
				vLayGroups->addWidget(nameGroup.back());
			}
		}

		Join2GroupBox->setLayout(vLayGroups);
		join2GroupWin->setLayout(new QVBoxLayout());
		join2GroupWin->layout()->addWidget(Join2GroupBox);

		// Set fixed Dialog size (user cannot resize it)
		join2GroupWin->setWindowFlag(Qt::MSWindowsFixedSizeDialogHint, true);

		// For Sending Requests to join a group, a label button is created and connected to a signal for each existing group
		for (labelButton*& groupReq : nameGroup)
		{
			QObject::connect(groupReq, &QPushButton::clicked, [=]() {

				if (joinRequest(groupReq->text())) // When join request is successfully done
				{
					join2GroupWin->close();
				}

				selectView();
				});
		}
	}

	//Create a Dialog useful to send invitations to users (db is Database object, 'dbOpen' is a flag used to open and check the database)
	void createInvitationWindow(sqlite3* db, bool dbOpen)
	{

		sendInvitationWin->setWindowTitle("Send invitations");
		sendInvitationWin->setWindowIcon(QIcon(icons::groupPrefIcon));

		QGroupBox* sendInvBox = new QGroupBox("Send Invitation");

		// A vector that contains each group as a labelButton is created
		std::vector<labelButton*> userInvLabel{};

		auto vLayUsrs{ new QVBoxLayout() };

		// Check the users that has been already invited to this group
		QString outRequests{}; // This QString will contain the list of users that already received an invitation

		if (dbOpen) // Database is requested to be opened and checked
		{
			// Check te availability and opens the Database
			if (checkAndOpenSQLiteDB(db, userInfoBox, { DB::tableGroups, DB::tableUsers }) == DB::OPEN_SUCCESS)
			{
				// Declaring the groupID of the current User sending the invitation
				std::string groupID{ currentUser->getGroupID().toStdString() };

				// Selecting the cell from Groups Table regarding the "out_requests" or "in_requests" column in for the selected "group ID"
				outRequests = getRecords(db, DB::tableGroups, DB::Groups::col_outrequests, "WHERE " + DB::Groups::col_ID + " = '" + groupID + "'")[0][0];

				closeSQLiteDB(db);
			}
		}
		else // Database is already opened and checked and there is no need to do it
		{
			// Declaring the groupID of the current User sending the invitation
			std::string groupID{ currentUser->getGroupID().toStdString() };

			// Selecting the cell from Groups Table regarding the "out_requests" or "in_requests" column in for the selected "group ID"
			outRequests = getRecords(db, DB::tableGroups, DB::Groups::col_outrequests, "WHERE " + DB::Groups::col_ID + " = '" + groupID + "'")[0][0];
		}

		// For each user, a labelButton is created and added to the layout
		for (User* user : users)
		{
			userInvLabel.push_back(new labelButton(user->getUserName()));
			vLayUsrs->addWidget(userInvLabel.back());

			// If user was previously invited to this group
			if (outRequests.contains(user->getUserName()))
			{
				userInvLabel.back()->setText(user->getUserName() + " (Invitation sent)");
				userInvLabel.back()->setDisabled(true);
			}
		}

		sendInvBox->setLayout(vLayUsrs);
		sendInvitationWin->setLayout(new QVBoxLayout());
		sendInvitationWin->layout()->addWidget(sendInvBox);

		// Set fixed Dialog size (user cannot resize it)
		sendInvitationWin->setWindowFlag(Qt::MSWindowsFixedSizeDialogHint, true);

		// For Sending Invitations to request a user to join your group, a label button is created and connected to a signal for each existing user
		for (int pos = 0; pos < userInvLabel.size(); ++pos)
		{
			QObject::connect(userInvLabel[pos], &QPushButton::clicked, [=]() {

				if (sendInvitation(userInvLabel[pos], pos))
				{
					userInvLabel[pos]->setText(users[pos]->getUserName() + " (Invitation sent)");
					userInvLabel[pos]->setDisabled(true);
				}

				});
		}

	}
	
	// Load the users from Database and load them into 'users' vector
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
					if (!(record[0] == currentUser->getUserName())) // Not selecting the current user
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

	// Load the groups from Database and load them into 'groups' vector
	void loadGroupsFromDB()
	{
		// Clearing Users vector in order to get better results
		groups.clear();

		sqlite3* db{};

		// Checks and Open the Database
		if (checkAndOpenSQLiteDB(db, userInfoBox, { DB::tableGroups }) == DB::OPEN_SUCCESS)
		{
			//records gets the output of the SELECT query given by 'getRecords()'
			std::string columns
			{
				DB::Groups::col_ID + ", " + DB::Groups::col_group_name + ", " + DB::Groups::col_users + ", " + DB::Groups::col_inrequests + ", " + DB::Groups::col_outrequests + ", " + DB::Groups::col_status
			};
			std::vector<QStringList> records{ getRecords(db, DB::tableGroups, columns) };

			// Load the database groups to the Groups vector 'groups' in case they exist in Database
			if (!records.empty())
			{
				// For every record, a group is read and stored in the groups vector
				for (const QStringList& record : records)
				{

					Group* groupDB{ new Group {
						record[0].toInt(),     // Group ID
						record[1],			   // Group Name	
						record[2].split(", "), // users in the group
						record[3].split(", "), // in requests
						record[4].split(", "), // out requests
						record[5]			   // status group
					} };

					// Inserting the Group to the groups vector
					groups.push_back(groupDB);

				}
			}
			closeSQLiteDB(db);
		}
	}	

	// Load the section for creating a group
	void loadCreateGroupSection()
	{
		
		// Show and Hide the widgets regarding this view
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

	// Load the view when a user has no group and nor request/invitation
	void loadNoGroupView()
	{
		// Set Image for not belonging to a group
		GroupImage = icons::noGroupfIcon;
		imageLabel->setPixmap(GroupImage.scaled(imageSize::noGroup));

		// Reset the Line Edit of a new Group Name
		newNameLineEdit->setText("");

		// Show and Hide the widgets regarding this view
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

	// Load the view when user is a member of a group
	void loadGroupView()
	{
		QString groupName{"unknown"};

		// Setting the name group of the current logged user
		auto groupIt{ std::find_if(groups.begin(), groups.end(), [=](Group* group) 	{ return group->ID == currentUser->getGroupID().toInt(); })};		
		
		if (groupIt != groups.end())
		{
			groupName = (*groupIt)->name;
		}

		infoText->setText("You are a member of the group\n'" + groupName + "'");

		// Set the image for a user belonging to a group
		GroupImage = icons::groupPrefIcon;
		imageLabel->setPixmap(GroupImage.scaled(imageSize::groupMember));

		// Show and Hide the widgets regarding this view
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

	// Load the view when user sent a request to join a group
	void loadRequestSentView()
	{
		QString groupName{ "unknown" };

		// Setting the name group of the current logged user
		QString reqToGroupID { currentUser->getGroupID().sliced(DB::REQTO_TAG.size()) };

		auto groupIt{ std::find_if(groups.begin(), groups.end(), [=](Group* group) { return group->ID == reqToGroupID.toInt(); }) };

		if (groupIt != groups.end())
		{
			groupName = (*groupIt)->name;
		}

		infoText->setText("You requested to join the group\n'" + groupName + "'\nWaiting for a response from this group.");

		// Set the image for a user belonging to a group
		GroupImage = icons::reqToGroup;
		imageLabel->setPixmap(GroupImage.scaled(imageSize::requestToGroup));

		// Show and Hide the widgets regarding this view
		newNameLabel->hide();
		newNameLineEdit->hide();
		createGroupButt->setVisible(false);
		joinGroupButt->setVisible(false);
		removeReqButt->setVisible(true);
		inviteUserButt->setVisible(false);
		requestsButt->setVisible(false);
		leaveGroupButt->setVisible(false);
		renameGroupButt->setVisible(false);
		removeGroupButt->setVisible(false);
		saveButt->setVisible(false);
		cancelButt->setVisible(false);
	}

	// Load the view when user is invited to join a group
	void loadRequestReceivedView()
	{

	}

	// User creates a group and he/she is enrolled to it
	void createGroup()
	{
		// Gets the name of the group from the LineEdit
		QString newGroupName{ newNameLineEdit->text() };

		// Group Name cannot be empty
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
			// If the new group name already exists for an active group, a warning pops up
			if ((newGroupName == group->name) && (group->status == DB::Groups::status_active))
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

			// Sets the new Group ID for this group
			int groupNum{ static_cast<int>(groups.size()) + 1};

			// Creates a vector of values that will be inserted to Database
			std::vector<std::string> values
			{
				std::to_string(static_cast<int>(groupNum)), //ID INTEGER NOT NULL,
				newGroupName.toStdString(), //name NVARCHAR(50) NOT NULL,
				currentUser->getUserName().toStdString(), //users VARCHAR(500),
				"", //in_requests VARCHAR(100),
				"", //out_requests VARCHAR(100)
				DB::Groups::status_active.toStdString() // Setting the status group to active
			};

			// Tries to insert the new User to DB and also adds it to the 'users' vector
			if (insertRecord(db, DB::tableGroups, values)) // Group could be successfully added to Database
			{
				userInfoBox->setIcon(QMessageBox::Information);
				userInfoBox->setText("New Group Created!");

				// Generate the invitation window from a Database reading
				createInvitationWindow(db, false);

				// Enrolling the user to the group (set the goup_id for the logged user according the new created group into Database)
				std::string condition{ DB::Users::col_username + " = '" + currentUser->getUserName().toStdString() + "'" };

				if (updateRecords(db, DB::tableUsers, DB::Users::col_groupID, std::to_string(groupNum), condition)) 
					// Group ID of the user could be added, and the user could not be added to this group
				{
					// Adds this new group to the 'groups' vector
					groups.push_back(new Group{ groupNum, newGroupName, {currentUser->getUserName()}, {"0"}, {"0"} });

					// Sets the new group ID to the current user
					currentUser->setGroupID(QString::number(groupNum));
				}
				else 
				{
					// When user could not be added to the group in Database
					userInfoBox->setIcon(QMessageBox::Warning);
					userInfoBox->setText("Group was created, however, user could not be enrolled to the group due to unknown issues!");
					
				}

			}
			else // Group could not be added to Database
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

	// Creates a join request to a group according the input parameter 'nameGroup'. Returns true if request is done, returns false if it is not.
	bool joinRequest(const QString& nameGroup)
	{
		// groupID will be useful to write/read information to Database
		int groupID{ DB::NO_GROUP };

		// Trying to search which is the group of 'groups' vector that matches with the group name 'nameGroup'
		auto id_search{ std::find_if(groups.begin(), groups.end(), [=](Group* group) {return group->name == nameGroup; }) };

		QMessageBox msgBox{};
		msgBox.setWindowTitle("Join group request");
		msgBox.setWindowIcon(QIcon(icons::groupPrefIcon));

		if (id_search != groups.end()) // If there is a group with 'nameGroup' in groups vector
		{
			if ((*id_search)->status == DB::Groups::status_inactive) // In case this group is an inactive group, it is considered as it does not exist
			{
				msgBox.setText("No group was found with name '" + nameGroup + "'");
				msgBox.setStandardButtons(QMessageBox::Close);
				msgBox.setIcon(QMessageBox::Warning);
				msgBox.exec();
				return false;
			}

			groupID = (*id_search)->ID;

			msgBox.setText("Do you want to request to join group '" + nameGroup + "'?");
			msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
			msgBox.setIcon(QMessageBox::Question);
			int answer = msgBox.exec();

			// When Pressing "Yes" button 
			if (answer == QMessageBox::Yes)
			{
				sqlite3* db{};

				// Check te availability and opense the Database
				if (checkAndOpenSQLiteDB(db, userInfoBox, { DB::tableGroups, DB::tableUsers }) == DB::OPEN_SUCCESS)
				{

					// This flag will be the returning value for this function according a successful request writing to database or not
					bool requestFlag{};

					// Setting the condition for updating the "group_id" column (Users Table) for the selected user
					std::string conditionUsers { DB::Users::col_username + " = '" + currentUser->getUserName().toStdString() + "'" };

					// Updating in the database (Table Users and Table Groups) the user information for this request:
					QString reqGroupID { DB::REQTO_TAG + QString::number(groupID) };					

					if (updateRecords(db, DB::tableUsers, DB::Users::col_groupID, reqGroupID.toStdString(), conditionUsers)
						&& updateRequestCol(db, currentUser, DB::Groups::col_inrequests, groupID, ADD_REQUEST))
					{
						userInfoBox->setIcon(QMessageBox::Information);
						userInfoBox->setText("User request was successfuly sent!");

						currentUser->setGroupID(reqGroupID);

						requestFlag = true;
					}
					else
					{
						userInfoBox->setIcon(QMessageBox::Warning);
						userInfoBox->setText("User request could not be added propperly at database due to unknown issues!");

						requestFlag = false;
					}

					//Show infoBox
					userInfoBox->exec();

					// Closing database
					closeSQLiteDB(db);

					// Returning a true or a false in case the writing request to database was successfully done or not
					return requestFlag;
				}
				else // In case the Database is not successfully opened
				{
					return false;
				}
			}
			else if (answer == QMessageBox::No) // When Pressing "No" 
			{
				return false;
			}
			else // In case other buttons are pressed
			{
				return false;
			}

		}
		else // Something went wrong and no group was found with 'nameGroup' into groups vector
		{
			msgBox.setText("No group was found with name '" + nameGroup + "'");
			msgBox.setStandardButtons(QMessageBox::Close);
			msgBox.setIcon(QMessageBox::Warning);
			msgBox.exec();

			return false;
		}

	}

	// Removes a request to join a group
	void removeRequest()
	{
		QMessageBox msgBox{};
		msgBox.setWindowTitle("Remove request");
		msgBox.setWindowIcon(QIcon(icons::groupPrefIcon));

		msgBox.setText("Are you sure you want to remove the request?");
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		msgBox.setIcon(QMessageBox::Question);
		int answer = msgBox.exec();

		// Declaring, as a std::string, the group ID of the current logged User
		int groupID { currentUser->getGroupID().sliced(DB::REQTO_TAG.size()).toInt() };

		// When Pressing "Yes" button 
		if (answer == QMessageBox::Yes)
		{
			sqlite3* db{};

			// Check te availability and opense the Database
			if (checkAndOpenSQLiteDB(db, userInfoBox, { DB::tableGroups, DB::tableUsers }) == DB::OPEN_SUCCESS)
			{

				// Setting the condition for updating the "group_id" column (Users Table) for the selected user
				std::string conditionUsers{ DB::Users::col_username + " = '" + currentUser->getUserName().toStdString() + "'" };

				// Updating in the database (Table Users and Table Groups) the user information for this request
				if (updateRecords(db, DB::tableUsers, DB::Users::col_groupID, std::to_string(DB::NO_GROUP), conditionUsers)
					&& updateRequestCol(db, currentUser, DB::Groups::col_inrequests, groupID, REMOVE_REQUEST))
				{
					userInfoBox->setIcon(QMessageBox::Information);
					userInfoBox->setText("User request was removed.");

					currentUser->setGroupID(QString::number(DB::NO_GROUP));
				}
				else
				{
					userInfoBox->setIcon(QMessageBox::Warning);
					userInfoBox->setText("User request could not be removed propperly from database due to unknown issues!");
				}

				//Show infoBox
				userInfoBox->exec();

				closeSQLiteDB(db);
			}
		}
		else if (answer == QMessageBox::No)
		{
			// No action needed.
		}

		selectView();
	}

	// Sends an invitation to one specific User (returns true for a successful invitation request. False otherwise)
	bool sendInvitation(labelButton* userLabel, int pos)
	{
		QMessageBox msgBox{};
		msgBox.setWindowTitle("Send invitation");
		msgBox.setWindowIcon(QIcon(icons::groupPrefIcon));

		msgBox.setText("Do you want to send an invitation to this user?");
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		msgBox.setIcon(QMessageBox::Question);
		int answer = msgBox.exec();

		bool invitationFlag{ false };

		// When Pressing "Yes" button 
		if (answer == QMessageBox::Yes)
		{
			sqlite3* db{};

			// Check te availability and opense the Database
			if (checkAndOpenSQLiteDB(db, userInfoBox, { DB::tableGroups, DB::tableUsers }) == DB::OPEN_SUCCESS)
			{			

				int groupID{ currentUser->getGroupID().toInt() };
				
				if (updateRequestCol(db, users[pos], DB::Groups::col_outrequests, groupID, ADD_REQUEST))
				{
					userInfoBox->setIcon(QMessageBox::Information);
					userInfoBox->setText("User invitation was successfuly sent!");
					invitationFlag = true;
				}
				else
				{
					invitationFlag = false;
				}
				
				closeSQLiteDB(db);
			}

			return invitationFlag;

		}
		else // (answer == QMessageBox::No)
		{
			return false;
		}

	}

	// User leaves the group he/she is belonging
	void leaveGroup()
	{

	}

};




#endif