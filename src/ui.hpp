#pragma once

#include "core.hpp"

#include <QWidget>
#include <QPixmap>
#include <QTimer>
#include <filesystem>
#include <vector>

class QLabel;
class QPushButton;

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
		core::ImageProvider images_in_folder;
		std::future<QImage> next_image;
		core::MirrorModes current_mirror_mode;

		QPushButton* start_stop_button;
		QTimer* change_image_timer;
		QPixmap current_image;
		QLabel* image_display;
		QLabel* folder_label;
	};
}