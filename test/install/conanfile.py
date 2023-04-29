from conans import ConanFile

class LDDProDepsConan(ConanFile):
  settings = "os", "compiler", "build_type", "arch", "arch_build"

  options = {"system_curl_uuid": [True, False]}
  default_options = {"system_curl_uuid": False}

  def requirements(self):
    self.requires("nlohmann_json/3.11.2")
    self.requires("sqlite3/3.42.0")
    if not self.options.system_curl_uuid and self.settings.os != "Macos" and (self.settings.os != "Windows" or self.settings.os.subsystem == "cygwin" or self.settings.os.subsystem == "wsl"):
      self.requires("libcurl/8.0.1")
      self.requires("libuuid/1.0.3")
