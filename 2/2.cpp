#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include <limits>
#include <algorithm>
#include <unordered_set>

using namespace std;

// Split a string using a delimiter
vector<string> split(const string& str, char delimiter) {
    vector<string> tokens;
    stringstream ss(str);
    string item;
    while (getline(ss, item, delimiter)) {
        tokens.push_back(item);
    }
    return tokens;
}

// Write output to file
void saveToCSV(const vector<vector<string>>& data, const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Could not write to file: " << filename << endl;
        return;
    }

    for (const auto& row : data) {
        for (size_t i = 0; i < row.size(); ++i) {
            file << row[i];
            if (i < row.size() - 1) file << ",";
        }
        file << "\n";
    }

    file.close();
}

// Normalize a column by min-max
vector<double> normalizeMinMax(const vector<double>& values) {
    double minVal = numeric_limits<double>::max();
    double maxVal = numeric_limits<double>::lowest();
    for (double v : values) {
        if (!isnan(v)) {
            minVal = min(minVal, v);
            maxVal = max(maxVal, v);
        }
    }
    vector<double> result;
    for (double v : values) {
        if (isnan(v)) result.push_back(numeric_limits<double>::quiet_NaN());
        else result.push_back((maxVal - minVal == 0) ? 0 : (v - minVal) / (maxVal - minVal));
    }
    return result;
}

// Normalize a column by Z-score
vector<double> normalizeZScore(const vector<double>& values) {
    double sum = 0;
    int count = 0;
    for (double v : values) {
        if (!isnan(v)) {
            sum += v;
            count++;
        }
    }
    double mean = sum / count;
    double sqDiff = 0;
    for (double v : values) {
        if (!isnan(v)) sqDiff += pow(v - mean, 2);
    }
    double stddev = sqrt(sqDiff / count);
    vector<double> result;
    for (double v : values) {
        if (isnan(v)) result.push_back(numeric_limits<double>::quiet_NaN());
        else result.push_back((stddev == 0) ? 0 : (v - mean) / stddev);
    }
    return result;
}

// Normalize a column by decimal scaling
vector<double> normalizeDecimal(const vector<double>& values) {
    double maxAbs = 0;
    for (double v : values) {
        if (!isnan(v)) maxAbs = max(maxAbs, fabs(v));
    }
    int k = (maxAbs == 0) ? 0 : static_cast<int>(ceil(log10(maxAbs)));
    double divisor = pow(10, k);
    vector<double> result;
    for (double v : values) {
        if (isnan(v)) result.push_back(numeric_limits<double>::quiet_NaN());
        else result.push_back(v / divisor);
    }
    return result;
}

int main() {
    cout << "--- Dataset Normalization Tool ---\n";
    cout << "Enter input file name: ";
    string filePath;
    getline(cin, filePath);
    
    ifstream file(filePath);
    if (!file.is_open()) {
        cerr << "Failed to open file: " << filePath << endl;
        return 1;
    }

    cout << "Enter column names to normalize (comma-separated): ";
    string input;
    getline(cin, input);

    vector<string> targetCols = split(input, ',');
    for (auto& col : targetCols) col.erase(remove_if(col.begin(), col.end(), ::isspace), col.end());

    string headerLine;
    getline(file, headerLine);
    vector<string> headers = split(headerLine, ',');

    unordered_set<int> targetIndices;
    for (const string& col : targetCols) {
        auto it = find(headers.begin(), headers.end(), col);
        if (it != headers.end()) {
            targetIndices.insert(distance(headers.begin(), it));
        } else {
            cerr << "Column not found: " << col << endl;
        }
    }

    vector<vector<string>> rawData = {headers};
    vector<vector<double>> columnData(headers.size());

    string rowLine;
    while (getline(file, rowLine)) {
        vector<string> row = split(rowLine, ',');
        rawData.push_back(row);
        for (size_t i = 0; i < row.size(); ++i) {
            try {
                columnData[i].push_back(stod(row[i]));
            } catch (...) {
                columnData[i].push_back(numeric_limits<double>::quiet_NaN());
            }
        }
    }
    file.close();

    // Process each method
    vector<string> methods = {"MinMax", "ZScore", "Decimal"};
    for (const string& method : methods) {
        vector<vector<string>> result;
        
        // Create header row with only normalized columns
        vector<string> headerRow;
        for (int colIdx : targetIndices) {
            headerRow.push_back(headers[colIdx] + " [" + method + "]");
        }
        result.push_back(headerRow);
        
        // Create data rows with only normalized columns
        vector<vector<double>> normalizedColumns;
        for (int colIdx : targetIndices) {
            vector<double> normalized;
            if (method == "MinMax") normalized = normalizeMinMax(columnData[colIdx]);
            else if (method == "ZScore") normalized = normalizeZScore(columnData[colIdx]);
            else normalized = normalizeDecimal(columnData[colIdx]);
            normalizedColumns.push_back(normalized);
        }
        
        // Add data rows
        for (size_t i = 0; i < columnData[0].size(); ++i) {
            vector<string> dataRow;
            for (size_t j = 0; j < normalizedColumns.size(); ++j) {
                char buf[32];
                if (!isnan(normalizedColumns[j][i]))
                    snprintf(buf, sizeof(buf), "%.6f", normalizedColumns[j][i]);
                else
                    snprintf(buf, sizeof(buf), "");
                dataRow.push_back(buf);
            }
            result.push_back(dataRow);
        }

        string outFile = "iris_normalized_" + method + ".csv";
        saveToCSV(result, outFile);
        cout << "Saved: " << outFile << "\n";
    }

    cout << "All selected columns normalized successfully.\n";
    return 0;
}
