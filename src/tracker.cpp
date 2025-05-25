#include "tracker.h"
#include <iostream>
#include <fstream>
#include <filesystem>


Tracker::Tracker() { 
    std::fstream file(TRACKER_FILENAME, std::ios::out | std::ios::app);
    if (!file.is_open()) {
        std::cerr << "ERROR: Could not open or create " << TRACKER_FILENAME << ". Program may not function correctly.\n";
    }
    file.close();

    loadVersionsFromFile();
}

void Tracker::pushVersion(std::string version, std::string pckg_name, std::string currentUrl) {
    ver_queue.push_back({version, pckg_name, currentUrl});
    saveVersionsToFile(); 
    std::cout << "Package Management Info updated in '" << TRACKER_FILENAME << "'.\n";
}

void Tracker::loadVersionsFromFile() {
    package_queue.clear();

    std::ifstream inFile(TRACKER_FILENAME);
    if (!inFile.is_open()) {
        std::cerr << "Warning: Could not open '" << TRACKER_FILENAME << "' for reading. No previous versions loaded.\n";
        return;
    }

    std::string line;
    while (std::getline(inFile, line)) {
        size_t first_pipe = line.find(" | ");
        size_t second_pipe = line.find(" | ", first_pipe + 3);

        if (first_pipe != std::string::npos && second_pipe != std::string::npos) {
            PackageInfo p;
            p.version = line.substr(0, first_pipe);
            p.name = line.substr(first_pipe + 3, second_pipe - (first_pipe + 3));
            p.downloadUrl = line.substr(second_pipe + 3);
            ver_queue.push_back(p);
        } else {
            std::cerr << "Warning: Malformed line in " << TRACKER_FILENAME << ": " << line << '\n';
        }
    }
    inFile.close();
}

void Tracker::saveVersionsToFile() const {
    std::ofstream outFile(TRACKER_FILENAME);

    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open '" << TRACKER_FILENAME << "' for saving.\n";
        return;
    }

    for (const auto& p : ver_queue) {
        outFile << p.version << " | " << p.name << " | " << p.downloadUrl << '\n';
    }
    outFile.close();
}

void Tracker::manageVersions() {
    std::cout << "I TAKE IN VERSIONS AND MANAGE COLLISIONS (Currently " << ver_queue.size() << " packages tracked).\n";
}
