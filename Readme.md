[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)
[![Build Status:](https://travis-ci.com/DonRomanos/Challenge_Template.svg?branch=master)](https://travis-ci.com/DonRomanos/Challenge_Template.svg?branch=master)

# An Interview Problem I was given to solve

A Qt Application running a slideshow of images in a selected folder.

* CMake
* Qt
* C++17 Compiler
* Conan (Recommended but optional)
* Catch2 (if you want to run the tests)

## How to use

Requirements:

* CMake
* Conan
* Python (for Conan)

Installing the Libraries using Conan (I recommend using a [conan profile](https://docs.conan.io/en/latest/reference/profiles.html) to specify your settings, e.g. run conan install .. -p gcc-9 ...)

```shell
conan remote add catchorg https://api.bintray.com/conan/catchorg/Catch2
conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan

mkdir build && cd build
conan install .. --build missing
conan build ..
```

Now you should see the library successfully compiling and running the tests.

### Disabling some of the Dependencies

If you don't have the dependencies or dont want to build with them for some reason you can disable them with these CMake Options, simply set them to 'OFF'. (If CMake cannot find Catch2 it will automatically turn them off)

```cmake
BUILD_TESTS
```

### Alternative without Conan

Requirements:

* CMake

You can build the library without Conan if you manually install Qt (and Catch2 if you want to build the tests) and point cmake to it.

Just tell Cmake where to find them by setting following CMake Variables

```cmake
Qt5_DIR
Catch2_DIR (optional)
``` 

**Note**: CMake should take care of copying required shared libraries and setting up the platforms folder, if not you should do it manually. Otherwise you will get runtime errors.

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