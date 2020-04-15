#include "ui.hpp"

#include <QLayout>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QComboBox>
#include <QImageReader>
#include <QGroupBox>

namespace fs = std::filesystem;

namespace
{
	QString supported_image_formats()
	{
		auto formats = QImageReader::supportedImageFormats();
		QString result;
		std::for_each(std::begin(formats), std::end(formats), [&result](QByteArray format) {result.append(format + QString(", ")); });
		result.chop(2); // remove last comma and space.
		return result;
	}

	bool has_supported_extension(const fs::path& file)
	{
		auto formats = QImageReader::supportedImageFormats();
		return formats.end() != std::find_if(formats.begin(), formats.end(), [file](QByteArray extension) {return "." + extension.toStdString() == file.extension().string(); });
	}
}

ui::MainPanel::MainPanel()
	: images_in_folder(fs::current_path())
	, next_image(images_in_folder.load_next_image())
	, current_mirror_mode(core::MirrorModes::None)
	, start_stop_button(new QPushButton(tr("Stop Slideshow"), this))
	, change_image_timer(new QTimer(this))
	, image_display(new QLabel{ nullptr })
	, folder_label(new QLabel{ nullptr })
{
	// A lot of boilerplate for the UI ...
	image_display->setPixmap(current_image);
	image_display->setMinimumWidth(640);
	image_display->setMinimumHeight(480);
	image_display->setAlignment(Qt::AlignCenter);
	auto horizontal_layout = new QHBoxLayout{ this };
	horizontal_layout->addWidget(image_display);

	auto vertical_layout = new QVBoxLayout;
	vertical_layout->setAlignment(Qt::AlignTop);

	folder_label->setText(tr("Current Folder: ") + QString::fromStdString(fs::current_path().string()));
	vertical_layout->addWidget(folder_label);
	auto file_selection = new QPushButton{ tr("Select Folder..."), this };

	auto mirror_mode_frame = new QGroupBox(tr("Mirror Image:"));
	auto vbox = new QVBoxLayout;
	auto mirror_mode = new QComboBox;
	vbox->addWidget(mirror_mode);
	mirror_mode_frame->setLayout(vbox);
	mirror_mode->addItem("Not at all");
	mirror_mode->addItem("Horizontally");
	mirror_mode->addItem("Vertically");

	auto supported_formats = new QLabel(tr("Supported Formats: ") + supported_image_formats(), this);

	vertical_layout->addWidget(file_selection);
	vertical_layout->addWidget(start_stop_button);
	vertical_layout->addWidget(mirror_mode_frame);
	vertical_layout->addWidget(supported_formats);

	horizontal_layout->addLayout(vertical_layout);

	connect(start_stop_button, SIGNAL(clicked()), this, SLOT(start_stop_slideshow()));
	connect(file_selection, SIGNAL(clicked()), this, SLOT(change_folder()));
	connect(change_image_timer, SIGNAL(timeout()), this, SLOT(update_image()));
	connect(mirror_mode, SIGNAL(currentIndexChanged(int)), this, SLOT(mirror_mode_changed(int)));

	const int interval_between_image_changes_in_ms = 2000;
	change_image_timer->start(interval_between_image_changes_in_ms);

	update_image();
}

void ui::MainPanel::start_stop_slideshow()
{
	if (change_image_timer->isActive())
	{
		change_image_timer->stop();
		start_stop_button->setText(tr("Start Slideshow"));
	}
	else
	{
		start_stop_button->setText(tr("Stop Slideshow"));
		change_image_timer->start();
	}
}

void ui::MainPanel::mirror_mode_changed(int mode)
{
	// important to keep the enum indices in sync with the combobox, another way would be QENUM or XMacro, for this small application I think this is fine.
	auto new_mode = static_cast<core::MirrorModes>(mode);
	
	if (!current_image.isNull())
	{
		// Want the change to be visible immediately without skipping an image.
		auto mirrored = core::ImageProvider::apply_mirror_mode(new_mode, { current_image.toImage(), current_mirror_mode });
		current_image.convertFromImage(mirrored.get());
		image_display->setPixmap(current_image);
	}
	current_mirror_mode = new_mode;
}

void ui::MainPanel::change_folder()
{
	auto from_file_dialog = QFileDialog::getExistingDirectory(this, tr("Open Directory"), QDir::currentPath(), QFileDialog::DontResolveSymlinks);
	if (from_file_dialog.isNull()) // important, this happens when the user presses abort
	{
		return;
	}
	
	folder_label->setText(tr("Current Folder: ") + from_file_dialog);
	images_in_folder = core::ImageProvider(from_file_dialog.toStdString());
	next_image = images_in_folder.load_next_image(current_mirror_mode);
	update_image();
}

void ui::MainPanel::update_image()
{
	if (!next_image.valid()) // No images in folder...
	{
		current_image = QPixmap();
		image_display->setText(tr("No supported images in folder"));
		return;
	}

	auto loaded_image = next_image.get();
	if (loaded_image.isNull()) // Folder does not contain any supported formats or next image could not be loaded
	{
		current_image = QPixmap();
		image_display->setText(tr("Unable to load image"));
	}
	else
	{
		current_image.convertFromImage(std::move(loaded_image));
		image_display->setPixmap(current_image);
	}
	
	next_image = images_in_folder.load_next_image(current_mirror_mode);
}
