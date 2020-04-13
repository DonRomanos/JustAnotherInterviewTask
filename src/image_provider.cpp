#include "image_provider.hpp"

#include <algorithm>

namespace fs = std::filesystem;

namespace
{
	// Supporting both linux and windows, so we need char and wchar ...
	QString toQString(const char* in) { return QString::fromStdString(in); }
	QString toQString(const wchar_t* in) { return QString::fromStdWString(in); }

	// named lambda for better readability
	auto is_allowed_image_file(const std::vector<std::string_view>& formats)
	{
		return [formats](const fs::path& path) { 
			return fs::is_regular_file(path) 
				&& std::end(formats) != std::find(std::begin(formats), std::end(formats), path.extension().string()); 
		};
	}
}

core::ImageProvider::ImageProvider(const std::filesystem::path& directory) : current_file(directory)
{
}

std::pair<std::filesystem::path, std::future<QImage> >  core::ImageProvider::load_next_image(const std::vector<std::string_view>& allowed_formats)
{
	allowed_formats;
	return { fs::path(), std::future<QImage>() };
}

std::future<QImage> core::ImageProvider::apply_mirror(QImage&& image, MirrorModes mode)
{
	image;
	mode;
	return std::future<QImage>();
}

std::future<QImage> core::ImageProvider::load_next_image_and_apply_mirror(MirrorModes mode, const std::vector<std::string_view>& allowed_formats)
{
	mode;
	allowed_formats;
	return std::future<QImage>();
}
