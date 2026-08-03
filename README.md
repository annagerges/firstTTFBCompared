# Server TTFB Performance Analyzer

[![License: MIT](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)
[![Build Status](https://img.shields.io/badge/build-status%20placeholder-lightgrey.svg)](https://github.com/annagerges/firstTTFBCompared/actions)
[![Language](https://img.shields.io/badge/language-C%2B%2B17%20%7C%20Python%203-blue.svg)](#project-overview)
[![Repo Size](https://img.shields.io/github/repo-size/annagerges/firstTTFBCompared)](https://github.com/annagerges/firstTTFBCompared)

Measure server responsiveness end-to-end and model latency behavior with a Python TTFB collector and a C++ statistical analyzer.

## Changelog
- **2026-08-03**: Upgraded README to a flagship-quality project overview with full setup, architecture, workflows, telemetry format, validation guidance, and contribution details.

## Table of Contents
- [Demo / Screenshot](#demo--screenshot)
- [Project Overview](#project-overview)
- [Key Features](#key-features)
- [Quick Start](#quick-start)
- [Usage Examples & Common Workflows](#usage-examples--common-workflows)
- [Architecture and Design](#architecture-and-design)
- [Data Format and Telemetry](#data-format-and-telemetry)
- [Building for Release](#building-for-release)
- [Testing & Validation](#testing--validation)
- [Contributing](#contributing)
- [License and Authors](#license-and-authors)
- [Contact, Acknowledgements, and Related Resources](#contact-acknowledgements-and-related-resources)

## Demo / Screenshot
![Demo placeholder](https://via.placeholder.com/1200x420.png?text=Server+TTFB+Performance+Analyzer+Demo)

To replace this placeholder:
1. Add your image or GIF under `assets/` (for example: `assets/demo.png`).
2. Update the markdown link above to point to the local file.
3. Commit the asset with your README update.

## Project Overview
This collaborative project explores how low-level network timing can be combined with practical numerical methods for performance analysis. It measures **Time To First Byte (TTFB)** repeatedly, then analyzes the resulting distribution using mean, standard deviation, and Simpson's Rule integration.

**What is TTFB?**  
TTFB is the elapsed time between sending a request and receiving the first byte of response data. It matters because it reflects server responsiveness, network path quality, and overall request startup cost. Tracking TTFB helps identify latency spikes, backend bottlenecks, and stability regressions.

The repository currently includes:
- A Python collector (`module1.py`) that emits one latency value per line.
- A C++ analyzer (`server latency.cpp` + `latency_monitor.cpp`) that reads those values, computes summary stats, and estimates tail probability.

## Key Features
- Stream-based TTFB capture in Python with request timeout handling.
- C++ pipeline integration via process pipes (`_popen` / `_pclose` in current implementation).
- Statistical summary (mean, standard deviation, observed max, z-score).
- Simpson's Rule integration with error-bound-based step sizing.
- Lightweight design with no heavy scientific dependency stack.

## Quick Start
### Prerequisites
- Python **3.8+**
- C++17 compiler:
  - Windows: **MSVC** (`cl`) or **MinGW/GCC** (`g++`)
  - Linux/macOS: **g++**
- Python dependency: `requests`

```bash
python -m pip install requests
```

### Build the C++ analyzer
> Note: the file name includes a space, so keep quotes around `"server latency.cpp"`.

#### Linux / macOS (g++)
```bash
g++ -std=c++17 -O2 "server latency.cpp" latency_monitor.cpp -o ttfb_analyzer
```

#### Windows (MSVC)
```powershell
cl /std:c++17 /EHsc /O2 "server latency.cpp" latency_monitor.cpp /Fe:ttfb_analyzer.exe
```

#### Windows (MinGW / GCC)
```powershell
g++ -std=c++17 -O2 "server latency.cpp" latency_monitor.cpp -o ttfb_analyzer.exe
```

### Run collector only (Python-only telemetry collection)
```bash
python module1.py 25
```

### Run analyzer (which internally launches the Python collector)
```bash
./ttfb_analyzer
```

### Pipe collector output into analyzer (workflow example)
The current C++ executable launches Python internally, but if you add/read from `stdin` in your analyzer variant, this is the intended IPC shape:
```bash
python -u module1.py 100 | ./ttfb_analyzer
```

### Example console output
```text
Collecting 10 latency measurements...
Running command: python "module1.py" 10
Python output:
Data point 1: 34.932114
  -> Parsed as: 34.9321
...
Successfully collected 10 data points.
Observed Latency: 54.726841 ms
Calculated Z-score: 1.287194
Probability (Area under tail): 0.098245
Percentile: 90.175500
```

### Sample parsed CSV-like lines
```text
34.932114
31.402771
49.110824
```

## Usage Examples & Common Workflows
### Single run
```bash
python module1.py 10
```

### Batch run (more samples)
```bash
python module1.py 500
```

### Redirect collector output to file
```bash
python -u module1.py 250 > ttfb_raw.txt
```

### Redirect analyzer output to file
```bash
./ttfb_analyzer > analysis.txt
```

### Changing sample count (number of pings)
- Current collector argument: positional count (`module1.py <count>`)
- Current analyzer prompt: enter measurement count interactively when asked.

### Timeout behavior
- Current collector uses `timeout=10` seconds in `module1.py`.
- To change it, edit the `session.get(..., timeout=10)` value in the script.

## Architecture and Design
```text
Python collector (module1.py)
  -> emits latency samples line-by-line
  -> IPC via process pipe
C++ analyzer (latency_monitor.cpp + server latency.cpp)
  -> parses numeric samples
  -> computes mean/stddev/z-score
  -> integrates bell-curve tail using Simpson's Rule
```

### Simpson's Rule and error control
- Integration is applied over a tail interval `[observed_x, mean + 5*std_dev]`.
- Step count `n` is selected by `errorBoundFormula(...)`.
- `n` is forced to be even (required by Simpson's Rule).
- To tune precision, adjust the tolerance constant in `errorBoundFormula` in `latency_monitor.cpp`.

### Where to change parameters
- Sample spacing delay: `time.sleep(0.2005)` in `module1.py`
- Request timeout: `timeout=10` in `module1.py`
- Integration bound / tolerance: `errorBoundFormula(...)` and interval logic in C++ files

## Data Format and Telemetry
The collector currently emits **one data point per line** as floating-point milliseconds:
```text
42.118236
37.991004
```

For future compatibility, treat both of these as valid input formats:
1. `ttfb-ms`
2. `timestamp-ms,ttfb-ms`

Examples:
```text
42.118236
1722555200123,42.118236
```

### Parsing examples
#### C++ (support both formats)
```cpp
std::string line;
if (std::getline(std::cin, line)) {
    auto comma = line.find(',');
    double ttfb = (comma == std::string::npos)
        ? std::stod(line)
        : std::stod(line.substr(comma + 1));
}
```

#### Python (support both formats)
```python
line = line.strip()
ttfb = float(line.split(",")[-1])  # works for "ttfb" and "timestamp,ttfb"
```

## Building for Release
Recommended compiler flags:
- `-O3 -DNDEBUG` for optimized release binaries
- `-Wall -Wextra -Wpedantic` for stricter diagnostics during development

Example release build (Linux/macOS GCC):
```bash
g++ -std=c++17 -O3 -DNDEBUG -Wall -Wextra -Wpedantic "server latency.cpp" latency_monitor.cpp -o ttfb_analyzer
```

Makefile/CMake placeholder workflow:
- Add a `Makefile` or `CMakeLists.txt` with Debug/Release targets.
- Keep Python collector as a runtime dependency (not compiled into C++).

Packaging notes:
- **Windows**: ship `ttfb_analyzer.exe`, `module1.py`, and dependency instructions (`pip install requests`).
- **Linux/macOS**: ship `ttfb_analyzer`, `module1.py`, and the same Python dependency instructions.

## Testing & Validation
Quick self-test options:
1. Public target (current default in script): `https://www.google.com`
2. Localhost test server (recommended for deterministic checks):
   ```bash
   python -m http.server 8000
   ```
   Then point collector URL to `http://127.0.0.1:8000`.

Recommended sanity checks:
- Ensure at least 10 valid numeric samples are collected.
- Confirm mean and standard deviation are finite and non-negative.
- Verify percentile output remains within `[0, 100]`.
- Compare two runs to check trend stability (large spikes should be explainable).

Validation idea for result quality:
- Run multiple batches (e.g., 3x100).
- Compare mean/stddev deltas across batches.
- Investigate outliers if one batch diverges significantly.

## Contributing
Contributions are welcome.

1. Open an issue describing the bug, improvement, or proposal.
2. Submit a focused pull request with clear before/after behavior.
3. Follow existing code style in Python and C++.
4. Ensure local build and sanity checks pass before requesting review.
5. If CI is enabled, keep checks green before merge.

For community conduct expectations, follow standard GitHub community guidelines and add a `CODE_OF_CONDUCT.md` in future iterations if needed.

## License and Authors
- **License:** [MIT](LICENSE)
- **Authors:** Anna Gerges and contributors
- **Contributors:** Please add yourself in PR descriptions and commit history

## Contact, Acknowledgements, and Related Resources
- **Contact:** Open a GitHub issue in this repository for questions or feedback.
- **Acknowledgements:** Built as a collaborative learning project connecting networking, systems programming, and applied calculus.
- **Related resources:**
  - MDN: Performance fundamentals and networking concepts
  - RFC references for HTTP behavior
  - Numerical methods references for Simpson's Rule and normal distribution
