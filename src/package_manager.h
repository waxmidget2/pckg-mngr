#include "package_manager.h"
#include "tracker.h"
#include "curl_downloader.h"

#include <iostream>
#include <limits>
#include <sstream>
#include <filesystem>

// Constructor implementation
PackageManager::PackageManager() : tracker(), downloader() {
    std::fstream file(QUEUE_FILENAME, std::ios::in | std::ios::out | std::ios::app);
    if (!file.is_open()) {
        std::cerr << "ERROR: Could not open or create " << QUEUE_FILENAME << ". Exiting.\n";
        return;
    }
    file.close();

    load_queue_from_file();
}

// Processes the user's mode choice.
bool PackageManager::chooseMode(const std::string& choice) {
    std::string package_command;

    if (choice == "a") {
        prompt_for_command(package_command);
        add_to_queue(package_command);
    } else if (choice == "p") {
        print_queue();
    } else if (choice == "d") {
        delete_from_queue();
    } else if (choice == "e") {
        edit_queue();
    } else if (choice == "x") {
        execute_queue();
    } else if (choice == "quit" || choice == "q") {
        save_queue_to_file();
        return false;
    } else {
        std::cout << "Invalid choice. Please enter 'a', 'p', 'd', 'x', or 'q'.\n";
    }
    return true;
}

// Prompts user for a package command string.
void PackageManager::prompt_for_command(std::string& package_command) {
    std::cout << "Enter package command (e.g., 'download mypackage 1.0 https://example.com/mypackage.zip'): ";
    std::getline(std::cin, package_command);
}

// Edits a given queue element.
void PackageManager::edit_queue() {
    if (package_queue.empty()) {
        std::cout << "\nQueue is empty!\n";
        return;
    }
    print_queue();

    int pckg_index{};
    std::string mut_package_command{};

    std::cout << "Enter package index (1-" << package_queue.size() << "): ";
    std::getline(std::cin, mut_package_command);

    try {
        pckg_index = std::stoi(mut_package_command) - 1;
    } catch (const std::invalid_argument& e) {
        std::cerr << "Invalid input. Please enter a number.\n";
        return;
    } catch (const std::out_of_range& e) {
        std::cerr << "Number out of range.\n";
        return;
    }

    if (pckg_index >= 0 && pckg_index < static_cast<int>(package_queue.size())) {
        std::cout << "Current command (" << pckg_index + 1 << "): " << package_queue[pckg_index] << "\n";
        std::cout << "Enter new command for index " << pckg_index + 1 << ": ";
        std::getline(std::cin, mut_package_command);
        if (!mut_package_command.empty()) {
            package_queue[pckg_index] = mut_package_command;
            std::cout << "Command updated successfully.\n";
        } else {
            std::cout << "New command cannot be empty. Original command retained.\n";
        }
    } else {
        std::cout << "Invalid package index.\n";
    }
}

// Adds a command to the in-memory queue.
void PackageManager::add_to_queue(std::string& package_command) {
    if (!package_command.empty()) {
        package_queue.push_back(package_command);
        std::cout << "Command added to queue.\n";
    } else {
        std::cout << "Cannot add an empty command.\n";
    }
}

// Prints all commands in the queue.
void PackageManager::print_queue() const {
    std::cout << "\n====== Package Queue ======\n";
    if (package_queue.empty()) {
        std::cout << "The queue is empty.\n";
        return;
    }
    for (size_t i = 0; i < package_queue.size(); ++i) {
        std::cout << i + 1 << ": " << package_queue[i] << '\n';
    }
    std::cout << "===========================\n";
}

// Deletes a command from the queue by its number.
void PackageManager::delete_from_queue() {
    if (package_queue.empty()) {
        std::cout << "Queue is empty. Nothing to delete.\n";
        return;
    }

    print_queue();
    std::string command_selection_str{};
    int index_to_delete = -1;

    std::cout << "Enter the number of the command to delete: ";
    std::getline(std::cin, command_selection_str);

    try {
        index_to_delete = std::stoi(command_selection_str) - 1;
    } catch (const std::invalid_argument& e) {
        std::cerr << "Invalid input. Please enter a number.\n";
        return;
    } catch (const std::out_of_range& e) {
        std::cerr << "Number out of range.\n";
        return;
    }

    if (index_to_delete >= 0 && index_to_delete < static_cast<int>(package_queue.size())) {
        package_queue.erase(package_queue.begin() + index_to_delete);
        std::cout << "Command deleted successfully.\n";
    } else {
        std::cerr << "Invalid command number.\n";
    }
}

// Loads commands from "package_queue.txt" into the in-memory queue.
void PackageManager::load_queue_from_file() {
    std::ifstream inFile(QUEUE_FILENAME);

    if (!inFile.is_open()) {
        std::cerr << "Error: Could not open '" << QUEUE_FILENAME << "' for loading.\n";
        return;
    }

    std::string line;
    while (std::getline(inFile, line)) {
        if (!line.empty()) {
            size_t pos = line.find(". ");
            if (pos != std::string::npos && pos < 4) {
                package_queue.push_back(line.substr(pos + 2));
            } else {
                package_queue.push_back(line);
            }
        }
    }
    inFile.close();
    std::cout << "Loaded " << package_queue.size() << " commands from '" << QUEUE_FILENAME << "'.\n";
}

// Saves the current in-memory queue to "package_queue.txt", overwriting existing content.
void PackageManager::save_queue_to_file() const {
    std::ofstream outFile(QUEUE_FILENAME, std::ios::out);

    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open '" << QUEUE_FILENAME << "' for saving.\n";
        return;
    }

    for (size_t i = 0; i < package_queue.size(); ++i) {
        outFile << i + 1 << ". " << package_queue[i] << '\n';
    }
    outFile.close();
    std::cout << "Queue saved to '" << QUEUE_FILENAME << "'.\n";
}

// Processes a single command string, specifically handling 'download' commands.
bool PackageManager::process_command(const std::string& command_string) {
    std::stringstream ss(command_string);
    std::string command_type;
    ss >> command_type;

    if (command_type == "download") {
        PackageInfo package;
        std::string version_str;
        std::string url_str;

        if (!(ss >> package.name >> version_str)) {
            std::cerr << "Error: Malformed download command: " << command_string << ". Expected 'download <name> <version> <url>'.\n";
            return false;
        }

        std::getline(ss, url_str);
        size_t first_char_pos = url_str.find_first_not_of(" \t");
        if (first_char_pos != std::string::npos) {
            package.downloadUrl = url_str.substr(first_char_pos);
        } else {
            std::cerr << "Error: Malformed download command: " << command_string << ". URL not found.\n";
            return false;
        }
        package.version = version_str;

        tracker.pushVersion(package.version, package.name, package.downloadUrl);

        if (package.name.empty() || package.version.empty() || package.downloadUrl.empty()) {
            std::cerr << "Error: Incomplete download command: " << command_string << ". Missing name, version, or URL.\n";
            return false;
        }

        std::cout << "\nAttempting to download: " << package.name << " (v" << package.version << ") from " << package.downloadUrl << std::endl;

        std::filesystem::path install_dir = "packages";
        if (!std::filesystem::exists(install_dir)) {
            try {
                std::filesystem::create_directories(install_dir);
                std::cout << "Created directory: " << install_dir << std::endl;
            } catch (const std::filesystem::filesystem_error& e) {
                std::cerr << "Error creating directory '" << install_dir << "': " << e.what() << std::endl;
                return false;
            }
        }

        std::string filename = package.name + "-" + package.version;
        size_t last_slash = package.downloadUrl.find_last_of('/');
        std::string potential_filename_in_url = (last_slash != std::string::npos) ? package.downloadUrl.substr(last_slash + 1) : package.downloadUrl;
        size_t last_dot = potential_filename_in_url.find_last_of('.');
        if (last_dot != std::string::npos && last_dot < potential_filename_in_url.length() - 1) {
            filename += potential_filename_in_url.substr(last_dot);
        } else {
            filename += ".bin";
        }

        std::filesystem::path output_path = install_dir / filename;

        if (downloader.downloadFile(package.downloadUrl, output_path.string())) {
            std::cout << "Successfully downloaded '" << package.name << "' to: " << output_path << std::endl;
            return true;
        } else {
            std::cerr << "Failed to download package: " << package.name << std::endl;
            return false;
        }
    } else {
        std::cout << "Skipping unknown command type: '" << command_type << "' in queue: " << command_string << std::endl;
        return true;
    }
}

// Executes all commands in the package queue.
void PackageManager::execute_queue() {
    if (package_queue.empty()) {
        std::cout << "The package queue is empty. Nothing to execute.\n";
        return;
    }

    std::cout << "\n--- Executing Package Queue ---\n";
    std::vector<std::string> unprocessed_commands;

    for (size_t i = 0; i < package_queue.size(); ++i) {
        std::cout << "\nProcessing command " << i + 1 << ": " << package_queue[i] << std::endl;
        if (!process_command(package_queue[i])) {
            std::cerr << "Failed to execute command: " << package_queue[i] << std::endl;
            unprocessed_commands.push_back(package_queue[i]);
        }
    }

    std::cout << "\n--- Queue Execution Complete ---\n";

    if (!unprocessed_commands.empty()) {
        std::cout << "The following commands could not be executed:\n";
        for (const auto& cmd : unprocessed_commands) {
            std::cout << "  - " << cmd << std::endl;
        }
    }

    std::string clear_choice;
    std::cout << "Do you want to clear the executed commands from the queue? (y/n): ";
    std::getline(std::cin, clear_choice);

    if (clear_choice == "y" || clear_choice == "Y") {
        package_queue.clear();
        std::cout << "Queue cleared.\n";
        save_queue_to_file();
    } else {
        std::cout << "Queue not cleared. Commands remain.\n";
    }
}
