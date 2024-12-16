#pragma once

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

    void parseCSV(const std::string& filename, std::vector<std::string>& DEF, int ID){
        LoadCSV(filename);

        try {
            // Query the row with the matching ID
            const auto& row = GetRow(filename, 0, ID); // Assuming ID is in column 0
            DEF = row; // Store the row in your Pokémon-specific vector
        } catch (const std::exception& e) {
            std::cerr << "Error retrieving data: " << e.what() << std::endl;
        }
    }
    void parseCSVFromString(const std::string& filename, std::vector<std::string>& DEF, std::string name){
        LoadCSV(filename);

        try {
            // Query the row with the matching ID
            const auto& row = GetRowFromString(filename, 0, name); // Assuming ID is in column 0
            DEF = row; // Store the row in your Pokémon-specific vector
        } catch (const std::exception& e) {
            std::cerr << "Error retrieving data: " << e.what() << std::endl;
        }
    }

    const std::vector<std::string>& GetRow(const std::string& filename, int idColumn, int id) const {
        const auto& rows = cache.at(filename);
        for (const auto& row : rows) {
            if (std::stoi(row[idColumn]) == id) {
                return row;
            }
        }
        throw std::runtime_error("ID: " + std::to_string(id) + " not found in CSV: " + filename);
    }

        const std::vector<std::string>& GetRowFromString(const std::string& filename, int idColumn, std::string name) const {
        const auto& rows = cache.at(filename);
        for (const auto& row : rows) {
            if (row[idColumn] == name) {
                return row;
            }
        }
        throw std::runtime_error("String not found in CSV: " + name);
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

    const std::vector<std::vector<std::string>> GetRowsForID(const std::string& filename, int pokemonID) {
        std::vector<std::vector<std::string>> matchingRows;
        const auto& rows = cache.at(filename); // Assume `loadedCSVs` stores the parsed rows
        for (const auto& row : rows) {
            if (std::stoi(row[0]) == pokemonID) { // Column 0: Pokémon ID
                matchingRows.push_back(row);
            }
        }
        return matchingRows;
    }



private:
    std::unordered_map<std::string, std::vector<std::vector<std::string>>> cache;

    CSVCache() = default;
    ~CSVCache() = default;

    // Disable copying
    CSVCache(const CSVCache&) = delete;
    CSVCache& operator=(const CSVCache&) = delete;
};
