#include "ui.hpp"

#include <QLayout>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QComboBox>

ui::MainPanel::MainPanel() : current_image(QSize(640,480)), image_display(new QLabel{ nullptr})
{
	image_display->setPixmap(current_image);
	auto horizontal_layout = new QHBoxLayout{ this };
	horizontal_layout->addWidget(image_display);

	auto vertical_layout = new QVBoxLayout;
	auto file_selection = new QPushButton{ "Select Folder", this };
	//QFileDialog::getOpenFileName(this, tr("Open Image"), QDir::currentPath(), tr("Image Files (*.jpg, *.tif, *.bmp)"));
	auto start_stop_button = new QPushButton{ "Start Slideshow", this };
	auto mirror_mode = new QComboBox(this);
	mirror_mode->addItem("None");
	mirror_mode->addItem("Horizontal");
	mirror_mode->addItem("Vertical");
	vertical_layout->addWidget(file_selection);
	vertical_layout->addWidget(start_stop_button);
	vertical_layout->addWidget(mirror_mode);
	vertical_layout->setAlignment(Qt::AlignTop);

	horizontal_layout->addLayout(vertical_layout);
}