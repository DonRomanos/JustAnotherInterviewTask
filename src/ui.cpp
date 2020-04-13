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
		for (auto format : formats)
		{
			result += format + " ";
		}
		return result;
	}

	bool has_supported_extension(const fs::path& file)
	{
		auto formats = QImageReader::supportedImageFormats();
		return formats.end() != std::find_if(formats.begin(), formats.end(), [file](QByteArray extension) {return "." + extension.toStdString() == file.extension().string(); });
	}
}

ui::MainPanel::MainPanel()
	: start_stop_button(new QPushButton(tr("Stop Slideshow"), this))
	, current_mirror_mode(core::MirrorModes::None)
	, change_image_timer(new QTimer(this))
	, current_image(QSize(640, 480))
	, image_display(new QLabel{ nullptr })
	, selected_directory(fs::current_path())
	, folder_label(new QLabel{ nullptr })
{
	image_display->setPixmap(current_image);
	image_display->setMinimumWidth(640);
	image_display->setMinimumHeight(480);
	image_display->setAlignment(Qt::AlignCenter);
	auto horizontal_layout = new QHBoxLayout{ this };
	horizontal_layout->addWidget(image_display);

	auto vertical_layout = new QVBoxLayout;
	vertical_layout->setAlignment(Qt::AlignTop);

	folder_label->setText(tr("Current Folder: ") + QString::fromStdString(selected_directory.string()));
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

	update_files_to_display();
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

void ui::MainPanel::mirror_mode_changed(int new_mode)
{
	current_mirror_mode = static_cast<core::MirrorModes>(new_mode); // important to keep the enum indices in sync with the combobox
}

void ui::MainPanel::change_folder()
{
	auto from_file_dialog = QFileDialog::getExistingDirectory(this, tr("Open Directory"), QDir::currentPath(), QFileDialog::DontResolveSymlinks);
	if (from_file_dialog.isNull())
	{
		return;
	}
	
	folder_label->setText(tr("Current Folder: ") + from_file_dialog);
	selected_directory = fs::path(from_file_dialog.toStdString());
	update_files_to_display();
}

void ui::MainPanel::update_files_to_display()
{
	files_to_display.clear();
	std::copy_if(fs::directory_iterator(selected_directory), fs::directory_iterator(), std::back_inserter(files_to_display),
		[](fs::directory_entry elem) {return elem.is_regular_file() && has_supported_extension(elem); });
	next_file_to_display = files_to_display.begin();

	if (next_file_to_display != std::end(files_to_display))
	{
		next_image = core::start_loading_image(*next_file_to_display, current_mirror_mode);
	}
}

void ui::MainPanel::start_loading_next_image()
{
	++next_file_to_display;
	if (next_file_to_display == std::end(files_to_display)) // wrap around if we reach the end
	{
		update_files_to_display();
	}

	if (next_file_to_display != std::end(files_to_display))
	{
		next_image = core::start_loading_image(*next_file_to_display, current_mirror_mode);
	}
}

void ui::MainPanel::update_image()
{
	if (next_file_to_display == std::end(files_to_display)) // Folder does not contain any supported formats...
	{
		image_display->setText(tr("Selected folder does not contain any supported images"));
		return;
	}

	QImage loaded_image = next_image.get();
	if (loaded_image.isNull())
	{
		image_display->setText(tr("Unable to load: ") + QString::fromStdString(next_file_to_display->string()));
	} 
	else
	{
		current_image.convertFromImage(std::move(loaded_image));
		image_display->setPixmap(current_image);
	}

	start_loading_next_image();
}
