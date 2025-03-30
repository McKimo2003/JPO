#pragma once
#include <wx/wx.h>
#include <iostream>
#include <string>
#include <curl/curl.h>
#include <json/json.h>
#include <locale.h>
using namespace std;

class Application : public wxApp
{
public:
    bool OnInit();

    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
    static bool performCurlRequest(const string& url, string& response);
    static bool parseJsonResponse(const string& jsonResponse, Json::Value& parsedRoot);
};