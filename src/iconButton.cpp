#include "iconButton.h"

iconButton::iconButton(const QIcon& icon) : QPushButton()
{
    // Set Icon and Icon Size
    setIcon(icon);
    setIconSize(22);

    // Disabling borders and background color of the button when it is pressed. Setting a color when PushButton is in 'hover'
    setStyleSheet("QPushButton:pressed { background-color: none; border: none; } QPushButton:hover { color: green; }");

    // Changing the cursor style to clicking-style when the cursor is on the labelButton
    setCursor(Qt::PointingHandCursor);

    // Disabling borders and background color of the button when it is not pressed.
    setFlat(true);

    // Adapting the width button
    setFixedWidth(sizeHint().width());
}

// Setting Icon Size in a faster way than the usual one (QPushButton::setIconSize)
void iconButton::setIconSize(int size)
{
    // Set Icon Size as 'size' pixels width and 'size' pixels height
    QPushButton::setIconSize(QSize(size, size));
}
