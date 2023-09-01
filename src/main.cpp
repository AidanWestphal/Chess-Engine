// Including header files for wxWidgets
#include <wx/wx.h>

class MyApp : public wxApp {
public:
  bool OnInit() override;
};

// Main window of the application. Derive a custom class to store additional
// data.
class MyFrame : public wxFrame {
public:
  MyFrame();

private:
  void OnHello(wxCommandEvent &event);
  void OnExit(wxCommandEvent &event);
  void OnAbout(wxCommandEvent &event);
};

// We have a frame now, so we can create the main window (initialize).
bool MyApp::OnInit() {
  // No memory leak as wxWidgets will destroy all objects upon closure of the
  // window.
  MyFrame *frame = new MyFrame();
  frame->Show();
  return true;
}

// Unique identifiers needed, often done in enum because many need to be
// defined. Note that we don't need identifiers for wxID_ABOUT or wxID_EXIT as
// wxWidgets predefine
enum { ID_Hello = 1 };

// Event handling:
void MyFrame::OnExit(wxCommandEvent& event)
{
    // TRUE: No veto power from other windows (do you want to close?)
    Close(true);
}
void MyFrame::OnAbout(wxCommandEvent& event)
{
    // Small window display with text in it (MessageBox)
    wxMessageBox("This is a wxWidgets Hello World example",
                 "About Hello World", wxOK | wxICON_INFORMATION);
}
void MyFrame::OnHello(wxCommandEvent& event)
{   
    //Shows standard message
    wxLogMessage("Hello world from wxWidgets!");
}


// Define frame contents
MyFrame::MyFrame() : wxFrame(nullptr, wxID_ANY, "Hello World") {
  // Menu dropdown for File and Help tabs
  wxMenu *menuFile = new wxMenu;
  menuFile->Append(ID_Hello, "&Hello...\tCtrl+H",
                   "Help string shown in status bar for this menu item");
  menuFile->AppendSeparator();
  // wxIDs have default text and values, i.e. above information not necessary.
  menuFile->Append(wxID_EXIT);

  // Help menu dropdown with an about tab.
  wxMenu *menuHelp = new wxMenu;
  menuHelp->Append(wxID_ABOUT);

  // Navbar-like menu which contains menus above.
  wxMenuBar *menuBar = new wxMenuBar;
  menuBar->Append(menuFile, "&File");
  menuBar->Append(menuHelp, "&Help");
  // Selects a menuBar to be displayed
  SetMenuBar(menuBar);

  // Status bar displays status of site which can be controlled by above
  // hovering.
  CreateStatusBar();
  SetStatusText("Welcome to wxWidgets!");

  // Connects events from above to our defined actions.
  // Uses functions defined above, but may be better to use lambdas
  Bind(wxEVT_MENU, &MyFrame::OnHello, this, ID_Hello);
  Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);
  Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);
}

// This defines the equivalent of main() for the current platform.
wxIMPLEMENT_APP(MyApp);