#include "http_client_curl.hpp"
#include <stdexcept>
#include <sstream>

namespace sauron {
namespace client {

HttpClientCurl::HttpClientCurl(const std::string& baseUrl)
    : m_baseUrl(baseUrl), m_globalInit(false) {
    if (curl_global_init(CURL_GLOBAL_ALL) != CURLE_OK) {
        throw std::runtime_error("Failed to initialize CURL");
    }
    m_globalInit = true;

    m_curl = curl_easy_init();
    if (!m_curl) {
        curl_global_cleanup();
        throw std::runtime_error("Failed to create CURL handle");
    }

    // Set default options
    curl_easy_setopt(m_curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(m_curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(m_curl, CURLOPT_SSL_VERIFYHOST, 2L);
}

HttpClientCurl::~HttpClientCurl() {
    if (m_curl) {
        curl_easy_cleanup(m_curl);
    }
    if (m_globalInit) {
        curl_global_cleanup();
    }
}

void HttpClientCurl::setBaseUrl(const std::string& url) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_baseUrl = url;
}

std::string HttpClientCurl::getBaseUrl() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_baseUrl;
}

void HttpClientCurl::setDefaultHeader(const std::string& name, const std::string& value) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_defaultHeaders[name] = value;
}

void HttpClientCurl::removeDefaultHeader(const std::string& name) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_defaultHeaders.erase(name);
}

void HttpClientCurl::setBearerToken(const std::string& token) {
    setDefaultHeader("Authorization", "Bearer " + token);
}

void HttpClientCurl::clearAuthorization() {
    removeDefaultHeader("Authorization");
}

size_t HttpClientCurl::WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    size_t totalSize = size * nmemb;
    userp->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

size_t HttpClientCurl::HeaderCallback(char* buffer, size_t size, size_t nitems, void* userdata) {
    size_t totalSize = size * nitems;
    auto* headers = static_cast<std::vector<std::string>*>(userdata);
    headers->emplace_back(buffer, totalSize);
    return totalSize;
}

size_t HttpClientCurl::StreamWriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    auto* callback = static_cast<StreamCallback*>(userp);
    size_t totalSize = size * nmemb;
    std::string chunk(static_cast<char*>(contents), totalSize);
    return (*callback)(chunk, false) ? totalSize : 0;
}

std::string HttpClientCurl::buildUrl(const std::string& path) const {
    if (path.empty()) return m_baseUrl;
    if (path[0] == '/') return m_baseUrl + path;
    return m_baseUrl + "/" + path;
}

struct curl_slist* HttpClientCurl::prepareHeaders(const std::vector<std::string>& additionalHeaders) const {
    struct curl_slist* headers = nullptr;
    
    // Add default headers
    for (const auto& header : m_defaultHeaders) {
        std::string headerStr = header.first + ": " + header.second;
        headers = curl_slist_append(headers, headerStr.c_str());
    }

    // Add additional headers
    for (const auto& header : additionalHeaders) {
        headers = curl_slist_append(headers, header.c_str());
    }

    return headers;
}

void HttpClientCurl::setupCurl(CURL* curl, const std::string& url, const std::vector<std::string>& headers) {
    curl_easy_reset(curl);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

    struct curl_slist* headerList = prepareHeaders(headers);
    if (headerList) {
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerList);
    }
}

HttpResponse HttpClientCurl::get(const std::string& path, const std::vector<std::string>& headers) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::string url = buildUrl(path);
    setupCurl(m_curl, url, headers);

    std::string responseBody;
    std::vector<std::string> responseHeaders;

    curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &responseBody);
    curl_easy_setopt(m_curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
    curl_easy_setopt(m_curl, CURLOPT_HEADERDATA, &responseHeaders);

    CURLcode res = curl_easy_perform(m_curl);
    if (res != CURLE_OK) {
        throw std::runtime_error(std::string("GET request failed: ") + curl_easy_strerror(res));
    }

    long statusCode;
    curl_easy_getinfo(m_curl, CURLINFO_RESPONSE_CODE, &statusCode);

    return HttpResponse{static_cast<int>(statusCode), responseBody, responseHeaders};
}

HttpResponse HttpClientCurl::post(const std::string& path, const nlohmann::json& body,
                                const std::vector<std::string>& headers) {
    std::string jsonStr = body.dump();
    std::vector<std::string> newHeaders = headers;
    newHeaders.push_back("Content-Type: application/json");
    return post(path, jsonStr, "application/json", newHeaders);
}

HttpResponse HttpClientCurl::post(const std::string& path, const std::string& body,
                                const std::string& contentType,
                                const std::vector<std::string>& headers) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::string url = buildUrl(path);
    setupCurl(m_curl, url, headers);

    std::string responseBody;
    std::vector<std::string> responseHeaders;

    curl_easy_setopt(m_curl, CURLOPT_POST, 1L);
    curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, body.c_str());
    curl_easy_setopt(m_curl, CURLOPT_POSTFIELDSIZE, body.length());
    curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &responseBody);
    curl_easy_setopt(m_curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
    curl_easy_setopt(m_curl, CURLOPT_HEADERDATA, &responseHeaders);

    CURLcode res = curl_easy_perform(m_curl);
    if (res != CURLE_OK) {
        throw std::runtime_error(std::string("POST request failed: ") + curl_easy_strerror(res));
    }

    long statusCode;
    curl_easy_getinfo(m_curl, CURLINFO_RESPONSE_CODE, &statusCode);

    return HttpResponse{static_cast<int>(statusCode), responseBody, responseHeaders};
}

int HttpClientCurl::postStream(const std::string& path, const nlohmann::json& body,
                             const StreamCallback& callback,
                             const std::vector<std::string>& headers) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::string url = buildUrl(path);
    std::string jsonStr = body.dump();
    
    std::vector<std::string> newHeaders = headers;
    newHeaders.push_back("Content-Type: application/json");
    setupCurl(m_curl, url, newHeaders);

    curl_easy_setopt(m_curl, CURLOPT_POST, 1L);
    curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, jsonStr.c_str());
    curl_easy_setopt(m_curl, CURLOPT_POSTFIELDSIZE, jsonStr.length());
    curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, StreamWriteCallback);
    curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &const_cast<StreamCallback&>(callback));

    CURLcode res = curl_easy_perform(m_curl);
    if (res != CURLE_OK) {
        throw std::runtime_error(std::string("POST stream request failed: ") + curl_easy_strerror(res));
    }

    long statusCode;
    curl_easy_getinfo(m_curl, CURLINFO_RESPONSE_CODE, &statusCode);

    // Call callback one last time to indicate completion
    callback("", true);

    return static_cast<int>(statusCode);
}

} // namespace client
} // namespace sauron 