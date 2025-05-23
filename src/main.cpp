#include <iostream>
#include <string>
#include <cstdlib>
#include "package_manager.h"
#include <curl/curl.h>

int main()
{
    curl_global_init(CURL_GLOBAL_ALL); // Libcurl initialization.

    PackageManager manager; // Package manager.

    bool running = true;
    std::string choice;

    // Main Loop
    while (running) {
        std::cout << "\n--- Package Queue Manager ---\n";
        std::cout << "Available modes:\n";
        std::cout << "  'a' - Add a package command\n";
        std::cout << "  'e' - Edit a package command\n";
        std::cout << "  'p' - Print all commands\n";
        std::cout << "  'd' - Delete a command\n";
        std::cout << "  'x' - Execute (download) all commands\n";
        std::cout << "  'q' - Quit and save\n";
        std::cout << "Enter mode: ";
        std::getline(std::cin, choice);

        running = manager.chooseMode(choice); // Process user input.
    }

    curl_global_cleanup(); // Libcurl cleanup.

    return 0;
}
