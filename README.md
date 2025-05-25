# C++ Package Queue Manager

A lightweight command-line tool to manage package downloads.

## Features:
- Add, print, and delete package download commands.
- Executes queued "download" commands using `libcurl`.
- Persists queue state to `package_queue.txt`.
- Basic implementation for learning.
- Tracks versions.

## Usage:
1. Compile: `g++ -std=c++17 main.cpp package_manager.cpp curl_downloader.cpp -o package_manager -lcurl`
2. Run: `./package_manager`
