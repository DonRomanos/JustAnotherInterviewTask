#include "image_provider.hpp"

#include <algorithm>

namespace fs = std::filesystem;

namespace
{
	// Supporting both linux and windows, so we need char and wchar ...
	QString toQString(const char* in) { return QString::fromStdString(in); }
	QString toQString(const wchar_t* in) { return QString::fromStdWString(in); }
}

std::future<QImage> core::start_loading_image(const fs::path& file, MirrorModes mirror_mode)
{
	return std::async(std::launch::async, [file, mirror_mode]()
		{
			auto result = QImage(toQString(file.c_str()));
			if (!result.isNull() && mirror_mode == MirrorModes::Horizontal)
			{
				result = result.mirrored(true, false); // bool parameters... yay (horizontal, vertical)
			}
			else if (!result.isNull() && mirror_mode == MirrorModes::Vertical)
			{
				result = result.mirrored(false, true);
			}
			return std::move(result); // since we are creating a future here it makes sense for me to move.
		}
	);
}
