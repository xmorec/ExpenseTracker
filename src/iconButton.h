#ifndef ICONBUTTON_H
#define ICONBUTTON_H

#include <QPushButton>
#include <QIcon>

//////////////////////////////////////////////////////////////////////////////////////////
// Description:
// It creates a QIcon that acts as QPushButton
//////////////////////////////////////////////////////////////////////////////////////////

class iconButton : public QPushButton
{

	Q_OBJECT

private:

public:
	iconButton(const QIcon& icon);

	// Setting Icon Size in a faster way than the usual one (QPushButton::setIconSize)
	void setIconSize(int size);

};

#endif