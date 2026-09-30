from conan import ConanFile
from conan.tools.cmake import CMakeDeps, CMakeToolchain


class MyLvlAppConan(ConanFile):
    name = "my-lvgl-app"
    version = "0.1.0"

    settings = "os", "arch", "compiler", "build_type"

    requires = (
        "sdl/2.32.10",
        "kangaru/4.3.2",
        "boost/1.91.0",
        "openssl/4.0.2",
    )

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()

        toolchain = CMakeToolchain(self)
        toolchain.user_presets_path = False
        toolchain.generate()

    def configure(self):
        self.options["boost/*"].without_cobalt = True

        if self.settings.os == "Linux":
            self.options["sdl/*"].pulse = False
