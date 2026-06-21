# TTFB Latency Analysis Tool

## Overview
A cross-platform tool that measures Time To First Byte (TTFB) latency for HTTP requests and uses 
numerical integration (Simpson's Rule) to estimate the probability of future latencies. Built as an 
honors collaboration, this project combines network measurement with Calculus II to answer a practical 
question: *Given observed latency data, what's the probability the next request will be fast?*

## The Research Question

When you make an HTTP request, latency varies. But how much? And can we predict it?

We collected TTFB samples from repeated requests, fit a normal distribution to the data, then used 
Simpson's Rule to integrate under the curve. This let us compute: *What's the probability that the 
next request's TTFB falls within a specific range (e.g., 100–200ms)?*

It's a bridge between math (numerical integration) and systems thinking (network behavior).

## What It Does

- Sends repeated HTTP requests to a configurable target URL
- Measures Time To First Byte (latency in milliseconds) for each request
- Collects minimum 10 data points per run
- Fits a normal distribution to observed latency
- Uses Simpson's Rule to integrate the probability density function
- Outputs probability estimates for user-defined latency ranges
- Displays summary statistics (first ttfb, z-score, probability, percentile)

## How It Works

1. **Data Collection (C++)**: Initiates network requests and captures TTFB values
2. **Data Pipeline**: Passes raw latency data to Python for analysis
3. **Statistical Modeling (Python)**: Fits a normal distribution to the samples
4. **Numerical Integration**: Applies Simpson's Rule to compute P(a ≤ TTFB ≤ b) and calculates std deviation
6. **Output**: Returns probability estimates and visualization data

## Technologies Used

- **C++**: Statistical modeling, numerical integration, visualization
- **Python**: Network request handling and data collection
- **Requests library**: HTTP client for reliable latency measurement

## What I Learned

- How to design a data pipeline between compiled and interpreted languages
- How to use python requests and sys library
- Cross-platform development: managing language runtimes and environment setup
- Collaborative development: coordinating code ownership with Nico and integrating contributions

## Platform Support

Currently supports:
- **Windows** (PowerShell)
- **macOS** (Bash) — in progress
- 
## How to Run

### Prerequisites
- C++ compiler (g++, clang, or MSVC)
- Python 3.7+
- Install Python dependencies: `pip install requests `

### Windows
```bash
g++ ttfb\_collector.cpp -o ttfb\_collector.exe
./ttfb\_collector.exe

# Prompts for target URL and number of samples
# Automatically calls Python for analysis

