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

int main(int argc, char **argv) {
  fmt::print("Hello, world!\n");
  fmt::print("Platform: {}\n", platform);
}
