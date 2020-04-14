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
	
	[[nodiscard]] std::future<QImage> start_loading_image(const std::filesystem::path& file, MirrorModes mirror_mode = MirrorModes::None);
	[[nodiscard]] std::future<QImage> apply_mirror(const QImage& image, MirrorModes mirrorMode);
}