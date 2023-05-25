#ifndef LABELBUTTON_H
#define LABELBUTTON_H

#include <QPushButton>
#include <QString>

//////////////////////////////////////////////////////////////////////////////////////////
// Description:
// It creates a QPushButton that has the aspect of a QLabel. The intention is creating 
// a Clickable QLabel that works as a button.
//////////////////////////////////////////////////////////////////////////////////////////

class labelButton : public QPushButton
{

	Q_OBJECT

private:
	
public:
	labelButton(const QString& textButton);

};

#endif