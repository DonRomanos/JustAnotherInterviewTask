from conans import ConanFile, CMake

class JustAnotherInterviewTask(ConanFile):
    name = 'JustAnotherInterviewTask'
    version = '0.1'
    settings = 'os', 'arch', 'compiler', 'build_type'
    requires = "qt/5.14.2@bincrafters/stable", "Catch2/2.11.1@catchorg/stable"
    generators = "cmake_find_package_multi"
    exports = ["LICENSE.md"]
    exports_sources = ["src/*","CMakeLists.txt"]

    def imports(self):
        self.copy("*.dll", "", "bin")
        self.copy("*.dylib", "", "lib")

    def build(self):
        cmake = CMake(self, generator='Ninja')
        cmake.configure()
        cmake.build()
        cmake.test() # If you dont have any tests this will fail!