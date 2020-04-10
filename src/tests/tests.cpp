#define CATCH_CONFIG_MAIN 
#include "catch2/catch.hpp"

TEST_CASE("Our first test  ", "[Basic]") {
    REQUIRE(true == true);
}