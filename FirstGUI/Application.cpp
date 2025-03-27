#include "Application.h"
#include "MainFrame.h"
#include <wx/wx.h>


wxIMPLEMENT_APP(Application);

bool Application::OnInit() {
	MainFrame* mainFrame = new MainFrame("C++ GUI");
	mainFrame->SetClientSize(900, 900);
	mainFrame->Center();
	mainFrame->Show();
	return true;
}