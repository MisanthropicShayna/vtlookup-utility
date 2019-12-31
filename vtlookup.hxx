#ifndef VTLOOKUP_HXX
#define VTLOOKUP_HXX

// LibCurl
#ifndef CURL_STATICLIB
    #define CURL_STATICLIB
#endif

#include <curl/curl.h>

// OpenSSL
#include <openssl/sha.h>

// Nlohmann JSON
#include <json.hpp>
using Json = nlohmann::json;

// Standard library
#include <functional>
#include <Windows.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <cstdint>
#include <utility>
#include <string>
#include <vector>
#include <array>
#include <map>

class VirusTotalReport {
public:
    // Enum struct that contains error codes returned by most methods in this class.
    enum struct VTERROR;

    // A struct to store response data related to an HTTP request.
    struct HttpResponse {
        std::string Body;
        std::string Header;
        long StatusCode;
    };

protected:
    /* This function uses OpenSSL to calculate the SHA-256 digest of the provided input data
     * and returns a hexadecimal string representation of the digest. */
    static std::string sha256Hexdigest(const std::vector<uint8_t>& input_data);

    /* This is a companion function to the getRequest function. It is provided to LibCurl as a "writer function"
     * meant to accumulate bytes of data received by curl, and append them to a string specified in the getRequest
     * function when setting the CURLOPT_WRITEDATA option via curl_easy_setopt. */
    static std::size_t getRequestWriter(void* data, std::size_t fake_size, std::size_t size, std::string* out_string);

    /* This function uses LibCurl to perform an HTTP/GET request to the specified URL. The body and header can be optionally outputted through
     * the out_body and out_header parameters. The verbose_curl parameter enables CURL verbose debug output to the standard output. */
    static VTERROR getRequest(const std::string& url, HttpResponse* out_response = nullptr, bool verbose_curl = false);

public:
    // Struct to store scan results of a specific engine.
    struct EngineScan {
        std::string EngineVersion;
        std::string Description;
        std::string EngineName;
        std::string ScanDate;
        bool Detected;
    };

    /* Report variables that represent their JSON equivilant in C++.
     * Negatives and DetectionRatio are manually calculated, and not
     * present in the report returned by the VirusToal API endpoint.
    * ========================================================== */
    std::vector<VirusTotalReport::EngineScan> EngineScans;

    std::string FileSha256Hexdigest;
    std::string FileSha1Hexdigest;
    std::string FileMd5Hexdigest;
    std::string ErrorMessage;
    std::string ReportLink;
    std::string Resource;
    std::string ScanDate;
    std::string ScanId;

    uint32_t ResponseCode;
    uint32_t Positives;
    uint32_t Negatives;
    uint32_t ScanCount;

    double DetectionRatio;
    /* ========================================================== */

    // This stores the API key that will be included in any API calls made to VirusTotal.
    // By default, it is assigned through the constructor.
    std::string ApiKey;

    /* Performs an HTTP/GET request to the VirusTotal API endpoint responsible for retrieving
     * reports in JSON format about a particular resource, and outputs the body and header to
     * the out_body and out_header parameters.
     * */
    VTERROR DownloadReport(const std::string& resource, HttpResponse* out_response);

    /* This is an overload of the DownloadReport method that essentially does the same, but
     * outputs the response body as a pre-parsed JSON object through the out_json parameter,
     * and optionally allows the outputting of the body and header as well.
     * */
    VTERROR DownloadReport(const std::string& resource, Json* out_json, HttpResponse* out_response = nullptr);

    /* This method takes a JSON object representing a VirusTotal report, as returned
     * by the VirusTotal API, iterates through the object and loads all of its values
     * into the corresponding member variable.
     * */
    VTERROR LoadReport(const Json& json_report);

    /* This method overload takes a JSON string and converts it into
     * a JSON object before passing it to the overload of LoadReport
     * that takes a JSON object. */
    VTERROR LoadReport(const std::string& raw_report);

    /* This method simply calls DownloadReport and then LoadReport using the provided variables,
     * and returns a pair containing the return values of DownloadReport, and LoadReport respectively.
     * */
    std::pair<VTERROR,VTERROR> DownloadAndLoadReport(const std::string& resource, Json* out_json = nullptr, HttpResponse* out_response = nullptr);

    /* Resets the report data stored in this instance's member variables,
     * used to ensure that no old data persists if new data fails to overwrite
     * the old data when loading a new report. */
    void ResetReportData();

    VirusTotalReport(const std::string& api_key);
    virtual ~VirusTotalReport();
};


#endif // VTLOOKUP_HXX