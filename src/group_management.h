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

	// Informing QMessageBox
	QMessageBox* userInfoBox = new QMessageBox();

	// Information text in Management User section
	QLabel* infoText{ new QLabel() };

	// Buttons
	labelButton* createGroupButt{ new labelButton("Create Group")};
	labelButton* joinGroupButt{ new labelButton("Join to a Group") };
	labelButton* removeReqButt{ new labelButton("Remove Request") };
	labelButton* InviteUserButt{ new labelButton("Invite a user to the group") };
	labelButton* RequestsButt{ new labelButton("Requests") };
	labelButton* LeaveGroupButt{ new labelButton("Leave the group") };
	labelButton* RenameGroupButt{ new labelButton("Rename group") };
	labelButton* RemoveGroupButt{ new labelButton("Remove group") };

	// Vertical Layout containing all widgets
	QVBoxLayout* mainVLay{ new QVBoxLayout() };

public:

	groupManWindow()
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
		mainVLay->addWidget(joinGroupButt, 0, Qt::AlignCenter | Qt::AlignTop);
		mainVLay->addWidget(removeReqButt, 0, Qt::AlignCenter | Qt::AlignTop);
		mainVLay->addWidget(InviteUserButt, 0, Qt::AlignCenter | Qt::AlignTop);
		mainVLay->addWidget(RequestsButt, 0, Qt::AlignCenter | Qt::AlignTop);
		mainVLay->addWidget(LeaveGroupButt, 0, Qt::AlignCenter | Qt::AlignTop);
		mainVLay->addWidget(RenameGroupButt, 0, Qt::AlignCenter | Qt::AlignTop);
		mainVLay->addWidget(RemoveGroupButt, 0, Qt::AlignCenter | Qt::AlignTop);

		loadNoGroupView();

		setLayout(mainVLay);
	}

	void loadNoGroupView()
	{
		infoText->setText("You are not a member of a group yet");
		createGroupButt->setVisible(true);
		joinGroupButt->setVisible(true);
		removeReqButt->setVisible(false);
		InviteUserButt->setVisible(false);
		RequestsButt->setVisible(false);
		LeaveGroupButt->setVisible(false);
		RenameGroupButt->setVisible(false);
		RemoveGroupButt->setVisible(false);
	}

};


#endif