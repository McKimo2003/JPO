#pragma once
#include <wx/wx.h>
#include <map>

class MainFrame : public wxFrame
{
public:
	MainFrame(const wxString& title);

private:
	//Wybory stacji, wyswietlanie tekstow - wskazniki
	wxChoice* m_stationChoice;
	wxChoice* m_measurementChoice;
	wxTextCtrl* m_dataDisplay;

	//Id Stacji
	std::map<wxString, int> m_stationMap;

	//Eventy np: klikanie, zamykanie, wybor
	void onAcceptClick(wxCommandEvent& evt);
	void onClose(wxCloseEvent& evt);
	void onStationSelect(wxCommandEvent& evt);
};