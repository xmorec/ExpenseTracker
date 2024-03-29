#include "group_management.h"


// Constructs the main QDialog window used to manage the group preferences (join, create, leave, or edit a group. 
// Also, requesting to be part of a group or invite someone to be part of your group could be performed here)
groupManWindow::groupManWindow(User* currentUser, std::vector<User*>& users, std::vector<Group*>& groups) :
	currentUser(currentUser), users(users), groups(groups)
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

	// Adding the Notification Image and the layout where they are going to be fit in
	notImgPix = icons::raisedNews;
	notificationInvImg->setPixmap(notImgPix.scaled(imageSize::notification));
	notificationReqImg->setPixmap(notImgPix.scaled(imageSize::notification));
	QHBoxLayout* receivedInvButtLay{ new QHBoxLayout() };
	QHBoxLayout* receivedReqButtLay{ new QHBoxLayout() };

	// Adding all labels and buttons and layouts into the main QDialog
	mainVLay->addWidget(createGroupButt, 0, Qt::AlignCenter | Qt::AlignTop);
	newNameLay->addWidget(newNameLabel, 0, Qt::AlignRight);
	newNameLay->addWidget(newNameLineEdit, 0, Qt::AlignLeft);
	newNameLineEdit->setFixedWidth(130);
	mainVLay->addLayout(newNameLay);
	newNameLay->setAlignment(Qt::AlignCenter);

	receivedInvButtLay->addItem(new QSpacerItem(55, 15, QSizePolicy::Expanding, QSizePolicy::Minimum));
	receivedInvButtLay->addWidget(notificationInvImg, 0, Qt::AlignRight);
	receivedInvButtLay->addWidget(receivedInvButt, 0, Qt::AlignCenter);
	receivedInvButtLay->addItem(new QSpacerItem(55, 15, QSizePolicy::Expanding, QSizePolicy::Minimum));
	mainVLay->addLayout(receivedInvButtLay);

	mainVLay->addWidget(joinGroupButt, 0, Qt::AlignCenter | Qt::AlignTop);
	mainVLay->addWidget(removeReqButt, 0, Qt::AlignCenter | Qt::AlignTop);
	mainVLay->addWidget(inviteUserButt, 0, Qt::AlignCenter | Qt::AlignTop);

	receivedReqButtLay->addWidget(notificationReqImg, 0, Qt::AlignRight | Qt::AlignCenter);
	receivedReqButtLay->addWidget(requestsButt, 0, Qt::AlignCenter | Qt::AlignTop);
	mainVLay->addLayout(receivedReqButtLay);

	mainVLay->addWidget(membersButt, 0, Qt::AlignCenter | Qt::AlignTop);
	mainVLay->addWidget(renameGroupButt, 0, Qt::AlignCenter | Qt::AlignTop);
	mainVLay->addWidget(leaveGroupButt, 0, Qt::AlignCenter | Qt::AlignTop);
	saveCancelLay->addWidget(saveButt, 0, Qt::AlignRight);
	saveCancelLay->addWidget(cancelButt, 0, Qt::AlignLeft);
	mainVLay->addLayout(saveCancelLay);
	saveCancelLay->setAlignment(Qt::AlignCenter);

	// Setting the layout shown in the main QDialog window
	setLayout(mainVLay);

	//Create a Dialog useful to join to any created group
	createSendRequestsWindow();

	//Create a Dialog useful to see and accept/decline Invitations from groups
	createReceivedInvitationsWindow();

	//Create a Dialogs depending whether the current user is member of a group or not
	if (currentUser->getGroupID().toInt() != DB::NO_GROUP)
	{
		sqlite3* db{}; // Create database object useful to read the invited users to the group
		createInvitationWindow(); //  Create a Dialog useful to send invitations to users (in case the current user is member of a group)
		createHandleInvReqWindow(); // Create a Dialog useful to handle sent invitations and accept/decline user requests
		createMembersWindow(); // Create a Dialog useful to show all group members of the current user group
		createdWindowsFlag = true; // Setting flag to 'true' as both Dialogs are now created
	}

	// Select and load proper view according the group status of logged user
	selectView();

	// Generate the lambda functions when pressing buttons
	QObject::connect(createGroupButt, &QPushButton::clicked, [=]() {
		loadCreateGroupSection();
		});

	QObject::connect(joinGroupButt, &QPushButton::clicked, [=]() {
		QTimer::singleShot(50, [&]() {
			join2GroupWin->resize(join2GroupWin->sizeHint());
			});
		join2GroupWin->exec();
		});

	QObject::connect(removeReqButt, &QPushButton::clicked, [=]() {
		removeRequest();
		});

	QObject::connect(inviteUserButt, &QPushButton::clicked, [=]() {
		QTimer::singleShot(50, [&]() {
			sendInvitationWin->resize(sendInvitationWin->sizeHint());
			});
		sendInvitationWin->exec();
		});

	QObject::connect(requestsButt, &QPushButton::clicked, [=]() {
		QTimer::singleShot(50, [&]() {
			handleInvReqWin->resize(handleInvReqWin->sizeHint());
			});
		handleInvReqWin->exec();
		});

	QObject::connect(leaveGroupButt, &QPushButton::clicked, [=]() {
		leaveGroup();
		});

	QObject::connect(renameGroupButt, &QPushButton::clicked, [=]() {
		loadRenameGroup();
		});

	QObject::connect(receivedInvButt, &QPushButton::clicked, [=]() {
		QTimer::singleShot(50, [&]() {
			receivedInvitationWin->resize(receivedInvitationWin->sizeHint());
			});
		receivedInvitationWin->exec();
		});

	QObject::connect(membersButt, &QPushButton::clicked, [=]() {
		QTimer::singleShot(50, [&]() {
			membersWin->resize(membersWin->sizeHint());
			});
		membersWin->exec();
		});

	QObject::connect(saveButt, &QPushButton::clicked, [=]() {
		// Save Button will act in different ways according the user wants to create a group or rename it.
		if (currentUser->getGroupID().toInt() == DB::NO_GROUP) // When User does not have a group, one is created
		{
			createGroup();
		}
		else // In case the user is a member of a group, he renames it
		{
			renameGroup();
		}

		});

	QObject::connect(cancelButt, &QPushButton::clicked, [=]() {
		selectView();
		});
}

// Returns true in case there are Invitations (and user is not member of a group), false otherwise
bool groupManWindow::getNewsInvStatus()
{

	// Trying to search the current User Group
	auto userGroup_it{ std::find_if(groups.begin(), groups.end(), [=](Group* group) { return group->ID == currentUser->getGroupID().toInt(); }) };

	// hasGroup = true (User is a member of a group)
	bool hasGroup{ userGroup_it != groups.end() };

	// Trying to search if there is at least one group that sent an invitation to current User
	auto groupInv_it{ std::find_if(groups.begin(), groups.end(), [=](Group* group) { return group->out_requests.contains(currentUser->getUserName()); }) };

	// hasInvitation = true (User received at least one invitation)
	bool hasInvitation{ groupInv_it != groups.end() };

	if (!hasGroup)
	{
		return hasInvitation;
	}
	else
	{
		return false;
	}

}

// Returns true in case there are InRequests and user is part of a group, false otherwise
bool groupManWindow::getNewsReqStatus()
{

	// Trying to search the current User Group
	auto userGroup_it{ std::find_if(groups.begin(), groups.end(), [=](Group* group) { return group->ID == currentUser->getGroupID().toInt(); }) };

	// hasGroup = true (User is a member of a group)
	bool hasGroup{ userGroup_it != groups.end() };

	// hasRequests = true (User Group received at least one request)
	bool hasRequests{ hasGroup && ((*userGroup_it)->in_requests.size() > 0) };

	if (hasGroup)
	{
		return hasRequests;
	}
	else
	{
		return false;
	}

}

// Returns true if Dialog windows regarding Group are created or not (Invitation Users, Members and Handle Inv/Reqs)
bool groupManWindow::getGroupDialogsStatus()
{
	return createdWindowsFlag;
}

// Selects a view according whether the user is member of a group or not, and taking into account if he/she sent or received a request
// to join to a group.
void groupManWindow::groupManWindow::selectView()
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
void groupManWindow::groupManWindow::createSendRequestsWindow()
{
	join2GroupWin->setWindowTitle("Join to a group");
	join2GroupWin->setWindowIcon(QIcon(icons::groupPrefIcon));

	QGroupBox* Join2GroupBox = new QGroupBox("Join to a Group");

	// Adding the information text to the main layout
	join2GroupText->setText("There are no existing groups.");
	vLayReqToGroups->addWidget(join2GroupText);

	// Update Send Requests Window
	updateSendRequestsWindow();

	Join2GroupBox->setLayout(vLayReqToGroups);
	join2GroupWin->setLayout(new QVBoxLayout());
	join2GroupWin->layout()->addWidget(Join2GroupBox);

	// Set fixed Dialog size (user cannot resize it)
	join2GroupWin->setWindowFlag(Qt::MSWindowsFixedSizeDialogHint, true);


}

// Update Send Requests Window
void groupManWindow::groupManWindow::updateSendRequestsWindow()
{
	// Clear all Label Buttons of groups
	for (labelButton*& groupLbl : reqToGroupLabel)
	{
		// Clear all labelButtons for each user
		delete groupLbl;
		groupLbl = nullptr;
	}
	reqToGroupLabel.clear();

	// For each active group, a labelButton is created and added to the layout
	for (Group* group : groups)
	{
		if (group->status == DB::Groups::status_active) // just adding the active groups
		{
			reqToGroupLabel.push_back(new labelButton(group->name));
			vLayReqToGroups->addWidget(reqToGroupLabel.back());
		}
	}

	// Checking if there is any existing active group
	if (reqToGroupLabel.size() > 0)
	{
		join2GroupText->hide();
	}
	else
	{
		join2GroupText->show();
	}

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
void groupManWindow::createReceivedInvitationsWindow()
{
	receivedInvitationWin = new QDialog();
	receivedInvitationWin->setWindowTitle("Group invitations");
	receivedInvitationWin->setWindowIcon(QIcon(icons::groupPrefIcon));

	QGroupBox* recievedInvBox = new QGroupBox("Received Invitations");

	// Adding the info text for this window
	receivedInvText->setText("There are no received group\ninvitations for the moment.");
	receivedInvText->setAlignment(Qt::AlignCenter);
	vLayGroups->addWidget(receivedInvText);

	// Update the Received invitations window
	updateReceivedInvitationsWindow();

	recievedInvBox->setLayout(vLayGroups);
	receivedInvitationWin->setLayout(new QVBoxLayout());
	receivedInvitationWin->layout()->addWidget(recievedInvBox);

	// Set fixed Dialog size (user cannot resize it)
	receivedInvitationWin->setWindowFlag(Qt::MSWindowsFixedSizeDialogHint, true);

}

// Update the received invitations window
void groupManWindow::updateReceivedInvitationsWindow()
{
	// Clear all Member Layout 
	for (QHBoxLayout*& mHLay : groupInvLay)
	{
		deleteLayout(mHLay);
		mHLay = nullptr;
	}
	groupInvLay.clear();

	// A struct of Button and User is needed to send this user pointer to functions when button is clicked
	struct invAcceptDeclGroup {
		iconButton* acceptButt{ nullptr };
		iconButton* declineButt{ nullptr };
		QHBoxLayout* invLay{ nullptr };
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
			invAcceptDeclGroup.push_back({ new iconButton(QIcon(icons::accept), 15), new iconButton(QIcon(icons::decline), 15), groupInvLay.back(), group });

			// Setting Label, Icon, and layout regarding the corresponding 'user' to the layout for the requests section:
			groupInvLay.back()->addWidget(invAcceptDeclGroup.back().acceptButt);
			groupInvLay.back()->addItem(new QSpacerItem(-10, 10));
			groupInvLay.back()->addWidget(invAcceptDeclGroup.back().declineButt);
			vLayGroups->addLayout(groupInvLay.back());
		}
	}

	// In case there are groups that sent an invitation to the currentUser
	if (groupInvLay.size() > 0)
	{
		receivedInvText->hide();
		if (currentUser->getGroupID().toInt() == DB::NO_GROUP)
		{
			emit raisedNews(); // This signal is sent to update the Group Preferences Icon in the Main Window as showing there are invitations/requests to check.
		}
	}
	else
	{
		receivedInvText->show();
	}

	receivedInvButt->setText(receivedInvButt->text().left(receivedInvButt->text().indexOf('(')) + "(" + QString::number(groupInvLay.size()) + ")");

	// For Accepting/Denying Invitations to join a group, accept/deny buttons are connected to a signal for each existing group
	for (struct invAcceptDeclGroup& invitation : invAcceptDeclGroup)
	{
		QObject::connect(invitation.acceptButt, &QPushButton::clicked, [=]() {

			acceptInvitation(invitation.group, invitation.invLay);
			});

		QObject::connect(invitation.declineButt, &QPushButton::clicked, [=]() {

			removeInvitation(currentUser, invitation.group, invitation.invLay, receivedInvitationWin);
			});
	}

}

//Create a Dialog useful to send invitations to users (db is Database object, 'dbOpen' is a flag used to open and check the database)
void groupManWindow::createInvitationWindow()
{

	sendInvitationWin = new QDialog();

	sendInvitationWin->setWindowTitle("Send invitations");
	sendInvitationWin->setWindowIcon(QIcon(icons::groupPrefIcon));

	QGroupBox* sendInvBox = new QGroupBox("Send Invitation");

	// Set the information text for this window
	sendInvToUsrText->setText("There are no existing users\nto send an invitation.");
	vLayUsrs->addWidget(sendInvToUsrText);

	// Update the Invitation Window
	updateInvitationWindow();

	sendInvBox->setLayout(vLayUsrs);
	sendInvitationWin->setLayout(new QVBoxLayout());
	sendInvitationWin->layout()->addWidget(sendInvBox);

	// Set fixed Dialog size (user cannot resize it)
	sendInvitationWin->setWindowFlag(Qt::MSWindowsFixedSizeDialogHint, true);



}

// Update the Invitation Window
void groupManWindow::updateInvitationWindow()
{

	for (labelButton*& usrLbl : userInvLabel)
	{
		// Clear all labelButtons for each user
		delete usrLbl;
		usrLbl = nullptr;
	}
	userInvLabel.clear();

	// Find the current user Group
	auto group_it{ std::find_if(groups.begin(), groups.end(), [&](Group* groupx) {return groupx->ID == currentUser->getGroupID().toInt(); }) };

	// For each user, a labelButton is created and added to the layout
	for (User* user : users)
	{
		if (user != currentUser)
		{
			userInvLabel.push_back(new labelButton(user->getUserName()));
			vLayUsrs->addWidget(userInvLabel.back());

			// If user was previously invited to this group
			if ((*group_it)->out_requests.contains(user->getUserName()))
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
	}

	// In case there are existing users to show
	if (userInvLabel.size() > 0)
	{
		sendInvToUsrText->hide();
	}
	else
	{
		sendInvToUsrText->show();
	}

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
				auto userInvLayRmv{ userInvLay.back() };

				QObject::connect(removeInvButt, &QPushButton::clicked, [=]() {

					removeInvitation(users[pos], (*(std::find_if(groups.begin(), groups.end(), [=](Group* groupx) {return groupx->ID == currentUser->getGroupID().toInt(); }))), userInvLayRmv, handleInvReqWin);
					});
			}

			});
	}
}

//Create a Dialog useful to remove invitations or accept/reject received joining requests
void groupManWindow::createHandleInvReqWindow()
{
	handleInvReqWin = new QDialog();

	handleInvReqWin->setWindowTitle("Handle Invitations and Requests");
	handleInvReqWin->setWindowIcon(QIcon(icons::groupPrefIcon));

	// Declaring a Box for Invitations and a Box for Requests
	QGroupBox* sentInvBox = new QGroupBox("Sent Invitations");
	QGroupBox* receivedReqBox = new QGroupBox("Received Requests");

	// Set information texts regarding the part of Invitations and Requests
	handleSentInvText->setText("There are no\ninvitations to show.");
	handleSentInvText->setAlignment(Qt::AlignCenter);
	handleReceivedReqText->setText("There are no\nrequests to show.");
	handleReceivedReqText->setAlignment(Qt::AlignCenter);
	vLayInvitations->addWidget(handleSentInvText);
	vLayRequests->addWidget(handleReceivedReqText);

	// Update Handle Invitations/Requests Window
	updateHandleInvReqWindow();

	sentInvBox->setLayout(vLayInvitations);
	vLayInvitations->setAlignment(Qt::AlignTop);
	receivedReqBox->setLayout(vLayRequests);
	vLayRequests->setAlignment(Qt::AlignTop);
	handleInvReqWin->setLayout(new QHBoxLayout());
	handleInvReqWin->layout()->addWidget(sentInvBox);
	handleInvReqWin->layout()->addWidget(receivedReqBox);

	// Set fixed Dialog size (user cannot resize it)
	handleInvReqWin->setWindowFlag(Qt::MSWindowsFixedSizeDialogHint, true);
}

// Update Handle Invitation/Request Window
void groupManWindow::updateHandleInvReqWindow()
{

	// Clear all Invitation Layout 
	for (QHBoxLayout*& mHLay : userInvLay)
	{
		deleteLayout(mHLay);
		mHLay = nullptr;
	}
	userInvLay.clear();

	// Clear all Request Layout 
	for (QHBoxLayout*& mHLay : userReqLay)
	{
		deleteLayout(mHLay);
		mHLay = nullptr;
	}
	userReqLay.clear();

	// A struct of Button and User is needed to send this user pointer to functions when button is clicked
	struct invDeclUsr {
		iconButton* declineButt{ nullptr };
		QHBoxLayout* invLay{ nullptr };
		User* user{ nullptr };
	};

	// A struct of Button and User is needed to send this user pointer to functions when button is clicked
	struct reqAcceptDecllUsr {
		iconButton* acceptButt{ nullptr };
		iconButton* declineButt{ nullptr };
		QHBoxLayout* reqLay{ nullptr };
		User* user{ nullptr };
	};

	std::vector<invDeclUsr> invDeclUsr{};
	std::vector<reqAcceptDecllUsr> reqAcceptDecllUsr{};

	// Declaring labels vector. Each position will store the user name
	std::vector<QLabel*> userInvLabelx{};
	std::vector<QLabel*> userReqLabelx{};

	// Find the current user Group
	auto group_it{ std::find_if(groups.begin(), groups.end(), [&](Group* groupx) {return groupx->ID == currentUser->getGroupID().toInt(); }) };

	// Each invitation or user request for each user is added to the corresponding group (Invitation or Requests)
	for (User* user : users)
	{
		// If user was previously invited to this group
		if ((*group_it)->out_requests.contains(user->getUserName()))
		{
			// Creating the horizontal layout for the corresponding 'user'
			userInvLay.push_back(new QHBoxLayout());

			// Creating the Label for the corresponding 'user'
			userInvLabelx.push_back(new QLabel(user->getUserName()));

			// Creating the 'Decline' icon for the corresponding 'user' and Creating a relationship between the specific button and its user
			invDeclUsr.push_back({ new iconButton(QIcon(icons::decline), 15), userInvLay.back(), user });

			// Setting Label, Icon, and layout regarding the corresponding 'user' to the layout for the invitations section:
			userInvLay.back()->addWidget(userInvLabelx.back());
			userInvLay.back()->addWidget(invDeclUsr.back().declineButt);
			vLayInvitations->addLayout(userInvLay.back());
		}

		// If user sent a request to this group
		if ((*group_it)->in_requests.contains(user->getUserName()))
		{
			// Creating the horizontal layout for the corresponding 'user'
			userReqLay.push_back(new QHBoxLayout());

			// Creating the Label for the corresponding 'user'
			userReqLabelx.push_back(new QLabel(user->getUserName()));

			// Creating the 'Decline' icon for the corresponding 'user' and Creating a relationship between the specific buttons and its user
			reqAcceptDecllUsr.push_back({ new iconButton(QIcon(icons::accept), 15), new iconButton(QIcon(icons::decline), 15), userReqLay.back(), user });

			// Setting Label, Icon, and layout regarding the corresponding 'user' to the layout for the requests section:
			userReqLay.back()->addWidget(userReqLabelx.back());
			userReqLay.back()->addWidget(reqAcceptDecllUsr.back().acceptButt);
			userReqLay.back()->addItem(new QSpacerItem(-10, 10));
			userReqLay.back()->addWidget(reqAcceptDecllUsr.back().declineButt);
			vLayRequests->addLayout(userReqLay.back());
		}
	}

	// Check if there is any user invitation
	if (userInvLay.size() > 0)
	{
		handleSentInvText->hide();
	}
	else
	{
		handleSentInvText->show();
	}

	// Check if there is any user request
	if (userReqLay.size() > 0)
	{
		handleReceivedReqText->hide();
		emit raisedNews(); // Emiting this signal to change the Group Preferences Icon in the Main Window
	}
	else
	{
		handleReceivedReqText->show();
	}

	// Setting Button Text for the Handle Invitations/Requests button according the number of input user requests 
	QString numberInvReq{ "(" + QString::number((*group_it)->out_requests.size()) + "/" + QString::number((*group_it)->in_requests.size()) + ")" };
	requestsButt->setText(requestsButt->text().left(requestsButt->text().indexOf('(')) + numberInvReq);

	// For Removing Invitations sent to users, every Decline button is connected to a signal for each existing user
	for (struct invDeclUsr& invitation : invDeclUsr)
	{
		QObject::connect(invitation.declineButt, &QPushButton::clicked, [=]() {
			// Remove Invitation sent to the user for the current user group
			auto group_it{ std::find_if(groups.begin(), groups.end(), [=](Group* groupx) {return groupx->ID == currentUser->getGroupID().toInt(); }) };
			removeInvitation(invitation.user, (*group_it), invitation.invLay, handleInvReqWin);
			});
	}

	// For Accepting or Removing user requests to join the currentUser group
	for (struct reqAcceptDecllUsr& request : reqAcceptDecllUsr)
	{
		QObject::connect(request.acceptButt, &QPushButton::clicked, [=]() {
			acceptRequest(request.user, request.reqLay);
			});

		QObject::connect(request.declineButt, &QPushButton::clicked, [=]() {
			removeRequest(request.user, request.reqLay);
			});
	}

}

// Create Showing Members Window
void groupManWindow::createMembersWindow()
{
	membersWin = new QDialog();

	membersWin->setWindowTitle("Group Members");
	membersWin->setWindowIcon(QIcon(icons::groupPrefIcon));

	// Box which contains a VLayout with all Group Members
	QGroupBox* membersBox = new QGroupBox("Group members");

	// Adding the information text to the main layout
	membersText->setText("There are no other group\nmembers apart from you.");
	membersText->setAlignment(Qt::AlignCenter);
	vLayMembers->addWidget(membersText);

	updateMembersWindow();

	membersBox->setLayout(vLayMembers);
	membersWin->setLayout(new QVBoxLayout());
	membersWin->layout()->addWidget(membersBox);

	// Set fixed Dialog size (user cannot resize it)
	membersBox->setWindowFlag(Qt::MSWindowsFixedSizeDialogHint, true);
}

// Update Members Win with current Members
void groupManWindow::updateMembersWindow()
{
	// Clear all Member Layout 
	for (QHBoxLayout*& mHLay : memberLay)
	{
		deleteLayout(mHLay);
		mHLay = nullptr;
	}
	memberLay.clear();

	// Find the current user Group
	auto group_it{ std::find_if(groups.begin(), groups.end(), [&](Group* groupx) {return groupx->ID == currentUser->getGroupID().toInt(); }) };

	// For each user, a QLabel is created
	for (User* user : users)
	{
		if ((*group_it)->ID == user->getGroupID().toInt()) // just adding the group member
		{
			memberLay.push_back(new QHBoxLayout());
			memberLay.back()->addWidget(new QLabel(user->getUserName()));
			vLayMembers->addLayout(memberLay.back());
		}
	}

	// In case there are other group members
	if (memberLay.size() > 0)
	{
		membersText->hide();
	}
	else
	{
		membersText->show();
	}
}

// Clears all content from a specific layout
template <typename LayoutType>
void groupManWindow::deleteLayout(LayoutType*& layout)
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

// Load the section for creating a group
void groupManWindow::loadCreateGroupSection()
{
	// Show and Hide the widgets regarding this view
	newNameLabel->show();
	newNameLineEdit->show();
	createGroupButt->setVisible(false);
	notificationInvImg->setVisible(false);
	notificationReqImg->setVisible(false);
	joinGroupButt->setVisible(false);
	removeReqButt->setVisible(false);
	inviteUserButt->setVisible(false);
	requestsButt->setVisible(false);
	leaveGroupButt->setVisible(false);
	renameGroupButt->setVisible(false);
	membersButt->setVisible(false);
	receivedInvButt->setVisible(false);
	saveButt->setVisible(true);
	cancelButt->setVisible(true);
}

// Loas Rename Group View
void groupManWindow::loadRenameGroup()
{
	// When Renaming a Group, same fields are active as Create Group Section
	loadCreateGroupSection();

	// Update the Dialog size according the current content
	QTimer::singleShot(20, [=]() {
		setFixedSize(sizeHint());
		});
}

// Load the view when a user has no group and nor request/invitation
void groupManWindow::loadNoGroupView()
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
	notificationInvImg->setVisible(getNewsInvStatus());
	notificationReqImg->setVisible(false);
	createGroupButt->setVisible(true);
	joinGroupButt->setVisible(true);
	removeReqButt->setVisible(false);
	inviteUserButt->setVisible(false);
	requestsButt->setVisible(false);
	leaveGroupButt->setVisible(false);
	renameGroupButt->setVisible(false);
	membersButt->setVisible(false);
	receivedInvButt->setVisible(true);
	saveButt->setVisible(false);
	cancelButt->setVisible(false);

}

// Load the view when user is a member of a group
void groupManWindow::loadGroupView()
{
	QString groupName{ "unknown" };

	// Setting the name group of the current logged user
	auto groupIt{ std::find_if(groups.begin(), groups.end(), [=](Group* group) { return group->ID == currentUser->getGroupID().toInt(); }) };

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
	notificationInvImg->setVisible(false);
	notificationReqImg->setVisible(getNewsReqStatus());
	createGroupButt->setVisible(false);
	joinGroupButt->setVisible(false);
	removeReqButt->setVisible(false);
	inviteUserButt->setVisible(true);
	requestsButt->setVisible(true);
	leaveGroupButt->setVisible(true);
	renameGroupButt->setVisible(true);
	membersButt->setVisible(true);
	receivedInvButt->setVisible(false);
	saveButt->setVisible(false);
	cancelButt->setVisible(false);

}

// Load the view when user sent a request to join a group
void groupManWindow::loadRequestSentView()
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
	notificationInvImg->setVisible(getNewsInvStatus());
	notificationReqImg->setVisible(false);
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
void groupManWindow::createGroup()
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
		int groupNum{ static_cast<int>(groups.size()) + 1 };

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

		// Tries to insert the new group to DB
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
				groups.push_back(new Group{ groupNum, newGroupName, {currentUser->getUserName()}, {}, {} });

				// Sets the new group ID to the current user
				currentUser->setGroupID(QString::number(groupNum));

				// In case windos for Invite users or handle Invitations/Requests is not created yet, both dialogs will be created
				if (!createdWindowsFlag)
				{
					// Generate the invitation window from a Database reading
					createInvitationWindow();

					// Generate the handle invitation and received requests window from a Database reading
					createHandleInvReqWindow();

					// Generate the Members Window to see all group members in the group
					createMembersWindow();

					// Set flag to true
					createdWindowsFlag = true;
				}

				// Setting Button Text for the Handle Invitations/Requests button according the number of input user requests 
				QString numberInvReq{ "(" + QString::number(groups.back()->out_requests.size()) + "/" + QString::number(groups.back()->in_requests.size()) + ")" };
				requestsButt->setText(requestsButt->text().left(requestsButt->text().indexOf('(')) + numberInvReq);

				// Setting which is the group that user requested to join
				auto groupRequested{ std::find_if(groups.begin(), groups.end(), [=](Group* groupx) {return groupx->in_requests.contains(currentUser->getUserName()); }) };

				// Removing the In Request from the user to another group
				if (groupRequested != groups.end())
				{
					updateRequestCol(db, currentUser, DB::Groups::col_inrequests, (*groupRequested), REMOVE_REQUEST);
				}

				// Remove each Invitation sent to this user
				for (Group* groupx : groups)
				{
					if (groupx->out_requests.contains(currentUser->getUserName()))
					{
						updateRequestCol(db, currentUser, DB::Groups::col_outrequests, groupx, REMOVE_REQUEST);
					}
				}

				// Clear all Invitation layouts from 'Received Layout Window'
				for (QHBoxLayout* hLay : groupInvLay)
				{
					deleteLayout(hLay);
				}
				groupInvLay.clear();

				// Since creating a new group deletes all invitations, Group Preferences Icon should notice
				emit clearedNews();

				// Clear all Member Layout 
				for (QHBoxLayout* mHLay : memberLay)
				{
					deleteLayout(mHLay);
				}
				memberLay.clear();

				// In Members Window, there is a message there are no other group members (after a group creation)
				membersText->show();

				// Update the Dialog size according the current content
				QTimer::singleShot(50, [=]() {
					membersWin->resize(membersWin->sizeHint());
					});

				// Set the 'no invitations' text in the "Received Invitations" Window
				receivedInvText->show();

				// Set the button text of Received Invitation Window showing there are no invitations
				receivedInvButt->setText(receivedInvButt->text().left(receivedInvButt->text().indexOf('(')) + "(0)");

				// Update the Dialog size according the current content
				QTimer::singleShot(50, [=]() {
					receivedInvitationWin->setFixedSize(receivedInvitationWin->sizeHint());
					});

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
bool groupManWindow::joinRequest(const QString& nameGroup)
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
				std::string conditionUsers{ DB::Users::col_username + " = '" + currentUser->getUserName().toStdString() + "'" };

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
void groupManWindow::acceptRequest(User* user, QHBoxLayout* reqLay)
{
	QMessageBox msgBox{};
	msgBox.setWindowTitle("Accept request");
	msgBox.setWindowIcon(QIcon(icons::groupPrefIcon));

	msgBox.setText("Are you sure you want to accept the request?");
	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	msgBox.setIcon(QMessageBox::Question);
	int answer = msgBox.exec();

	// Trying to search which is the group of 'groups' vector that user sent a request
	auto group_it{ std::find_if(groups.begin(), groups.end(), [=](Group* group) {return group->in_requests.contains(user->getUserName()); }) };

	bool acceptFlag{ false };

	// When Pressing "Yes" button 
	if (answer == QMessageBox::Yes)
	{
		sqlite3* db{};

		// Check te availability and opense the Database
		if (checkAndOpenSQLiteDB(db, userInfoBox, { DB::tableGroups, DB::tableUsers }) == DB::OPEN_SUCCESS)
		{

			// Enrolling the user to the group (set the goup_id for the logged user according the new created group into Database)
			std::string condition{ DB::Users::col_username + " = '" + user->getUserName().toStdString() + "'" };

			// Updating in the database (Table Users) to set the user group to the one he/she requested
			if (updateRecords(db, DB::tableUsers, DB::Users::col_groupID, std::to_string((*group_it)->ID), condition))
			{

				// Updating in the database (Table Groups): adding the user to the group and removing him from "In Requests" (col_inrequests)
				if (updateRequestCol(db, user, DB::Groups::col_inrequests, (*group_it), REMOVE_REQUEST)
					&& updateRequestCol(db, user, DB::Groups::col_users, (*group_it), ADD_REQUEST))
				{
					userInfoBox->setIcon(QMessageBox::Information);
					userInfoBox->setText("User " + user->getUserRName() + " is now a member of your group.");

					acceptFlag = true;

					// Adding the new user to the 'groups' vector
					(*group_it)->users.append(user->getUserName());

					// Sets the new group ID to the current user
					currentUser->setGroupID(QString::number((*group_it)->ID));

					// Adding a Member to Members Window
					memberLay.push_back(new QHBoxLayout());
					memberLay.back()->addWidget(new QLabel(user->getUserName()));
					vLayMembers->addLayout(memberLay.back());

					// Hidint the text that says no other group members exist in the group
					membersText->hide();
				}
				else
				{
					userInfoBox->setIcon(QMessageBox::Warning);
					userInfoBox->setText("User request could not be approved propperly from database due to unknown issues!");

					acceptFlag = false;
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
		acceptFlag = false;
	}

	if (acceptFlag)
	{
		// Remove Layout from its vector
		userReqLay.erase(std::find(userReqLay.begin(), userReqLay.end(), reqLay));

		// Delete the layout content
		deleteLayout(reqLay);

		// Disabling sending invitations to a the new member of your group
		auto userInvLbl{ std::find_if(userInvLabel.begin(), userInvLabel.end(), [=](labelButton* userButton) { return userButton->text().contains(user->getUserName()); }) };
		(*userInvLbl)->setText(user->getUserName() + " (Member of your group)");
		(*userInvLbl)->setDisabled(true);

		// Check if there are no more user requests
		if ((*group_it)->in_requests.isEmpty())
		{
			handleReceivedReqText->show();

			// Since creating a new group deletes all invitations, Group Preferences Icon should notice
			emit clearedNews();
		}

		// Setting Button Text for the Handle Invitations/Requests button according the number of input user requests 
		QString numberInvReq{ "(" + QString::number(userInvLay.size()) + "/" + QString::number((*group_it)->in_requests.size()) + ")" };
		requestsButt->setText(requestsButt->text().left(requestsButt->text().indexOf('(')) + numberInvReq);

		// Update the Dialog size according the current content
		QTimer::singleShot(50, [=]() {
			handleInvReqWin->resize(handleInvReqWin->sizeHint());
			});
	}

	selectView();
}

// Removes a request to join a group
void groupManWindow::removeRequest(User* user, QHBoxLayout* reqLay)
{
	QMessageBox msgBox{};
	msgBox.setWindowTitle("Remove request");
	msgBox.setWindowIcon(QIcon(icons::groupPrefIcon));

	msgBox.setText("Are you sure you want to remove the request?");
	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	msgBox.setIcon(QMessageBox::Question);
	int answer = msgBox.exec();

	// In case is the current user who wants to remove his request from main menu (not from Handle Invitations/Requests window)
	// Note: When no 'user' is sent as input parameter is the case when the current user wants to remove his own joining request
	if (!user)
	{
		user = currentUser;
	}

	// Trying to search which is the group of 'groups' vector that user sent a request
	auto group_it{ std::find_if(groups.begin(), groups.end(), [=](Group* group) {return group->in_requests.contains(user->getUserName()); }) };

	// Flag useful to know if the database update was done or not
	bool removeFlag{ false };

	// When Pressing "Yes" button 
	if (answer == QMessageBox::Yes)
	{
		sqlite3* db{};

		// Check te availability and opense the Database
		if (checkAndOpenSQLiteDB(db, userInfoBox, { DB::tableGroups, DB::tableUsers }) == DB::OPEN_SUCCESS)
		{

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

	// Updating Handle Invitations/Request Window (it is not the current user who wanted to remove a request coming from himself)
	if (removeFlag && user != currentUser)
	{
		// Removing layout from Handle Inv/Req window when is not the current user who wnated to remove a request coming from himself
		userReqLay.erase(std::find(userReqLay.begin(), userReqLay.end(), reqLay));
		deleteLayout(reqLay);

		// Setting Button Text for the Handle Invitations/Requests button according the number of input user requests 
		QString numberInvReq{ "(" + QString::number((*group_it)->out_requests.size()) + "/" + QString::number((*group_it)->in_requests.size()) + ")" };
		requestsButt->setText(requestsButt->text().left(requestsButt->text().indexOf('(')) + numberInvReq);

		// In case there are no more requests to show
		if ((*group_it)->in_requests.size() == 0)
		{
			handleReceivedReqText->show();

			// Since creating a new group deletes all invitations, Group Preferences Icon should notice
			emit clearedNews();
		}

		// Update the Dialog size according the current content
		QTimer::singleShot(50, [=]() {
			handleInvReqWin->resize(handleInvReqWin->sizeHint());
			});

	}

	selectView();
}

// Sends an invitation to one specific User (returns true for a successful invitation request. False otherwise)
bool groupManWindow::sendInvitation(labelButton* userLabel, int pos)
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

				// Setting Button Text for the Handle Invitations/Requests button according the number of input user requests 
				QString numberInvReq{ "(" + QString::number((*group_it)->out_requests.size()) + "/" + QString::number((*group_it)->in_requests.size()) + ")" };
				requestsButt->setText(requestsButt->text().left(requestsButt->text().indexOf('(')) + numberInvReq);

			}
			else
			{
				invitationFlag = false;
			}

			closeSQLiteDB(db);

			// Hide the info text that tells there are no invitations to users
			handleSentInvText->hide();

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
void groupManWindow::acceptInvitation(Group* group, QHBoxLayout* groupLay)
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
		if (checkAndOpenSQLiteDB(db, userInfoBox, { DB::tableGroups, DB::tableUsers }) == DB::OPEN_SUCCESS) {

			// Enrolling the user to the group (set the goup_id for the logged user according the new created group into Database)
			std::string condition{ DB::Users::col_username + " = '" + currentUser->getUserName().toStdString() + "'" };

			// Updating in the database (Table Users) to set the user group to the one he/she requested
			if (updateRecords(db, DB::tableUsers, DB::Users::col_groupID, std::to_string(group->ID), condition))
			{
				// Setting which is the group that user requested to join
				auto groupRequested{ std::find_if(groups.begin(), groups.end(), [=](Group* groupx) {return groupx->in_requests.contains(currentUser->getUserName()); }) };

				// Updating in the database (Table Groups): adding the user to the group and removing him from "In Requests" (col_inrequests)
				if (updateRequestCol(db, currentUser, DB::Groups::col_users, group, ADD_REQUEST))
				{
					// Removing the In Request from the user to another group
					if (groupRequested != groups.end())
					{
						updateRequestCol(db, currentUser, DB::Groups::col_inrequests, (*groupRequested), REMOVE_REQUEST);
					}

					// Remove each Invitation sent to this user
					for (Group* groupx : groups)
					{
						if (groupx->out_requests.contains(currentUser->getUserName()))
						{
							updateRequestCol(db, currentUser, DB::Groups::col_outrequests, groupx, REMOVE_REQUEST);
						}
					}

					// After removing the invitations to the user, if the new group has no In_Requests, Group Preferences Icon is updated
					if (group->in_requests.isEmpty())
					{
						emit clearedNews();
					}

					// Sets the new group ID to the current user
					currentUser->setGroupID(QString::number(group->ID));

					// Adding the new user to the 'groups' vector
					group->users.append(currentUser->getUserName());

					// In case windos for Invite users or handle Invitations/Requests is not created yet, both dialogs will be created
					if (!createdWindowsFlag)
					{
						// Generate the invitation window from a Database reading
						createInvitationWindow();

						// Generate the handle invitation and received requests window from a Database reading
						createHandleInvReqWindow();

						// Generate the Members Window to see all group members in the group
						createMembersWindow();

						// Set flag to true
						createdWindowsFlag = true;
					}

					userInfoBox->setIcon(QMessageBox::Information);
					userInfoBox->setText("Now you are member of group: " + group->name);

					acceptFlag = true;
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
		// Clear all Invitation layouts from 'Received Layout Window'
		for (QHBoxLayout* hLay : groupInvLay)
		{
			deleteLayout(hLay);
		}
		groupInvLay.clear();

		// Set the 'no invitations' text in the "Received Invitations" Window
		receivedInvText->show();

		// Set the button text of Received Invitation Window showing there are no invitations
		receivedInvButt->setText(receivedInvButt->text().left(receivedInvButt->text().indexOf('(')) + "(0)");

		// Close Received Invitation Window
		receivedInvitationWin->close();

		// Update the Dialog size according the current content
		QTimer::singleShot(50, [=]() {
			receivedInvitationWin->setFixedSize(receivedInvitationWin->sizeHint());
			});

	}

	selectView();
}

// Removes a Invitation that user received coming from a specific group
void groupManWindow::removeInvitation(User* user, Group* group, QHBoxLayout* invLay, QDialog* inWindow)
{
	QMessageBox msgBox{};
	msgBox.setWindowTitle("Decline Invitation");
	msgBox.setWindowIcon(QIcon(icons::groupPrefIcon));

	msgBox.setText("Are you sure you want to decline this invitation?");
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

		if (inWindow == receivedInvitationWin) // Remove received invitation 
		{
			// Remove Layout from its vector
			groupInvLay.erase(std::find(groupInvLay.begin(), groupInvLay.end(), invLay));

			// Check if there are no more group invitations
			if (groupInvLay.empty())
			{
				receivedInvText->show();
				emit clearedNews(); // In the case all invitations are declined, there no more news, so the Group Preferences Icon is updated accordingly
			}

			receivedInvButt->setText(receivedInvButt->text().left(receivedInvButt->text().indexOf('(')) + "(" + QString::number(groupInvLay.size()) + ")");
		}

		if (inWindow == handleInvReqWin) // Remove sent invitation
		{
			auto userInvLbl{ std::find_if(userInvLabel.begin(), userInvLabel.end(), [=](labelButton* userButton) { return userButton->text().contains(user->getUserName()); }) };
			(*userInvLbl)->setText(user->getUserName());
			(*userInvLbl)->setDisabled(false);

			// Remove Layout from its vector
			userInvLay.erase(std::find(userInvLay.begin(), userInvLay.end(), invLay));

			// Setting Button Text for the Handle Invitations/Requests button according the number of input user requests 
			auto group_it{ std::find_if(groups.begin(), groups.end(),[&](Group* groupx) {return groupx->ID == currentUser->getGroupID().toInt(); }) };

			// Setting Button Text for the Handle Invitations/Requests button according the number of input user requests 
			QString numberInvReq{ "(" + QString::number((*group_it)->out_requests.size()) + "/" + QString::number((*group_it)->in_requests.size()) + ")" };
			requestsButt->setText(requestsButt->text().left(requestsButt->text().indexOf('(')) + numberInvReq);

			// In case there are no more invitations to show
			if ((*group_it)->out_requests.size() == 0)
			{
				handleSentInvText->show();
			}

		}

		// Delete the content of the Layout that contains the request/invitation to be pulled off
		deleteLayout(invLay);

		// Update the Dialog size according the current content
		QTimer::singleShot(50, [=]() {
			inWindow->resize(inWindow->sizeHint());
			});

		selectView();

	}

}

// User leaves the group he/she is belonging
void groupManWindow::leaveGroup()
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
					userInvLay.clear();

					// Removing request layouts from Handle Invitations/Requests window
					for (QHBoxLayout* reqLay : userReqLay)
					{
						deleteLayout(reqLay);
					}
					userReqLay.clear();

					// Setting the text that there are no invitations or requests to show
					handleSentInvText->show();
					handleReceivedReqText->show();

					// Removes the group to the 'groups' vector and disable it in Database in case this group has not other members
					if ((*group_it)->users.isEmpty())
					{

						// Deleting the group from group vector and database
						deleteGroup(db, (*group_it), userInfoBox, false);

						// Removing the group as requestable to be joined
						auto reqLabel_it{ std::find_if(reqToGroupLabel.begin(), reqToGroupLabel.end(), [&](labelButton* reqLabel) {
							return reqLabel && reqLabel->text() == (*group_it)->name;
							}) };

						if (reqLabel_it != reqToGroupLabel.end())
						{
							delete (*reqLabel_it);
							(*reqLabel_it) = nullptr;
						}
					}

					// When a User leaves the group, he does not have neither invitations nor user requests
					emit clearedNews();

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
			if (user != currentUser)
			{
				auto userInvLbl{ std::find_if(userInvLabel.begin(), userInvLabel.end(), [=](labelButton* userButton) { return userButton->text().contains(user->getUserName()); }) };
				(*userInvLbl)->setText(user->getUserName());
				(*userInvLbl)->setDisabled(false);
			}
		}
	}

	selectView();
}

// Renaming the group
void groupManWindow::renameGroup()
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

	// Trying to search which is the group of 'groups' vector that user sent a request
	auto group_it{ std::find_if(groups.begin(), groups.end(), [=](Group* group) {return group->ID == currentUser->getGroupID().toInt(); }) };

	// Checks if the userName already exists from all other users
	for (Group* group : groups)
	{
		// If the new group name already exists for an active group, a warning pops up
		if ((newGroupName == group->name) && (group->status == DB::Groups::status_active) && ((*group_it)->ID != group->ID))
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

		// Setting the condition to rename the group name in database
		std::string condition{ DB::Groups::col_ID + " = '" + currentUser->getGroupID().toStdString() + "'" };

		if (updateRecords(db, DB::tableGroups, DB::Groups::col_group_name, newGroupName.toStdString(), condition))
		{
			userInfoBox->setIcon(QMessageBox::Information);
			userInfoBox->setText("Group Name has been successfuly updated");

			// Updating the name group in the group vector
			(*group_it)->name = newGroupName;

			infoText->setText("You are a member of the group\n'" + newGroupName + "'");

			// Emiting a signal in order to warn the outside routine the current user group name has been changed
			emit renamedGroup();
		}
		else
		{
			// When user could not be added to the group in Database
			userInfoBox->setIcon(QMessageBox::Warning);
			userInfoBox->setText("Group Name could not be rewritten in database due to unknown issues!");
		}

		//Show infoBox
		userInfoBox->exec();

		closeSQLiteDB(db);
	}

	selectView();
}




//  Adds or removes (MODE) a user to Groups Table of the database column (col_update) according the group. Returns TRUE (successful Database update) or False otherwise
bool updateRequestCol(sqlite3* db, User* user, const std::string& col_update, Group* group, const int MODE)
{

	// Selecting the cell from a Table regarding the 'col_update' for the selected "group ID"
	QString requests{ getRecords(db, DB::tableGroups, col_update, "WHERE " + DB::Groups::col_ID + " = '" + std::to_string(group->ID) + "'")[0][0] };


	if (MODE == ADD_REQUEST) // Add a user to the 'col_update'
	{

		// Adding the user to the QString 'requests' 
		if (requests.isEmpty()) // When 'col_update' column has not any other user
		{
			requests = user->getUserName();
		}
		else //When one or more users want were previously added to the 'col_update' column
		{
			requests += ", " + user->getUserName();
		}

	}
	else if (MODE == REMOVE_REQUEST) // Remove a user from 'col_update' column
	{

		if (requests.contains(user->getUserName())) // Checks whether the user exists or not to this 'col_update' column
		{
			if (requests.startsWith(user->getUserName() + ", ")) // When user is the first who appears in the 'col_update' column
			{
				requests.remove(user->getUserName() + ", ");
			}
			else if (requests.contains(", " + user->getUserName())) // When user is not the first appearing in 'col_update' column
			{
				requests.remove(", " + user->getUserName());
			}
			else // This scenario only is possible when the current user is the only appearing in 'col_update' column
			{
				requests.clear();
			}
		}

	}
	else
	{
		// When no Mode (Adding/Removing) was selected
		return false;
	}

	// Setting the condition for updating 'col_update' column
	std::string conditionGroups{ DB::Groups::col_ID + " = '" + std::to_string(group->ID) + "'" };

	// Updating the 'col_update' column in database and returning true or false according if the Update was successful or not
	bool updateFlag { updateRecords(db, DB::tableGroups, col_update, requests.toStdString(), conditionGroups) };

	// In case the updating records in database was successful, group from 'groups' vector is updated as well
	if (updateFlag)
	{
		if (col_update == DB::Groups::col_inrequests)
		{
			group->in_requests = requests.split(", ");

			// In case there only value in the column is an empty string (""), group->col_update is set to 0
			if (group->in_requests[0] == "") group->in_requests.clear();

		}
		else if (col_update == DB::Groups::col_outrequests)
		{
			group->out_requests = requests.split(", ");

			// In case there only value in the column is an empty string (""), group->col_update is set to 0
			if (group->out_requests[0] == "") group->out_requests.clear();

		}
		else if (col_update == DB::Groups::col_users)
		{
			group->users = requests.split(", ");

			// In case there only value in the column is an empty string (""), group->col_update is set to 0
			if (group->users[0] == "") group->users.clear();
		}
	}

	return updateFlag;



}

// Deletes a group from Database and from 'groups' vector. Returns true for a successful removing, false otherwise
bool deleteGroup(sqlite3* db, Group* group, QMessageBox* userInfoBox, bool dbOpen)
{

	bool deleteFlag{ true };

	if (dbOpen) // In case Database should be opened and checked
	{
		deleteFlag = checkAndOpenSQLiteDB(db, userInfoBox, { DB::tableGroups, DB::tableUsers }) == DB::OPEN_SUCCESS;
	}

	// Setting the group as inactive in Database and removing all In Requests and Out Requests
	std::string condition = DB::Groups::col_ID + " = '" + std::to_string(group->ID) + "'";

	if (deleteFlag &= updateRecords(db, DB::tableGroups, DB::Groups::col_status, DB::Groups::status_inactive.toStdString(), condition)
		&& updateRecords(db, DB::tableGroups, DB::Groups::col_inrequests, "", condition)
		&& updateRecords(db, DB::tableGroups, DB::Groups::col_outrequests, "", condition))
	{

		// Setting the group as inactive in 'groups' vector
		group->status = DB::Groups::status_inactive;

		// Clearing the Users of the group
		group->users.clear();

		// Clearing the In Requests and Out Requests of the group
		group->in_requests.clear();
		group->out_requests.clear();

	}

	if (dbOpen) // In case Database should were opened and check within this function
	{
		closeSQLiteDB(db);
	}

	return deleteFlag;


}