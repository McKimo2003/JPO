#pragma once
#include <wx/wx.h>
//#include <json.hpp>

class MainFrame : public wxFrame
{
public:
	MainFrame(const wxString& title);
private:
	void onButtonClick(wxCommandEvent& evt);
	wxDECLARE_EVENT_TABLE();
};