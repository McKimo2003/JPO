#include "MainFrame.h"
#include "Application.h"

#include <curl/curl.h>
#include <json/json.h>
#include <wx/wx.h>

using namespace std;

MainFrame::MainFrame(const wxString& title) :wxFrame(nullptr, wxID_ANY, title) { //GUI
	wxPanel* panel = new wxPanel(this); //panel g��wny
	
	//BoxSizer(obszar)
	wxBoxSizer* boxSizer = new wxBoxSizer(wxVERTICAL);
	wxSizerFlags flags = wxSizerFlags().Left().Border(wxALL, 10); //wyr�wnanie oraz jaka odleglosc od kazdej krawedzi

	//Text statyczny nr:1
	wxStaticText* statictext1 = new wxStaticText(panel, wxID_ANY, "Wybierz stacje pogodowa");
	statictext1->SetBackgroundColour(*wxLIGHT_GREY); //zmiana teskstu (t�o)
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
			wxLogMessage("B��d pobierania danych");
		}
	}
	curl_global_cleanup();

	//dodawanie stacji do polca wyboru
	m_stationChoice = new wxChoice(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, choices1, wxCB_SORT);
	m_stationChoice->Bind(wxEVT_CHOICE, &MainFrame::onStationSelect, this);
	boxSizer->Add(m_stationChoice, flags);

	//Tekst statyczny nr: 2
	wxStaticText* statictext2 = new wxStaticText(panel, wxID_ANY, "Wybierz stanowiska pomiarowe");
	statictext2->SetBackgroundColour(*wxLIGHT_GREY);
	boxSizer->Add(statictext2, flags);

	//Wybor tego co moze wyswietlac stacja
	wxArrayString choices2;
	m_measurementChoice = new wxChoice(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, choices2, wxCB_SORT);
	boxSizer->Add(m_measurementChoice, flags);

	//Przycisk do zatwierdzania wyboru stacji oraz sensor�w do pokazania danych
	wxButton* accept = new wxButton(panel, wxID_ANY, "Zatwierdz", wxDefaultPosition, wxSize(150, 30), wxBU_EXACTFIT);
	accept->Bind(wxEVT_BUTTON, &MainFrame::onAcceptClick, this);
	boxSizer->Add(accept, flags);

	//Text statyczny nr:3
	wxStaticText* statictext3 = new wxStaticText(panel, wxID_ANY, "Dane wy�wietlane tekstowo (od najnowszych wpis�w):");
	statictext3->SetBackgroundColour(*wxCYAN); //zmiana teskstu (t�o)
	boxSizer->Add(statictext3, flags); //dodawanie tekstu do BoxSizera

	m_dataDisplay = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxSize(1400, 400), wxTE_MULTILINE | wxTE_READONLY);
	boxSizer->Add(m_dataDisplay, flags);

	//Text statyczny nr:4
	wxStaticText* statictext4 = new wxStaticText(panel, wxID_ANY, "Dane wy�wietlane na wykresie, w celu zaktualizowania filtr�w oraz danych na wykresie naci�nij przycisk 'Filtruj zakres danych'");
	statictext4->SetBackgroundColour(*wxGREEN); //zmiana teskstu (t�o)
	boxSizer->Add(statictext4, flags); //dodawanie tekstu do BoxSizera

	//Graf
	wxBoxSizer* timeSizer = new wxBoxSizer(wxHORIZONTAL);

	//Dodawanie filtr�w daty do wykresu - graf
	m_startDateTime = new wxChoice(panel, wxID_ANY); //pocz�tkowa data filtru
	m_endDateTime = new wxChoice(panel, wxID_ANY); //ko�cowa data filtru
	m_filterButton = new wxButton(panel, wxID_ANY, "Filtruj zakres danych"); //przycisk filtruj�cy

	timeSizer->Add(new wxStaticText(panel, wxID_ANY, "Od (YYYY-MM-DD HH:MM):"), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	timeSizer->Add(m_startDateTime, 0, wxALL, 5);
	timeSizer->Add(new wxStaticText(panel, wxID_ANY, "Do (YYYY-MM-DD HH:MM):"), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	timeSizer->Add(m_endDateTime, 0, wxALL, 5);
	timeSizer->Add(m_filterButton, 0, wxALL, 5);

	boxSizer->Add(timeSizer, flags);

	m_filterButton->Bind(wxEVT_BUTTON, &MainFrame::onFilterClick, this);

	//Max oraz Min dla wykresu - po filtracji
	m_statsText = new wxStaticText(panel, wxID_ANY, "Min: - | Max: -");
	timeSizer->Add(m_statsText, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	//wyswietlanie grafu
	m_graphPanel = new Graph(panel);
	boxSizer->Add(m_graphPanel, flags);
	
	//wylaczanie programu
	this->Bind(wxEVT_CLOSE_WINDOW, &MainFrame::onClose, this);

	//Przypisywanie panelu zeby dzialal z boxSizerem (wartosci typu odleglosci od krawedzi dla wszystkich obiektow
	panel->SetSizer(boxSizer);
	boxSizer->SetSizeHints(this);
}




void MainFrame::onAcceptClick(wxCommandEvent& evt) {
	wxString selectedStation = m_stationChoice->GetStringSelection();
	wxString selectedParam = m_measurementChoice->GetStringSelection();

	//Je�eli nie ma wybranych stacji lub sensor�w
	if (selectedStation.IsEmpty() || selectedParam.IsEmpty()) {
		wxLogMessage("Wybierz stacj� i parametr.");
		return;
	}

	int stationId = m_stationMap[selectedStation];

	//Pobieranie danych ze stacji
	string sensorUrl = "https://api.gios.gov.pl/pjp-api/rest/station/sensors/" + to_string(stationId);
	string sensorResponse;

	//B��d pobierania listy sensor�w
	if (!App::performCurlRequest(sensorUrl, sensorResponse)) {
		wxLogMessage("Nie uda�o si� pobra� listy sensor�w.");
		return;
	}

	//B��d JSON'a
	Json::Value sensorsRoot;
	if (!App::parseJsonResponse(sensorResponse, sensorsRoot)) {
		wxLogMessage("B��d parsowania JSON (sensory).");
		return;
	}

	//
	int sensorId = -1;
	//Dodawanie sensora sensor�w do listy
	for (const auto& sensor : sensorsRoot) {
		string paramName = sensor["param"]["paramName"].asString();
		if (paramName == selectedParam.ToStdString()) {
			sensorId = sensor["id"].asInt();
			break;
		}
	}

	//Nie znaleziono sensora - b��d
	if (sensorId == -1) {
		wxLogMessage("Nie znaleziono sensora dla wybranego parametru.");
		return;
	}

	//Pobieranie danych sensora
	string dataUrl = "https://api.gios.gov.pl/pjp-api/rest/data/getData/" + to_string(sensorId);
	string dataResponse;

	//Nie ma listy pomiar�w
	if (!App::performCurlRequest(dataUrl, dataResponse)) {
		wxLogMessage("Nie uda�o si� pobra� danych pomiarowych.");
		return;
	}

	//B��d parsowania JSON'a
	Json::Value dataRoot;
	if (!App::parseJsonResponse(dataResponse, dataRoot)) {
		wxLogMessage("B��d parsowania JSON (dane).");
		return;
	}

	//Zapisywanie warto�ci z JSON'a do warto�ci w tablicy
	const Json::Value& values = dataRoot["values"];
	wxString display;

	//Tworzenie pola vecotora(tablicy) z danymi osie X i Y
	vector<double> xData, yData;

	m_dateTimes.clear();
	m_startDateTime->Clear();
	m_endDateTime->Clear();
	m_allYData.clear();

	//Przypisywanie danych do parametrow i daty + tworzenie wykres�w
	for (int index = 0; index < values.size();++index) {
		const auto& v = values[index];
		string date = v["date"].asString();
		string value = v["value"].isNull() ? "0" : v["value"].asString();
		display += wxString::Format("Data: %s  |  Warto��: %s\n", date, value);

		//Data (do zakres�w)
		wxDateTime dt;
		bool parsed = false;

		if (!parsed && dt.ParseISOCombined(date)) {
			parsed = true;
		}

		if (!parsed && dt.ParseDateTime(date)) {
			parsed = true;
		}

		if (!parsed) {
			wxString wxDateStr = wxString::FromUTF8(date);
			parsed = dt.ParseFormat(wxDateStr, "%Y-%m-%d %H:%M:%S");
		}

		if (!v["value"].isNull()) {
			wxDateTime dt;
			bool parsed = false;

			if (dt.ParseISOCombined(date)) parsed = true;
			else if (dt.ParseDateTime(date)) parsed = true;
			else if (dt.ParseFormat(wxString::FromUTF8(date), "%Y-%m-%d %H:%M:%S")) parsed = true;

			if (parsed) {
				m_dateTimes.push_back(dt);
				wxString formatted = dt.FormatISOCombined(' ');
				m_startDateTime->Append(formatted);
				m_endDateTime->Append(formatted);

				double val = v["value"].asDouble();
				xData.push_back(index);
				yData.push_back(val);
				m_allYData.push_back(val);
				index++;
			}
			else {
				wxLogMessage("B��d z parsowania daty: %s", date);
			}
		}
	}

	if (m_dateTimes.size() == m_allYData.size() && !m_allYData.empty()) {
		m_dataDisplay->SetValue(display);
	}
	else {
		wxLogMessage("Dane nie s� kompletne. - podaj zakres dat");
	}
}

//Po naci�ni�ciu filtrowania (wybor przedzialu czasowego)
void MainFrame::onFilterClick(wxCommandEvent& evt) {
	int startIdx = m_startDateTime->GetSelection();
	int endIdx = m_endDateTime->GetSelection();

	if (startIdx == wxNOT_FOUND || endIdx == wxNOT_FOUND) {
		wxLogMessage("Prosz� wybra� daty pocz�tku i ko�ca");
		return;
	}

	//Sprawdzanie indeks�w i dat
	if (startIdx >= m_dateTimes.size() || endIdx >= m_dateTimes.size()) {
		wxLogMessage("B��d: Indeks daty poza zakresem");
		return;
	}

	wxDateTime start = m_dateTimes[startIdx];
	wxDateTime end = m_dateTimes[endIdx];

	//Zabezpieczenie przed ustawieniem �le daty (na odwr�t)
	if (start > end) {
		wxLogMessage("Data pocz�tkowa musi by� wcze�niejsza ni� ko�cowa");
		return;
	}

	std::vector<double> filteredX, filteredY;
	wxString filteredDisplay;
	int index = 0;

	//Sprawdzenie czy vektory maja te same wielkosci (czy sa zgodne)
	if (m_dateTimes.size() != m_allYData.size()) {
		wxLogMessage("B��d: Niezgodno�� rozmiar�w tablic dat i warto�ci");
		return;
	}

	//sprawdzenie czy i jest zgodne z zakresem vektora od daty
	for (size_t i = 0; i < m_dateTimes.size(); ++i) {
		if (i >= m_allYData.size()) {
			wxLogMessage("B��d: Indeks poza zakresem tablicy warto�ci");
			break;
		}

		//je�li jest ok to dodajemy do przefiltrowanych warto�ci
		if (m_dateTimes[i].IsBetween(start, end) || m_dateTimes[i].IsEqualTo(start) || m_dateTimes[i].IsEqualTo(end)) {
			filteredX.push_back(index++);
			filteredY.push_back(m_allYData[i]);

			wxString formatted = m_dateTimes[i].Format("%Y-%m-%d %H:%M:%S");
			filteredDisplay += wxString::Format("Data: %s  |  Warto��: %.2f\n",
				formatted, m_allYData[i]);
		}
	}

	if (filteredX.empty()) {
		wxLogMessage("Brak danych w podanym zakresie.");
		return;
	}

	m_dataDisplay->SetValue(filteredDisplay);
	m_graphPanel->SetData(filteredX, filteredY);


	//Min i max wykresu - po dodaniu filtracji odpowiedniej
	if (!filteredY.empty()) {
		auto minmax = std::minmax_element(filteredY.begin(), filteredY.end());
		wxString stats = wxString::Format("Min: %.2f | Max: %.2f", *minmax.first, *minmax.second);
		m_statsText->SetLabel(stats);
	}
}


void MainFrame::onClose(wxCloseEvent& evt) {
	wxLogMessage("Zamykanie programu...");
	evt.Skip();
}

//Po wyborze stacji - pobieranie sensor�w
void MainFrame::onStationSelect(wxCommandEvent& evt) {
	wxString selectedStation = m_stationChoice->GetStringSelection();
	int stationId = m_stationMap[selectedStation];
	string url = "https://api.gios.gov.pl/pjp-api/rest/station/sensors/" + to_string(stationId);
	string response;

	//Pobieranie danych z JSON'a (jakie pomiary mo�e mie� dana stacja)
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
			wxLogMessage("B��d parsowania JSON (sensory)");
		}
	}
	else {
		wxLogMessage("B��d pobierania danych sensorycznych");
	}
}