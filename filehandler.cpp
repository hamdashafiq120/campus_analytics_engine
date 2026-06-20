#include "filehandler.h"

#include <fstream>
#include <sstream>

std::string trim(const std::string& s) {
    int start = 0;
    int end = (int)s.length() - 1;
    while (start <= end && (s[start] == ' ' || s[start] == '\t' || s[start] == '\r')) {
        start++;
    }
    while (end >= start && (s[end] == ' ' || s[end] == '\t' || s[end] == '\r')) {
        end--;
    }
    if (start > end) {
        return "";
    }
    return s.substr(start, end - start + 1);
}

std::vector<std::string> parseCSVLine(const std::string& line) {
    std::vector<std::string> fields;
    std::string current = "";
    bool inQuotes = false;
    int i = 0;
    while (i < (int)line.length()) {
        char c = line[i];
        if (c == '"') {
            if (inQuotes && i + 1 < (int)line.length() && line[i + 1] == '"') {
                current += '"';
                i += 2;
                continue;
            }
            inQuotes = !inQuotes;
            i++;
            continue;
        }
        if (c == ',' && !inQuotes) {
            fields.push_back(trim(current));
            current = "";
            i++;
            continue;
        }
        current += c;
        i++;
    }
    fields.push_back(trim(current));
    return fields;
}

std::vector<std::vector<std::string> > readTXT(const std::string& filename) {
    std::vector<std::vector<std::string> > rows;
    std::ifstream file(filename.c_str());
    if (!file.is_open()) {
        return rows;
    }

    std::string line = "";
    bool firstLine = true;
    char ch;
    while (file.get(ch)) {
        if (ch == '\r') {
            continue;
        }
        if (ch == '\n') {
            if (line.length() > 0) {
                if (firstLine) {
                    firstLine = false;
                } else {
                    rows.push_back(parseCSVLine(line));
                }
                line = "";
            }
            continue;
        }
        line += ch;
    }

    if (line.length() > 0 && !firstLine) {
        rows.push_back(parseCSVLine(line));
    }

    file.close();
    return rows;
}

bool writeTXT(const std::string& filename,
              const std::vector<std::string>& header,
              const std::vector<std::vector<std::string> >& rows) {
    std::ofstream file(filename.c_str());
    if (!file.is_open()) {
        return false;
    }

    for (int i = 0; i < (int)header.size(); i++) {
        if (i > 0) {
            file << ",";
        }
        std::string field = header[i];
        bool needsQuotes = false;
        for (int j = 0; j < (int)field.length(); j++) {
            if (field[j] == ',') {
                needsQuotes = true;
                break;
            }
        }
        if (needsQuotes) {
            file << '"' << field << '"';
        } else {
            file << field;
        }
    }
    file << "\n";

    for (int r = 0; r < (int)rows.size(); r++) {
        for (int c = 0; c < (int)rows[r].size(); c++) {
            if (c > 0) {
                file << ",";
            }
            std::string field = rows[r][c];
            bool needsQuotes = false;
            for (int j = 0; j < (int)field.length(); j++) {
                if (field[j] == ',') {
                    needsQuotes = true;
                    break;
                }
            }
            if (needsQuotes) {
                file << '"' << field << '"';
            } else {
                file << field;
            }
        }
        file << "\n";
    }

    file.close();
    return true;
}

bool appendTXT(const std::string& filename, const std::vector<std::string>& row) {
    std::ofstream file(filename.c_str(), std::ios::app);
    if (!file.is_open()) {
        return false;
    }

    for (int i = 0; i < (int)row.size(); i++) {
        if (i > 0) {
            file << ",";
        }
        std::string field = row[i];
        bool needsQuotes = false;
        for (int j = 0; j < (int)field.length(); j++) {
            if (field[j] == ',') {
                needsQuotes = true;
                break;
            }
        }
        if (needsQuotes) {
            file << '"' << field << '"';
        } else {
            file << field;
        }
    }
    file << "\n";
    file.close();
    return true;
}

std::vector<std::string> findRow(const std::string& filename,
                                 int colIndex,
                                 const std::string& value) {
    std::vector<std::vector<std::string> > rows = readTXT(filename);
    for (int i = 0; i < (int)rows.size(); i++) {
        if (colIndex < (int)rows[i].size() && rows[i][colIndex] == value) {
            return rows[i];
        }
    }
    return std::vector<std::string>();
}

bool rowExists(const std::string& filename, int colIndex, const std::string& value) {
    std::vector<std::string> row = findRow(filename, colIndex, value);
    return row.size() > 0;
}
