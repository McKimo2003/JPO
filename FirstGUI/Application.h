#pragma once
#include <wx/wx.h>
#include <curl/curl.h>
#include <json/json.h>

using namespace std;

class GUI : public wxApp
{
public:
    bool OnInit();
};

class App : public wxApp {
public:
    static bool parseJsonResponse(const string& jsonResponse, Json::Value& parsedRoot);
    static bool performCurlRequest(const string& url, string& response);
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
};