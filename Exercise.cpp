#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <limits>
#include <cmath>

using namespace std;

struct TradeData {
    int64_t max_time_gap; // Maximum time gap
    int total_volume; // Sum of the quantity for all trades in a symbol
    int64_t total_price_quantity;
    int total_quantity;
    int max_trade_price;
    int64_t last_timestamp;

    TradeData() : max_time_gap(0), total_volume(0), total_price_quantity(0), total_quantity(0), max_trade_price(0), last_timestamp(-1) {}

    void update(int64_t timestamp, int quantity, int price) {
        if (last_timestamp != -1) {
            int64_t gap = timestamp - last_timestamp;
            if (gap > max_time_gap) {
                max_time_gap = gap;
            }
        }
        last_timestamp = timestamp;

        total_volume += quantity;
        total_price_quantity += static_cast<int64_t>(quantity) * price;
        total_quantity += quantity;
        if (price > max_trade_price) {
            max_trade_price = price;
        }
    }

    int weighted_average_price() const {
        return total_quantity == 0 ? 0 : static_cast<int>(total_price_quantity / total_quantity);
    }
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_file> <output_file>" << std::endl;
        return 1;
    }

    const char* input_file = argv[1];
    const char* output_file = argv[2];

    ifstream inputFile(input_file);
    if (!inputFile.is_open()) {
        cerr << "Fail to open file" << endl;
        return 1;
    }

    map<string, TradeData> symbol_data;
    string line;
    while (getline(inputFile, line)) {
        istringstream ss(line);
        string timestamp_str, symbol, quantity_str, price_str;

        getline(ss, timestamp_str, ',');
        getline(ss, symbol, ',');
        getline(ss, quantity_str, ',');
        getline(ss, price_str, ',');

        int64_t timestamp = stoll(timestamp_str);
        int quantity = stoi(quantity_str);
        int price = stoi(price_str);

        symbol_data[symbol].update(timestamp, quantity, price);
    }

    ofstream outputFile(output_file);
    outputFile << "Symbol,MaxTimeGap,Volume,WeightedAveragePrice,MaxPrice\n";
    for (const auto& [symbol, data] : symbol_data) {
        outputFile << symbol << ","
                << data.max_time_gap << ","
                << data.total_volume << ","
                << data.weighted_average_price() << ","
                << data.max_trade_price << "\n";
    }

    inputFile.close();
    outputFile.close();
    return 0;
}