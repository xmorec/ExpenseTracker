#pragma once
#include <QtWidgets>

class loggingWindow : public QDialog
{

	Q_OBJECT

private:
	bool loggingStatus{ false };

	QLineEdit* userEdit{ new QLineEdit()};
	QLineEdit* passEdit{ new QLineEdit() };

	static constexpr int lineWidth{ 200 };

public:

	loggingWindow() : QDialog()
	{
		setWindowIcon(QIcon("resources/icons/ExpTrIcon.png"));
		setWindowTitle("Sign In");

		userEdit->setFixedWidth(lineWidth);
		passEdit->setFixedWidth(lineWidth);

		
		QLabel* titlePopup{ new QLabel("Sign In") };
		QFont font(titlePopup->font().family(), 11, QFont::Bold);
		titlePopup->setFont(font);

		QVBoxLayout* vLay{ new QVBoxLayout() };
		vLay->addWidget(titlePopup, 0, Qt::AlignCenter);
		vLay->addWidget(new QLabel("Username"));
		vLay->addWidget(userEdit);
		vLay->addWidget(new QLabel("Password"));
		vLay->addWidget(passEdit);
		QPushButton* signInButton{ new QPushButton("LOGIN") };
		vLay->addItem(new QSpacerItem(0, 10));
		vLay->addWidget(signInButton, 0, Qt::AlignCenter);

		setLayout(vLay);

		QObject::connect(signInButton, &QPushButton::clicked, [=]() {
			loggingStatus = true;
			this->close();
			});

		setWindowFlag(Qt::MSWindowsFixedSizeDialogHint, true);
		
		exec();
	}
	
	
	bool getLoggingStatus()
	{
		return loggingStatus;
	}
	
};