#include "ui.hpp"

#include <QLayout>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QComboBox>

#include <iostream>

ui::MainPanel::MainPanel() 
	: current_mirror_mode(core::MirrorModes::None)
	, change_image_timer(new QTimer(this))
	, current_image(QSize(640,480))
	, image_display(new QLabel{ nullptr})
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

	connect(start_stop_button, SIGNAL(clicked()), this, SLOT(start_stop_slideshow()));
	connect(file_selection, SIGNAL(clicked()), this, SLOT(change_folder()));
	connect(change_image_timer, SIGNAL(timeout()), this, SLOT(update_image()));
	connect(mirror_mode, SIGNAL(currentIndexChanged(int)), this, SLOT(mirror_mode_changed(int)));

	const int interval_between_image_changes_in_ms = 2000;
	change_image_timer->start(interval_between_image_changes_in_ms);
}

void ui::MainPanel::start_stop_slideshow()
{
	if (change_image_timer->isActive())
	{
		change_image_timer->stop();
	}
	else
	{
		change_image_timer->start();
	}
}

void ui::MainPanel::mirror_mode_changed(int new_mode)
{
	current_mirror_mode = static_cast<core::MirrorModes>(new_mode); // important to keep the enum indices in sync with the combobox
}

void ui::MainPanel::change_folder()
{
	auto new_directory = QFileDialog::getExistingDirectory(this, tr("Open Directory"), QDir::currentPath(), QFileDialog::ShowDirsOnly );
}

void ui::MainPanel::update_image()
{
	QImage new_current_image = next_image.get();
	if (!new_current_image.isNull())
	{
		current_image.convertFromImage(std::move(new_current_image));
	} 
	else
	{
		// if for some reason the image could not be loaded we go for the next one and just write a log.
		std::cerr << "Unable to load image ..." << std::endl;
	}
}
