#define CATCH_CONFIG_MAIN 
#include "catch2/catch.hpp"

#include "core.hpp"

namespace fs = std::filesystem;

TEST_CASE("Set of tests for loading an image and applying a mirror", "[start_loading_image]")
{
	SECTION("loading of .bmp images works")
	{
		auto result = core::start_loading_image(fs::current_path() / "a_real_bmp_file.bmp");
		REQUIRE(!result.get().isNull());
	}

	SECTION("loading of .jpg images works")
	{
		auto result = core::start_loading_image(fs::current_path() / "a_real_jpg_file.jpg");
		REQUIRE(!result.get().isNull());
	}
	
	SECTION("loading an invalid file returns Null image")
	{
		auto result = core::start_loading_image(fs::current_path() / "an_empty_file_with_jpg_extension.jpg");
		REQUIRE(result.get().isNull());
	}

	SECTION("loading a nonexistent file returns Null image")
	{
		auto result = core::start_loading_image(fs::current_path() / "xxxx");
		REQUIRE(result.get().isNull());
	}

	auto unmirrored_image = core::start_loading_image(fs::current_path() / "a_real_bmp_file.bmp").get();
	REQUIRE(!unmirrored_image.isNull());

	SECTION("Mirror is applied when loading an image")
	{
		auto horizontally_mirrored = core::start_loading_image(fs::current_path() / "a_real_bmp_file.bmp", core::MirrorModes::Horizontal).get();
		auto vertically_mirrored = core::start_loading_image(fs::current_path() / "a_real_bmp_file.bmp", core::MirrorModes::Vertical).get();
		
		REQUIRE(!horizontally_mirrored.isNull());
		REQUIRE(!vertically_mirrored.isNull());

		REQUIRE(unmirrored_image.cacheKey() != horizontally_mirrored.cacheKey());
		REQUIRE(unmirrored_image.cacheKey() != vertically_mirrored.cacheKey());
		REQUIRE(vertically_mirrored.cacheKey() != horizontally_mirrored.cacheKey());
	}
}

TEST_CASE("Few tests checking if a mirror is applied correctly", "[apply_mirror]")
{
	auto original = core::start_loading_image(fs::current_path() / "a_real_bmp_file.bmp").get();
	SECTION("applying horizontal mirror twice returns original")
	{
		auto horizontally_mirrored = core::apply_mirror(original.copy(), core::MirrorModes::Horizontal).get();
		REQUIRE(horizontally_mirrored.cacheKey() != original.cacheKey());

		auto twice_mirrored = core::apply_mirror(std::move(horizontally_mirrored), core::MirrorModes::Horizontal).get();
		REQUIRE(twice_mirrored == original);
	}

	SECTION("applying vertical mirror twice returns original")
	{
		auto horizontally_mirrored = core::apply_mirror(original.copy(), core::MirrorModes::Vertical).get();
		REQUIRE(horizontally_mirrored.cacheKey() != original.cacheKey());

		auto twice_mirrored = core::apply_mirror(std::move(horizontally_mirrored), core::MirrorModes::Vertical).get();
		REQUIRE(twice_mirrored == original);
	}
}