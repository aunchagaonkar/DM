#include <bits/stdc++.h>
#include <fstream>
using namespace std;

int main()
{
    cout << "Enter input file name: ";
    string fileName;
    getline(cin, fileName);
    
    fstream file(fileName, ios::in);
    if (!file.is_open())
    {
        cout << "Couldn't open file: " << fileName << endl;
        return 0;
    }

    string line, country, company, countStr;
    int count;
    map<string, map<string, int>> dataMap; // Country -> (Company -> Count)
    map<string, int> companyTotal;
    map<string, int> countryTotal;
    int lineNo = 0;

    // Read and parse input CSV
    while (getline(file, line))
    {
        stringstream str(line);
        if (lineNo++ == 0) continue;
        getline(str, country, ',');
        getline(str, company, ',');
        getline(str, countStr, ',');
        count = stoi(countStr);

        dataMap[country][company] = count;
        countryTotal[country] += count;
        companyTotal[company] += count;
    }

    file.close();

    // Calculate overall total
    int grandTotal = 0;
    for (auto& ct : countryTotal)
        grandTotal += ct.second;

    // Create output file
    ofstream fw("exp4_output.csv", ios::out);
    fw << "Country,";
    for (auto& c : companyTotal)
        fw << c.first << ", , ,";
    fw << "Total\n";

    fw << " ,";
    for (int i = 0; i < companyTotal.size(); ++i)
        fw << "Count,t-weight,d-weight,";
    fw << "Count,t-weight,d-weight\n";

    // For each country (row), output values per company (columns)
    for (auto& r : dataMap)
    {
        string country = r.first;
        fw << country << ",";

        for (auto& c : companyTotal)
        {
            string company = c.first;
            int val = dataMap[country][company];
            float t_weight = ((float)val / countryTotal[country]) * 100;
            float d_weight = ((float)val / companyTotal[company]) * 100;

            fw << val << ",";
            fw << fixed << setprecision(2) << t_weight << "%,";
            fw << fixed << setprecision(2) << d_weight << "%,";
        }

        fw << countryTotal[country] << ",";
        fw << "100%,"; // t-weight total for the country
        fw << fixed << setprecision(2) << ((float)countryTotal[country] / grandTotal) * 100 << "%\n";
    }

    // Footer row for total
    fw << "Total,";
    for (auto& c : companyTotal)
    {
        fw << companyTotal[c.first] << ",";
        fw << "100%,";
        fw << fixed << setprecision(2) << ((float)companyTotal[c.first] / grandTotal) * 100 << "%,";
    }
    fw << grandTotal << ",100%,100%\n";

    fw.close();
    cout << "Output written to exp4_output.csv\n";
    return 0;
}
