#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <iomanip>

using namespace std;

int cube[3][3][3];

vector<string> time_dim = {"Q1", "Q2", "Q3"};
vector<string> item_dim = {"Milk", "Egg", "Bread"};
vector<string> location_dim = {"Kolkata", "Delhi", "Mumbai"};

map<string, int> time_map;
map<string, int> item_map;
map<string, int> location_map;

map<string, vector<string>> time_hierarchy_drill_down = {
    {"Q1", {"Jan", "Feb", "Mar"}},
    {"Q2", {"Apr", "May", "Jun"}},
    {"Q3", {"Jul", "Aug", "Sep"}}
};

void initializeMaps();
void loadFromCSV(const string& filename);
void displayCube();
void rollUp();
void drillDown();
void slice();
void dice();
void printSlice(const vector<vector<int>>& slice_data, const string& dim1_name, const vector<string>& dim1_labels, const string& dim2_name, const vector<string>& dim2_labels);

int main() {
    initializeMaps();

    string filename;
    cout << "Enter the name of the CSV file (e.g., data.csv): ";
    cin >> filename;

    loadFromCSV(filename);

    int choice;
    do {
        cout << "\n--- OLAP Operations Menu ---" << endl;
        cout << "1. Display 3D Cube" << endl;
        cout << "2. Roll-up" << endl;
        cout << "3. Drill-down" << endl;
        cout << "4. Slice" << endl;
        cout << "5. Dice" << endl;
        cout << "0. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                displayCube();
                break;
            case 2:
                rollUp();
                break;
            case 3:
                drillDown();
                break;
            case 4:
                slice();
                break;
            case 5:
                dice();
                break;
            case 0:
                cout << "Exiting program." << endl;
                break;
            default:
                cout << "Invalid choice. Please try again." << endl;
        }
    } while (choice != 0);

    return 0;
}

void initializeMaps() {
    for (int i = 0; i < 3; ++i) {
        time_map[time_dim[i]] = i;
        item_map[item_dim[i]] = i;
        location_map[location_dim[i]] = i;
    }
}

void loadFromCSV(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open file '" << filename << "'. Please make sure the file exists in the same directory." << endl;
        exit(1); 
    }

    string line;
    getline(file, line);

    while (getline(file, line)) {
        stringstream ss(line);
        string time_val, item_val, location_val, sales_val;

        getline(ss, time_val, ',');
        getline(ss, item_val, ',');
        getline(ss, location_val, ',');
        getline(ss, sales_val, ',');

        if (time_map.count(time_val) && item_map.count(item_val) && location_map.count(location_val)) {
            int time_idx = time_map[time_val];
            int item_idx = item_map[item_val];
            int loc_idx = location_map[location_val];
            cube[time_idx][item_idx][loc_idx] = stoi(sales_val);
        }
    }
    cout << "Successfully loaded data from " << filename << endl;
    file.close();
}

void displayCube() {
    cout << "\n--- 3D Data Cube ---" << endl;
    cout << "------------------------------------------------------------------" << endl;
    cout << "|        |";
    for (const auto& loc : location_dim) {
        cout << setw(20) << "Location=\"" + loc + "\" |";
    }
    cout << endl;

    cout << "| Time   |";
    for (size_t i = 0; i < location_dim.size(); ++i) {
        for (const auto& item : item_dim) {
            cout << setw(6) << item << " |";
        }
    }
    cout << "\b\b |" << endl;
    cout << "------------------------------------------------------------------" << endl;

    for (int t = 0; t < 3; ++t) {
        cout << "| " << setw(6) << left << time_dim[t] << " |";
        for (int l = 0; l < 3; ++l) {
            for (int i = 0; i < 3; ++i) {
                cout << setw(6) << right << cube[t][i][l] << " |";
            }
        }
        cout << endl;
    }
    cout << "------------------------------------------------------------------" << endl;
}

void rollUp() {
    int choice;
    cout << "\n--- Roll-up Operation ---" << endl;
    cout << "Select dimension to roll up:" << endl;
    cout << "1. Location (Cities to Country)" << endl;
    cout << "2. Time (Quarters to Year)" << endl;
    cout << "Enter choice: ";
    cin >> choice;

    if (choice == 1) {
        cout << "\n-- Roll-up on Location (Cities -> India) --" << endl;
        vector<vector<int>> rolled_up_data(3, vector<int>(3, 0));
        for (int t = 0; t < 3; ++t) {
            for (int i = 0; i < 3; ++i) {
                for (int l = 0; l < 3; ++l) {
                    rolled_up_data[t][i] += cube[t][i][l];
                }
            }
        }
        vector<string> country_dim = {"India"};
        printSlice(rolled_up_data, "Time", time_dim, "Item", item_dim);

    } else if (choice == 2) {
         cout << "\n-- Roll-up on Time (Quarters -> Year) --" << endl;
        vector<vector<int>> rolled_up_data(3, vector<int>(3, 0));
        for (int l = 0; l < 3; ++l) {
            for (int i = 0; i < 3; ++i) {
                for (int t = 0; t < 3; ++t) {
                    rolled_up_data[l][i] += cube[t][i][l];
                }
            }
        }
        printSlice(rolled_up_data, "Location", location_dim, "Item", item_dim);
    } else {
        cout << "Invalid choice." << endl;
    }
}

void drillDown() {
    cout << "\n--- Drill-down Operation ---" << endl;
    cout << "Drill down on Time (from Quarters to Months)" << endl;
    string quarter;
    cout << "Enter Quarter (Q1, Q2, Q3): ";
    cin >> quarter;

    if (time_hierarchy_drill_down.count(quarter)) {
        int time_idx = time_map[quarter];
        cout << "\n-- Drill-down for " << quarter << " --" << endl;

        vector<vector<int>> quarter_slice(3, vector<int>(3));
        for(int l=0; l<3; ++l){
            for(int i=0; i<3; ++i){
                quarter_slice[l][i] = cube[time_idx][i][l];
            }
        }
        
        cout << "Monthly breakdown for " << quarter << ":" << endl;
        vector<string> months = time_hierarchy_drill_down[quarter];
        
        cout << setw(10) << "Location" << " |";
        for(const auto& item : item_dim) {
            cout << setw(8) << item;
        }
        cout << endl << "------------------------------------" << endl;

        for (int l = 0; l < 3; ++l) {
            cout << setw(10) << location_dim[l] << " |";
            for (int i = 0; i < 3; ++i) {
                cout << setw(8) << fixed << setprecision(2) << (quarter_slice[l][i] / 3.0);
            }
            cout << endl;
        }
         cout << "(Note: Monthly values are the quarterly total divided by 3 for demonstration)" << endl;

    } else {
        cout << "Invalid Quarter selected." << endl;
    }
}

void slice() {
    cout << "\n--- Slice Operation ---" << endl;
    string time_val;
    cout << "Enter Time for slice (e.g., Q1): ";
    cin >> time_val;

    if (time_map.count(time_val)) {
        int time_idx = time_map[time_val];
        cout << "\n-- Slice for Time = " << time_val << " --" << endl;
        vector<vector<int>> slice_data(3, vector<int>(3));
        for (int l = 0; l < 3; ++l) {
            for (int i = 0; i < 3; ++i) {
                slice_data[l][i] = cube[time_idx][i][l];
            }
        }
        printSlice(slice_data, "Location", location_dim, "Item", item_dim);
    } else {
        cout << "Invalid Time value." << endl;
    }
}

void dice() {
    cout << "\n--- Dice Operation ---" << endl;
    cout << "Select ranges for dimensions to create a sub-cube." << endl;
    
    string time_val, item_val, loc_val;
    cout << "Enter Time (Q1, Q2, or Q3): ";
    cin >> time_val;
    cout << "Enter Item (Milk, Egg, or Bread): ";
    cin >> item_val;
    cout << "Enter Location (Kolkata, Delhi, or Mumbai): ";
    cin >> loc_val;

    if (time_map.count(time_val) && item_map.count(item_val) && location_map.count(loc_val)) {
        int time_idx = time_map[time_val];
        int item_idx = item_map[item_val];
        int loc_idx = location_map[loc_val];

        cout << "\n-- Dice Result --" << endl;
        cout << "Value for (Time=" << time_val << ", Item=" << item_val << ", Location=" << loc_val << "): "
             << cube[time_idx][item_idx][loc_idx] << endl;

        cout << "\n-- Dice for Location = " << loc_val << " and Time = " << time_val << " --" << endl;
        vector<vector<int>> dice_data(1, vector<int>(3));
        vector<string> single_loc_dim = {loc_val};
        
        for(int i=0; i<3; ++i){
            dice_data[0][i] = cube[time_idx][i][loc_idx];
        }
        printSlice(dice_data, "Location", single_loc_dim, "Item", item_dim);

    } else {
        cout << "Invalid dimension value entered." << endl;
    }
}

void printSlice(const vector<vector<int>>& slice_data, const string& dim1_name, const vector<string>& dim1_labels, const string& dim2_name, const vector<string>& dim2_labels) {
    cout << setw(12) << left << dim1_name << "|";
    for (const auto& label : dim2_labels) {
        cout << setw(8) << right << label << " |";
    }
    cout << endl;

    cout << string(12, '-') << "+";
    for (size_t i = 0; i < dim2_labels.size(); ++i) {
        cout << string(10, '-') << "+";
    }
    cout << endl;

    for (size_t i = 0; i < dim1_labels.size(); ++i) {
        cout << setw(12) << left << dim1_labels[i] << "|";
        for (size_t j = 0; j < dim2_labels.size(); ++j) {
            cout << setw(8) << right << slice_data[i][j] << " |";
        }
        cout << endl;
    }
}
