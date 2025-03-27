#include "Application.h"
#include "MainFrame.h"
#include <wx/wx.h>


wxIMPLEMENT_APP(Application);

bool Application::OnInit() {
	MainFrame* mainFrame = new MainFrame("C++ GUI");
	mainFrame->SetClientSize(800, 800);
	mainFrame->Center();
	mainFrame->Show();
	return true;
}