#include "core.hpp"

#include <QImageReader>
#include <algorithm>

namespace fs = std::filesystem;

namespace
{
	// Supporting both linux and windows, so we need char and wchar ...
	QString toQString(const char* in) { return QString::fromStdString(in); }
	QString toQString(const wchar_t* in) { return QString::fromStdWString(in); }

	inline QImage mirror_image(const QImage& image, core::MirrorModes mirror_mode)
	{
		if (mirror_mode == core::MirrorModes::Horizontal)
		{
			return image.mirrored(false, true); // bool parameters... yay (horizontal, vertical)
		}
		else if (mirror_mode == core::MirrorModes::Vertical)
		{
			return image.mirrored(true, false);
		}
		return image;
	}
}

std::future<QImage> core::start_loading_image(const fs::path& file, MirrorModes mirror_mode)
{
	return std::async(std::launch::async, [file, mirror_mode]()
		{
			auto result = QImage(toQString(file.c_str()));
			if (!result.isNull())
			{
				return mirror_image(result, mirror_mode);
			}
			// Unable to load image...
			return QImage();
		}
	);
}

std::future<QImage> core::apply_mirror(const QImage& image, MirrorModes mirror_mode)
{
	// Don't have to move image since QImage is copy on write by default.
	return std::async(std::launch::async, [image, mirror_mode]()
		{
			return mirror_image(image, mirror_mode);
		}
	);
}

namespace
{
	bool has_supported_extension(const fs::path& file)
	{
		const auto formats = QImageReader::supportedImageFormats();
		return formats.end() != std::find_if(formats.begin(), formats.end(), [file](QByteArray extension) {return "." + extension.toStdString() == file.extension().string(); });
	}

	void advance_until_supported_image(fs::directory_iterator& current_entry)
	{
		while (current_entry != fs::directory_iterator() && !has_supported_extension(*current_entry))
		{
			++current_entry;
		}
	}
}

core::ImageProvider::ImageProvider(const std::filesystem::path& dir) 
	: directory{dir}
	, next_entry{directory}
{
}

std::future<QImage> core::ImageProvider::load_next_image(MirrorModes mirror_mode)
{
	// finding the next file is done synchronously
	advance_until_supported_image(next_entry);
	if (next_entry == fs::directory_iterator())
	{
		next_entry = fs::directory_iterator(directory); // wrap around behavior if we reach the end we start from beginning
		advance_until_supported_image(next_entry);
		if (next_entry == fs::directory_iterator())
		{
			return std::async([](){return QImage(); }); // no files available for loading.
		}
	}

	auto file_to_load = *next_entry; // There is no postfix for directory_iterator
	++next_entry;

	return std::async(std::launch::async, [file_to_load, mirror_mode]()
		{
			auto result = QImage(toQString(file_to_load.path().c_str()));
			if (!result.isNull())
			{
				return mirror_image(result, mirror_mode);
			}
			// Unable to load image...
			return QImage();
		}
	);
}

std::future<QImage> core::ImageProvider::apply_mirror_mode(MirrorModes new_mirror_mode, ImageInfo info)
{
	// first undo old mode then apply the new mode
	return std::async(std::launch::async, [info, new_mirror_mode]()
		{
			QImage result = mirror_image(info.image, info.mirror_mode);
			return mirror_image(result, new_mirror_mode);
		}
	);
}
