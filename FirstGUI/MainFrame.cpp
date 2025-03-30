#include "MainFrame.h"
#include "Application.h"
#include <wx/wx.h>
#include <string>
#include <curl/curl.h>
#include <json/json.h>
#include <locale.h>

using namespace std;

MainFrame::MainFrame(const wxString& title) :wxFrame(nullptr, wxID_ANY, title) { //GUI
	wxPanel* panel = new wxPanel(this); //panel g³ówny

	wxBoxSizer* boxSizer = new wxBoxSizer(wxVERTICAL);
	wxSizerFlags flags = wxSizerFlags().Left().Border(wxALL, 10);

	wxStaticText* statictext1 = new wxStaticText(panel, wxID_ANY, "Wybierz stacje pogodowa"); //tekst
	statictext1->SetBackgroundColour(*wxLIGHT_GREY); //zmiana teskstu (t³o)

	setlocale(LC_CTYPE, "Polish");

	string api_url = "https://api.gios.gov.pl/pjp-api/rest/station/findAll";
	string response;

	/*if (Application::performCurlRequest(api_url, response)) {
		Json::Value root;
		if (Application::parseJsonResponse(response, root)) {
			wxArrayString choices1;
			for (const auto& station : root) {
				if (station.isMember("stationName")) {
					choices1.Add(station["stationName"].asString());
				}
			}
			wxChoice* choice1 = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, choices1, wxCB_SORT);
			boxSizer->Add(choice1, flags);
			wxLogMessage("Dane zosta³y pobrane automatycznie.");
		}
	}*/

	wxStaticText* statictext2 = new wxStaticText(panel, wxID_ANY, "Wybierz stanowiska pomiarowe", wxPoint(80, 100));
	statictext2->SetBackgroundColour(*wxCYAN); //zmiana teskstu (t³o)

	wxArrayString choices2;
	wxChoice* choice2 = new wxChoice(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, choices2, wxCB_SORT);

	wxButton* accept = new wxButton(panel, wxID_ANY, "Zatwierdz", wxDefaultPosition, wxSize(150, 30), wxBU_EXACTFIT);

	accept->Bind(wxEVT_BUTTON, &MainFrame::onAcceptClick, this);

	this->Bind(wxEVT_CLOSE_WINDOW, &MainFrame::onClose, this);

	boxSizer->Add(accept,flags);
	boxSizer->Add(statictext1, flags);
	boxSizer->Add(statictext2, flags);
	boxSizer->Add(choice2, flags);

	panel->SetSizer(boxSizer);
	boxSizer->SetSizeHints(this);
}

/*void MainFrame::onAcceptClick(wxCommandEvent& evt) {
	wxLogStatus("Dane:");
	evt.Skip();
}*/

void MainFrame::onClose(wxCloseEvent& evt) {
	wxLogMessage("Zamykanie programu...");
	evt.Skip();
}