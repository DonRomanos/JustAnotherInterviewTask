cmake_minimum_required(VERSION 3.12)

project(Slideshow)

# Setting up everything for Qt5 including qt.conf pointing to platforms folder (relies on qt folder structure root/bin/dlls...)
find_package(Qt5 COMPONENTS Widgets REQUIRED)

get_target_property(Qt5_WidgetLocation Qt5::Widgets LOCATION)
get_filename_component(Qt5_bin_folder ${Qt5_WidgetLocation} DIRECTORY)
get_filename_component(Qt5_root ${Qt5_bin_folder} DIRECTORY)

message(STATUS "Writing qt.conf file pointing to: ${Qt5_root}")
file(WRITE "${CMAKE_BINARY_DIR}/qt.conf" "[Paths]\n\nPrefix = ${Qt5_root}")

#***************************************************************************

add_executable(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE src/)

target_sources(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt5::Widgets
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

target_compile_options(${PROJECT_NAME}
 PRIVATE
     $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
        -Wall -Werror -pedantic-errors -Wextra -Wconversion -Wsign-conversion>
     $<$<CXX_COMPILER_ID:MSVC>:
        /WX /W4>
)

# Very specific, could also copy all dlls from bin folder for example or use windeploy qt. Not sure whats better, but this works.
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::Core> $<TARGET_FILE_DIR:${PROJECT_NAME}>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::Gui> $<TARGET_FILE_DIR:${PROJECT_NAME}>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::Widgets> $<TARGET_FILE_DIR:${PROJECT_NAME}>
)

#***************************************************************************
option(BUILD_TESTS "Will build the tests verifying your result" ON)
if( BUILD_TESTS)
#    find_package(Catch2 REQUIRED)
#    add_executable(${PROJECT_NAME}_Tests)
#    target_sources(${PROJECT_NAME}_Tests PRIVATE 
#        src/tests.cpp)
#    target_include_directories(${PROJECT_NAME}_Tests PRIVATE
#        src/)
#    target_link_libraries(${PROJECT_NAME}_Tests PRIVATE
#        Catch2::Catch2
#        )
#    include(Catch)
#    catch_discover_tests(${PROJECT_NAME}_Tests)
#    enable_testing()
endif()