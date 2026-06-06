#ifndef LATENCY_MONITOR_H
#define LATENCY_MONITOR_H

#include <vector>
#include <string>

// Function prototypes
int errorBoundFormula(double, double, double);
double bellCurveFunction(double, double, double);
void dataMunch(const std::vector<double>& data, double& mean, double& std_dev);
std::vector<double> collectLatencyData(int count);

#endif