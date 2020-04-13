#pragma once

#include "image_provider.hpp"

#include <QWidget>
#include <QPixmap>
#include <QTimer>

class QLabel;

namespace ui
{
	class MainPanel : public QWidget
	{
		Q_OBJECT

	public:
		MainPanel();

	private slots:
		void update_image();
		void start_stop_slideshow();
		void mirror_mode_changed(int new_mode);
		void change_folder();

	private:
		core::MirrorModes current_mirror_mode;
		QTimer* change_image_timer;
		QPixmap current_image;
		QLabel* image_display;
		std::future<QImage> next_image;
	};
}