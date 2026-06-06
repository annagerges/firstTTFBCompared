#include <iostream>
#include <iomanip>
#include<vector>
#include "latency_monitor.h"
using namespace std;

int main() {
    // Project data
    int num_measurements;

    cout << "Enter amount of data points would you like to collect (minimum: 10): ";
    cin >> num_measurements;

    while (num_measurements < 10) {
        cout << "Enter at least 10 data points.";
        cout << "Enter amount of data points would you like to collect (minimum: 10): ";
        cin >> num_measurements;
    }

    // Collect latency data from the Python script
    cout << "Collecting " << num_measurements << " latency measurements..." << endl;
    vector<double> latency_data = collectLatencyData(num_measurements);

    //if no data was collected, exit the program with an error message
    if (latency_data.empty()) {
        cerr << "Error: No latency data collected. Make sure Python script is accessible." << endl;
        return 1;
    }

    // Calculate mean and std_dev from collected data
    double mean, std_dev;
    dataMunch(latency_data, mean, std_dev);

    // Select observed value (highest latency in dataset)
    double observed_x = *max_element(latency_data.begin(), latency_data.end());

    // Integrate to "infinity" (5 standard deviations)
    double a = observed_x;
    double b = mean + 5 * std_dev;

    // n must be an even number for Simpson's Rule
    int n = errorBoundFormula(a, b, std_dev);
    double h = (b - a) / static_cast<double>(n); // This is Delta X / 3

    // Start with adding the first and last values of Simpson's rule
    double sum = bellCurveFunction(a, mean, std_dev) + bellCurveFunction(b, mean, std_dev);

    // Add all values within the summation formula to sum
    for (int i = 1; i < n; i++) {
        double current_x = a + i * h;

        // Toggles between 2 and 4
        double coefficient = 3.0 + pow(-1, i - 1);

        sum += coefficient * bellCurveFunction(current_x, mean, std_dev);
    }

    // Final step for Simpson's rule which is to
    // multiply everything by h (delta X) / 3
    double probability = (h / 3.0) * sum;

    // Outputs the results
    double z_score = (observed_x - mean) / std_dev;

    cout << fixed << setprecision(6);
    cout << "--- Google Latency Analysis Project ---" << endl;
    cout << "Observed Latency: " << observed_x << " ms" << endl;
    cout << "Calculated Z-score: " << z_score << endl;
    cout << "Probability (Area under tail): " << probability << endl;
    cout << "Percentile: " << (1.0 - probability) * 100 << endl;

    return 0;
}