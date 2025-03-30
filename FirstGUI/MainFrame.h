#pragma once
#include <wx/wx.h>
//#include <json.hpp>

class MainFrame : public wxFrame
{
public:
	MainFrame(const wxString& title);
private:
	void onDownloadClick(wxCommandEvent& evt);
	void onAcceptClick(wxCommandEvent& evt);
	void onClose(wxCloseEvent& evt);
};