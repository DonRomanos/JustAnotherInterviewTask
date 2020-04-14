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

	struct ImageInfo
	{
		ImageInfo(const QImage& image, MirrorModes mode = MirrorModes::None) : image(image), mirror_mode(mode){}

		QImage image;
		MirrorModes mirror_mode;
	};

	class ImageProvider
	{
	public:
		explicit ImageProvider(const std::filesystem::path& dir);
		
		// Loads an image asynchronously and mirrors it if requested
		[[nodiscard]] std::future<QImage> load_next_image(MirrorModes mirror_mode = MirrorModes::None);

		// Mirrors an image asynchronously while first undoing any old mirror, if there was any
		[[nodiscard]] static std::future<QImage> apply_mirror_mode(MirrorModes new_mirror_mode, ImageInfo image_with_any_applied_mirrors);

	private:
		std::filesystem::path directory;
		std::filesystem::directory_iterator next_entry;
	};
	
	[[nodiscard]] std::future<QImage> start_loading_image(const std::filesystem::path& file, MirrorModes mirror_mode = MirrorModes::None);
	[[nodiscard]] std::future<QImage> apply_mirror(const QImage& image, MirrorModes mirrorMode);
}