[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)
[![Build Status:](https://travis-ci.com/DonRomanos/Challenge_Template.svg?branch=master)](https://travis-ci.com/DonRomanos/Challenge_Template.svg?branch=master)

# Challenge Template

This is a basic setup to create some of my programming Challenges (and actually most of my other projects). It contains a *CMakeLists.txt* and *conanfile.py* as well as a basic Benchmark and Tests. It sets up and uses the following tools / libraries:

* CMake
* Conan
* GTest
* Google Microbenchmark

**Note**: By default this uses C++20 because I want to play around with the newest things.

## How to use

Requirements:

* CMake
* Conan
* Python (for Conan)

Installing the Libraries using Conan (I recommend using a [conan profile](https://docs.conan.io/en/latest/reference/profiles.html) to specify your settings, e.g. run conan install .. -p gcc-9 ...)

```shell
mkdir build && cd build
conan install .. --build missing
conan build ..
```

Now you should see the library successfully compiling and running the tests.

### Disabling some of the Dependencies

If you don't have the dependencies or dont want to build with them for some reason you can disable them with these CMake Options, simply set them to 'OFF'.

```cmake
BUILD_REFERENCE
BUILD_TESTS
BUILD_BENCHMARK
```

### Alternative without Conan

Requirements:

* CMake

You can build the library without Conan if you manually install the gtest and google benchmark libraries.

Just tell Cmake where to find them by setting following CMake Variables

```cmake
gtest_DIR
benchmark_ROOT_DIR
```

You can do so via command line

```cmake
cmake -Dgtest_DIR=usr/local/... -Dbenchmark_ROOT_DIR=usr/...
```

or via the gui by adding a path entry with the name.

## Continous Integration

I use [travis](https://travis-ci.com/) as continous integration tool, it is free to use for open source projects. Setup is a little tricky when using conan (there is some lack of documentation) so here is my experience.

### Setup

Travis normally integrates quite nicely with github, however the distros they use come with quite outdated compilers and don't run conan by default.

There are two ways to approach this problem:

#### Customize .travisl.yml to install all dependencies

This basically means installing gcc, cmake, and conan. I ran into the Problem that cmake did not recognize the proper gcc version and always tried to build with the default shipped one.

#### Use a docker image with preinstalled conan and gcc

This relies on [conan-package-tools](https://github.com/conan-io/conan-package-tools) which is a convenience module to create multiple conan packages for different platforms and settings.

It also offers the use of docker containers. There are prebuild versions of most docker containers.

Since this is the more common approach and also provides the possibility to specify a build matrix, this is the approach i took. (In the future this will hopefully also upload my packages directly)

##### Issues and Solutions for setting up Travis

When building with gcc and gtest using the c++20 flags lead to a missmatch of abi compatibility used and therefore to the following linker error

```cpp
undefined reference to `testing::Message::GetString[abi:cxx11]() const
```

The gtest package only provides C++11 compatibility while travis was trying to build both, starting with `libcxx=libstdc++` which failed due to the incompatibility of the gtest package.

My solution was to simply remove the build with old std library.

```python
    builder.add_common_builds(pure_c=False)
    builder.remove_build_if(lambda build: build.settings["compiler.libcxx"] == "libstdc++")
    for settings, options, env_vars, build_requires, reference in builder.items:
        settings["compiler.cppstd"] = "20"
```

## Packaging and Deployment

**WIP** will be added later.

## For the Future

Some ideas that can be tried out...

* At the moment I use the Visual Studio open CMakeCache feature on windows, however I can not switch the config with the current setup. Maybe this can be improved (e.g. with cmake_find_package_multi generator).
* Experiment a little bit with CMake calling conan, as then there would be only one entry point. (Saw this somewhere before, there should be some helpers available online)

## Issues discovered during Developement

While setting up this repository I came across several problems which I want to note down here.

### Conan Generators dealing with Targets

Currently I'm using the 'cmake_find_package' generator which will create a 'findXXX.Cmake' file. This way I don't have to modify anything in the CMakeLists.txt compared to an approach without Conan. However since this file is automatically created it does not take into account all targets but creates only one for gtest for example.

Therefore I could not link against gtest::main but had to provide the main for myself. As the Gtest package comes with a Config.cmake I wonder if its possible to have the 'cmake_find_package' generator use that instead. But this is a separate story that should be adressed within Conan.

### Ctest swallowing output

At first I wanted to show the benchmark output as part of the tests. However ctest by default swallows all the output and can only be brought to bring it up for failing tests, as far as I could see. I could not work my way around it and decided to make a target and not a test executing the benchmark.

Another separate Story would be adding an option to add_test in cmake to show the standard output.
