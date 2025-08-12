#include <bits/stdc++.h>
using namespace std;

const int NUM_REGIONS = 4;
const int NUM_SALESMEN = 6;
const int NUM_PRODUCTS = 8;

struct Record
{
    int RegionNo;
    int SalesmanNo;
    string ProductCode;
    int UnitSold;
};

// Function to generate random product names and prices
void generateProductData(vector<pair<string, double>> &productData)
{
    srand(time(0));
    vector<string> productNames = {"P1", "P2", "P3", "P4", "P5", "P6", "P7", "P8"};

    for (int i = 0; i < NUM_PRODUCTS; ++i)
    {
        double price = (rand() % 5000) / 100.0; // Random price between 0.00 and 50.00
        productData.push_back({productNames[i], price});
    }
}

// Function to print the product data
void printProductData(const vector<pair<string, double>> &productData)
{
    cout << "Product Name\tUnit Price" << endl;
    for (const auto &product : productData)
    {
        cout << product.first << "\t\tRs. " << fixed << setprecision(2) << product.second << "/-" << endl; // It display upto 2 decimal places
    }
}

// Function to Generate Random Sales Records
void generateRandomSalesRecords(vector<Record> &records, int numRecords, int numRegions, int numSalesmen, int numProducts)
{
    srand(time(0));

    for (int i = 0; i < numRecords; ++i)
    {
        Record record;
        record.RegionNo = rand() % numRegions + 1;                      // Random region number between 1 and numRegions
        record.SalesmanNo = rand() % numSalesmen + 1;                   // Random salesman number between 1 and numSalesmen
        record.ProductCode = "P" + to_string(rand() % numProducts + 1); // Random product code (e.g., P1, P2, ..., P8)
        record.UnitSold = rand() % 100 + 1;                             // Random units sold between 1 and 100
        records.push_back(record);
    }
}

// Write Records to Input File
void writeRecordsToFile(const string &sales_data, const vector<Record> &records)
{
    ofstream inputFile(sales_data);
    if (!inputFile) // if any case there in opening the file
    {
        // cerr:'character error stream' and it is unbuffered
        cerr << "Error opening file." << endl;
        return;
    }

    for (const auto &record : records)
    {
        inputFile << record.RegionNo << " "
                  << record.SalesmanNo << " "
                  << record.ProductCode << " "
                  << record.UnitSold << endl;
    }

    inputFile.close();
}

// Read the input file and store it in records
void readRecordsFromFile(const string &sales_data, vector<Record> &records)
{

    ifstream inputFile(sales_data);
    if (!inputFile)
    {
        cerr << "Error opening file." << endl;
        return;
    }

    string line;
    while (getline(inputFile, line))
    {
        stringstream ss(line);
        Record record;
        ss >> record.RegionNo >> record.SalesmanNo >> record.ProductCode >> record.UnitSold;
        records.push_back(record);
    }

    inputFile.close();
}

// Generate sales report
void generateSalesReport(const string &report_file, const vector<Record> &records, const vector<pair<string, double>> &productData)
{
    // Map to hold product prices
    map<string, double> productPrices;
    for (const auto &product : productData)
    {
        productPrices[product.first] = product.second;
    }

    // Map to hold sales data by region
    map<int, map<int, double>> salesData;

    // Calculate total sales per salesman and region
    for (const auto &record : records)
    {
        double saleAmount = record.UnitSold * productPrices.at(record.ProductCode); // Get unit price from productPrices map
        salesData[record.RegionNo][record.SalesmanNo] += saleAmount;
    }

    // Write the report to file
    ofstream reportFileStream(report_file);
    if (!reportFileStream)
    {
        cerr << "Error opening report file." << endl;
        return;
    }

    reportFileStream << "ABC Company Sales Report" << endl;
    reportFileStream << endl;

    for (const auto &regionEntry : salesData)
    {
        int regionNo = regionEntry.first;
        const auto &salesmanData = regionEntry.second;

        reportFileStream << "Region " << regionNo << endl;

        double totalSales = 0.0;
        for (const auto &salesmanEntry : salesmanData)
        {
            int salesmanNo = salesmanEntry.first;
            double salesAmount = salesmanEntry.second;
            totalSales += salesAmount;
            reportFileStream << "Salesman " << salesmanNo << " Rs. " << fixed << setprecision(2) << salesAmount << "/-" << endl;
        }

        reportFileStream << "Total sale at Region " << regionNo << " Rs. " << fixed << setprecision(2) << totalSales << "/-" << endl;
        reportFileStream << endl;
    }

    reportFileStream.close();
}

int main()
{
    int numRecords = 100; // Number of records to generate

    vector<pair<string, double>> productData; // 2D array
    generateProductData(productData);

    vector<Record> records;
    generateRandomSalesRecords(records, numRecords, NUM_REGIONS, NUM_SALESMEN, NUM_PRODUCTS);
    writeRecordsToFile("sales_input.txt", records);

    vector<Record> inputRecord;
    readRecordsFromFile("sales_input.txt", inputRecord);

    // printRecords(inputRecord);
    generateSalesReport("report.txt", inputRecord, productData);

    return 0;
}
