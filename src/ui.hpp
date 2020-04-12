#pragma once

#include <QWidget>
#include <QPixmap>

class QLabel;

namespace ui
{
	class MainPanel : public QWidget
	{
		Q_OBJECT

	public:
		MainPanel();

	private:
		QPixmap current_image;
		QLabel* image_display;
	};
}