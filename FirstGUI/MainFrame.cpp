#include "MainFrame.h"
#include <wx/wx.h>
#include <curl/curl.h>
#include <json/json.h>
//#include <BoostPropertyTree>

using namespace std;

MainFrame::MainFrame(const wxString& title) :wxFrame(nullptr, wxID_ANY, title) {
	wxPanel* panel = new wxPanel(this);

	wxButton* download = new wxButton(panel, wxID_ANY, "POBIERZ DANE!", wxPoint(380, 10), wxSize(150, 30), wxBU_EXACTFIT);
	download->Bind(wxEVT_BUTTON, &MainFrame::onDownloadClick, this);

	CreateStatusBar();

	wxStaticText* statictext = new wxStaticText(panel, wxID_ANY, "Wybierz stacje pogodowa", wxPoint(380, 50));
	statictext->SetBackgroundColour(*wxLIGHT_GREY);

	wxArrayString choices;
	for (int i = 0; i < 100; i++) {
		choices.Add("ItemA");
	}
	wxChoice* choice = new wxChoice(panel, wxID_ANY, wxPoint(380, 70), wxSize(150, 100), choices, wxCB_SORT);

	wxStaticText* statictext2 = new wxStaticText(panel, wxID_ANY, "Wybierz stanowiska pomiarowe", wxPoint(380, 100));
	statictext2->SetBackgroundColour(*wxCYAN);

	wxArrayString choices2;
	for (int i = 0; i < 100; i++) {
		choices2.Add("ItemB");
	}
	wxChoice* choice2 = new wxChoice(panel, wxID_ANY, wxPoint(380, 120), wxSize(150, 100), choices2, wxCB_SORT);

	wxButton* accept = new wxButton(panel, wxID_ANY, "Zatwierdz", wxPoint(380, 150), wxSize(150, 30), wxBU_EXACTFIT);

	accept->Bind(wxEVT_BUTTON, &MainFrame::onAcceptClick, this);

	this->Bind(wxEVT_CLOSE_WINDOW, &MainFrame::onClose, this);
}

void MainFrame::onDownloadClick(wxCommandEvent& evt) {
	wxLogMessage("Dane zostały pobrane");
	evt.Skip();
}

void MainFrame::onAcceptClick(wxCommandEvent& evt) {
	wxLogStatus("Dane:");
	evt.Skip();
}

void MainFrame::onClose(wxCloseEvent& evt) {
	wxLogMessage("Zamykanie programu...");
	evt.Skip();
}