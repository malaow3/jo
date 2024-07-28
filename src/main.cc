#include <fmt/format.h>

#if defined(__APPLE__)
const char platform[] = "macOS";
#elif defined(__linux__)
const char platform[] = "Linux";
#elif defined(_WIN32)
const char platform[] = "Windows";
#else
const char platform[] = "Unknown";
#endif

struct version_t {
  int major;
  int minor;
  int patch;
};

const version_t version = {0, 1, 0};

int main(int argc, char **argv) {
  fmt::print("jo version {}.{}.{} - {}\n", version.major, version.minor,
             version.patch, platform);
}
