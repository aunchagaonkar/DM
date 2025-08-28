#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <limits>
using namespace std;

struct Entry {
    string patient;
    string stage;
    string round;
    int bp, sugar, cholesterol, index;
};

void show(const Entry& e) {
    cout << e.patient << "\t" << e.stage << "\t" << e.round << "\t"
         << e.bp << "\t" << e.sugar << "\t" << e.cholesterol << "\t" << e.index << "\n";
}

bool importCSV(const string& path, vector<Entry>& records) {
    ifstream fin(path);
    if (!fin) return false;
    string row;
    getline(fin, row);
    while (getline(fin, row)) {
        stringstream ss(row);
        Entry e;
        string field;
        getline(ss, e.patient, ',');
        getline(ss, e.stage, ',');
        getline(ss, e.round, ',');
        getline(ss, field, ','); e.bp = stoi(field);
        getline(ss, field, ','); e.sugar = stoi(field);
        getline(ss, field, ','); e.cholesterol = stoi(field);
        getline(ss, field, ','); e.index = stoi(field);
        records.push_back(e);
    }
    return true;
}

void performRollUp(const vector<Entry>& records, const string& d1, const string& d2 = "") {
    unordered_map<string, int> summary;
    for (const auto& e : records) {
        string key = (d1 == "Stage") ? e.stage : (d1 == "Round") ? e.round : e.patient;
        if (!d2.empty()) key += " | " + ((d2 == "Stage") ? e.stage : (d2 == "Round") ? e.round : e.patient);
        summary[key] += e.index;
    }
    for (const auto& pair : summary)
        cout << pair.first << " -> Wellness Index Sum: " << pair.second << "\n";
}

void drillDetails(const vector<Entry>& records, const string& dim, const string& val) {
    for (const auto& e : records) {
        if ((dim == "Stage" && e.stage == val) ||
            (dim == "Round" && e.round == val) ||
            (dim == "Patient" && e.patient == val)) {
            show(e);
        }
    }
}

void sliceData(const vector<Entry>& records, const string& dim, const string& val) {
    drillDetails(records, dim, val);
}

void diceData(const vector<Entry>& records, const string& d1, const string& v1, const string& d2, const string& v2) {
    for (const auto& e : records) {
        bool match1 = (d1 == "Stage" && e.stage == v1) || (d1 == "Round" && e.round == v1) || (d1 == "Patient" && e.patient == v1);
        bool match2 = (d2 == "Stage" && e.stage == v2) || (d2 == "Round" && e.round == v2) || (d2 == "Patient" && e.patient == v2);
        if (match1 && match2) show(e);
    }
}

int main() {
    string file;
    cout << "Enter input CSV file: ";
    getline(cin, file);
    vector<Entry> data;
    if (!importCSV(file, data)) {
        cerr << "Error loading file!\n";
        return 1;
    }
    cout << "Data loaded successfully.\nCommands: rollup <dim1> [dim2], drilldown <dim> <value>, slice <dim> <value>, dice <dim1> <val1> <dim2> <val2>, exit\n";

    while (true) {
        cout << "\nEnter command: ";
        string cmd, a, b, c, d;
        cin >> cmd;
        if (cmd == "rollup") {
            cin >> a;
            if (cin.peek() != '\n' && cin.peek() != EOF) cin >> b;
            performRollUp(data, a, b);
        } else if (cmd == "drilldown") {
            cin >> a >> b;
            drillDetails(data, a, b);
        } else if (cmd == "slice") {
            cin >> a >> b;
            sliceData(data, a, b);
        } else if (cmd == "dice") {
            cin >> a >> b >> c >> d;
            diceData(data, a, b, c, d);
        } else if (cmd == "exit") {
            break;
        } else {
            cout << "Unknown command. Available commands:\n";
            cout << "- rollup <dim1> [dim2]\n";
            cout << "- drilldown <dim> <value>\n";
            cout << "- slice <dim> <value>\n";
            cout << "- dice <dim1> <val1> <dim2> <val2>\n";
            cout << "- exit\n";
            cout << "Dimensions: Stage, Round, Patient\n";
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    return 0;
}
