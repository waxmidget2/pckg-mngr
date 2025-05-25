#ifndef TRACKER_H
#define TRACKER_H

#include <string>
#include <vector>

struct PackageInfo {
    std::string version;
    std::string name;
    std::string downloadUrl;
};

class Tracker {
public: 
  Tracker(); 
    void pushVersion(std::string version, std::string pckg_name, std::string currentUrl); // <-- Must be public to be called by PackageManager

    void manageVersions();

private: 
    void loadVersionsFromFile();
    void saveVersionsToFile() const;

    std::vector<PackageInfo> ver_queue;
    const std::string TRACKER_FILENAME = "PCKG_TRACKER.txt";
};

#endif // TRACKER_H
