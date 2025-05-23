#ifndef CURL_DOWNLOADER_H
#define CURL_DOWNLOADER_H

#include <string>
#include <curl/curl.h>

// Manages file downloads using libcurl.

class CurlDownloader {
public:
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);

    bool downloadFile(const std::string& url, const std::string& outputPath);
};

#endif 
