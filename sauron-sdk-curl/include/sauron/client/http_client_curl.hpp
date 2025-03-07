#pragma once

#include <sauron/client/HttpClient.hpp>
#include <map>
#include <mutex>
#include "sauron_sdk_curl_export.h"

typedef void CURL;
struct curl_slist;

namespace sauron {
namespace client {

class SAURON_SDK_CURL_API HttpClientCurl : public HttpClient {
public:
    HttpClientCurl(const std::string& baseUrl = "");
    ~HttpClientCurl() override;

    void setBaseUrl(const std::string& url) override;
    std::string getBaseUrl() const override;
    void setDefaultHeader(const std::string& name, const std::string& value) override;
    void removeDefaultHeader(const std::string& name) override;
    void setBearerToken(const std::string& token) override;
    void clearAuthorization() override;

    HttpResponse get(const std::string& path, 
                    const std::vector<std::string>& headers = {}) override;

    HttpResponse post(const std::string& path, 
                     const nlohmann::json& body,
                     const std::vector<std::string>& headers = {}) override;

    HttpResponse post(const std::string& path, 
                     const std::string& body,
                     const std::string& contentType,
                     const std::vector<std::string>& headers = {}) override;

    int postStream(const std::string& path, 
                  const nlohmann::json& body,
                  const StreamCallback& callback,
                  const std::vector<std::string>& headers = {}) override;

private:
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp);
    static size_t StreamWriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
    static size_t HeaderCallback(char* buffer, size_t size, size_t nitems, void* userdata);

    void setupCurl(CURL* curl, const std::string& url, const std::vector<std::string>& headers);
    std::string buildUrl(const std::string& path) const;
    struct curl_slist* prepareHeaders(const std::vector<std::string>& additionalHeaders) const;

    CURL* m_curl;
    std::string m_baseUrl;
    std::map<std::string, std::string> m_defaultHeaders;
    mutable std::mutex m_mutex;
    bool m_globalInit;
};

} // namespace client
} // namespace sauron
