#define CATCH_CONFIG_MAIN 
#include "catch2/catch.hpp"

#include "image_provider.hpp"

#include <QImageReader>

namespace fs = std::filesystem;

namespace
{
	auto has_filename(std::string_view filename)
	{
		return [filename](const fs::path& full_path) { return full_path.filename() == filename; };
	}
}

TEST_CASE("image_provider works on all images in a directory", "[image_provider]")
{
	std::vector<std::string_view> allowed_formats = { ".jpg", ".tiff", ".bmp" };
	core::ImageProvider images{ fs::current_path() };

	std::vector<fs::path> testfiles;
	std::transform(fs::directory_iterator(fs::current_path()), fs::directory_iterator(), std::back_inserter(testfiles), [](fs::directory_entry i) {return i.path(); });

	SECTION("no loadable images are skipped")
	{
		auto [first_image, first_future] = images.load_next_image();
		auto [second_image, second_future] = images.load_next_image();

		std::vector<fs::path> result = { first_image.filename(), second_image.filename() };
		REQUIRE(result == std::vector<fs::path>{"a_real_bmp_file.bmp", "a_real_jpg_file.jpg"});
	}

	std::vector<fs::path> found_images;
	for (int i = 0; i < testfiles.size(); ++i)
	{
		found_images.push_back(images.load_next_image(allowed_formats).first);
	}

	SECTION("not allowed formats are skipped")
	{
		std::for_each(std::begin(found_images), std::end(found_images), [](fs::path x) {REQUIRE(x.extension() != ".xyz"); });
	}

	SECTION("files that have correct formats but cannot be loaded are skipped")
	{
		REQUIRE(std::find_if(std::begin(found_images), std::end(found_images), has_filename("an_empty_file_with_jpg_extension.jpg")) == std::end(found_images));
	}

	SECTION("after reaching the end of the folder iteration starts from the beginning")
	{
		REQUIRE(std::count_if(std::begin(found_images), std::end(found_images), has_filename("a_real_bmp_file.bmp")) >= 2);
	}

	SECTION("no loadable files in folder does not produce an infinite loop")
	{
		core::ImageProvider images{ fs::current_path().parent_path() };
		auto [empty_path, empty_future] = images.load_next_image(allowed_formats);

		REQUIRE(empty_path.empty());
		REQUIRE(empty_future.get().isNull());
	}
}