#include <wx/wx.h>
#include <iostream>
#include <string>


using namespace std;

class App : public wxApp {
public:
    bool OnInit() {
        wxFrame* window = new wxFrame(NULL, wxID_ANY, "GUI Test", wxDefaultPosition, wxSize(600, 400));
        wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
        wxStaticText* text = new wxStaticText(window, wxID_ANY, "Well Done!\nEverything seems to be working",
            wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL);
        text->SetFont(wxFont(20, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
        sizer->Add(text, 1, wxALIGN_CENTER);
        window->SetSizer(sizer);
        window->Show();
        return true;
    }
};

wxIMPLEMENT_APP(App);

// Callback function to handle curl's response
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    /*
    * userp - pointer to user data, we cast void ptr to string and append the content
    * to the userp - user data. Finally the number of bytes that we taken care of is returned
    */
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Function to perform CURL request
bool performCurlRequest(const string& url, string& response) {
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
}

// Function to parse JSON response
bool parseJsonResponse(const string& jsonResponse, Json::Value& parsedRoot) {
    Json::Reader reader;

    bool parsingSuccessful = reader.parse(jsonResponse, parsedRoot);

    if (!parsingSuccessful) {
        cerr << "Failed to parse JSON: " << endl;
        return false;
    }

    return true;
}

int main() {

    setlocale(LC_CTYPE, "Polish"); // for Polish characters

    string api_url = "https://api.gios.gov.pl/pjp-api/rest/station/findAll";
    string response;
    curl_global_init(CURL_GLOBAL_DEFAULT);
    if (performCurlRequest(api_url, response)) {
        Json::Value root;
        if (parseJsonResponse(response, root)) {
            for (Json::Value::const_iterator outer = root.begin(); outer != root.end(); outer++)
            {
                cout << "Station: " << root[outer.index()]["stationName"];
            }
        }
    }

    curl_global_cleanup();
    return 0;
}
