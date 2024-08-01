#include <fmt/format.h>

#include <CLI/CLI.hpp>
#include <chrono>
#include <string>
#include <thread>

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
#include <signal.h>
#include <stdint.h>

#include <atomic>
#include <fstream>
#include <sstream>
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

void mac_info();
void linux_info();

int main(int argc, char** argv) {
  CLI::App app;
  bool timeout{false};
  app.add_flag("-t", timeout, "timeout after 10 seconds");
  CLI11_PARSE(app, argc, argv);

  fmt::print("jo version {}.{}.{} - {}\n", version.major, version.minor,
             version.patch, to_string(platform));

  switch (platform) {
    case macOS:
      mac_info();
      break;
    case Linux:
      // If timeout is true, run the linux_info function with a timeout of 10
      // seconds
      if (timeout) {
        std::thread t(linux_info);
        t.detach();
        std::this_thread::sleep_for(std::chrono::seconds(10));
        if (t.joinable()) {
          t.join();
        }
      } else {
        linux_info();
      }
      break;
    case Windows:
      fmt::print("Windows platform is not supported at the moment\n");
      break;
    case Unknown:
      fmt::print("Unknown platform\n");
      fmt::print("Currently supported platforms: macOS, Linux\n");
      break;
  }
}

#ifdef PLATFORM_MACOS
void mac_info() {
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
}
#endif

#ifdef PLATFORM_LINUX
struct CPUInfo {
  uint64_t user;
  uint64_t nice;
  uint64_t system;
  uint64_t idle;
  uint64_t iowait;
  uint64_t irq;
  uint64_t softirq;
  uint64_t steal;
  uint64_t guest;
  uint64_t guest_nice;
};
CPUInfo* linux_cpu_info() {
  std::ifstream file("/proc/stat");
  std::string line;
  // Parse the cpu line
  while (std::getline(file, line)) {
    if (line.find("cpu ") != std::string::npos) {
      int64_t user, nice, system, idle, iowait, irq, softirq, steal, guest,
          guest_nice;

      std::istringstream iss(line);
      string cpu;
      iss >> cpu >> user >> nice >> system >> idle >> iowait >> irq >>
          softirq >> steal >> guest >> guest_nice;

      CPUInfo* cpu_info = new CPUInfo();
      cpu_info->user = user;
      cpu_info->nice = nice;
      cpu_info->system = system;
      cpu_info->idle = idle;
      cpu_info->iowait = iowait;
      cpu_info->irq = irq;
      cpu_info->softirq = softirq;
      cpu_info->steal = steal;
      cpu_info->guest = guest;
      cpu_info->guest_nice = guest_nice;
      return cpu_info;
    }
  }
  return nullptr;
}

std::atomic<bool> keep_running(true);
void signal_handler(int s) {
  keep_running = false;
}

void linux_info() {
  signal(SIGINT, signal_handler);

  auto calc_total = [](const CPUInfo* info) {
    return info->user + info->nice + info->system + info->idle + info->iowait +
           info->irq + info->softirq + info->steal + info->guest +
           info->guest_nice;
  };

  auto calc_idle = [](const CPUInfo* info) {
    return info->idle + info->iowait;
  };

  const CPUInfo* curr_cpu_info = nullptr;
  const CPUInfo* prev_cpu_info = nullptr;

  try {
    while (keep_running) {
      prev_cpu_info = linux_cpu_info();
      if (prev_cpu_info == nullptr) {
        fmt::print("Failed to get cpu info\n");
        exit(1);
      }
      // Wait 1 second
      std::this_thread::sleep_for(std::chrono::seconds(1));

      // Get the current cpu info
      curr_cpu_info = linux_cpu_info();
      if (curr_cpu_info == nullptr) {
        fmt::print("Failed to get cpu info\n");
        exit(1);
      }

      uint64_t prev_idle = calc_idle(prev_cpu_info);
      uint64_t curr_idle = calc_idle(curr_cpu_info);
      const uint64_t idle_delta = curr_idle - prev_idle;
      uint64_t prev_total = calc_total(prev_cpu_info);
      uint64_t curr_total = calc_total(curr_cpu_info);
      const uint64_t total_delta = curr_total - prev_total;

      if (keep_running) {
        if (total_delta == 0) {
          fmt::print("CPU usage: 0.00%\n");
        } else {
          auto usage_percent =
              100.0 * (1.0 - static_cast<float>(idle_delta) / total_delta);
          fmt::print("CPU usage: {:.2f}%\n", usage_percent);
        }
      }
    }
  } catch (const std::exception& e) {
    fmt::print("Error: {}", e.what());
  }
  delete prev_cpu_info;
  delete curr_cpu_info;
}

#endif
