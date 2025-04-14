#include "Application.h"
#include "MainFrame.h"

#include <wx/wx.h>
#include <curl/curl.h>
#include <json/json.h>


using namespace std;

wxIMPLEMENT_APP(GUI);

//GUI z wxwidgets - takie jak wielkosc
bool GUI::OnInit() {
    MainFrame* mainFrame = new MainFrame("Stacje pogodowe - Projekt JPO");
    mainFrame->SetClientSize(1600, 900);
    mainFrame->Center();
    mainFrame->Show();
    return true;
}

size_t App::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}
bool App::performCurlRequest(const string& url, string& response) {
    try {
    CURL* curl = curl_easy_init();
    if (!curl) {
        cerr << "Failed to initialize CURL" << endl;
        return false;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;
        return false;
    }

    return true;
    } catch (const std::exception& e) {
        std::cerr << "[B£¥D CURL] " << e.what() << std::endl;
        return false;
    }

}
bool App::parseJsonResponse(const string& jsonResponse, Json::Value& parsedRoot) {
    Json::Reader reader;

    bool parsingSuccessful = reader.parse(jsonResponse, parsedRoot);

    if (!parsingSuccessful) {
        cerr << "Nie uda³o siê sparsowaæ JSON'a " << endl;
        return false;
    }

    return true;
}