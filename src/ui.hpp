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
		void update_files_to_display();

	private:
		void start_loading_next_image();

		QPushButton* start_stop_button;
		core::MirrorModes current_mirror_mode;
		QTimer* change_image_timer;
		QPixmap current_image;
		QLabel* image_display;
		QLabel* folder_label;
		std::future<QImage> next_image;
		std::filesystem::path selected_directory;
		std::vector<std::filesystem::path> files_to_display;
		std::vector<std::filesystem::path>::iterator next_file_to_display;
	};
}