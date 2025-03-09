#include <iostream>
#include <sauron/Sauron.hpp>
#include <sauron/client/http_client_curl.hpp>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

// Base64 encoding table
static const std::string base64_chars = 
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

std::string base64_encode(const std::vector<char>& data) {
    std::string ret;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];
    size_t data_len = data.size();
    
    while (data_len--) {
        char_array_3[i++] = data[j++];
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for(i = 0; i < 4; i++)
                ret += base64_chars[char_array_4[i]];
            i = 0;
        }
    }

    if (i) {
        for(j = i; j < 3; j++)
            char_array_3[j] = '\0';

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for (j = 0; j < i + 1; j++)
            ret += base64_chars[char_array_4[j]];

        while(i++ < 3)
            ret += '=';
    }

    return ret;
}

int main() {
    try {
        // Print SDK version
        std::cout << "Sauron SDK Version: " << sauron::Version::toString() << std::endl;

        // Create HTTP client
        auto httpClient = std::make_unique<sauron::client::HttpClientCurl>("localhost:3000");
        
        // Create Sauron client
        sauron::client::SauronClient client(std::move(httpClient));

        client.login(sauron::dto::LoginRequest("sk-proj-******", sauron::dto::AIProvider::OPENAI));

       auto response = client.query(sauron::dto::AIQueryRequest("Hello, how are you?", sauron::dto::AIProvider::OPENAI, "gpt-4o-mini"));

        std::cout << "Response: " << response.getResponse() << std::endl;

        std::atomic_bool is_done = false;
        // use stream to get the response
        if(!client.queryStream(sauron::dto::AIQueryRequest("I'm testing your stream api, could you provide me a long enough anwser to test the stream?", sauron::dto::AIProvider::OPENAI, "gpt-4o-mini"), [&is_done](const std::string& chunk, bool is_final) {
            std::cout << "Chunk: " << chunk << std::endl;
            if(is_final) {
                is_done = true;
            }
            return true;
        })) {
            std::cerr << "Stream failed" << std::endl;
        }

        while(!is_done) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        auto responseAlgorithm = client.queryAlgorithm(sauron::dto::AIQueryRequest("solve the k-neirest element problem in java", sauron::dto::AIProvider::OPENAI, "gpt-4o-mini"));

        std::cout << "Response: " << responseAlgorithm.toJson().dump(4) << std::endl;

        // load image in base64 format
        std::ifstream file("../../data-tests/image.png", std::ios::binary);
        std::vector<char> buffer(std::istreambuf_iterator<char>(file), {});
        std::string image_base64 = "data:image/png;base64," + base64_encode(buffer);
        
        auto aiAlgorithmWithImageQuery = sauron::dto::AIQueryRequest("solve it in java", sauron::dto::AIProvider::OPENAI, "gpt-4o-mini");
        aiAlgorithmWithImageQuery.addImage(image_base64);

        auto responseAlgorithmWithImage = client.queryAlgorithm(aiAlgorithmWithImageQuery);

        std::cout << "Response: " << responseAlgorithmWithImage.toJson().dump(4) << std::endl;

        // Example: Print client information
        std::cout << "Sauron client initialized successfully!" << std::endl;

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
} 