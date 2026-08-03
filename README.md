# firstTTFBCompared

A lightweight collaborative latency-analysis project that measures Time To First Byte (TTFB) with Python and analyzes tail-risk probability in C++ using Simpson's Rule.

## Demo / Screenshot
![Latency Analysis Output](image.png)

## Project Overview
This project started as a collaborative exploration of networking telemetry using math.(I plan to independently benchmark and validate as well as export csv, etc.) It collects TTFB samples from repeated HTTP requests and then models latency behavior with a normal-distribution approximation.

**What is TTFB?**
Time To First Byte (TTFB) is the time from request start until the first response byte arrives. It is a indicator of network and server responsiveness because it captures connection setup, routing, and initial backend latency.

**Why it matters:**
- Helps detect changes in perceived responsiveness.
- Supports data-driven comparisons across runs and environments.

The C++ side computes summary statistics (mean and standard deviation) and estimates tail probability using Simpson's Rule with a dynamic error-bound step calculation. Python is used for data collection.

## Key Features
- Accurate first-byte timing collection via Python `requests` with streaming enabled.
- Timeout/connection-error handling.
- Cross-language workflow: Python collector output used by a C++ analyzer.
- Mean and standard deviation calculation from acquired dataset.
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
Common workflow to edit those values directly for experiments.

## Architecture and Design

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


### Packaging notes
- **Windows:** distribute `ttfb_analyzer.exe` with `module1.py` and document Python dependency (`requests`).
- **Linux:** distribute compiled binary plus Python script; consider a shell wrapper to install dependencies and run end-to-end.

## Testing and Validation

### Quick self-test
- Public endpoint smoke test: run `python module1.py 10` and verify mostly numeric lines.
- Localhost test workflow: set `url` in `module1.py` to a local test server and compare against public-host results.

###Extra tests
- Ensure at least 10 valid samples are collected for analyzer stability.
- Confirm failed lines (`TIMEOUT`, `CONNECTION_ERROR`) are not parsed as numeric.
- Check mean and stddev are positive and within expected range for your network.

### Result validation heuristics
- Re-run the same sample size 2-3 times and compare means.
- Investigate if stddev is unexpectedly near zero (often too few valid samples).
- Verify percentile stays in `[0, 100]` and tail probability in `[0, 1]`.


## License and Authors
- **License:** MIT
- **Author:** Anna Gerges
- **Contributors:** Anna Gerges, Nico (collaborator on the original class project)
