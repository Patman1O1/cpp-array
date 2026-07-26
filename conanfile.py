from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps, cmake_layout
import os

class Array(ConanFile):
    name: str = "collections-array"
    version: str = "0.1.0"
    author: str = "Patman1O1"
    description: str = ""

    settings: tuple[str] = ("os", "arch", "compiler", "build_type")
    exports_sources: tuple[str] = ("CMakeLists.txt", "cmake/*", "include/*", "tests/*")

    options: dict[str, list[bool]] = {
        "build_all": [True, False],
        "build_benchmarks": [True, False],
        "build_tests": [True, False],
    }

    default_options: dict[str, bool] = {
        "build_all": False,
        "build_benchmarks": False,
        "build_tests": False,
    }

    def configure(self) -> None:
        if bool(self.options.build_all):
            self.options.build_benchmarks.value = True
            self.options.build_tests.value = True
        elif self.settings.build_type == "Debug":
            self.options.build_tests.value = True

    def build_requirements(self) -> None:
        self.tool_requires("cmake/[>=4.3.0]")

        if bool(self.options.build_benchmarks):
            self.test_requires("benchmark/[>=1.9.5]")

        if bool(self.options.build_tests) or self.settings.build_type == "Debug":
            self.test_requires("gtest/1.17.0")

    def layout(self) -> None: cmake_layout(self)

    def generate(self) -> None:
        toolchain = CMakeToolchain(self)
        toolchain.variables["BUILD_TESTS"] = bool(self.options.build_tests)
        toolchain.variables["BUILD_BENCHMARKS"] = bool(self.options.build_benchmarks)
        toolchain.generate()
        CMakeDeps(self).generate()

    def build(self) -> None:
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self) -> None: CMake(self).install()

    def package_info(self) -> None:
        self.cpp_info.set_property("cmake_target_name", "collections::array")
        self.cpp_info.bindirs = []
        self.cpp_info.libdirs = []
