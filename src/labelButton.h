#pragma once
#include <QtWidgets>

class labelButton : public QPushButton
{

	Q_OBJECT

private:
	
public:
	labelButton(const QString& textButton) : QPushButton(textButton)
	{
		setFlat(true);
		setStyleSheet("QPushButton:pressed { background-color: none; border: none; } QPushButton:hover { color: #02730A; }");
		setCursor(Qt::PointingHandCursor);
		QFont fontButton(this->font().family(), 9.5, QFont::Bold);
		setFont(fontButton);
		QRect boundingRect = QFontMetrics(this->font()).boundingRect(this->text());  // Obtiene la caja delimitadora del texto
		int widthF = boundingRect.width() + 10;
		setFixedWidth(widthF);

	}

};