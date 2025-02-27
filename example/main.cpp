#include <iostream>
#include <sauron/Sauron.hpp>
#include <client/http_client_curl.hpp>

int main() {
    try {
        // Print SDK version
        std::cout << "Sauron SDK Version: " << sauron::Version::toString() << std::endl;

        // Create HTTP client
        auto httpClient = std::make_unique<sauron::client::HttpClientCurl>();
        
        // Create Sauron client
        sauron::client::SauronClient client(std::move(httpClient));

        // Example: Print client information
        std::cout << "Sauron client initialized successfully!" << std::endl;

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
} 