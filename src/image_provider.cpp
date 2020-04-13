#include "image_provider.hpp"

#include <algorithm>

namespace fs = std::filesystem;

namespace
{
	// Supporting both linux and windows, so we need char and wchar ...
	QString toQString(const char* in) { return QString::fromStdString(in); }
	QString toQString(const wchar_t* in) { return QString::fromStdWString(in); }

	bool has_any_of_these_extensions(const fs::path& file, std::vector<std::string_view> extensions)
	{
		return std::end(extensions) != std::find_if(std::begin(extensions), std::end(extensions), [file](std::string_view ext) { return ext == file.extension(); });
	}
}

std::future<QImage> core::start_loading_image(const fs::path& file)
{
	return std::async(std::launch::async, [file](){return QImage(toQString(file.c_str()));});
}
