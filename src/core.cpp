#include "core.hpp"

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
			return image.mirrored(true, false); // bool parameters... yay (horizontal, vertical)
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

std::future<QImage> core::apply_mirror(QImage&& image, MirrorModes mirror_mode)
{
	return std::async(std::launch::async, [image{ std::move(image) }, mirror_mode]()
		{
			return mirror_image(image, mirror_mode);
		}
	);
}
