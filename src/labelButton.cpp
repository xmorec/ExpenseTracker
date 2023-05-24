#include "labelButton.h"

labelButton::labelButton(const QString& textButton) : QPushButton(textButton)
{
	// Disabling borders and background color of the button when it is not pressed.
	setFlat(true);

	// Disabling borders and background color of the button when it is pressed. Setting a color when PushButton is in 'hover'
	setStyleSheet("QPushButton:pressed { background-color: none; border: none; } QPushButton:hover { color: green; }");

	// Changing the cursor style to clicking-style when the cursor is on the labelButton
	setCursor(Qt::PointingHandCursor);

	// Changing the text-font button
	QFont fontButton(this->font().family(), 9.5, QFont::Bold);
	setFont(fontButton);

	// Adapting the width button to the text button
	QRect boundingRect = QFontMetrics(this->font()).boundingRect(this->text());
	int widthF = boundingRect.width() + 10;
	setFixedWidth(widthF);
}