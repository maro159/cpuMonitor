# cpuMonitor

A simple multi-threaded application for real-time CPU utilization monitoring on Linux.

## Project Overview

**cpuMonitor** reads `/proc/stat`, calculates per-core and total CPU usage, and outputs results to both the terminal and a CSV file for later analysis. It is designed for extensibility, reliability, and ease of use.

## Architecture

- **Modular Design:**
  - `Monitor` manages the workflow, using two threads: one for reading CPU stats, one for logging.
  - `StatsReader` parses `/proc/stat` and computes core utilization.
  - `SnapshotQueue` is a thread-safe queue for passing CPU utilization snapshots between threads.
  - `IDataWriter` interface allows for flexible output (console, CSV, etc.).
- **Modern C++:**
  - Uses `std::jthread` and `std::stop_token` (C++20) for safe, automatic thread management and cooperative cancellation.
- **Extensible:**
  - Possibility to add new output formats by implementing `IDataWriter`.
- **Testing:**
  - Includes unit tests for all major components (see `tests/`).


## Example Output
Console
```
CPU0: 12.34% CPU1: 10.56% TOTAL: 11.45%
```
CSV
```
timestamp,cpu0,cpu1,total
1718550000,12.34,10.56,11.45
```

## Build Instructions
Clone this repo, enter the folder and type commands:

```sh
mkdir build
cd build
cmake ..
make -j4
```

If you want to run unit tests:
```sh
ctest
```

## Usage

The executable will be placed in `build/src/cpuMonitor` and symlinked to `build/cpuMonitor`

To run it from build dir:
```sh
./cpuMonitor
```

### Command-Line Options

- `--interval ms`  Set refresh interval in milliseconds (default: 1000)
- `--help`, `-h`   Show help message and exit
- `--version`, `-v` Show version information and exit

### Example

```sh
./src/cpuMonitor --interval 500
./src/cpuMonitor -h
./src/cpuMonitor --version
```

- If `--interval` is invalid or missing, the default interval (1000 ms) will be used
- Press `Ctrl+C` to stop the monitor.
- Output is shown in the console and also saved to `cpu_usage.csv`.



## License
MIT
