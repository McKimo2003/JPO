#include "MainFrame.h"
#include "Application.h"

#include <curl/curl.h>
#include <json/json.h>
#include <wx/wx.h>

using namespace std;

MainFrame::MainFrame(const wxString& title) :wxFrame(nullptr, wxID_ANY, title) { //GUI
	wxPanel* panel = new wxPanel(this); //panel g³ówny
	
	//BoxSizer(obszar)
	wxBoxSizer* boxSizer = new wxBoxSizer(wxVERTICAL);
	wxSizerFlags flags = wxSizerFlags().Left().Border(wxALL, 10); //wyrównanie oraz jaka odleglosc od kazdej krawedzi

	//Text statyczny nr:1
	wxStaticText* statictext1 = new wxStaticText(panel, wxID_ANY, "Wybierz stacje pogodowa");
	statictext1->SetBackgroundColour(*wxLIGHT_GREY); //zmiana teskstu (t³o)
	boxSizer->Add(statictext1, flags); //dodawanie tekstu do BoxSizera

	//nawiazywanie polaczenia z baza danych json + dodawanie wyboru stacji
	setlocale(LC_CTYPE, "Polish");

	string api_url = "https://api.gios.gov.pl/pjp-api/rest/station/findAll";
	string response;
	wxArrayString choices1;
	curl_global_init(CURL_GLOBAL_DEFAULT);
	if (App::performCurlRequest(api_url, response)) {
		Json::Value root;
		if(App::parseJsonResponse(response, root)) {
			//dodawanie stacji z bazy danych do listy
			for (Json::Value::const_iterator outer = root.begin(); outer != root.end(); outer++){
				wxString station = wxString::FromUTF8(root[outer.index()]["stationName"].asCString());
				int stationId = root[outer.index()]["id"].asInt();
				choices1.Add(station);
				m_stationMap[station] = stationId;
			}
		}
		else {
			wxLogMessage("B³¹d pobierania danych");
		}
	}
	curl_global_cleanup();

	//dodawanie stacji do polca wyboru
	m_stationChoice = new wxChoice(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, choices1, wxCB_SORT);
	m_stationChoice->Bind(wxEVT_CHOICE, &MainFrame::onStationSelect, this);
	boxSizer->Add(m_stationChoice, flags);

	//Tekst statyczny nr: 2
	wxStaticText* statictext2 = new wxStaticText(panel, wxID_ANY, "Wybierz stanowiska pomiarowe");
	statictext2->SetBackgroundColour(*wxCYAN);
	boxSizer->Add(statictext2, flags);



	//Wybor tego co moze wyswietlac stacja
	wxArrayString choices2;
	m_measurementChoice = new wxChoice(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, choices2, wxCB_SORT);
	boxSizer->Add(m_measurementChoice, flags);

	//Przycisk do zatwierdzania wyboru stacji oraz danych
	wxButton* accept = new wxButton(panel, wxID_ANY, "Zatwierdz", wxDefaultPosition, wxSize(150, 30), wxBU_EXACTFIT);
	accept->Bind(wxEVT_BUTTON, &MainFrame::onAcceptClick, this);
	boxSizer->Add(accept, flags);

	m_dataDisplay = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxSize(600, 300), wxTE_MULTILINE | wxTE_READONLY);
	boxSizer->Add(m_dataDisplay, flags);

	//wylaczanie programu
	this->Bind(wxEVT_CLOSE_WINDOW, &MainFrame::onClose, this);

	//Przypisywanie panelu zeby dzialal z boxSizerem (wartosci typu odleglosci od krawedzi dla wszystkich obiektow
	panel->SetSizer(boxSizer);
	boxSizer->SetSizeHints(this);
}

void MainFrame::onAcceptClick(wxCommandEvent& evt) {
	wxString selectedStation = m_stationChoice->GetStringSelection();
	wxString selectedParam = m_measurementChoice->GetStringSelection();

	if (selectedStation.IsEmpty() || selectedParam.IsEmpty()) {
		wxLogMessage("Wybierz stacjê i parametr.");
		return;
	}

	int stationId = m_stationMap[selectedStation];

	// Krok 1: Pobierz sensory stacji
	string sensorUrl = "https://api.gios.gov.pl/pjp-api/rest/station/sensors/" + to_string(stationId);
	string sensorResponse;

	if (!App::performCurlRequest(sensorUrl, sensorResponse)) {
		wxLogMessage("Nie uda³o siê pobraæ listy sensorów.");
		return;
	}
	Json::Value sensorsRoot;
	if (!App::parseJsonResponse(sensorResponse, sensorsRoot)) {
		wxLogMessage("B³¹d parsowania JSON (sensory).");
		return;
	}
	int sensorId = -1;
	for (const auto& sensor : sensorsRoot) {
		string paramName = sensor["param"]["paramName"].asString();
		if (paramName == selectedParam.ToStdString()) {
			sensorId = sensor["id"].asInt();
			break;
		}
	}

	if (sensorId == -1) {
		wxLogMessage("Nie znaleziono sensora dla wybranego parametru.");
		return;
	}
	string dataUrl = "https://api.gios.gov.pl/pjp-api/rest/data/getData/" + to_string(sensorId);
	string dataResponse;

	if (!App::performCurlRequest(dataUrl, dataResponse)) {
		wxLogMessage("Nie uda³o siê pobraæ danych pomiarowych.");
		return;
	}

	Json::Value dataRoot;
	if (!App::parseJsonResponse(dataResponse, dataRoot)) {
		wxLogMessage("B³¹d parsowania JSON (dane).");
		return;
	}

	const Json::Value& values = dataRoot["values"];
	wxString display;

	for (const auto& v : values) {
		string date = v["date"].asString();
		string value = v["value"].isNull() ? "brak danych" : v["value"].asString();
		display += wxString::Format("Data: %s  |  Wartoœæ: %s\n", date, value);
	}

	m_dataDisplay->SetValue(display);
	evt.Skip();
}

void MainFrame::onClose(wxCloseEvent& evt) {
	wxLogMessage("Zamykanie programu...");
	evt.Skip();
}

void MainFrame::onStationSelect(wxCommandEvent& evt) {
	wxString selectedStation = m_stationChoice->GetStringSelection();
	int stationId = m_stationMap[selectedStation];
	string url = "https://api.gios.gov.pl/pjp-api/rest/station/sensors/" + to_string(stationId);
	string response;

	if (App::performCurlRequest(url, response)) {
		Json::Value root;
		if (App::parseJsonResponse(response, root)) {
			wxArrayString measurements;
			for (Json::Value::const_iterator it = root.begin(); it != root.end(); ++it) {
				string paramName = (*it)["param"]["paramName"].asString();
				measurements.Add(wxString::FromUTF8(paramName));
			}
			m_measurementChoice->Clear();
			m_measurementChoice->Append(measurements);
		}
		else {
			wxLogMessage("B³¹d parsowania JSON (sensory)");
		}
	}
	else {
		wxLogMessage("B³¹d pobierania danych sensorycznych");
	}
}