#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <string>

using namespace std;

float calculateMedian(vector<int> data) {
    int size = data.size();
    if (size % 2 == 1) {
        return data[size / 2];
    }
    else {
        return (data[(size / 2) - 1] + data[size / 2]) / 2.0;
    }
}

float calculateQuartile1(vector<int> full_dataset) {
    int n = full_dataset.size();
    vector<int> lower_half;

    for (int i = 0; i < n / 2; i++) {
        lower_half.push_back(full_dataset[i]);
    }
    return calculateMedian(lower_half);
}


float calculateQuartile3(vector<int> full_dataset) {
    int n = full_dataset.size();
    vector<int> upper_half;
    int start_index = (n % 2 == 0) ? (n / 2) : (n / 2 + 1);
    for (int i = start_index; i < n; i++) {
        upper_half.push_back(full_dataset[i]);
    }
    return calculateMedian(upper_half);
}

int main() {

    ifstream inputFile("exp5_input.csv");
    if (!inputFile.is_open()) {
        cout << "Error: Could not open the input file." << endl;
        return 1;
    }

    ofstream outputFile("exp5_output.csv");

    string line, value;
    vector<int> dataset;
    int line_count = 0;

    while (getline(inputFile, line)) {
        if (line_count == 0) {
            line_count++;
            continue;
        }

        stringstream str(line);
        getline(str, value, ',');
        dataset.push_back(stoi(value));
    }
    sort(dataset.begin(), dataset.end());
    int n = dataset.size();

    int minValue = dataset[0];
    float q1 = calculateQuartile1(dataset);
    float medianVal = calculateMedian(dataset);
    float q3 = calculateQuartile3(dataset);
    int maxValue = dataset[n - 1];

    outputFile << "Statistic,Value\n";
    outputFile << "Minimum," << minValue << "\n";
    outputFile << "Quartile 1 (Q1)," << q1 << "\n";
    outputFile << "Median (Q2)," << medianVal << "\n";
    outputFile << "Quartile 3 (Q3)," << q3 << "\n";
    outputFile << "Maximum," << maxValue << "\n";

    cout << "Five-Number Summary Calculation : " << endl;
    cout << "-----------------------------------------" << endl;
    cout << "Minimum: " << minValue << endl;
    cout << "Q1: " << q1 << endl;
    cout << "Median: " << medianVal << endl;
    cout << "Q3: " << q3 << endl;
    cout << "Maximum: " << maxValue << endl;
    cout << "\nResults have been saved to exp4_output.csv" << endl;

    inputFile.close();
    outputFile.close();
    return 0;
}
