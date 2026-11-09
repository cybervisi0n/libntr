#include <filesystem>
#include <iostream>
#include <string>
#include <fstream>

#include "common.h"

namespace fs = std::filesystem;
using std::cout;
using std::endl;
using std::string;
using std::exception;
using std::ifstream;
using std::getline;

static string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

int SIM_ROMReorder(string languagesfolder) {
    fs::path csvPath = fs::path(languagesfolder) / "language.csv";

    if (!fs::exists(csvPath)) {
        return 0;
    }

    ifstream csvFile(csvPath);
    if (!csvFile.is_open()) {
        cout << DWARNING << "Could not open reorder file: " << csvPath.string() << endl;
        return -1;
    }

    cout << DINFO << "Found reorder rules file: " << csvPath.filename().string() << endl;

    string line;
    int movedFiles = 0;

    while (getline(csvFile, line)) {
        line = trim(line);

        if (line.empty() || line[0] == '#') {
            continue;
        }

        size_t comma = line.find(',');
        if (comma == string::npos) {
            continue;
        }

        string source = trim(line.substr(0, comma));
        string dest = trim(line.substr(comma + 1));

        if (source.empty() || dest.empty()) {
            continue;
        }

        fs::path langsource = fs::path(languagesfolder) / source;
        fs::path langdest = fs::path(languagesfolder) / dest;

        if (fs::exists(langsource)) {
            fs::path directdest = langdest.parent_path();
            if (!fs::exists(directdest)) {
                try {
                    fs::create_directories(directdest);
                } catch (exception &e) {
                    cout << DERROR << "Failed to create directory " << directdest.string() << "\n"
                         << e.what() << endl;
                    return -1;
                }
            }

            try {
                fs::rename(langsource, langdest);
                movedFiles++;
            } catch (exception &e) {
                cout << DERROR << "Failed to move file " << langsource.string() << " to " << langdest.string() << "\n"
                     << e.what() << endl;
                return -1;
            }
        }
    }

    csvFile.close();

    if (movedFiles > 0) {
        cout << DINFO << "Reordered " << movedFiles << " file(s) successfully" << endl;
    }

    return 0;
}
