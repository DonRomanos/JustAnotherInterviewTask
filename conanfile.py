from conans import ConanFile, CMake

class JustAnotherInterviewTask(ConanFile):
    name = 'JustAnotherInterviewTask'
    version = '0.1'
    settings = 'os', 'arch', 'compiler', 'build_type'
    requires = "Qt/5.9.7@bincrafters/stable", "Catch2/2.11.1@catchorg/stable"
    generators = "cmake_find_package"
    exports = ["LICENSE.md"]
    exports_sources = ["src/*","CMakeLists.txt"]
    default_options = {
        "qt:shared": True,
        "qt:commercial": False,
        "qt:opengl": "no",
        "qt:with_glib": False,
        "qt:openssl": False,
        "qt:with_pcre2": False,
        "qt:with_doubleconversion": True,
        "qt:with_freetype": False,
        "qt:with_fontconfig": False,
        "qt:with_icu": False,
        "qt:with_harfbuzz": False,
        "qt:with_libjpeg": True,
        "qt:with_libpng": True,
        "qt:with_sqlite3": False,
        "qt:with_mysql": False,
        "qt:with_pq": False,
        "qt:with_odbc": False,
        "qt:with_sdl2": False,
        "qt:with_libalsa": False,
        "qt:with_mesa": False,
        "qt:with_openal": False,
        "qt:with_zstd": False
        }

    def imports(self):
        self.copy("*.dll", "", "bin")
        self.copy("*.dylib", "", "lib")

    def build(self):
        cmake = CMake(self, generator='Ninja')
        cmake.configure()
        cmake.build()
        cmake.test() # If you dont have any tests this will fail!