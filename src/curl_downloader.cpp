#include "curl_downloader.h"
#include <iostream>
#include <fstream>

// libcurl callback implementation.
size_t CurlDownloader::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    return fwrite(contents, size, nmemb, (FILE*)userp);
}

// Downloads a file.
bool CurlDownloader::downloadFile(const std::string& url, const std::string& outputPath) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        return false; // Failed state
    }
    
    // fstream is ultimately safer but FILE* is more common
    FILE* fp = fopen(outputPath.c_str(), "wb"); // not sure how i feel about this one
    if (!fp) {
        std::cerr << "Error opening file for writing: " << outputPath << std::endl;
        curl_easy_cleanup(curl);
        return false;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str()); // c_str for curl
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback); 
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // Follow redirects
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);       // 30-second timeout

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        fclose(fp);
        curl_easy_cleanup(curl);
        return false;
    }

    fclose(fp);
    curl_easy_cleanup(curl);
    return true;
}

