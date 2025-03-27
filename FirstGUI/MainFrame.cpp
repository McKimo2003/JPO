#include "MainFrame.h"
#include <wx/wx.h>
//#include <BoostPropertyTree>

enum IDs {
	BUTTON_ID = 2,
	TEXT_ID = 4
};

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
	EVT_BUTTON(BUTTON_ID, MainFrame::onButtonClick)
wxEND_EVENT_TABLE()

MainFrame::MainFrame(const wxString& title) :wxFrame(nullptr,wxID_ANY,title) {
	wxPanel* panel = new wxPanel(this);

	//wxCheckBox* checkbox = new wxCheckBox(panel,wxID_ANY,"CheckBox-Test",wxPoint(250,275));

	wxButton* download = new wxButton(panel, BUTTON_ID, "POBIERZ DANE!", wxPoint(380, 10), wxSize(150, 30), wxBU_EXACTFIT);

	CreateStatusBar();

	wxStaticText* statictext = new wxStaticText(panel, TEXT_ID, "Wybierz stacjê pogodow¹", wxPoint(380, 50));
	statictext->SetBackgroundColour(*wxLIGHT_GREY);

	wxArrayString choices;
	for (int i = 0; i < 100; i++) {
		choices.Add("ItemA");
	}
	wxChoice* choice = new wxChoice(panel, wxID_ANY, wxPoint(380, 70), wxSize(150,100), choices, wxCB_SORT);

	wxStaticText* statictext2 = new wxStaticText(panel, wxID_ANY, "Wybierz stanowiska pomiarowe", wxPoint(380, 100));
	statictext2->SetBackgroundColour(*wxCYAN);

	wxArrayString choices2;
	for (int i = 0; i < 100; i++) {
		choices2.Add("ItemB");
	}
	wxChoice* choice2 = new wxChoice(panel, wxID_ANY, wxPoint(380, 120), wxSize(150, 100), choices2, wxCB_SORT);

	wxButton* button = new wxButton(panel, wxID_ANY, "ZatwierdŸ", wxPoint(380, 150), wxSize(150, 30), wxBU_EXACTFIT);
}

void MainFrame::onButtonClick(wxCommandEvent& evt) {
	wxLogStatus("Pobieranie danych...");
}