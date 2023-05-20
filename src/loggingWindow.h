#pragma once
#include <QtWidgets>

class loggingWindow : public QDialog
{
private:

public:

	loggingWindow() :QDialog()
	{

		QVBoxLayout* vlay{ new QVBoxLayout() };
		QPushButton* acceptButton{ new QPushButton("Accept") };
		QTextEdit* userField{ new QTextEdit() };
		QTextEdit* passField{ new QTextEdit() };

		vlay->addWidget(userField);
		vlay->addWidget(passField);
		vlay->addWidget(acceptButton);

		setLayout(vlay);

		QObject::connect(acceptButton, &QPushButton::clicked, [=]() {
			this->close();
			});

		exec();
	}

};