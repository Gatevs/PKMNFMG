#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include "rapidcsv.h"

class CSVCache {
public:
    static CSVCache& GetInstance() {
        static CSVCache instance;
        return instance;
    }

    void LoadCSV(const std::string& filename) {
        if (cache.find(filename) != cache.end()) {
            return; // Already loaded
        }

        try {
            rapidcsv::Document doc(filename);
            std::vector<std::vector<std::string>> rows;

            for (size_t i = 0; i < doc.GetRowCount(); ++i) {
                rows.push_back(doc.GetRow<std::string>(i));
            }

            cache[filename] = rows;
        } catch (const std::exception& e) {
            std::cerr << "Error loading CSV file " << filename << ": " << e.what() << std::endl;
        }
    }

    const std::vector<std::string>& GetRow(const std::string& filename, int idColumn, int id) const {
        const auto& rows = cache.at(filename);
        for (const auto& row : rows) {
            if (std::stoi(row[idColumn]) == id) {
                return row;
            }
        }
        throw std::runtime_error("ID not found in CSV: " + std::to_string(id));
    }

    const std::vector<std::string>& GetRowAt(const std::string& filename, int cur) const {
        const auto& rows = cache.at(filename);

        return rows[cur];
    }

    int RowNumbers(const std::string& filename){
        const auto& rows = cache.at(filename);
        return rows.size();
    }

    bool IsIndexAt (const std::string& filename, int idColumn, int id, int cur) const {
        const auto& rows = cache.at(filename);

        auto& row = rows[cur];
        if (std::stoi(row[idColumn]) == id) {
            return true;
        } else{
            return false;
        }
    }

private:
    std::unordered_map<std::string, std::vector<std::vector<std::string>>> cache;

    CSVCache() = default;
    ~CSVCache() = default;

    // Disable copying
    CSVCache(const CSVCache&) = delete;
    CSVCache& operator=(const CSVCache&) = delete;
};
