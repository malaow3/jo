#include <fmt/format.h>

#include <string>

/*#include "./debug.hpp"*/

using std::string;

enum platform_t { macOS, Linux, Windows, Unknown };
// to_string function
string to_string(platform_t platform) {
  switch (platform) {
    case macOS:
      return "macOS";
    case Linux:
      return "Linux";
    case Windows:
      return "Windows";
    case Unknown:
      return "Unknown";
  }
}

#if defined(__APPLE__)
#define PLATFORM_MACOS
#include <sys/sysctl.h>
#include <sys/types.h>
const platform_t platform = macOS;

#elif defined(__linux__)
#define PLATFORM_LINUX
const platform_t platform = Linux;

#elif defined(_WIN32)
#define PLATFORM_WINDOWS
const platform_t platform = Windows;

#else
#define PLATFORM_UNKNOWN
const platform_t platform = Unknown;
#endif

struct version_t {
  int major;
  int minor;
  int patch;
};

const version_t version = {0, 1, 0};

int main(int argc, char **argv) {
  fmt::print("jo version {}.{}.{} - {}\n", version.major, version.minor,
             version.patch, to_string(platform));

#ifdef PLATFORM_MACOS
  char buffer[256];
  size_t length = sizeof(buffer);
  int error =
      sysctlbyname("machdep.cpu.brand_string", buffer, &length, NULL, 0);
  if (error != 0) {
    switch (error) {
      case ENOENT:
        fmt::print("sysctlbyname failed with error {}\n", error);
        break;
      case EINVAL:
        fmt::print("sysctlbyname failed with error {}\n", error);
        break;
      case EPERM:
        fmt::print("sysctlbyname failed with error {}\n", error);
        break;
      default:
        fmt::print("sysctlbyname failed with error {}\n", error);
    }
  }
  fmt::print("{}\n", buffer);
  return 0;
#endif

#ifdef PLATFORM_LINUX
  return 1;
#endif

#ifdef PLATFORM_WINDOWS
  return 1;
#endif

#ifdef PLATFORM_UNKNOWN
  return 1;
#endif
}
