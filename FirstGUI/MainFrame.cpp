#include "MainFrame.h"
#include <wx/wx.h>

MainFrame::MainFrame(const wxString& title) :wxFrame(nullptr,wxID_ANY,title) {
	wxPanel* panel = new wxPanel(this);

	wxButton* button = new wxButton(panel,wxID_ANY,"Przycisk",wxPoint(275,375),wxSize(250,50));

	wxCheckBox* checkbox = new wxCheckBox(panel,wxID_ANY,"CheckBox-Test",wxPoint(250,275));
}