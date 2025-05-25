#ifndef PACKAGE_MANAGER_H
#define PACKAGE_MANAGER_H

#include <string>
#include <vector>
#include <iostream> 
#include <fstream>  
#include <sstream>  
#include <filesystem> 
#include "tracker.h"       
#include "curl_downloader.h" 

class PackageManager {
public:
    // Constructor
    PackageManager();
    bool chooseMode(const std::string& choice);
    void prompt_for_command(std::string& package_command);
    void edit_queue();
    void add_to_queue(std::string& package_command);
    void print_queue() const;
    void delete_from_queue();
    void load_queue_from_file();
    void save_queue_to_file() const;
    bool process_command(const std::string& command_string);
    void execute_queue();

private:
    std::vector<std::string> package_queue; 
    Tracker tracker; 
    CurlDownloader downloader; 
    const std::string QUEUE_FILENAME = "package_queue.txt";
};

#endif // PACKAGE_MANAGER_H
