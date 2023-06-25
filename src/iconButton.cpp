#include "iconButton.h"

iconButton::iconButton(const QIcon& icon) : QPushButton()
{
    setIcon(icon);
    setIconSize(QSize(22, 22));  // Establece el tamaño del icono

    // Disabling borders and background color of the button when it is pressed. Setting a color when PushButton is in 'hover'
    setStyleSheet("QPushButton:pressed { background-color: none; border: none; } QPushButton:hover { color: green; }");

    // Changing the cursor style to clicking-style when the cursor is on the labelButton
    setCursor(Qt::PointingHandCursor);

    // Disabling borders and background color of the button when it is not pressed.
    setFlat(true);

    // Adapting the width button
    setFixedWidth(sizeHint().width());
}