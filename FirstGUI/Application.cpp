#include "Application.h"
#include "MainFrame.h"
#include <wx/wx.h>
#include <iostream>
#include <string>
#include <curl/curl.h>
#include <json/json.h>
#include <locale.h>
using namespace std;

wxIMPLEMENT_APP(Application);

bool Application::OnInit() {
    MainFrame* mainFrame = new MainFrame("C++ GUI");
    mainFrame->SetClientSize(900, 900);
    mainFrame->Center();
    mainFrame->Show();
    return true;
}

size_t Application::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

bool Application::performCurlRequest(const string& url, string& response) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        cerr << "Brak po³¹czenia z internetem" << endl;
        return false;
    }
    response.clear();
    CURLcode result;

    result = curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    if (result != CURLE_OK) {
        cerr << "curl_easy_setopt(CURLOPT_URL) b³¹d: " << curl_easy_strerror(result) << endl;
        curl_easy_cleanup(curl);
        return false;
    }

    result = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Application::WriteCallback);
    if (result != CURLE_OK) {
        cerr << "curl_easy_setopt(CURLOPT_WRITEFUNCTION) b³¹d: " << curl_easy_strerror(result) << endl;
        curl_easy_cleanup(curl);
        return false;
    }

    result = curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    if (result != CURLE_OK) {
        cerr << "curl_easy_setopt(CURLOPT_WRITEDATA) b³¹d: " << curl_easy_strerror(result) << endl;
        curl_easy_cleanup(curl);
        return false;
    }

    result = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    if (result != CURLE_OK) {
        cerr << "curl_easy_setopt(CURLOPT_FOLLOWLOCATION) b³¹d: " << curl_easy_strerror(result) << endl;
        curl_easy_cleanup(curl);
        return false;
    }
    result = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (result != CURLE_OK) {
        cerr << "curl_easy_perform() b³¹d: " << curl_easy_strerror(result) << endl;
        return false;
    }

    return true;
}

bool Application::parseJsonResponse(const string& jsonResponse, Json::Value& parsedRoot) {
    Json::Reader reader;
    bool parsingSuccessful = reader.parse(jsonResponse, parsedRoot);
    if (!parsingSuccessful) {
        return false;
    }
    return true;
}