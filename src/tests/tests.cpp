#define CATCH_CONFIG_MAIN 
#include "catch2/catch.hpp"

#include "core.hpp"

namespace fs = std::filesystem;

TEST_CASE("Test if ImageProvider loads images as expected", "[ImageProvider]")
{
	core::ImageProvider to_be_tested{ fs::current_path() };

	auto first_image = to_be_tested.load_next_image();
	auto second_image = to_be_tested.load_next_image().get();
	auto third_image = to_be_tested.load_next_image().get();

	// Good for debugging, since the tests depend on the order of files the directory iterator finds.
	std::vector<fs::path> order_of_files;
	std::transform(fs::directory_iterator(fs::current_path()), fs::directory_iterator(), std::back_inserter(order_of_files), [](const fs::directory_entry& entry) {return entry.path(); });

	SECTION("loading of an invalid image (first file) returns Null image")
	{
		REQUIRE(first_image.get().isNull());
	}

	SECTION("loading of a .bmp image (second file) works")
	{
		REQUIRE(!second_image.isNull());
	}

	SECTION("loading of a .jpg image (third file) works")
	{
		REQUIRE(!third_image.isNull());
	}

	SECTION("after reaching the end of the folder we start from the beginning again")
	{
		auto should_be_same_as_first = to_be_tested.load_next_image().get();
		REQUIRE(first_image.get() == should_be_same_as_first);
	}

	SECTION("folder without any supported files returns invalid future")
	{
		core::ImageProvider empty_provider(fs::current_path().parent_path());
		REQUIRE(!empty_provider.load_next_image().valid());
	}

	SECTION("Mirror is applied when loading an image")
	{
		core::ImageProvider another{ fs::current_path()};

		auto first_mirrored = another.load_next_image(core::MirrorModes::Horizontal);
		auto second_mirrored = another.load_next_image(core::MirrorModes::Horizontal).get();
		auto third_mirrored = another.load_next_image(core::MirrorModes::Vertical).get();

		REQUIRE(!third_mirrored.isNull());
		REQUIRE(third_mirrored != third_image);
		REQUIRE(!second_mirrored.isNull());
		REQUIRE(second_mirrored != second_image);

		SECTION("Mirroring twice should return the original")
		{
			auto second_twice_mirrored = core::ImageProvider::apply_mirror_mode(
				core::MirrorModes::Horizontal, core::ImageInfo{ second_mirrored }).get();
			REQUIRE(second_twice_mirrored == second_image);
			auto third_twice_mirrored = core::ImageProvider::apply_mirror_mode(
				core::MirrorModes::Vertical, core::ImageInfo{ third_mirrored }).get();
			REQUIRE(third_twice_mirrored == third_image);
		}

		SECTION("Reapplying a mirror should first undo the old mirroring")
		{
			auto reapplying_mirror_on_second = core::ImageProvider::apply_mirror_mode(
				core::MirrorModes::Horizontal, core::ImageInfo{ second_mirrored, core::MirrorModes::Horizontal }).get();
			REQUIRE(reapplying_mirror_on_second == second_mirrored);

			auto reapplying_mirror_on_third = core::ImageProvider::apply_mirror_mode(
				core::MirrorModes::Vertical, core::ImageInfo{ third_mirrored, core::MirrorModes::Vertical }).get();
			REQUIRE(reapplying_mirror_on_third == third_mirrored);
		}
	}
}