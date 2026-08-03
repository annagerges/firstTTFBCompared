# firstTTFBCompared

A lightweight collaborative latency-analysis project that measures Time To First Byte (TTFB) with Python and analyzes tail-risk probability in C++ using Simpson's Rule.

## Changelog
- **2026-08-03:** Rewrote this README into a flagship-quality guide with architecture, build/run instructions, usage workflows, telemetry format details, validation guidance, and contribution standards.

![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)
![Build](https://img.shields.io/badge/build-pending-lightgrey)
![Language](https://img.shields.io/badge/language-Python%20%7C%20C%2B%2B-blue)
![Repo size](https://img.shields.io/github/repo-size/annagerges/firstTTFBCompared)

## Table of Contents
- [Demo / Screenshot](#demo--screenshot)
- [Project Overview](#project-overview)
- [Key Features](#key-features)
- [Quick Start](#quick-start)
- [Usage Examples](#usage-examples)
- [Architecture and Design](#architecture-and-design)
- [Data Format and Telemetry](#data-format-and-telemetry)
- [Building for Release](#building-for-release)
- [Testing and Validation](#testing-and-validation)
- [Contributing](#contributing)
- [License and Authors](#license-and-authors)
- [Contact, Acknowledgements, and Related Resources](#contact-acknowledgements-and-related-resources)

## Demo / Screenshot
![Demo Placeholder](https://via.placeholder.com/1200x630.png?text=TTFB+Collector+%E2%86%92+C%2B%2B+Analyzer+Output)

To replace this placeholder:
1. Add your screenshot to `/assets` (example: `/assets/demo-terminal.png`).
2. Replace the image link above with `![Demo](assets/demo-terminal.png)`.
3. Prefer a terminal capture that shows collection logs plus final probability/percentile output.

## Project Overview
This project started as a **collaborative** exploration of practical networking telemetry and applied math. It collects TTFB samples from repeated HTTP requests and then models latency behavior with a normal-distribution approximation.

**What is TTFB?**
Time To First Byte (TTFB) is the time from request start until the first response byte arrives. It is a strong early indicator of network and server responsiveness because it captures connection setup, routing, and initial backend latency.

**Why it matters:**
- Helps detect regressions in perceived responsiveness.
- Gives a fast signal for latency spikes before full payload transfer.
- Supports data-driven comparisons across runs and environments.

The C++ side computes summary statistics (mean and standard deviation) and estimates tail probability using Simpson's Rule with a dynamic error-bound step calculation.

## Key Features
- Accurate first-byte timing collection via Python `requests` with streaming enabled.
- Robust sample loop with timeout/connection-error handling.
- Cross-language workflow: Python collector output consumed by a C++ analyzer.
- Mean and standard deviation calculation from measured telemetry.
- Tail-area probability estimation with Simpson's Rule and even-step correction.

## Quick Start

### Prerequisites
- Python 3.9+
- `pip install requests`
- C++17-capable compiler
  - Windows: MSVC (`cl`) or MinGW/GCC (`g++`)
  - Linux/macOS: `g++`

### 1) Install Python dependency

```bash
python -m pip install requests
```

### 2) Run collector only (Python)

```bash
python module1.py 20
```

Expected output shape (one TTFB value per line):

```text
108.233519
95.883201
TIMEOUT
101.442307
```

### 3) Build analyzer (C++)

#### Windows (MSVC, Developer Command Prompt)

```powershell
cl /EHsc /std:c++17 "server latency.cpp" latency_monitor.cpp /Fe:ttfb_analyzer.exe
```

#### Windows (MinGW/GCC)

```powershell
g++ -std=c++17 -O2 "server latency.cpp" latency_monitor.cpp -o ttfb_analyzer.exe
```

#### Linux/macOS (g++)

```bash
g++ -std=c++17 -O2 "server latency.cpp" latency_monitor.cpp -o ttfb_analyzer
```

> Note: current source includes Windows-specific calls (`_popen`, `_pclose`, `_getcwd`, `<direct.h>`). For native Linux/macOS builds, switch those calls to `popen`, `pclose`, `getcwd` (or build under Windows/WSL with compatible changes).

### 4) Run full pipeline

#### Default integrated flow (analyzer launches Python collector)

```bash
# Windows
./ttfb_analyzer.exe

# Linux/macOS
./ttfb_analyzer
```

#### Explicit pipe example (collector -> analyzer-compatible parser workflow)

```bash
python module1.py 30 | tee ttfb_samples.txt
```

You can then feed saved numeric lines to your own parser or future analyzer mode that reads `stdin`.

### Example analyzer output

```text
Collecting 30 latency measurements...
Python output:
Data point 1: 103.992188
  -> Parsed as: 103.992
...
Successfully collected 30 data points.

--- Google Latency Analysis Project ---
Observed Latency: 189.772995 ms
Calculated Z-score: 2.107341
Probability (Area under tail): 0.017468
Percentile: 98.253200
```

## Usage Examples

### Single run
```bash
python module1.py 25
```

### Batch runs (repeat experiment)

```bash
# Linux/macOS
for i in 1 2 3; do python module1.py 50 > "run_${i}.txt"; done

# PowerShell
1..3 | ForEach-Object { python module1.py 50 > "run_$_.txt" }
```

### Change number of pings/samples
`module1.py` already accepts a positional count:

```bash
python module1.py 100
```

### Timeout and target URL workflow
Current script uses a fixed timeout (`timeout=10`) and URL (`https://www.google.com`) in `module1.py`.
Common workflow is to edit those values directly for experiments.

### Redirect output to file

```bash
python module1.py 100 > telemetry_raw.txt
```

## Architecture and Design

```text
Python collector (module1.py)
    -> prints one latency sample per line
        -> IPC pipe (_popen/popen stream)
            -> C++ analyzer (latency_monitor.cpp + server latency.cpp)
                -> mean/stddev + Simpson's Rule tail probability
```

### Simpson's Rule and error control
- Integration target: probability mass from observed maximum latency to a high-tail bound (`mean + 5*stddev`).
- Step selection: `errorBoundFormula(...)` computes `n` from an error tolerance term and ensures `n` is even (required by Simpson's Rule).
- Result: approximate tail probability and derived percentile.

### Where to change parameters
- Sample count input: prompt in `server latency.cpp` and command arg in `module1.py`.
- Request timeout + URL: `module1.py` request config.
- Numerical tolerance behavior: `errorBoundFormula(...)` in `latency_monitor.cpp`.

## Data Format and Telemetry
Current collector emits one measurement per line as a numeric value:

```text
ttfb-ms
```

Example:

```text
102.445901
99.553112
110.229044
```

Recommended parser compatibility format (accept both):
- `ttfb-ms`
- `timestamp-ms,ttfb-ms`

Example mixed-compatible lines:

```text
1722623400123,103.88
97.42
```

### C++ parsing example (accept both formats)

```cpp
std::string line;
while (std::getline(std::cin, line)) {
    if (line.empty()) continue;
    std::stringstream ss(line);
    std::string first, second;
    if (std::getline(ss, first, ',') && std::getline(ss, second)) {
        double ttfb = std::stod(second); // timestamp,ttfb
        // use ttfb
    } else {
        double ttfb = std::stod(line);   // ttfb only
        // use ttfb
    }
}
```

### Python parsing example (accept both formats)

```python
for raw in lines:
    raw = raw.strip()
    if not raw:
        continue
    parts = raw.split(",")
    ttfb = float(parts[-1])  # works for "ttfb" and "timestamp,ttfb"
```

## Building for Release
Recommended compiler settings for optimized builds:

- GCC/Clang: `-O3 -DNDEBUG -march=native` (omit `-march=native` for portable binaries)
- MSVC: `/O2 /DNDEBUG`

### Makefile placeholder

```make
CXX := g++
CXXFLAGS := -std=c++17 -O3 -DNDEBUG
TARGET := ttfb_analyzer
SOURCES := server\ latency.cpp latency_monitor.cpp

all:
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)
```

### CMake placeholder

```cmake
cmake_minimum_required(VERSION 3.16)
project(firstTTFBCompared CXX)
set(CMAKE_CXX_STANDARD 17)
add_executable(ttfb_analyzer "server latency.cpp" latency_monitor.cpp)
```

### Packaging notes
- **Windows:** distribute `ttfb_analyzer.exe` with `module1.py` and document Python dependency (`requests`).
- **Linux:** distribute compiled binary plus Python script; consider a shell wrapper to install dependencies and run end-to-end.

## Testing and Validation

### Quick self-test
- Public endpoint smoke test: run `python module1.py 10` and verify mostly numeric lines.
- Localhost test workflow: set `url` in `module1.py` to a local test server and compare against public-host results.

### Sanity checks
- Ensure at least 10 valid samples are collected for analyzer stability.
- Confirm failed lines (`TIMEOUT`, `CONNECTION_ERROR`) are not parsed as numeric.
- Check mean and stddev are positive and within expected range for your network.

### Result validation heuristics
- Re-run the same sample size 2-3 times and compare means.
- Investigate if stddev is unexpectedly near zero (often too few valid samples).
- Verify percentile stays in `[0, 100]` and tail probability in `[0, 1]`.

## Contributing
Contributions are welcome.

- Open an issue with problem statement, environment details, and reproduction steps.
- Submit focused PRs with clear before/after behavior.
- Follow existing coding style and keep changes minimal and testable.
- Ensure CI checks pass for proposed changes.
- Follow GitHub's standard code-of-conduct expectations for respectful collaboration.

## License and Authors
- **License:** MIT (see `/LICENSE`)
- **Author:** Anna Gerges
- **Contributors:** Anna Gerges, Nico (collaborator on the original class project)

## Contact, Acknowledgements, and Related Resources
- **Contact:** Open a GitHub issue in this repository for questions or collaboration.
- **Acknowledgements:** Built as a collaborative learning project combining networking telemetry and calculus-driven analysis.
- **Related resources:**
  - [MDN: Time to First Byte (TTFB)](https://developer.mozilla.org/en-US/docs/Glossary/time_to_first_byte)
  - [Simpson's Rule (overview)](https://en.wikipedia.org/wiki/Simpson%27s_rule)
