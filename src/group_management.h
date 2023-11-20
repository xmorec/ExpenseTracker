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

// imageSize is useful to give the main pictures of the QDialog the correct size
namespace imageSize
{
	inline const QSize groupMember { 127, 132 };
	inline const QSize noGroup { 127, 95 };
	inline const QSize requestToGroup { 130, 100 };
	inline const QSize requestFromGroup{ 130, 120 };
	inline const QSize notification { 15, 15 };
}

// These constants are useful to add or remove requests/invitations in the Groups Table (database)
inline constexpr int ADD_REQUEST{ 0 };
inline constexpr int REMOVE_REQUEST{ 1 };

//  Adds or removes a user to the requests column (in_requests or out_requests) according the MODE in Database. Returns TRUE (successful Database). False otherwise
bool updateRequestCol(sqlite3* db, User* user, const std::string& col_requests, Group* group, int MODE);

// Deletes a group from Database and from 'groups' vector. Returns true for a successful removing, false otherwise
bool deleteGroup(sqlite3* db, Group* group, QMessageBox* userInfoBox, bool dbOpen);


class groupManWindow : public QDialog
{

	Q_OBJECT

private:

	// Informing QMessageBox
	QMessageBox* userInfoBox = new QMessageBox();

	// Current Logged User
	User* currentUser{};

	// Vector storing all Users
	std::vector<User*>& users;

	// Vector storing all Groups
	std::vector<Group*>& groups;

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
	labelButton* saveButt{ new labelButton("Save") };
	labelButton* cancelButt{ new labelButton("Cancel") };	

	// Vertical Layout containing all widgets
	QVBoxLayout* mainVLay{ new QVBoxLayout() };

	// Dialog section used to create new group
	QLineEdit* newNameLineEdit{ new QLineEdit()};
	QHBoxLayout* newNameLay{ new QHBoxLayout() };
	QHBoxLayout* saveCancelLay{ new QHBoxLayout() };
	QLabel* newNameLabel { new QLabel("Set a group name:")};

	/////////////////////////////////////////////////////////////
	// Create a Dialog useful to join to any created group
	QDialog* join2GroupWin {new QDialog ()};

	// Information text in Requesting to join to a group
	QLabel* join2GroupText { new QLabel() };

	// Layout which contains each group
	QVBoxLayout* vLayReqToGroups{ new QVBoxLayout() };

	// A vector that contains one label for each existing group.
	// This is used to send requests to join a group
	std::vector<labelButton*> reqToGroupLabel{};
	/////////////////////////////////////////////////////////////
	 
	/////////////////////////////////////////////////////////////
	// Create a Dialog useful to send invitation to a user
	QDialog* sendInvitationWin{ nullptr };

	// Box that will contain all possible users to send an invitation
	QVBoxLayout* vLayUsrs{ new QVBoxLayout() };

	// Information text in Sending invitation window
	QLabel* sendInvToUsrText{ new QLabel() };

	// A vector that contains each group as a labelButton is created
	std::vector<labelButton*> userInvLabel{};
	/////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////
	// Create a Dialog useful to handle sent invitations and received requests
	QDialog* handleInvReqWin{ nullptr };

	// VLayout used to fill all invitations sent to users in Handle Window
	QVBoxLayout* vLayInvitations{ new QVBoxLayout() };

	// VLayout used to fill all received requests to users in Handle Window
	QVBoxLayout* vLayRequests{ new QVBoxLayout() };

	// Information text in Sending invitation Box of Handle Inv/Req window
	QLabel* handleSentInvText { new QLabel() };

	// Information text in Received Requests Box of Handle Inv/Req window
	QLabel* handleReceivedReqText { new QLabel() };

	// Declaring HLayouts for each user. Each HLayout for each user will contain Label (user name) and icon buttons
	// to accept/decline invitations or rquests		
	std::vector<QHBoxLayout*> userInvLay{};
	std::vector<QHBoxLayout*> userReqLay{};
	/////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////
	// Create a Dialog useful to see received invitations
	QDialog* receivedInvitationWin{ nullptr };

	// This Layout will contain every received invitation to user
	QVBoxLayout* vLayGroups{ new QVBoxLayout() };

	// Information text in Received Invitations window
	QLabel* receivedInvText { new QLabel() };

	// Declaring HLayouts for each group that sent an invitation to currentUser. 
	// Each HLayout for each user will contain QLabel (group name) and icon buttons to accept/decline invitations	
	std::vector<QHBoxLayout*> groupInvLay{};
	/////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////
	// Create a Dialog useful to show Members Window
	QDialog* membersWin{ nullptr };

	// Information text in Members Windows
	QLabel* membersText{ new QLabel() };

	// VLayout used to fill all group members in Members Window
	QVBoxLayout* vLayMembers{ new QVBoxLayout() };

	// A vector that contains one layout for each member user
	std::vector<QHBoxLayout*> memberLay{};
	/////////////////////////////////////////////////////////////

	// Notification images shown when there are invitations or requests to handle
	QLabel* notificationInvImg{ new QLabel() };
	QLabel* notificationReqImg{ new QLabel() };
	QPixmap notImgPix{};

	// A Flag is needed to know wheter creating Dialogs to send invitations or handle invitations/requests
	bool createdWindowsFlag{ false };

public:

	// Constructs the main QDialog window used to manage the group preferences (join, create, leave, or edit a group. 
	// Also, requesting to be part of a group or invite someone to be part of your group could be performed here)
	groupManWindow(User* currentUser, std::vector<User*>& users, std::vector<Group*>& groups);

	// Returns true in case there are Invitations (and user is not member of a group), false otherwise
	bool getNewsInvStatus();

	// Returns true in case there are InRequests and user is part of a group, false otherwise
	bool getNewsReqStatus();

	// Returns true if Dialog windows regarding Group are created or not (Invitation Users, Members and Handle Inv/Reqs)
	bool getGroupDialogsStatus();

	// Selects a view according whether the user is member of a group or not, and taking into account if he/she sent or received a request
	// to join to a group.
	void selectView();

	//Create a Dialog useful to send invitations to users
	void createSendRequestsWindow();

	// Update Send Requests Window
	void updateSendRequestsWindow();

	//Create a Dialog useful to see and accept/decline Invitations from groups
	void createReceivedInvitationsWindow();

	// Update the received invitations window
	void updateReceivedInvitationsWindow();

	//Create a Dialog useful to send invitations to users (db is Database object, 'dbOpen' is a flag used to open and check the database)
	void createInvitationWindow();
	
	// Update the Invitation Window
	void updateInvitationWindow();

	//Create a Dialog useful to remove invitations or accept/reject received joining requests
	void createHandleInvReqWindow();

	// Update Handle Invitation/Request Window
	void updateHandleInvReqWindow();

	// Create Showing Members Window
	void createMembersWindow();

	// Update Members Win with current Members
	void updateMembersWindow();

	// Clears all content from a specific layout
	template <typename LayoutType>
	void deleteLayout(LayoutType*& layout);

	// Load the section for creating a group
	void loadCreateGroupSection();

	// Loas Rename Group View
	void loadRenameGroup();

	// Load the view when a user has no group and nor request/invitation
	void loadNoGroupView();

	// Load the view when user is a member of a group
	void loadGroupView();

	// Load the view when user sent a request to join a group
	void loadRequestSentView();

	// User creates a group and he/she is enrolled to it
	void createGroup();

	// Creates a join request to a group according the input parameter 'nameGroup'. Returns true if request is done, returns false if it is not.
	bool joinRequest(const QString& nameGroup);

	// Accepts a request from a user that wants to join the group
	void acceptRequest(User* user, QHBoxLayout* reqLay);

	// Removes a request to join a group
	void removeRequest(User* user = nullptr, QHBoxLayout* reqLay = nullptr);

	// Sends an invitation to one specific User (returns true for a successful invitation request. False otherwise)
	bool sendInvitation(labelButton* userLabel, int pos);

	// Accepts an Invitation coming from group
	void acceptInvitation(Group* group, QHBoxLayout* groupLay);

	// Removes a Invitation that user received coming from a specific group
	void removeInvitation(User* user, Group* group, QHBoxLayout* invLay, QDialog* inWindow);

	// User leaves the group he/she is belonging
	void leaveGroup();

	// Renaming the group
	void renameGroup();

	signals:
		// Emited signal when no other invitations or requests are present
		void clearedNews();

		// Emited signal when new requests are present because of joining to a group
		void raisedNews();

		// Emited signal when group name has changed
		void renamedGroup();
};


#endif