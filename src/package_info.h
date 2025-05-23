#ifndef PACKAGE_INFO_H
#define PACKAGE_INFO_H

#include <string>

struct PackageInfo {
    std::string name;        // Package name.
    std::string version;     // Package version.
    std::string downloadUrl; // URL for package download.
};

#endif
