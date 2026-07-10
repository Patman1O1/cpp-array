from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps, cmake_layout
import os

class CxxArray(ConanFile):
    name = "cxx_array"
    version = "0.1.0"
    author = "Patman1O1"
    description = ""

    settings = ("os", "arch", "compiler", "build_type")
    exports_sources = ("CMakeLists.txt", "cmake/*", "include/*", "tests/*")

    options = {
        "build_tests": [True, False]
    }

    default_options = {
        "build_tests": False
    }

    def build_requirements(self) -> None:
        self.tool_requires("cmake/[>=4.3.0]")
        if bool(self.options.build_tests) or self.settings.build_type == "Debug":
            self.test_requires("gtest/1.14.0")

    def layout(self) -> None:
        cmake_layout(self)

    def generate(self) -> None:
        toolchain = CMakeToolchain(self)
        if self.settings.build_type == "Debug":
            toolchain.variables["BUILD_TESTS"] = True
        else:
            toolchain.variables["BUILD_TESTS"] = bool(self.options.build_tests)
        toolchain.generate()
        CMakeDeps(self).generate()

    def build(self) -> None:
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self) -> None:
        CMake(self).install()

    def package_info(self) -> None:
        self.cpp_info.set_property("cmake_target_name", "cxx_array::cxx_array")
        self.cpp_info.bindirs = []
        self.cpp_info.libdirs = []
