#pragma once
#include "version.hpp"
#include <iostream>
#include <string_view>

namespace mp::cpuMonitor::helpers {

inline void printHelp() {
    std::cout << "Usage: " << kProjectNameStr << " [--interval ms] [--help] [--version]\n"
              << "  --interval ms  Set refresh interval in milliseconds (default: 1000)\n"
              << "  --help, -h     Show this help message\n"
              << "  --version, -v  Show version information\n";
}

inline void printVersion() { std::cout << kProjectNameStr << " " << kProjectVersionStr << "\n"; }

inline int parseInterval(int argc, char *argv[], int defaultMs) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if ((arg == "--interval") && (i + 1 < argc)) {
            try {
                int ms = std::stoi(argv[++i]);
                if (ms > 0)
                    return ms;
                throw std::invalid_argument{"Must be positive"};
            } catch (const std::exception &e) {
                std::cerr << "Invalid --interval argument: " << e.what() << ". Using default " << defaultMs << "ms.\n";
                return defaultMs;
            }
        }
    }
    return defaultMs;
}

inline bool handleMetaArgs(int argc, char *argv[]) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return true;
        } else if (arg == "--version" || arg == "-v") {
            printVersion();
            return true;
        }
    }
    return false;
}
} // namespace mp::cpuMonitor::helpers
