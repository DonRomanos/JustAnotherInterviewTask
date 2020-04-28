cmake_minimum_required(VERSION 3.12)

project(Slideshow)

# Setting up everything for Qt5 including qt.conf pointing to platforms folder (relies on qt folder structure root/bin/dlls...)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

get_target_property(Qt5_WidgetLocation Qt5::Widgets LOCATION)
get_filename_component(Qt5_bin_folder ${Qt5_WidgetLocation} DIRECTORY)
get_filename_component(Qt5_root ${Qt5_bin_folder} DIRECTORY)

if(NOT EXISTS ${CMAKE_BINARY_DIR}/qt.conf)
    message(STATUS "Writing qt.conf file pointing to: ${Qt5_root}")
    file(WRITE "${CMAKE_BINARY_DIR}/qt.conf" "[Paths]\n\nPrefix = ${Qt5_root}")
else()
    message(STATUS "Using existing qt.conf file: ${CMAKE_BINARY_DIR}/qt.conf")
endif()

#***************************************************************************
# Required for Linux

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
#***************************************************************************

add_executable(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE src/)

target_sources(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt5::Widgets
    Threads::Threads
)

target_link_options(${PROJECT_NAME} PRIVATE 
    $<$<CXX_COMPILER_ID:MSVC>:/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup> # don't want a console window
) 

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

target_compile_options(${PROJECT_NAME}
 PRIVATE
     $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
        -Wall -Werror -pedantic-errors -Wextra -Wconversion -Wsign-conversion -Wno-unused-function>
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
if(BUILD_TESTS)
    find_package(Catch2)
    if( NOT Catch2_FOUND)
        message(STATUS "Catch2 was not found, tests are not available. (Catch2_DIR = ${Catch2_DIR})")
        return()
    endif()

    add_executable(${PROJECT_NAME}_Tests)

    target_sources(${PROJECT_NAME}_Tests PRIVATE 
        src/tests/tests.cpp
        src/core.cpp
    )

    target_include_directories(${PROJECT_NAME}_Tests PRIVATE
        src/)
    target_link_libraries(${PROJECT_NAME}_Tests PRIVATE
        Catch2::Catch2
        Qt5::Widgets
        Threads::Threads
    )

    target_compile_features(${PROJECT_NAME}_Tests PUBLIC cxx_std_17)

    # FindPackage should set this variable...
    message(STATUS "Looking for Catch.cmake in ${Catch2_LIB_DIRS}/cmake/Catch2")

    include(${Catch2_LIB_DIRS}/cmake/Catch2/Catch.cmake)
    catch_discover_tests(${PROJECT_NAME}_Tests WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/src/tests/testfiles")
    enable_testing()

    ## Special Test for the runtime to see if the application actually runs, so all dynamic libraries and platforms etc. are available
    add_test(NAME "Qt_application_starts" COMMAND ${PROJECT_NAME} --dryrun)
endif()