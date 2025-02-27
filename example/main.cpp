#include <iostream>
#include <sauron/Sauron.hpp>
#include <client/http_client_curl.hpp>

int main() {
    try {
        // Print SDK version
        std::cout << "Sauron SDK Version: " << sauron::Version::toString() << std::endl;

        // Create HTTP client
        auto httpClient = std::make_unique<sauron::client::HttpClientCurl>("localhost:3000");
        
        // Create Sauron client
        sauron::client::SauronClient client(std::move(httpClient));

        client.login(sauron::dto::LoginRequest("sk-proj-123456789123456789123456789", sauron::dto::AIProvider::OPENAI));

        auto response = client.query(sauron::dto::AIQueryRequest("Hello, how are you?", sauron::dto::AIProvider::OPENAI, "gpt-4o-mini"));

        std::cout << "Response: " << response.getResponse() << std::endl;

        // Example: Print client information
        std::cout << "Sauron client initialized successfully!" << std::endl;

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
} 