#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <cmath>
#include <algorithm>
#include <ctime>
#include <memory>
#include <iomanip>
#include <cstdlib>
#include <cstring>

// Base class for Stock Data
class StockData {
protected:
    std::string date;
    std::string stockSymbol;
    double closingPrice;

public:
    StockData() : date(""), stockSymbol(""), closingPrice(0.0) {}

    StockData(const std::string& d, const std::string& s, double c)
        : date(d), stockSymbol(s), closingPrice(c) {}

    // Copy Constructor
    StockData(const StockData& other)
        : date(other.date), stockSymbol(other.stockSymbol), closingPrice(other.closingPrice) {}

    virtual ~StockData() {}

    // Getters
    const std::string& getDate() const { return date; }
    const std::string& getStockSymbol() const { return stockSymbol; }
    double getClosingPrice() const { return closingPrice; }

    // Setters
    void setDate(const std::string& d) { date = d; }
    void setStockSymbol(const std::string& s) { stockSymbol = s; }
    void setClosingPrice(double c) { closingPrice = c; }
};

// Derived class for Stock Data with Increase/Decrease calculations
class StockDataExtended : public StockData {
private:
    double priceChange;
    double priceChangePercent;

public:
    StockDataExtended() : StockData(), priceChange(0.0), priceChangePercent(0.0) {}

    StockDataExtended(const StockData& sd, double change, double changePercent)
        : StockData(sd), priceChange(change), priceChangePercent(changePercent) {}

    // Copy Constructor
    StockDataExtended(const StockDataExtended& other)
        : StockData(other), priceChange(other.priceChange), priceChangePercent(other.priceChangePercent) {}

    // Getters
    double getPriceChange() const { return priceChange; }
    double getPriceChangePercent() const { return priceChangePercent; }

    // Setters
    void setPriceChange(double change) { priceChange = change; }
    void setPriceChangePercent(double changePercent) { priceChangePercent = changePercent; }
};

// Function 1: Calculate Increase or Decrease Amount and Range
void calculatePriceChanges(const std::string& inputFileName, const std::string& outputFileName) {
    std::ifstream infile(inputFileName);
    std::ofstream outfile(outputFileName);
    if (!infile.is_open() || !outfile.is_open()) {
        std::cerr << "Error opening files for reading/writing.\n";
        return;
    }

    std::string line;
    std::map<std::string, double> lastClosingPrices;
    // Write header
    outfile << "Date,StockSymbol,ClosingPrice,PriceChange,PriceChangePercent\n";

    while (std::getline(infile, line)) {
        std::stringstream ss(line);
        std::string date, stockSymbol, closingPriceStr;
        std::getline(ss, date, ',');
        std::getline(ss, stockSymbol, ',');
        std::getline(ss, closingPriceStr, ',');

        double closingPrice = std::stod(closingPriceStr);
        double priceChange = 0.0;
        double priceChangePercent = 0.0;

        // Key for map is stock symbol
        auto it = lastClosingPrices.find(stockSymbol);
        if (it != lastClosingPrices.end()) {
            double lastPrice = it->second;
            priceChange = closingPrice - lastPrice;
            if (lastPrice != 0.0) {
                priceChangePercent = (priceChange / lastPrice) * 100.0;
            }
        }
        lastClosingPrices[stockSymbol] = closingPrice;

        // Write to output file
        outfile << date << "," << stockSymbol << "," << closingPrice << ","
                << priceChange << "," << priceChangePercent << "\n";
    }

    infile.close();
    outfile.close();
}

// Function 2: External Sorting
void externalSort(const std::string& inputFileName, const std::string& outputFileName) {
    // Read data into memory (assuming it's within memory constraints)
    std::ifstream infile(inputFileName);
    if (!infile.is_open()) {
        std::cerr << "Error opening input file.\n";
        return;
    }

    struct StockRecord {
        std::string date;
        std::string stockSymbol;
        double closingPrice;
        double priceChange;
        double priceChangePercent;

        // Static comparator for sorting
        static bool comparator(const StockRecord& a, const StockRecord& b) {
            if (a.date != b.date) {
                return a.date > b.date; // Descending date
            }
            return a.priceChangePercent > b.priceChangePercent; // Descending price change percent
        }
    };

    std::vector<StockRecord> records;
    std::string line;
    // Skip header
    std::getline(infile, line);

    while (std::getline(infile, line)) {
        std::stringstream ss(line);
        StockRecord record;
        std::string closingPriceStr, priceChangeStr, priceChangePercentStr;
        std::getline(ss, record.date, ',');
        std::getline(ss, record.stockSymbol, ',');
        std::getline(ss, closingPriceStr, ',');
        std::getline(ss, priceChangeStr, ',');
        std::getline(ss, priceChangePercentStr, ',');

        record.closingPrice = std::stod(closingPriceStr);
        record.priceChange = std::stod(priceChangeStr);
        record.priceChangePercent = std::stod(priceChangePercentStr);

        records.push_back(record);
    }

    infile.close();

    // Sort records
    std::sort(records.begin(), records.end(), StockRecord::comparator);

    // Write sorted data to output file
    std::ofstream outfile(outputFileName);
    if (!outfile.is_open()) {
        std::cerr << "Error opening output file.\n";
        return;
    }

    // Write header
    outfile << "Date,StockSymbol,ClosingPrice,PriceChange,PriceChangePercent\n";
    for (const auto& record : records) {
        outfile << record.date << "," << record.stockSymbol << "," << record.closingPrice << ","
                << record.priceChange << "," << record.priceChangePercent << "\n";
    }

    outfile.close();
}

// Function 3: Calculate Sharpe Ratio
void calculateSharpeRatio(const std::string& inputFileName, const std::string& outputFileName) {
    // Map to hold stock symbol to list of annual returns
    std::ifstream infile(inputFileName);
    if (!infile.is_open()) {
        std::cerr << "Error opening input file.\n";
        return;
    }

    std::map<std::string, std::vector<double>> stockReturns;
    std::string line;
    // Skip header
    std::getline(infile, line);

    while (std::getline(infile, line)) {
        std::stringstream ss(line);
        std::string date, stockSymbol, closingPriceStr, priceChangeStr, priceChangePercentStr;
        std::getline(ss, date, ',');
        std::getline(ss, stockSymbol, ',');
        std::getline(ss, closingPriceStr, ',');
        std::getline(ss, priceChangeStr, ',');
        std::getline(ss, priceChangePercentStr, ',');

        double dailyReturn = std::stod(priceChangePercentStr);
        stockReturns[stockSymbol].push_back(dailyReturn);
    }

    infile.close();

    // Calculate Sharpe Ratio for each stock
    std::ofstream outfile(outputFileName);
    if (!outfile.is_open()) {
        std::cerr << "Error opening output file.\n";
        return;
    }

    // Write header
    outfile << "StockSymbol,AverageReturn,StdDeviation,SharpeRatio\n";

    for (const auto& pair : stockReturns) {
        const std::string& stockSymbol = pair.first;
        const std::vector<double>& returns = pair.second;

        // Calculate average return
        double sum = 0.0;
        for (double r : returns) {
            sum += r;
        }
        double averageReturn = sum / returns.size();

        // Calculate standard deviation
        double variance = 0.0;
        for (double r : returns) {
            variance += (r - averageReturn) * (r - averageReturn);
        }
        variance /= returns.size();
        double stdDeviation = std::sqrt(variance);

        // Calculate Sharpe Ratio
        double sharpeRatio = 0.0;
        if (stdDeviation != 0.0) {
            sharpeRatio = averageReturn / stdDeviation;
        }

        outfile << stockSymbol << "," << averageReturn << "," << stdDeviation << "," << sharpeRatio << "\n";
    }

    outfile.close();
}

// Function 4: Output K Stocks with Largest Daily Increase/Decrease in a Certain Month
void outputTopKStocks(const std::string& monthFileName, int k) {
    // Limit memory usage to 10MB
    const size_t memoryLimit = 10 * 1024 * 1024; // 10MB

    struct StockRecord {
        std::string date;
        std::string stockSymbol;
        double closingPrice;
        double priceChange;
        double priceChangePercent;

        // Comparator based on absolute value of price change
        static bool comparator(const StockRecord& a, const StockRecord& b) {
            return std::abs(a.priceChange) > std::abs(b.priceChange);
        }
    };

    std::ifstream infile(monthFileName);
    if (!infile.is_open()) {
        std::cerr << "Error opening month data file.\n";
        return;
    }

    std::vector<StockRecord> records;
    std::string line;
    // Skip header
    std::getline(infile, line);

    size_t currentMemoryUsage = 0;

    while (std::getline(infile, line)) {
        std::stringstream ss(line);
        StockRecord record;
        std::string closingPriceStr, priceChangeStr, priceChangePercentStr;
        std::getline(ss, record.date, ',');
        std::getline(ss, record.stockSymbol, ',');
        std::getline(ss, closingPriceStr, ',');
        std::getline(ss, priceChangeStr, ',');
        std::getline(ss, priceChangePercentStr, ',');

        record.closingPrice = std::stod(closingPriceStr);
        record.priceChange = std::stod(priceChangeStr);
        record.priceChangePercent = std::stod(priceChangePercentStr);

        records.push_back(record);

        // Estimate memory usage
        currentMemoryUsage += sizeof(StockRecord);
        if (currentMemoryUsage >= memoryLimit) {
            std::cerr << "Memory limit exceeded.\n";
            break;
        }
    }

    infile.close();

    // Sort records based on absolute value of price change
    std::sort(records.begin(), records.end(), StockRecord::comparator);

    // Output top K records
    std::cout << "Top " << k << " stocks with largest daily increase/decrease:\n";
    std::cout << "Date,StockSymbol,ClosingPrice,PriceChange,PriceChangePercent\n";

    for (int i = 0; i < k && i < records.size(); ++i) {
        const auto& record = records[i];
        std::cout << record.date << "," << record.stockSymbol << "," << record.closingPrice << ","
                  << record.priceChange << "," << record.priceChangePercent << "\n";
    }
}

// Main function to demonstrate the functionalities
int main() {
    // Function 1
    calculatePriceChanges("market_data.csv", "market_data_with_changes.csv");

    // Function 2
    externalSort("market_data_with_changes.csv", "sorted_market_data.csv");

    // Function 3
    calculateSharpeRatio("market_data_with_changes.csv", "sharpe_ratios.csv");

    // Function 4
    int k = 10; // Example value for k
    outputTopKStocks("2020-06.csv", k);

    // Function 5 would involve visualization using Qt, which is beyond the scope of this text-based interface

    return 0;
}
