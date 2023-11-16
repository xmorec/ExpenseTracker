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
bool updateRequestCol(sqlite3* db, User* user, const std::string& col_requests, Group* group, int MODE);


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
	labelButton* requestsButt{ new labelButton("Handle invitations/requests") };
	labelButton* leaveGroupButt{ new labelButton("Leave the group") };
	labelButton* renameGroupButt{ new labelButton("Rename group") };
	labelButton* membersButt{ new labelButton("Members") };
	labelButton* receivedInvButt{ new labelButton("Received Invitations") };
	//labelButton* removeGroupButt{ new labelButton("Remove group") };
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

	// A vector that contains one label for each existing group.
	// This is used to send requests to join a group
	std::vector<labelButton*> reqToGroupLabel{};

	// Create a Dialog useful to send invitation to a user
	QDialog* sendInvitationWin{ new QDialog() };

	// Create a Dialog useful to handle sent invitations and received requests
	QDialog* handleInvReqWin{ new QDialog() };

	// Create a Dialog useful to see received invitations
	QDialog* receivedInvitationWin{ new QDialog() };

	// Declaring HLayouts for each group that sent an invitation to currentUser. Each HLayout for each user will contain Label (group name) and icon buttons
	// to accept/decline invitations	
	std::vector<QHBoxLayout*> groupInvLay{};

	// A Flag is needed to know wheter creating Dialogs to send invitations or handle invitations/requests
	bool createdWindowsFlag{ false };

	// A vector that contains each group as a labelButton is created
	std::vector<labelButton*> userInvLabel{};

	// Declaring HLayouts for each user. Each HLayout for each user will contain Label (user name) and icon buttons
	// to accept/decline invitations or rquests		
	std::vector<QHBoxLayout*> userInvLay{};
	std::vector<QHBoxLayout*> userReqLay{};

	// VLayout used to fill all invitations sent to users
	QVBoxLayout* vLayInvitations{ new QVBoxLayout() };

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
		mainVLay->addWidget(receivedInvButt, 0, Qt::AlignCenter | Qt::AlignTop);
		mainVLay->addWidget(joinGroupButt, 0, Qt::AlignCenter | Qt::AlignTop);
		mainVLay->addWidget(removeReqButt, 0, Qt::AlignCenter | Qt::AlignTop);
		mainVLay->addWidget(inviteUserButt, 0, Qt::AlignCenter | Qt::AlignTop);
		mainVLay->addWidget(requestsButt, 0, Qt::AlignCenter | Qt::AlignTop);	
		mainVLay->addWidget(membersButt, 0, Qt::AlignCenter | Qt::AlignTop);
		mainVLay->addWidget(renameGroupButt, 0, Qt::AlignCenter | Qt::AlignTop);		
		mainVLay->addWidget(leaveGroupButt, 0, Qt::AlignCenter | Qt::AlignTop);
		//mainVLay->addWidget(removeGroupButt, 0, Qt::AlignCenter | Qt::AlignTop);
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

		//Create a Dialog useful to see and accept/decline Invitations from groups
		createReceivedInvitationsWindow();

		//Create a Dialogs depending whether the current user is member of a group or not
		if (currentUser->getGroupID().toInt() != DB::NO_GROUP)
		{
			sqlite3* db{}; // Create database object useful to read the invited users to the group
			createInvitationWindow(db, true); //  Create a Dialog useful to send invitations to users (in case the current user is member of a group)
			createHandleInvReqWindow(db, true); // Create a Dialog usefol to handle sent invitations and accept/decline user requests
			createdWindowsFlag = true; // Setting flag to 'true' as both Dialogs are now created
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
			handleInvReqWin->exec();
			});

		QObject::connect(leaveGroupButt, &QPushButton::clicked, [=]() {
			leaveGroup();
			});

		QObject::connect(renameGroupButt, &QPushButton::clicked, [=]() {

			});

		QObject::connect(receivedInvButt, &QPushButton::clicked, [=]() {
			receivedInvitationWin->exec();
			});

		QObject::connect(membersButt, &QPushButton::clicked, [=]() {
			//showMembers();
			});

		//QObject::connect(removeGroupButt, &QPushButton::clicked, [=]() {
			//});

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

		if (currentUser->getGroupID().toInt() != DB::NO_GROUP) // In case the User has a group
		{
			loadGroupView();
		}
		else // The user has no group
		{
			// Trying to search which is the group of 'groups' vector that user sent a request (in case he/she sent it)
			auto group_it{ std::find_if(groups.begin(), groups.end(), [=](Group* group) {return group->in_requests.contains(currentUser->getUserName()); }) };

			if (group_it != groups.end()) // In case the User has requested to be in a group
			{
				loadRequestSentView();
			}
			else if (currentUser->getGroupID().toInt() == DB::NO_GROUP) // In case the User has no group and has not sent a request to be part of one
			{
				loadNoGroupView();
			}
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

		auto vLayGroups{ new QVBoxLayout() };

		// For each active group, a labelButton is created and added to the layout
		for (Group* group : groups)
		{
			if (group->status == DB::Groups::status_active) // just adding the active groups
			{
				reqToGroupLabel.push_back(new labelButton(group->name));
				vLayGroups->addWidget(reqToGroupLabel.back());
			}
		}

		Join2GroupBox->setLayout(vLayGroups);
		join2GroupWin->setLayout(new QVBoxLayout());
		join2GroupWin->layout()->addWidget(Join2GroupBox);

		// Set fixed Dialog size (user cannot resize it)
		join2GroupWin->setWindowFlag(Qt::MSWindowsFixedSizeDialogHint, true);

		// For Sending Requests to join a group, a label button is created and connected to a signal for each existing group
		for (labelButton*& groupReq : reqToGroupLabel)
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

	//Create a Dialog useful to see and accept/decline Invitations from groups
	void createReceivedInvitationsWindow()
	{

		receivedInvitationWin->setWindowTitle("Group invitations");
		receivedInvitationWin->setWindowIcon(QIcon(icons::groupPrefIcon));

		QGroupBox* recievedInvBox = new QGroupBox("Join to a Group");

		// A VLayout is created which will contain all groups that sent an invitation to the current user
		auto vLayGroups{ new QVBoxLayout() };


		// A struct of Button and User is needed to send this user pointer to functions when button is clicked
		struct invAcceptDeclGroup {
			iconButton* acceptButt{ nullptr };
			iconButton* declineButt{ nullptr };
			Group* group{ nullptr };
		};

		std::vector<invAcceptDeclGroup> invAcceptDeclGroup{};

		// For each active group, a labelButton is created and added to the layout
		for (Group* group : groups)
		{
			if (group->status == DB::Groups::status_active && group->out_requests.contains(currentUser->getUserName())) // just adding the active groups
			{						
				// Creating a HLayout for current group, which will includ a QLabel and 2 buttons for accepting/denying invitation
				groupInvLay.push_back(new QHBoxLayout());

				// Adding widget QLabel with the Group Name
				groupInvLay.back()->addWidget(new QLabel(group->name));

				// Creating the 'Accept' and 'Decline' icon for the corresponding 'group' and Creating a relationship between the specific buttons and this group
				invAcceptDeclGroup.push_back({ new iconButton(QIcon(icons::accept), 15), new iconButton(QIcon(icons::decline), 15), group });

				// Setting Label, Icon, and layout regarding the corresponding 'user' to the layout for the requests section:
				//groupInvLay.back()->addWidget(userReqLabelx.back());
				groupInvLay.back()->addWidget(invAcceptDeclGroup.back().acceptButt);
				//groupInvLay.back()->addItem(new QSpacerItem(-10, 10));
				groupInvLay.back()->addWidget(invAcceptDeclGroup.back().declineButt);
				vLayGroups->addLayout(groupInvLay.back());

			}
		}

		recievedInvBox->setLayout(vLayGroups);
		receivedInvitationWin->setLayout(new QVBoxLayout());
		receivedInvitationWin->layout()->addWidget(recievedInvBox);

		// Set fixed Dialog size (user cannot resize it)
		receivedInvitationWin->setWindowFlag(Qt::MSWindowsFixedSizeDialogHint, true);

		// For Accepting/Denying Invitations to join a group, accept/deny buttons are connected to a signal for each existing group
		int pos{ 0 };
		// For Accepting or Denying group invitation
		for (struct invAcceptDeclGroup& invitation : invAcceptDeclGroup)
		{
			QObject::connect(invitation.acceptButt, &QPushButton::clicked, [=]() {

				acceptInvitation(invitation.group, groupInvLay[pos]);
				});

			QObject::connect(invitation.declineButt, &QPushButton::clicked, [=]() {

				removeInvitation(currentUser, invitation.group, groupInvLay[pos]);
				});

			pos++;
		}
	}


	//Create a Dialog useful to send invitations to users (db is Database object, 'dbOpen' is a flag used to open and check the database)
	void createInvitationWindow(sqlite3* db, bool dbOpen)
	{

		sendInvitationWin->setWindowTitle("Send invitations");
		sendInvitationWin->setWindowIcon(QIcon(icons::groupPrefIcon));

		QGroupBox* sendInvBox = new QGroupBox("Send Invitation");

		auto vLayUsrs{ new QVBoxLayout() };

		// Check the users that has been already invited to this group
		QString outRequests{}; // This QString will contain the list of users that already received an invitation

		// Declaring the groupID of the current User sending the invitation
		std::string groupID{ currentUser->getGroupID().toStdString() };

		if (dbOpen) // Database is requested to be opened and checked
		{
			// Check te availability and opens the Database
			if (checkAndOpenSQLiteDB(db, userInfoBox, { DB::tableGroups, DB::tableUsers }) == DB::OPEN_SUCCESS)
			{			
				// Selecting the cell from Groups Table regarding the "out_requests" or "in_requests" column in for the selected "group ID"
				outRequests = getRecords(db, DB::tableGroups, DB::Groups::col_outrequests, "WHERE " + DB::Groups::col_ID + " = '" + groupID + "'")[0][0];

				closeSQLiteDB(db);
			}
		}
		else // Database is already opened and checked and there is no need to do it
		{
			// Selecting the cell from Groups Table regarding the "out_requests" or "in_requests" column in for the selected "group ID"
			outRequests = getRecords(db, DB::tableGroups, DB::Groups::col_outrequests, "WHERE " + DB::Groups::col_ID + " = '" + groupID + "'")[0][0];
		}

		// Trying to search which is the group of 'groups' vector that user sent a request
		//auto group_it{ std::find_if(groups.begin(), groups.end(), [=](Group* group) {return group->ID == currentUser->getGroupID().toInt(); }) };

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

			// If user is a member of the currentUser group
			if (user->getGroupID() == currentUser->getGroupID())
			{
				userInvLabel.back()->setText(user->getUserName() + " (Member of your group)");
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

					// Creating the horizontal layout for the corresponding 'user'
					userInvLay.push_back(new QHBoxLayout());

					// Setting Label, Icon, and layout regarding the corresponding 'user' to the layout for the invitations section:
					userInvLay.back()->addWidget(new QLabel(users[pos]->getUserName()));
					iconButton* removeInvButt{ new iconButton(QIcon(icons::decline), 15) };
					userInvLay.back()->addWidget(removeInvButt);
					vLayInvitations->addLayout(userInvLay.back());

					QObject::connect(removeInvButt, &QPushButton::clicked, [=]() {
						auto group_it{ std::find_if(groups.begin(), groups.end(), [=](Group* groupx) {return groupx->ID == currentUser->getGroupID().toInt(); }) };
						removeInvitation(users[pos], (*group_it), userInvLay.back());
						});
				}

				});
		}

	}
	
	//Create a Dialog useful to remove invitations or accept/reject received joining requests
	void createHandleInvReqWindow(sqlite3* db, bool dbOpen)
	{
		handleInvReqWin->setWindowTitle("Handle Invitations and Requests");
		handleInvReqWin->setWindowIcon(QIcon(icons::groupPrefIcon));

		// Check the users that has been already invited to this group
		QString outRequests{}; // This QString will contain the list of users that already received an invitation
		QString inRequests{}; // This QString will contain the list of users that already sent a request

		// Declaring the groupID of the current User sending the invitation
		std::string groupID{ currentUser->getGroupID().toStdString() };

		if (dbOpen) // Database is requested to be opened and checked
		{

			// Check te availability and opens the Database
			if (checkAndOpenSQLiteDB(db, userInfoBox, { DB::tableGroups, DB::tableUsers }) == DB::OPEN_SUCCESS)
			{
				// Selecting the cell from Groups Table regarding the "out_requests" or "in_requests" column in for the selected "group ID"
				outRequests = getRecords(db, DB::tableGroups, DB::Groups::col_outrequests, "WHERE " + DB::Groups::col_ID + " = '" + groupID + "'")[0][0];
				inRequests = getRecords(db, DB::tableGroups, DB::Groups::col_inrequests, "WHERE " + DB::Groups::col_ID + " = '" + groupID + "'")[0][0];

				closeSQLiteDB(db);
			}
		}
		else // Database is already opened and checked and there is no need to do it
		{
			// Selecting the cell from Groups Table regarding the "out_requests" or "in_requests" column in for the selected "group ID"
			outRequests = getRecords(db, DB::tableGroups, DB::Groups::col_outrequests, "WHERE " + DB::Groups::col_ID + " = '" + groupID + "'")[0][0];
			inRequests = getRecords(db, DB::tableGroups, DB::Groups::col_inrequests, "WHERE " + DB::Groups::col_ID + " = '" + groupID + "'")[0][0];
		}

		// Declaring a Box for Invitations and a Box for Requests
		QGroupBox* sentInvBox = new QGroupBox("Sent Invitations");
		QGroupBox* receivedReqBox = new QGroupBox("Received Requests");

		// In each Box, a VLayout will be added to store each User invitation or request
		//auto vLayInvitations{ new QVBoxLayout() };
		auto vLayRequests{ new QVBoxLayout() };

		// Declaring labels vector. Each position will store the user name
		std::vector<QLabel*> userInvLabelx{};
		std::vector<QLabel*> userReqLabelx{};

		// A struct of Button and User is needed to send this user pointer to functions when button is clicked
		struct invDeclUsr {
			iconButton* declineButt{ nullptr };
			User* user { nullptr };
		};

		// A struct of Button and User is needed to send this user pointer to functions when button is clicked
		struct reqAcceptDecllUsr {
			iconButton* acceptButt{ nullptr };
			iconButton* declineButt{ nullptr };
			User* user{ nullptr };
		};			

		std::vector<invDeclUsr> invDeclUsr{};
		std::vector<reqAcceptDecllUsr> reqAcceptDecllUsr{};

		// Each invitation or user request for each user is added to the corresponding group (Invitation or Requests)
		for (User* user : users)
		{
			// If user was previously invited to this group
			if (outRequests.contains(user->getUserName()))
			{
				// Creating the horizontal layout for the corresponding 'user'
				userInvLay.push_back(new QHBoxLayout());

				// Creating the Label for the corresponding 'user'
				userInvLabelx.push_back(new QLabel(user->getUserName()));	

				// Creating the 'Decline' icon for the corresponding 'user' and Creating a relationship between the specific button and its user
				invDeclUsr.push_back({ new iconButton(QIcon(icons::decline), 15), user });
								
				// Setting Label, Icon, and layout regarding the corresponding 'user' to the layout for the invitations section:
				userInvLay.back()->addWidget(userInvLabelx.back());
				userInvLay.back()->addWidget(invDeclUsr.back().declineButt);
				vLayInvitations->addLayout(userInvLay.back());
			}

			// If user sent a request to this group
			if (inRequests.contains(user->getUserName()))
			{
				// Creating the horizontal layout for the corresponding 'user'
				userReqLay.push_back(new QHBoxLayout());

				// Creating the Label for the corresponding 'user'
				userReqLabelx.push_back(new QLabel(user->getUserName()));

				// Creating the 'Decline' icon for the corresponding 'user' and Creating a relationship between the specific buttons and its user
				reqAcceptDecllUsr.push_back({ new iconButton(QIcon(icons::accept), 15), new iconButton(QIcon(icons::decline), 15), user });

				// Setting Label, Icon, and layout regarding the corresponding 'user' to the layout for the requests section:
				userReqLay.back()->addWidget(userReqLabelx.back());				
				userReqLay.back()->addWidget(reqAcceptDecllUsr.back().acceptButt);
				userReqLay.back()->addItem(new QSpacerItem(-10, 10));
				userReqLay.back()->addWidget(reqAcceptDecllUsr.back().declineButt);
				vLayRequests->addLayout(userReqLay.back());
			}
		}

		sentInvBox->setLayout(vLayInvitations);
		vLayInvitations->setAlignment(Qt::AlignTop);
		receivedReqBox->setLayout(vLayRequests);
		vLayRequests->setAlignment(Qt::AlignTop);
		handleInvReqWin->setLayout(new QHBoxLayout());
		handleInvReqWin->layout()->addWidget(sentInvBox);
		handleInvReqWin->layout()->addWidget(receivedReqBox);

		// Set fixed Dialog size (user cannot resize it)
		handleInvReqWin->setWindowFlag(Qt::MSWindowsFixedSizeDialogHint, true);


		int pos{ 0 };
		// For Removing Invitations sent to users, every Decline button is connected to a signal for each existing user
		for (struct invDeclUsr& invitation : invDeclUsr)
		{
			QObject::connect(invitation.declineButt, &QPushButton::clicked, [=]() {
				// Remove Invitation sent to the user for the current user group
				auto group_it{ std::find_if(groups.begin(), groups.end(), [=](Group* groupx) {return groupx->ID == currentUser->getGroupID().toInt(); }) };
				removeInvitation(invitation.user, (*group_it), userInvLay[pos]);
				});
			pos++;
		}


		pos = 0;
		// For Accepting or Removing user requests to join the currentUser group
		for (struct reqAcceptDecllUsr& request : reqAcceptDecllUsr)
		{
			QObject::connect(request.acceptButt, &QPushButton::clicked, [=]() {

				acceptRequest(request.user, userReqLay[pos]);
				});

			QObject::connect(request.declineButt, &QPushButton::clicked, [=]() {

				removeRequest(request.user, userReqLay[pos]);
				});

			pos++;
		}
	}

	// Clears all content from a specific layout
	void deleteLayout(QHBoxLayout*& layout)
	{
		// Check if layout exists
		if (layout != nullptr)
		{
			// Delete all widgets from the layout
			while (QLayoutItem* item = layout->takeAt(0))
			{
				if (QWidget* widget = item->widget())
				{
					delete widget;
				}
				delete item;
			}

			// Free memory
			delete layout;

			// Set the layout pointer to nullptr
			layout = nullptr;
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
						record[3].split(", "), // in requests (users)
						record[4].split(", "), // out requests (users)
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
		membersButt->setVisible(false);
		receivedInvButt->setVisible(false);
		//removeGroupButt->setVisible(false);
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
		membersButt->setVisible(false);
		receivedInvButt->setVisible(true);
		//removeGroupButt->setVisible(false);
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
		membersButt->setVisible(true);
		receivedInvButt->setVisible(false);
		//removeGroupButt->setVisible(false);
		saveButt->setVisible(false);
		cancelButt->setVisible(false);

	}

	// Load the view when user sent a request to join a group
	void loadRequestSentView()
	{
		QString groupName{ "unknown" };

		// Trying to search which is the group of 'groups' vector that user sent a request
		auto group_it{ std::find_if(groups.begin(), groups.end(), [=](Group* group) {return group->in_requests.contains(currentUser->getUserName()); }) };

		if (group_it != groups.end())
		{
			groupName = (*group_it)->name;
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
		membersButt->setVisible(false);
		receivedInvButt->setVisible(true);
		//removeGroupButt->setVisible(false);
		saveButt->setVisible(false);
		cancelButt->setVisible(false);
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

				// Enrolling the user to the group (set the goup_id for the logged user according the new created group into Database)
				std::string condition{ DB::Users::col_username + " = '" + currentUser->getUserName().toStdString() + "'" };

				if (updateRecords(db, DB::tableUsers, DB::Users::col_groupID, std::to_string(groupNum), condition)) 
					// Group ID of the user could be added, and the user could not be added to this group
				{
					// Adds this new group to the 'groups' vector
					groups.push_back(new Group{ groupNum, newGroupName, {currentUser->getUserName()}, {"0"}, {"0"} });

					// Sets the new group ID to the current user
					currentUser->setGroupID(QString::number(groupNum));

					// In case windos for Invite users or handle Invitations/Requests is not created yet, both dialogs will be created
					if (!createdWindowsFlag)
					{
						// Generate the invitation window from a Database reading
						createInvitationWindow(db, false);

						// Generate the handle invitation and received requests window from a Database reading
						createHandleInvReqWindow(db, false);

						// Set flag to true
						createdWindowsFlag = true;
					}
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
		// Trying to search which is the group of 'groups' vector that matches with the group name 'nameGroup' and also is an active group
		auto group_it{ std::find_if(groups.begin(), groups.end(), [=](Group* group) { return (group->name == nameGroup) && (group->status == DB::Groups::status_active); }) };

		QMessageBox msgBox{};
		msgBox.setWindowTitle("Join group request");
		msgBox.setWindowIcon(QIcon(icons::groupPrefIcon));

		if (group_it != groups.end()) // If there is a group with 'nameGroup' in groups vector
		{
			if ((*group_it)->status == DB::Groups::status_inactive) // In case this group is an inactive group, it is considered as it does not exist
			{
				msgBox.setText("No group was found with name '" + nameGroup + "'");
				msgBox.setStandardButtons(QMessageBox::Close);
				msgBox.setIcon(QMessageBox::Warning);
				msgBox.exec();
				return false;
			}			

			msgBox.setText("Do you want to request to join group '" + nameGroup + "'?");
			msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
			msgBox.setIcon(QMessageBox::Question);
			int answer = msgBox.exec();

			// When Pressing "Yes" button 
			if (answer == QMessageBox::Yes)
			{
				sqlite3* db{};

				// Check te availability and opense the Database
				if (checkAndOpenSQLiteDB(db, userInfoBox, { DB::tableGroups }) == DB::OPEN_SUCCESS)
				{

					// This flag will be the returning value for this function according a successful request writing to database or not
					bool requestFlag{};

					// Setting the condition for updating the "group_id" column (Users Table) for the selected user
					std::string conditionUsers { DB::Users::col_username + " = '" + currentUser->getUserName().toStdString() + "'" };		

					if (updateRequestCol(db, currentUser, DB::Groups::col_inrequests, (*group_it), ADD_REQUEST))
					{
						userInfoBox->setIcon(QMessageBox::Information);
						userInfoBox->setText("User request was successfuly sent!");

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

	// Accepts a request from a user that wants to join the group
	void acceptRequest(User* user, QHBoxLayout* userReqLay)
	{
		QMessageBox msgBox{};
		msgBox.setWindowTitle("Accept request");
		msgBox.setWindowIcon(QIcon(icons::groupPrefIcon));

		msgBox.setText("Are you sure you want to accept the request?");
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		msgBox.setIcon(QMessageBox::Question);
		int answer = msgBox.exec();

		bool removeFlag{ false };

		// When Pressing "Yes" button 
		if (answer == QMessageBox::Yes)
		{
			sqlite3* db{};

			// Check te availability and opense the Database
			if (checkAndOpenSQLiteDB(db, userInfoBox, { DB::tableGroups, DB::tableUsers }) == DB::OPEN_SUCCESS)
			{

				// Enrolling the user to the group (set the goup_id for the logged user according the new created group into Database)
				std::string condition{ DB::Users::col_username + " = '" + user->getUserName().toStdString() + "'" };

				// Trying to search which is the group of 'groups' vector that user sent a request
				auto group_it{ std::find_if(groups.begin(), groups.end(), [=](Group* group) {return group->in_requests.contains(user->getUserName()); }) };

				// Updating in the database (Table Users) to set the user group to the one he/she requested
				if (updateRecords(db, DB::tableUsers, DB::Users::col_groupID, std::to_string((*group_it)->ID), condition))
				{

					// Updating in the database (Table Groups): adding the user to the group and removing him from "In Requests" (col_inrequests)
					if (updateRequestCol(db, user, DB::Groups::col_inrequests, (*group_it), REMOVE_REQUEST)
						&& updateRequestCol(db, user, DB::Groups::col_users, (*group_it), ADD_REQUEST))
					{
						userInfoBox->setIcon(QMessageBox::Information);
						userInfoBox->setText("User " + user->getUserRName() + " is now a member of your group.");

						removeFlag = true;

						// Adding the new user to the 'groups' vector
						(*group_it)->users.append(user->getUserName());

						// Sets the new group ID to the current user
						currentUser->setGroupID(QString::number((*group_it)->ID));
					}
					else
					{
						userInfoBox->setIcon(QMessageBox::Warning);
						userInfoBox->setText("User request could not be approved propperly from database due to unknown issues!");

						removeFlag = false;
					}
				}
				else
				{
					// When user could not be added to the group in Database
					userInfoBox->setIcon(QMessageBox::Warning);
					userInfoBox->setText("User request could not be approved propperly from database due to unknown issues!");
				}

				//Show infoBox
				userInfoBox->exec();

				closeSQLiteDB(db);
			}
		}
		else if (answer == QMessageBox::No)
		{
			removeFlag = false;
		}

		if (removeFlag)
		{
			deleteLayout(userReqLay);

			// Disabling sending invitations to a the new member of your group
			auto userInvLbl{ std::find_if(userInvLabel.begin(), userInvLabel.end(), [=](labelButton* userButton) { return userButton->text().contains(user->getUserName()); }) };
			(*userInvLbl)->setText(user->getUserName() + " (Member of your group)");
			(*userInvLbl)->setDisabled(true);

			// Update the Dialog size according the current content
			QTimer::singleShot(50, [=]() {
				handleInvReqWin->resize(handleInvReqWin->sizeHint());
				});

		}

		selectView();
	}

	// Removes a request to join a group
	void removeRequest(User* user = nullptr, QHBoxLayout* userReqLay = nullptr)
	{
		QMessageBox msgBox{};
		msgBox.setWindowTitle("Remove request");
		msgBox.setWindowIcon(QIcon(icons::groupPrefIcon));

		msgBox.setText("Are you sure you want to remove the request?");
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		msgBox.setIcon(QMessageBox::Question);
		int answer = msgBox.exec();

		bool removeFlag{ false };

		if (!user) // In case no user is set as input (nullptr)
		{
			user = currentUser;
		}

		// When Pressing "Yes" button 
		if (answer == QMessageBox::Yes)
		{
			sqlite3* db{};

			// Check te availability and opense the Database
			if (checkAndOpenSQLiteDB(db, userInfoBox, { DB::tableGroups, DB::tableUsers }) == DB::OPEN_SUCCESS)
			{

				// Trying to search which is the group of 'groups' vector that user sent a request
				auto group_it{ std::find_if(groups.begin(), groups.end(), [=](Group* group) {return group->in_requests.contains(user->getUserName()); }) };

				// Updating in the database (Table Users and Table Groups) the user information for this request
				if (updateRequestCol(db, user, DB::Groups::col_inrequests, (*group_it), REMOVE_REQUEST))
				{
					userInfoBox->setIcon(QMessageBox::Information);
					userInfoBox->setText("User request was removed.");

					removeFlag = true;
				}
				else
				{
					userInfoBox->setIcon(QMessageBox::Warning);
					userInfoBox->setText("User request could not be removed propperly from database due to unknown issues!");

					removeFlag = false;
				}

				//Show infoBox
				userInfoBox->exec();

				closeSQLiteDB(db);
			}
		}
		else if (answer == QMessageBox::No)
		{
			removeFlag = false;
		}

		if (removeFlag)
		{
			deleteLayout(userReqLay);


			//auto userInvLbl{ std::find_if(userInvLabel.begin(), userInvLabel.end(), [=](labelButton* userButton) { return userButton->text().contains(user->getUserName()); }) };
			//(*userInvLbl)->setText(user->getUserName());
			//(*userInvLbl)->setDisabled(false);

			// Update the Dialog size according the current content
			QTimer::singleShot(50, [=]() {
				handleInvReqWin->resize(handleInvReqWin->sizeHint());
				});

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
				// Trying to search which is the group of 'groups' vector that user sent a request
				auto group_it{ std::find_if(groups.begin(), groups.end(), [=](Group* group) {return group->users.contains(currentUser->getUserName()); }) };

				if (updateRequestCol(db, users[pos], DB::Groups::col_outrequests, (*group_it), ADD_REQUEST))
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

				return invitationFlag;

			}
			else // In case Database is not successfully opened
			{
				return false;
			}
		}
		else // (answer == QMessageBox::No)
		{
			return false;
		}

	}

	// Accepts an Invitation coming from group
	void acceptInvitation(Group* group, QHBoxLayout* groupLay)
	{
		QMessageBox msgBox{};
		msgBox.setWindowTitle("Accept invitation");
		msgBox.setWindowIcon(QIcon(icons::groupPrefIcon));

		msgBox.setText("Are you sure you want to accept this invitation?");
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		msgBox.setIcon(QMessageBox::Question);
		int answer = msgBox.exec();

		bool acceptFlag{ false };

		// When Pressing "Yes" button 
		if (answer == QMessageBox::Yes)
		{
			sqlite3* db{};

			// Check te availability and opense the Database
			if (checkAndOpenSQLiteDB(db, userInfoBox, { DB::tableGroups, DB::tableUsers }) == DB::OPEN_SUCCESS)			{

				// Enrolling the user to the group (set the goup_id for the logged user according the new created group into Database)
				std::string condition{ DB::Users::col_username + " = '" + currentUser->getUserName().toStdString() + "'" };

				// Updating in the database (Table Users) to set the user group to the one he/she requested
				if (updateRecords(db, DB::tableUsers, DB::Users::col_groupID, std::to_string(group->ID), condition))
				{

					// Updating in the database (Table Groups): adding the user to the group and removing him from "In Requests" (col_inrequests)
					if (updateRequestCol(db, currentUser, DB::Groups::col_inrequests, group, REMOVE_REQUEST)
						&& updateRequestCol(db, currentUser, DB::Groups::col_users, group, ADD_REQUEST))
					{
						userInfoBox->setIcon(QMessageBox::Information);
						userInfoBox->setText("Now you are member of group: " + group->name);

						acceptFlag = true;

						// Adding the new user to the 'groups' vector
						group->users.append(currentUser->getUserName());

						// Sets the new group ID to the current user
						currentUser->setGroupID(QString::number(group->ID));
					}
					else
					{
						userInfoBox->setIcon(QMessageBox::Warning);
						userInfoBox->setText("Invitation could not be accepted propperly from database due to unknown issues!");

						acceptFlag = false;
					}
				}
				else
				{
					// When user could not be added to the group in Database
					userInfoBox->setIcon(QMessageBox::Warning);
					userInfoBox->setText("Invitation could not be accepted propperly from database due to unknown issues!");
				}

				//Show infoBox
				userInfoBox->exec();

				closeSQLiteDB(db);
			}
		}
		else if (answer == QMessageBox::No)
		{
			acceptFlag = false;
		}

		if (acceptFlag)
		{
			deleteLayout(groupLay);

			// Update the Dialog size according the current content
			QTimer::singleShot(50, [=]() {
				handleInvReqWin->resize(handleInvReqWin->sizeHint());
				});

		}

		selectView();
	}

	// Removes a Invitation to a user coming from a specific group
	void removeInvitation(User* user, Group* group, QHBoxLayout* userInvLay)
	{
		QMessageBox msgBox{};
		msgBox.setWindowTitle("Remove Invitation");
		msgBox.setWindowIcon(QIcon(icons::groupPrefIcon));

		msgBox.setText("Are you sure you want to remove this invitation?");
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		msgBox.setIcon(QMessageBox::Question);
		int answer = msgBox.exec();

		// Recieves the successful updating state in Database (true) or not (false).
		bool removeFlag{ false };

		// When Pressing "Yes" button 
		if (answer == QMessageBox::Yes)
		{
			sqlite3* db{};

			// Check te availability and opense the Database
			if (checkAndOpenSQLiteDB(db, userInfoBox, { DB::tableGroups }) == DB::OPEN_SUCCESS)
			{

				// Updating in the database (Table Groups) the user information for this request
				if (updateRequestCol(db, user, DB::Groups::col_outrequests, group, REMOVE_REQUEST))
				{
					userInfoBox->setIcon(QMessageBox::Information);
					userInfoBox->setText("Invitation was removed.");

					currentUser->setGroupID(QString::number(DB::NO_GROUP));

					removeFlag = true;
				}
				else
				{
					userInfoBox->setIcon(QMessageBox::Warning);
					userInfoBox->setText("Invitation could not be removed propperly from database due to unknown issues!");

					removeFlag = false;
				}

				//Show infoBox
				userInfoBox->exec();

				closeSQLiteDB(db);				
			}

		}
		else // (answer == QMessageBox::No)
		{
			removeFlag = false;
		}


		if (removeFlag)
		{
			deleteLayout(userInvLay);

			auto userInvLbl{ std::find_if(userInvLabel.begin(), userInvLabel.end(), [=](labelButton* userButton) { return userButton->text().contains(user->getUserName()); }) };
			(*userInvLbl)->setText(user->getUserName());
			(*userInvLbl)->setDisabled(false);

			// Update the Dialog size according the current content
			QTimer::singleShot(50, [=]() {
				handleInvReqWin->resize(handleInvReqWin->sizeHint());
				});

		}

	}	

	// User leaves the group he/she is belonging
	void leaveGroup()
	{
		QMessageBox msgBox{};
		msgBox.setWindowTitle("Leave the group");
		msgBox.setWindowIcon(QIcon(icons::groupPrefIcon));

		msgBox.setText("Are you sure you want to leave the group?");
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		msgBox.setIcon(QMessageBox::Question);
		int answer = msgBox.exec();

		bool leaveFlag{ false };

		// When Pressing "Yes" button 
		if (answer == QMessageBox::Yes)
		{
			sqlite3* db{};

			// Check te availability and opense the Database
			if (checkAndOpenSQLiteDB(db, userInfoBox, { DB::tableGroups, DB::tableUsers }) == DB::OPEN_SUCCESS)
			{

				// Enrolling the user to the group (set the goup_id for the logged user according the new created group into Database)
				std::string condition{ DB::Users::col_username + " = '" + currentUser->getUserName().toStdString() + "'" };

				// Trying to search which is the group of 'groups' vector that user is part of
				auto group_it{ std::find_if(groups.begin(), groups.end(), [=](Group* group) {return group->users.contains(currentUser->getUserName()); }) };
				
				// Updating in the database (Table Users) to set the user group to the one he/she requested
				if (updateRecords(db, DB::tableUsers, DB::Users::col_groupID, std::to_string(DB::NO_GROUP), condition))
				{

					// Updating in the database (Table Groups): adding the user to the group and removing him from "In Requests" (col_inrequests)
					if (updateRequestCol(db, currentUser, DB::Groups::col_users, (*group_it), REMOVE_REQUEST))
					{
						userInfoBox->setIcon(QMessageBox::Information);
						userInfoBox->setText("You are not longer member of the group.");

						leaveFlag = true;

						// Sets the new group ID to the current user
						currentUser->setGroupID(QString::number(DB::NO_GROUP));

						// Removing invitation layouts from Handle Invitations/Requests window
						for (QHBoxLayout* invLay : userInvLay)
						{
							deleteLayout(invLay);
						}

						// Removing request layouts from Handle Invitations/Requests window
						for (QHBoxLayout* reqLay : userReqLay)
						{
							deleteLayout(reqLay);
						}	

						// Removes the group to the 'groups' vector and disable it in Database in case this group has not other members
						if ((*group_it)->users.isEmpty())
						{
							// Setting the group as inactive in Database and removing all In Requests and Out Requests
							condition = DB::Groups::col_ID + " = '" + std::to_string((*group_it)->ID) + "'";
							if (updateRecords(db, DB::tableGroups, DB::Groups::col_status, DB::Groups::status_inactive.toStdString(), condition)
								&& updateRecords(db, DB::tableGroups, DB::Groups::col_inrequests, "", condition)
								&& updateRecords(db, DB::tableGroups, DB::Groups::col_outrequests, "", condition))
							{			
								// Setting the group as inactive in 'groups' vector
								(*group_it)->status = DB::Groups::status_inactive;

								// Clearing the In Requests and Out Requests of the group
								(*group_it)->in_requests.clear();
								(*group_it)->out_requests.clear();

								// Removing the group as requestable to be joined
								auto reqLabel_it{ std::find_if(reqToGroupLabel.begin(), reqToGroupLabel.end(), [=](labelButton* reqLabel) {return reqLabel->text() == (*group_it)->name; }) };
								if (reqLabel_it != reqToGroupLabel.end())
								{
									delete (*reqLabel_it);
								}
							}
						}

					}
					else
					{
						userInfoBox->setIcon(QMessageBox::Warning);
						userInfoBox->setText("You could not be removed from the group propperly from database due to unknown issues!");

						leaveFlag = false;
					}
				}
				else
				{
					// When user could not be added to the group in Database
					userInfoBox->setIcon(QMessageBox::Warning);
					userInfoBox->setText("You could not be removed from the group propperly from database due to unknown issues!");
				}

				//Show infoBox
				userInfoBox->exec();

				closeSQLiteDB(db);
			}
		}
		else if (answer == QMessageBox::No)
		{
			leaveFlag = false;
		}

		if (leaveFlag)
		{

			// Enabling sending invitations to all users again
			for (User* user : users)
			{
				auto userInvLbl{ std::find_if(userInvLabel.begin(), userInvLabel.end(), [=](labelButton* userButton) { return userButton->text().contains(user->getUserName()); }) };
				(*userInvLbl)->setText(user->getUserName());
				(*userInvLbl)->setDisabled(false);
			}			

		}

		selectView();
	}

	// Renaming the group
	void renameGroup()
	{

	}

};




#endif