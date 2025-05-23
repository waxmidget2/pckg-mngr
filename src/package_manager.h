#ifndef PACKAGE_MANAGER_H
#define PACKAGE_MANAGER_H

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <filesystem>

#include "package_info.h"  
#include "curl_downloader.h" 
class PackageManager {
public:
    PackageManager();

    bool chooseMode(const std::string& choice);
private:
    std::vector<std::string> package_queue; // In-memory storage for package commands.
    CurlDownloader downloader;              // Abstraction for handling file downloads.

    // Private methods
    void add_to_queue(std::string& package_command);
    void edit_queue();
    void save_queue_to_file() const;
    void delete_from_queue();
    void print_queue() const;
    void prompt_for_command(std::string& package_command);
    void load_queue_from_file();
    void execute_queue();
    bool process_command(const std::string& command_string);
};

#endif 
