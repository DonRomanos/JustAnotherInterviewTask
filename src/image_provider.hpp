#pragma once

#include <QImage>

#include <filesystem>
#include <vector>
#include <future>
#include <string_view>
#include <utility>

namespace core
{
	enum class MirrorModes
	{
		None,
		Horizontal,
		Vertical
	};
	
	class ImageProvider
	{
	public:
		explicit ImageProvider(const std::filesystem::path& directory);

		std::pair<std::filesystem::path, std::future<QImage> > load_next_image(const std::vector<std::string_view>& allowed_formats = { ".jpg", ".tiff", ".bmp" });
		std::future<QImage> apply_mirror(QImage&& image, MirrorModes mode);
		
		// Optimization: uses the same thread for loading and applying the mirror.
		std::future<QImage> load_next_image_and_apply_mirror(MirrorModes mode, const std::vector<std::string_view>& allowed_formats = { ".jpg", ".tiff", ".bmp" });

	private:
		std::filesystem::directory_iterator current_file;
	};
}