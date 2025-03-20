// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
#include <wx/aui/aui.h>

#include <wx/preferences.h>

#include <wx/artprov.h>

#include <algorithm>

#ifndef EXT_SAMPLE
#include "wx/wxprec.h"


#include "wx/app.h"
#include "wx/grid.h"
#include "wx/treectrl.h"
#include "wx/spinctrl.h"
#include "wx/artprov.h"
#include "wx/clipbrd.h"
#include "wx/image.h"
#include "wx/colordlg.h"
#include "wx/wxhtml.h"
#include "wx/imaglist.h"
#include "wx/dataobj.h"
#include "wx/dcclient.h"
#include "wx/bmpbuttn.h"
#include "wx/menu.h"
#include "wx/toolbar.h"
#include "wx/statusbr.h"
#include "wx/msgdlg.h"
#include "wx/textdlg.h"

#include "wx/aui/aui.h"
#include "../sample.xpm"

#include "wx/propgrid/propgrid.h"
#include "wx/propgrid/advprops.h"

//#include "Model/Node.h"
// -- application --

/*class MyApp : public wxApp
{
public:
    bool OnInit() wxOVERRIDE;
};

wxDECLARE_APP(MyApp);
wxIMPLEMENT_APP(MyApp);*/


class wxSizeReportCtrl;

// -- frame --

class MyFrame : public wxFrame
{
    enum
    {
        ID_CreateTree = wxID_HIGHEST+1,
        ID_CreateGrid,
        ID_CreateText,
        ID_CreateHTML,
        ID_CreateNotebook,
        ID_CreateSizeReport,
        ID_GridContent,
        ID_TextContent,
        ID_TreeContent,
        ID_HTMLContent,
        ID_NotebookContent,
        ID_SizeReportContent,
        ID_CreatePerspective,
        ID_CopyPerspectiveCode,
        ID_AllowFloating,
        ID_AllowActivePane,
        ID_TransparentHint,
        ID_VenetianBlindsHint,
        ID_RectangleHint,
        ID_NoHint,
        ID_HintFade,
        ID_NoVenetianFade,
        ID_TransparentDrag,
        ID_NoGradient,
        ID_VerticalGradient,
        ID_HorizontalGradient,
        ID_LiveUpdate,
        ID_AllowToolbarResizing,
        ID_Settings,
        ID_CustomizeToolbar,
        ID_DropDownToolbarItem,
        ID_NotebookNoCloseButton,
        ID_NotebookCloseButton,
        ID_NotebookCloseButtonAll,
        ID_NotebookCloseButtonActive,
        ID_NotebookAllowTabMove,
        ID_NotebookAllowTabExternalMove,
        ID_NotebookAllowTabSplit,
        ID_NotebookWindowList,
        ID_NotebookScrollButtons,
        ID_NotebookTabFixedWidth,
        ID_NotebookArtGloss,
        ID_NotebookArtSimple,
        ID_NotebookAlignTop,
        ID_NotebookAlignBottom,

        ID_SampleItem,

        ID_FirstPerspective = ID_CreatePerspective+1000
    };

public:
    MyFrame(wxWindow* parent,
            wxWindowID id,
            const wxString& title,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxDEFAULT_FRAME_STYLE | wxSUNKEN_BORDER);

    wxAuiDockArt* GetDockArt();
    void DoUpdate();

private:
    wxTextCtrl* CreateTextCtrl(const wxString& text = wxEmptyString);
    wxGrid* CreateGrid();
    wxTreeCtrl* CreateTreeCtrl();
    wxSizeReportCtrl* CreateSizeReportCtrl(const wxSize &size = wxWindow::FromDIP(wxSize(80, 80), NULL));
    wxPoint GetStartPosition();
    wxHtmlWindow* CreateHTMLCtrl(wxWindow* parent = NULL);
    wxAuiNotebook* CreateNotebook();

    wxString GetIntroText();

private:

    void OnEraseBackground(wxEraseEvent& evt);
    void OnSize(wxSizeEvent& evt);

    void OnCreateTree(wxCommandEvent& evt);
    void OnCreateGrid(wxCommandEvent& evt);
    void OnCreateHTML(wxCommandEvent& evt);
    void OnCreateNotebook(wxCommandEvent& evt);
    void OnCreateText(wxCommandEvent& evt);
    void OnCreateSizeReport(wxCommandEvent& evt);
    void OnChangeContentPane(wxCommandEvent& evt);
    void OnDropDownToolbarItem(wxAuiToolBarEvent& evt);
    void OnCreatePerspective(wxCommandEvent& evt);
    void OnCopyPerspectiveCode(wxCommandEvent& evt);
    void OnRestorePerspective(wxCommandEvent& evt);
    void OnSettings(wxCommandEvent& evt);
    void OnCustomizeToolbar(wxCommandEvent& evt);
    void OnAllowNotebookDnD(wxAuiNotebookEvent& evt);
    void OnNotebookPageClose(wxAuiNotebookEvent& evt);
    void OnNotebookPageClosed(wxAuiNotebookEvent& evt);
    void OnNotebookPageChanging(wxAuiNotebookEvent &evt);
    void OnExit(wxCommandEvent& evt);
    void OnAbout(wxCommandEvent& evt);
    void OnTabAlignment(wxCommandEvent &evt);

    void OnGradient(wxCommandEvent& evt);
    void OnToolbarResizing(wxCommandEvent& evt);
    void OnManagerFlag(wxCommandEvent& evt);
    void OnNotebookFlag(wxCommandEvent& evt);
    void OnUpdateUI(wxUpdateUIEvent& evt);

    void OnPaneClose(wxAuiManagerEvent& evt);

private:

    wxAuiManager m_mgr;
    wxArrayString m_perspectives;
    wxMenu* m_perspectives_menu;
    long m_notebook_style;
    long m_notebook_theme;

    wxDECLARE_EVENT_TABLE();
};


// -- wxSizeReportCtrl --
// (a utility control that always reports it's client size)

class wxSizeReportCtrl : public wxControl
{
public:

    wxSizeReportCtrl(wxWindow* parent, wxWindowID id = wxID_ANY,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     wxAuiManager* mgr = NULL)
                     : wxControl(parent, id, pos, size, wxNO_BORDER)
    {
        m_mgr = mgr;
    }

private:

    void OnPaint(wxPaintEvent& WXUNUSED(evt))
    {
        wxPaintDC dc(this);
        wxSize size = GetClientSize();
        wxString s;
        int h, w, height;

        s.Printf("Size: %d x %d", size.x, size.y);

        dc.SetFont(*wxNORMAL_FONT);
        dc.GetTextExtent(s, &w, &height);
        height += FromDIP(3);
        dc.SetBrush(*wxWHITE_BRUSH);
        dc.SetPen(*wxWHITE_PEN);
        dc.DrawRectangle(0, 0, size.x, size.y);
        dc.SetPen(*wxLIGHT_GREY_PEN);
        dc.DrawLine(0, 0, size.x, size.y);
        dc.DrawLine(0, size.y, size.x, 0);
        dc.DrawText(s, (size.x-w)/2, ((size.y-(height*5))/2));

        if (m_mgr)
        {
            wxAuiPaneInfo pi = m_mgr->GetPane(this);

            s.Printf("Layer: %d", pi.dock_layer);
            dc.GetTextExtent(s, &w, &h);
            dc.DrawText(s, (size.x-w)/2, ((size.y-(height*5))/2)+(height*1));

            s.Printf("Dock: %d Row: %d", pi.dock_direction, pi.dock_row);
            dc.GetTextExtent(s, &w, &h);
            dc.DrawText(s, (size.x-w)/2, ((size.y-(height*5))/2)+(height*2));

            s.Printf("Position: %d", pi.dock_pos);
            dc.GetTextExtent(s, &w, &h);
            dc.DrawText(s, (size.x-w)/2, ((size.y-(height*5))/2)+(height*3));

            s.Printf("Proportion: %d", pi.dock_proportion);
            dc.GetTextExtent(s, &w, &h);
            dc.DrawText(s, (size.x-w)/2, ((size.y-(height*5))/2)+(height*4));
        }
    }

    void OnEraseBackground(wxEraseEvent& WXUNUSED(evt))
    {
        // intentionally empty
    }

    void OnSize(wxSizeEvent& WXUNUSED(evt))
    {
        Refresh();
    }
private:

    wxAuiManager* m_mgr;

    wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(wxSizeReportCtrl, wxControl)
    EVT_PAINT(wxSizeReportCtrl::OnPaint)
    EVT_SIZE(wxSizeReportCtrl::OnSize)
    EVT_ERASE_BACKGROUND(wxSizeReportCtrl::OnEraseBackground)
wxEND_EVENT_TABLE()


class SettingsPanel : public wxPanel
{
    enum
    {
        ID_PaneBorderSize = wxID_HIGHEST+1,
        ID_SashSize,
        ID_CaptionSize,
        ID_BackgroundColor,
        ID_SashColor,
        ID_InactiveCaptionColor,
        ID_InactiveCaptionGradientColor,
        ID_InactiveCaptionTextColor,
        ID_ActiveCaptionColor,
        ID_ActiveCaptionGradientColor,
        ID_ActiveCaptionTextColor,
        ID_BorderColor,
        ID_GripperColor
    };

public:

    SettingsPanel(wxWindow* parent, MyFrame* frame)
            : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize),
              m_frame(frame)
    {
        //wxBoxSizer* vert = new wxBoxSizer(wxVERTICAL);

        //vert->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);

        wxBoxSizer* s1 = new wxBoxSizer(wxHORIZONTAL);
        m_border_size = new wxSpinCtrl(this, ID_PaneBorderSize, wxString::Format("%d", frame->GetDockArt()->GetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE)), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, FromDIP(100), frame->GetDockArt()->GetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE));
        s1->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s1->Add(new wxStaticText(this, wxID_ANY, "Pane Border Size:"));
        s1->Add(m_border_size);
        s1->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s1->SetItemMinSize((size_t)1, FromDIP(wxSize(180, 20)));
        //vert->Add(s1, 0, wxEXPAND | wxLEFT | wxBOTTOM, FromDIP(5));

        wxBoxSizer* s2 = new wxBoxSizer(wxHORIZONTAL);
        m_sash_size = new wxSpinCtrl(this, ID_SashSize, wxString::Format("%d", frame->GetDockArt()->GetMetric(wxAUI_DOCKART_SASH_SIZE)), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, FromDIP(100), frame->GetDockArt()->GetMetric(wxAUI_DOCKART_SASH_SIZE));
        s2->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s2->Add(new wxStaticText(this, wxID_ANY, "Sash Size:"));
        s2->Add(m_sash_size);
        s2->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s2->SetItemMinSize((size_t)1, FromDIP(wxSize(180, 20)));
        //vert->Add(s2, 0, wxEXPAND | wxLEFT | wxBOTTOM, FromDIP(5));

        wxBoxSizer* s3 = new wxBoxSizer(wxHORIZONTAL);
        m_caption_size = new wxSpinCtrl(this, ID_CaptionSize, wxString::Format("%d", frame->GetDockArt()->GetMetric(wxAUI_DOCKART_CAPTION_SIZE)), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, FromDIP(100), frame->GetDockArt()->GetMetric(wxAUI_DOCKART_CAPTION_SIZE));
        s3->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s3->Add(new wxStaticText(this, wxID_ANY, "Caption Size:"));
        s3->Add(m_caption_size);
        s3->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s3->SetItemMinSize((size_t)1, FromDIP(wxSize(180, 20)));
        //vert->Add(s3, 0, wxEXPAND | wxLEFT | wxBOTTOM, FromDIP(5));

        //vert->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);


        wxBitmap b = CreateColorBitmap(*wxBLACK);

        wxBoxSizer* s4 = new wxBoxSizer(wxHORIZONTAL);
        m_background_color = new wxBitmapButton(this, ID_BackgroundColor, b, wxDefaultPosition, FromDIP(wxSize(50,25)));
        s4->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s4->Add(new wxStaticText(this, wxID_ANY, "Background Color:"));
        s4->Add(m_background_color);
        s4->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s4->SetItemMinSize((size_t)1, FromDIP(wxSize(180, 20)));

        wxBoxSizer* s5 = new wxBoxSizer(wxHORIZONTAL);
        m_sash_color = new wxBitmapButton(this, ID_SashColor, b, wxDefaultPosition, wxSize(50,25));
        s5->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s5->Add(new wxStaticText(this, wxID_ANY, "Sash Color:"));
        s5->Add(m_sash_color);
        s5->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s5->SetItemMinSize((size_t)1, FromDIP(wxSize(180, 20)));

        wxBoxSizer* s6 = new wxBoxSizer(wxHORIZONTAL);
        m_inactive_caption_color = new wxBitmapButton(this, ID_InactiveCaptionColor, b, wxDefaultPosition, wxSize(50,25));
        s6->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s6->Add(new wxStaticText(this, wxID_ANY, "Normal Caption:"));
        s6->Add(m_inactive_caption_color);
        s6->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s6->SetItemMinSize((size_t)1, FromDIP(wxSize(180, 20)));

        wxBoxSizer* s7 = new wxBoxSizer(wxHORIZONTAL);
        m_inactive_caption_gradient_color = new wxBitmapButton(this, ID_InactiveCaptionGradientColor, b, wxDefaultPosition, wxSize(50,25));
        s7->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s7->Add(new wxStaticText(this, wxID_ANY, "Normal Caption Gradient:"));
        s7->Add(m_inactive_caption_gradient_color);
        s7->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s7->SetItemMinSize((size_t)1, FromDIP(wxSize(180, 20)));

        wxBoxSizer* s8 = new wxBoxSizer(wxHORIZONTAL);
        m_inactive_caption_text_color = new wxBitmapButton(this, ID_InactiveCaptionTextColor, b, wxDefaultPosition, wxSize(50,25));
        s8->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s8->Add(new wxStaticText(this, wxID_ANY, "Normal Caption Text:"));
        s8->Add(m_inactive_caption_text_color);
        s8->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s8->SetItemMinSize((size_t)1, FromDIP(wxSize(180, 20)));

        wxBoxSizer* s9 = new wxBoxSizer(wxHORIZONTAL);
        m_active_caption_color = new wxBitmapButton(this, ID_ActiveCaptionColor, b, wxDefaultPosition, wxSize(50,25));
        s9->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s9->Add(new wxStaticText(this, wxID_ANY, "Active Caption:"));
        s9->Add(m_active_caption_color);
        s9->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s9->SetItemMinSize((size_t)1, FromDIP(wxSize(180, 20)));

        wxBoxSizer* s10 = new wxBoxSizer(wxHORIZONTAL);
        m_active_caption_gradient_color = new wxBitmapButton(this, ID_ActiveCaptionGradientColor, b, wxDefaultPosition, wxSize(50,25));
        s10->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s10->Add(new wxStaticText(this, wxID_ANY, "Active Caption Gradient:"));
        s10->Add(m_active_caption_gradient_color);
        s10->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s10->SetItemMinSize((size_t)1, FromDIP(wxSize(180, 20)));

        wxBoxSizer* s11 = new wxBoxSizer(wxHORIZONTAL);
        m_active_caption_text_color = new wxBitmapButton(this, ID_ActiveCaptionTextColor, b, wxDefaultPosition, wxSize(50,25));
        s11->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s11->Add(new wxStaticText(this, wxID_ANY, "Active Caption Text:"));
        s11->Add(m_active_caption_text_color);
        s11->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s11->SetItemMinSize((size_t)1, FromDIP(wxSize(180, 20)));

        wxBoxSizer* s12 = new wxBoxSizer(wxHORIZONTAL);
        m_border_color = new wxBitmapButton(this, ID_BorderColor, b, wxDefaultPosition, wxSize(50,25));
        s12->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s12->Add(new wxStaticText(this, wxID_ANY, "Border Color:"));
        s12->Add(m_border_color);
        s12->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s12->SetItemMinSize((size_t)1, FromDIP(wxSize(180, 20)));

        wxBoxSizer* s13 = new wxBoxSizer(wxHORIZONTAL);
        m_gripper_color = new wxBitmapButton(this, ID_GripperColor, b, wxDefaultPosition, wxSize(50,25));
        s13->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s13->Add(new wxStaticText(this, wxID_ANY, "Gripper Color:"));
        s13->Add(m_gripper_color);
        s13->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s13->SetItemMinSize((size_t)1, FromDIP(wxSize(180, 20)));

        wxGridSizer* grid_sizer = new wxGridSizer(2);
        grid_sizer->SetHGap(FromDIP(5));
        grid_sizer->Add(s1);  grid_sizer->Add(s4);
        grid_sizer->Add(s2);  grid_sizer->Add(s5);
        grid_sizer->Add(s3);  grid_sizer->Add(s13);
        grid_sizer->Add(FromDIP(1),FromDIP(1)); grid_sizer->Add(s12);
        grid_sizer->Add(s6);  grid_sizer->Add(s9);
        grid_sizer->Add(s7);  grid_sizer->Add(s10);
        grid_sizer->Add(s8);  grid_sizer->Add(s11);

        wxBoxSizer* cont_sizer = new wxBoxSizer(wxVERTICAL);
        cont_sizer->Add(grid_sizer, 1, wxEXPAND | wxALL, FromDIP(5));
        SetSizer(cont_sizer);
        GetSizer()->SetSizeHints(this);

        m_border_size->SetValue(frame->GetDockArt()->GetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE));
        m_sash_size->SetValue(frame->GetDockArt()->GetMetric(wxAUI_DOCKART_SASH_SIZE));
        m_caption_size->SetValue(frame->GetDockArt()->GetMetric(wxAUI_DOCKART_CAPTION_SIZE));

        UpdateColors();
    }

private:

    wxBitmap CreateColorBitmap(const wxColour& c)
    {
        wxImage image;
        wxSize size = FromDIP(wxSize(25, 14));
        image.Create(size);
        for (int x = 0; x < size.x; ++x)
            for (int y = 0; y < size.y; ++y)
            {
                wxColour pixcol = c;
                if (x == 0 || x == size.x || y == 0 || y == size.y)
                    pixcol = *wxBLACK;
                image.SetRGB(x, y, pixcol.Red(), pixcol.Green(), pixcol.Blue());
            }
        return wxBitmap(image);
    }

    void UpdateColors()
    {
        wxColour bk = m_frame->GetDockArt()->GetColor(wxAUI_DOCKART_BACKGROUND_COLOUR);
        m_background_color->SetBitmapLabel(CreateColorBitmap(bk));

        wxColour cap = m_frame->GetDockArt()->GetColor(wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR);
        m_inactive_caption_color->SetBitmapLabel(CreateColorBitmap(cap));

        wxColour capgrad = m_frame->GetDockArt()->GetColor(wxAUI_DOCKART_INACTIVE_CAPTION_GRADIENT_COLOUR);
        m_inactive_caption_gradient_color->SetBitmapLabel(CreateColorBitmap(capgrad));

        wxColour captxt = m_frame->GetDockArt()->GetColor(wxAUI_DOCKART_INACTIVE_CAPTION_TEXT_COLOUR);
        m_inactive_caption_text_color->SetBitmapLabel(CreateColorBitmap(captxt));

        wxColour acap = m_frame->GetDockArt()->GetColor(wxAUI_DOCKART_ACTIVE_CAPTION_COLOUR);
        m_active_caption_color->SetBitmapLabel(CreateColorBitmap(acap));

        wxColour acapgrad = m_frame->GetDockArt()->GetColor(wxAUI_DOCKART_ACTIVE_CAPTION_GRADIENT_COLOUR);
        m_active_caption_gradient_color->SetBitmapLabel(CreateColorBitmap(acapgrad));

        wxColour acaptxt = m_frame->GetDockArt()->GetColor(wxAUI_DOCKART_ACTIVE_CAPTION_TEXT_COLOUR);
        m_active_caption_text_color->SetBitmapLabel(CreateColorBitmap(acaptxt));

        wxColour sash = m_frame->GetDockArt()->GetColor(wxAUI_DOCKART_SASH_COLOUR);
        m_sash_color->SetBitmapLabel(CreateColorBitmap(sash));

        wxColour border = m_frame->GetDockArt()->GetColor(wxAUI_DOCKART_BORDER_COLOUR);
        m_border_color->SetBitmapLabel(CreateColorBitmap(border));

        wxColour gripper = m_frame->GetDockArt()->GetColor(wxAUI_DOCKART_GRIPPER_COLOUR);
        m_gripper_color->SetBitmapLabel(CreateColorBitmap(gripper));
    }

    void OnPaneBorderSize(wxSpinEvent& event)
    {
        m_frame->GetDockArt()->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE,
                                         event.GetPosition());
        m_frame->DoUpdate();
    }

    void OnSashSize(wxSpinEvent& event)
    {
        m_frame->GetDockArt()->SetMetric(wxAUI_DOCKART_SASH_SIZE,
                                         event.GetPosition());
        m_frame->DoUpdate();
    }

    void OnCaptionSize(wxSpinEvent& event)
    {
        m_frame->GetDockArt()->SetMetric(wxAUI_DOCKART_CAPTION_SIZE,
                                         event.GetPosition());
        m_frame->DoUpdate();
    }

    void OnSetColor(wxCommandEvent& event)
    {
        wxColourDialog dlg(m_frame);
        dlg.SetTitle(_("Color Picker"));
        if (dlg.ShowModal() != wxID_OK)
            return;

        int var = 0;
        switch (event.GetId())
        {
            case ID_BackgroundColor:              var = wxAUI_DOCKART_BACKGROUND_COLOUR; break;
            case ID_SashColor:                    var = wxAUI_DOCKART_SASH_COLOUR; break;
            case ID_InactiveCaptionColor:         var = wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR; break;
            case ID_InactiveCaptionGradientColor: var = wxAUI_DOCKART_INACTIVE_CAPTION_GRADIENT_COLOUR; break;
            case ID_InactiveCaptionTextColor:     var = wxAUI_DOCKART_INACTIVE_CAPTION_TEXT_COLOUR; break;
            case ID_ActiveCaptionColor:           var = wxAUI_DOCKART_ACTIVE_CAPTION_COLOUR; break;
            case ID_ActiveCaptionGradientColor:   var = wxAUI_DOCKART_ACTIVE_CAPTION_GRADIENT_COLOUR; break;
            case ID_ActiveCaptionTextColor:       var = wxAUI_DOCKART_ACTIVE_CAPTION_TEXT_COLOUR; break;
            case ID_BorderColor:                  var = wxAUI_DOCKART_BORDER_COLOUR; break;
            case ID_GripperColor:                 var = wxAUI_DOCKART_GRIPPER_COLOUR; break;
            default: return;
        }

        m_frame->GetDockArt()->SetColor(var, dlg.GetColourData().GetColour());
        m_frame->DoUpdate();
        UpdateColors();
    }

private:

    MyFrame* m_frame;
    wxSpinCtrl* m_border_size;
    wxSpinCtrl* m_sash_size;
    wxSpinCtrl* m_caption_size;
    wxBitmapButton* m_inactive_caption_text_color;
    wxBitmapButton* m_inactive_caption_gradient_color;
    wxBitmapButton* m_inactive_caption_color;
    wxBitmapButton* m_active_caption_text_color;
    wxBitmapButton* m_active_caption_gradient_color;
    wxBitmapButton* m_active_caption_color;
    wxBitmapButton* m_sash_color;
    wxBitmapButton* m_background_color;
    wxBitmapButton* m_border_color;
    wxBitmapButton* m_gripper_color;

    wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(SettingsPanel, wxPanel)
    EVT_SPINCTRL(ID_PaneBorderSize, SettingsPanel::OnPaneBorderSize)
    EVT_SPINCTRL(ID_SashSize, SettingsPanel::OnSashSize)
    EVT_SPINCTRL(ID_CaptionSize, SettingsPanel::OnCaptionSize)
    EVT_BUTTON(ID_BackgroundColor, SettingsPanel::OnSetColor)
    EVT_BUTTON(ID_SashColor, SettingsPanel::OnSetColor)
    EVT_BUTTON(ID_InactiveCaptionColor, SettingsPanel::OnSetColor)
    EVT_BUTTON(ID_InactiveCaptionGradientColor, SettingsPanel::OnSetColor)
    EVT_BUTTON(ID_InactiveCaptionTextColor, SettingsPanel::OnSetColor)
    EVT_BUTTON(ID_ActiveCaptionColor, SettingsPanel::OnSetColor)
    EVT_BUTTON(ID_ActiveCaptionGradientColor, SettingsPanel::OnSetColor)
    EVT_BUTTON(ID_ActiveCaptionTextColor, SettingsPanel::OnSetColor)
    EVT_BUTTON(ID_BorderColor, SettingsPanel::OnSetColor)
    EVT_BUTTON(ID_GripperColor, SettingsPanel::OnSetColor)
wxEND_EVENT_TABLE()


/*bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    wxFrame* frame = new MyFrame(NULL,
                                 wxID_ANY,
                                 "wxAUI Sample Application",
                                 wxDefaultPosition,
                                 wxWindow::FromDIP(wxSize(800, 600), NULL));
    frame->Show();

    return true;
}*/

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_ERASE_BACKGROUND(MyFrame::OnEraseBackground)
    EVT_SIZE(MyFrame::OnSize)
    EVT_MENU(MyFrame::ID_CreateTree, MyFrame::OnCreateTree)
    EVT_MENU(MyFrame::ID_CreateGrid, MyFrame::OnCreateGrid)
    EVT_MENU(MyFrame::ID_CreateText, MyFrame::OnCreateText)
    EVT_MENU(MyFrame::ID_CreateHTML, MyFrame::OnCreateHTML)
    EVT_MENU(MyFrame::ID_CreateSizeReport, MyFrame::OnCreateSizeReport)
    EVT_MENU(MyFrame::ID_CreateNotebook, MyFrame::OnCreateNotebook)
    EVT_MENU(MyFrame::ID_CreatePerspective, MyFrame::OnCreatePerspective)
    EVT_MENU(MyFrame::ID_CopyPerspectiveCode, MyFrame::OnCopyPerspectiveCode)
    EVT_MENU(ID_AllowFloating, MyFrame::OnManagerFlag)
    EVT_MENU(ID_TransparentHint, MyFrame::OnManagerFlag)
    EVT_MENU(ID_VenetianBlindsHint, MyFrame::OnManagerFlag)
    EVT_MENU(ID_RectangleHint, MyFrame::OnManagerFlag)
    EVT_MENU(ID_NoHint, MyFrame::OnManagerFlag)
    EVT_MENU(ID_HintFade, MyFrame::OnManagerFlag)
    EVT_MENU(ID_NoVenetianFade, MyFrame::OnManagerFlag)
    EVT_MENU(ID_TransparentDrag, MyFrame::OnManagerFlag)
    EVT_MENU(ID_LiveUpdate, MyFrame::OnManagerFlag)
    EVT_MENU(ID_AllowActivePane, MyFrame::OnManagerFlag)
    EVT_MENU(ID_NotebookTabFixedWidth, MyFrame::OnNotebookFlag)
    EVT_MENU(ID_NotebookNoCloseButton, MyFrame::OnNotebookFlag)
    EVT_MENU(ID_NotebookCloseButton, MyFrame::OnNotebookFlag)
    EVT_MENU(ID_NotebookCloseButtonAll, MyFrame::OnNotebookFlag)
    EVT_MENU(ID_NotebookCloseButtonActive, MyFrame::OnNotebookFlag)
    EVT_MENU(ID_NotebookAllowTabMove, MyFrame::OnNotebookFlag)
    EVT_MENU(ID_NotebookAllowTabExternalMove, MyFrame::OnNotebookFlag)
    EVT_MENU(ID_NotebookAllowTabSplit, MyFrame::OnNotebookFlag)
    EVT_MENU(ID_NotebookScrollButtons, MyFrame::OnNotebookFlag)
    EVT_MENU(ID_NotebookWindowList, MyFrame::OnNotebookFlag)
    EVT_MENU(ID_NotebookArtGloss, MyFrame::OnNotebookFlag)
    EVT_MENU(ID_NotebookArtSimple, MyFrame::OnNotebookFlag)
    EVT_MENU(ID_NotebookAlignTop,     MyFrame::OnTabAlignment)
    EVT_MENU(ID_NotebookAlignBottom,  MyFrame::OnTabAlignment)
    EVT_MENU(ID_NoGradient, MyFrame::OnGradient)
    EVT_MENU(ID_VerticalGradient, MyFrame::OnGradient)
    EVT_MENU(ID_HorizontalGradient, MyFrame::OnGradient)
    EVT_MENU(ID_AllowToolbarResizing, MyFrame::OnToolbarResizing)
    EVT_MENU(ID_Settings, MyFrame::OnSettings)
    EVT_MENU(ID_CustomizeToolbar, MyFrame::OnCustomizeToolbar)
    EVT_MENU(ID_GridContent, MyFrame::OnChangeContentPane)
    EVT_MENU(ID_TreeContent, MyFrame::OnChangeContentPane)
    EVT_MENU(ID_TextContent, MyFrame::OnChangeContentPane)
    EVT_MENU(ID_SizeReportContent, MyFrame::OnChangeContentPane)
    EVT_MENU(ID_HTMLContent, MyFrame::OnChangeContentPane)
    EVT_MENU(ID_NotebookContent, MyFrame::OnChangeContentPane)
    EVT_MENU(wxID_EXIT, MyFrame::OnExit)
    EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
    EVT_UPDATE_UI(ID_NotebookTabFixedWidth, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_NotebookNoCloseButton, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_NotebookCloseButton, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_NotebookCloseButtonAll, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_NotebookCloseButtonActive, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_NotebookAllowTabMove, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_NotebookAllowTabExternalMove, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_NotebookAllowTabSplit, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_NotebookScrollButtons, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_NotebookWindowList, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_AllowFloating, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_TransparentHint, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_VenetianBlindsHint, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_RectangleHint, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_NoHint, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_HintFade, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_NoVenetianFade, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_TransparentDrag, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_LiveUpdate, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_NoGradient, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_VerticalGradient, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_HorizontalGradient, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_AllowToolbarResizing, MyFrame::OnUpdateUI)
    EVT_MENU_RANGE(MyFrame::ID_FirstPerspective, MyFrame::ID_FirstPerspective+1000,
                   MyFrame::OnRestorePerspective)
    EVT_AUITOOLBAR_TOOL_DROPDOWN(ID_DropDownToolbarItem, MyFrame::OnDropDownToolbarItem)
    EVT_AUI_PANE_CLOSE(MyFrame::OnPaneClose)
    EVT_AUINOTEBOOK_ALLOW_DND(wxID_ANY, MyFrame::OnAllowNotebookDnD)
    EVT_AUINOTEBOOK_PAGE_CLOSE(wxID_ANY, MyFrame::OnNotebookPageClose)
    EVT_AUINOTEBOOK_PAGE_CLOSED(wxID_ANY, MyFrame::OnNotebookPageClosed)
    EVT_AUINOTEBOOK_PAGE_CHANGING(wxID_ANY, MyFrame::OnNotebookPageChanging)
wxEND_EVENT_TABLE()


MyFrame::MyFrame(wxWindow* parent,
                 wxWindowID id,
                 const wxString& title,
                 const wxPoint& pos,
                 const wxSize& size,
                 long style)
        : wxFrame(parent, id, title, pos, size, style)
{
    // tell wxAuiManager to manage this frame
    m_mgr.SetManagedWindow(this);

    // set frame icon
    SetIcon(wxIcon(sample_xpm));

    // set up default notebook style
    m_notebook_style = wxAUI_NB_DEFAULT_STYLE | wxAUI_NB_TAB_EXTERNAL_MOVE | wxNO_BORDER;
    m_notebook_theme = 0;

    // create menu
    wxMenuBar* mb = new wxMenuBar;

    wxMenu* file_menu = new wxMenu;
    file_menu->Append(wxID_EXIT);

    wxMenu* view_menu = new wxMenu;
    view_menu->Append(ID_CreateText, _("Create Text Control"));
    view_menu->Append(ID_CreateHTML, _("Create HTML Control"));
    view_menu->Append(ID_CreateTree, _("Create Tree"));
    view_menu->Append(ID_CreateGrid, _("Create Grid"));
    view_menu->Append(ID_CreateNotebook, _("Create Notebook"));
    view_menu->Append(ID_CreateSizeReport, _("Create Size Reporter"));
    view_menu->AppendSeparator();
    view_menu->Append(ID_GridContent, _("Use a Grid for the Content Pane"));
    view_menu->Append(ID_TextContent, _("Use a Text Control for the Content Pane"));
    view_menu->Append(ID_HTMLContent, _("Use an HTML Control for the Content Pane"));
    view_menu->Append(ID_TreeContent, _("Use a Tree Control for the Content Pane"));
    view_menu->Append(ID_NotebookContent, _("Use a wxAuiNotebook control for the Content Pane"));
    view_menu->Append(ID_SizeReportContent, _("Use a Size Reporter for the Content Pane"));

    wxMenu* options_menu = new wxMenu;
    options_menu->AppendRadioItem(ID_TransparentHint, _("Transparent Hint"));
    options_menu->AppendRadioItem(ID_VenetianBlindsHint, _("Venetian Blinds Hint"));
    options_menu->AppendRadioItem(ID_RectangleHint, _("Rectangle Hint"));
    options_menu->AppendRadioItem(ID_NoHint, _("No Hint"));
    options_menu->AppendSeparator();
    options_menu->AppendCheckItem(ID_HintFade, _("Hint Fade-in"));
    options_menu->AppendCheckItem(ID_AllowFloating, _("Allow Floating"));
    options_menu->AppendCheckItem(ID_NoVenetianFade, _("Disable Venetian Blinds Hint Fade-in"));
    options_menu->AppendCheckItem(ID_TransparentDrag, _("Transparent Drag"));
    options_menu->AppendCheckItem(ID_AllowActivePane, _("Allow Active Pane"));
    // Only show "live resize" toggle if it's actually functional.
    if ( !wxAuiManager::AlwaysUsesLiveResize() )
        options_menu->AppendCheckItem(ID_LiveUpdate, _("Live Resize Update"));
    options_menu->AppendSeparator();
    options_menu->AppendRadioItem(ID_NoGradient, _("No Caption Gradient"));
    options_menu->AppendRadioItem(ID_VerticalGradient, _("Vertical Caption Gradient"));
    options_menu->AppendRadioItem(ID_HorizontalGradient, _("Horizontal Caption Gradient"));
    options_menu->AppendSeparator();
    options_menu->AppendCheckItem(ID_AllowToolbarResizing, _("Allow Toolbar Resizing"));
    options_menu->AppendSeparator();
    options_menu->Append(ID_Settings, _("Settings Pane"));

    wxMenu* notebook_menu = new wxMenu;
    notebook_menu->AppendRadioItem(ID_NotebookArtGloss, _("Glossy Theme (Default)"));
    notebook_menu->AppendRadioItem(ID_NotebookArtSimple, _("Simple Theme"));
    notebook_menu->AppendSeparator();
    notebook_menu->AppendRadioItem(ID_NotebookNoCloseButton, _("No Close Button"));
    notebook_menu->AppendRadioItem(ID_NotebookCloseButton, _("Close Button at Right"));
    notebook_menu->AppendRadioItem(ID_NotebookCloseButtonAll, _("Close Button on All Tabs"));
    notebook_menu->AppendRadioItem(ID_NotebookCloseButtonActive, _("Close Button on Active Tab"));
    notebook_menu->AppendSeparator();
    notebook_menu->AppendRadioItem(ID_NotebookAlignTop, _("Tab Top Alignment"));
    notebook_menu->AppendRadioItem(ID_NotebookAlignBottom, _("Tab Bottom Alignment"));
    notebook_menu->AppendSeparator();
    notebook_menu->AppendCheckItem(ID_NotebookAllowTabMove, _("Allow Tab Move"));
    notebook_menu->AppendCheckItem(ID_NotebookAllowTabExternalMove, _("Allow External Tab Move"));
    notebook_menu->AppendCheckItem(ID_NotebookAllowTabSplit, _("Allow Notebook Split"));
    notebook_menu->AppendCheckItem(ID_NotebookScrollButtons, _("Scroll Buttons Visible"));
    notebook_menu->AppendCheckItem(ID_NotebookWindowList, _("Window List Button Visible"));
    notebook_menu->AppendCheckItem(ID_NotebookTabFixedWidth, _("Fixed-width Tabs"));

    m_perspectives_menu = new wxMenu;
    m_perspectives_menu->Append(ID_CreatePerspective, _("Create Perspective"));
    m_perspectives_menu->Append(ID_CopyPerspectiveCode, _("Copy Perspective Data To Clipboard"));
    m_perspectives_menu->AppendSeparator();
    m_perspectives_menu->Append(ID_FirstPerspective+0, _("Default Startup"));
    m_perspectives_menu->Append(ID_FirstPerspective+1, _("All Panes"));

    wxMenu* help_menu = new wxMenu;
    help_menu->Append(wxID_ABOUT);

    mb->Append(file_menu, _("&File"));
    mb->Append(view_menu, _("&View"));
    mb->Append(m_perspectives_menu, _("&Perspectives"));
    mb->Append(options_menu, _("&Options"));
    mb->Append(notebook_menu, _("&Notebook"));
    mb->Append(help_menu, _("&Help"));

    SetMenuBar(mb);

    CreateStatusBar();
    GetStatusBar()->SetStatusText(_("Ready"));


    // min size for the frame itself isn't completely done.
    // see the end up wxAuiManager::Update() for the test
    // code. For now, just hard code a frame minimum size
    SetMinSize(FromDIP(wxSize(400,300)));



    // prepare a few custom overflow elements for the toolbars' overflow buttons

    wxAuiToolBarItemArray prepend_items;
    wxAuiToolBarItemArray append_items;
    wxAuiToolBarItem item;
    item.SetKind(wxITEM_SEPARATOR);
    append_items.Add(item);
    item.SetKind(wxITEM_NORMAL);
    item.SetId(ID_CustomizeToolbar);
    item.SetLabel(_("Customize..."));
    append_items.Add(item);


    // create some toolbars
    wxAuiToolBar* tb1 = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                         wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_OVERFLOW);
    tb1->AddTool(ID_SampleItem+1, "Test", wxArtProvider::GetBitmapBundle(wxART_ERROR));
    tb1->AddSeparator();
    tb1->AddTool(ID_SampleItem+2, "Test", wxArtProvider::GetBitmapBundle(wxART_QUESTION));
    tb1->AddTool(ID_SampleItem+3, "Test", wxArtProvider::GetBitmapBundle(wxART_INFORMATION));
    tb1->AddTool(ID_SampleItem+4, "Test", wxArtProvider::GetBitmapBundle(wxART_WARNING));
    tb1->AddTool(ID_SampleItem+5, "Test", wxArtProvider::GetBitmapBundle(wxART_MISSING_IMAGE));
    tb1->SetCustomOverflowItems(prepend_items, append_items);
    tb1->Realize();


    wxAuiToolBar* tb2 = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                         wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_OVERFLOW | wxAUI_TB_HORIZONTAL);

    wxBitmapBundle tb2_bmp1 = wxArtProvider::GetBitmapBundle(wxART_QUESTION, wxART_OTHER, wxSize(16,16));
    tb2->AddTool(ID_SampleItem+6, "Disabled", tb2_bmp1);
    tb2->AddTool(ID_SampleItem+7, "Test", tb2_bmp1);
    tb2->AddTool(ID_SampleItem+8, "Test", tb2_bmp1);
    tb2->AddTool(ID_SampleItem+9, "Test", tb2_bmp1);
    tb2->AddSeparator();
    tb2->AddTool(ID_SampleItem+10, "Test", tb2_bmp1);
    tb2->AddTool(ID_SampleItem+11, "Test", tb2_bmp1);
    tb2->AddSeparator();
    tb2->AddTool(ID_SampleItem+12, "Test", tb2_bmp1);
    tb2->AddTool(ID_SampleItem+13, "Test", tb2_bmp1);
    tb2->AddTool(ID_SampleItem+14, "Test", tb2_bmp1);
    tb2->AddTool(ID_SampleItem+15, "Test", tb2_bmp1);
    tb2->SetCustomOverflowItems(prepend_items, append_items);
    tb2->EnableTool(ID_SampleItem+6, false);
    tb2->Realize();


    wxAuiToolBar* tb3 = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                         wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_OVERFLOW);
    wxBitmapBundle tb3_bmp1 = wxArtProvider::GetBitmapBundle(wxART_FOLDER, wxART_OTHER, wxSize(16,16));
    tb3->AddTool(ID_SampleItem+16, "Check 1", tb3_bmp1, "Check 1", wxITEM_CHECK);
    tb3->AddTool(ID_SampleItem+17, "Check 2", tb3_bmp1, "Check 2", wxITEM_CHECK);
    tb3->AddTool(ID_SampleItem+18, "Check 3", tb3_bmp1, "Check 3", wxITEM_CHECK);
    tb3->AddTool(ID_SampleItem+19, "Check 4", tb3_bmp1, "Check 4", wxITEM_CHECK);
    tb3->AddSeparator();
    tb3->AddTool(ID_SampleItem+20, "Radio 1", tb3_bmp1, "Radio 1", wxITEM_RADIO);
    tb3->AddTool(ID_SampleItem+21, "Radio 2", tb3_bmp1, "Radio 2", wxITEM_RADIO);
    tb3->AddTool(ID_SampleItem+22, "Radio 3", tb3_bmp1, "Radio 3", wxITEM_RADIO);
    tb3->AddSeparator();
    tb3->AddTool(ID_SampleItem+23, "Radio 1 (Group 2)", tb3_bmp1, "Radio 1 (Group 2)", wxITEM_RADIO);
    tb3->AddTool(ID_SampleItem+24, "Radio 2 (Group 2)", tb3_bmp1, "Radio 2 (Group 2)", wxITEM_RADIO);
    tb3->AddTool(ID_SampleItem+25, "Radio 3 (Group 2)", tb3_bmp1, "Radio 3 (Group 2)", wxITEM_RADIO);
    tb3->SetCustomOverflowItems(prepend_items, append_items);
    tb3->Realize();


    wxAuiToolBar* tb4 = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                         wxAUI_TB_DEFAULT_STYLE |
                                         wxAUI_TB_OVERFLOW |
                                         wxAUI_TB_TEXT |
                                         wxAUI_TB_HORZ_TEXT);
    wxBitmapBundle tb4_bmp1 = wxArtProvider::GetBitmapBundle(wxART_NORMAL_FILE, wxART_OTHER, wxSize(16,16));
    tb4->AddTool(ID_DropDownToolbarItem, "Item 1", tb4_bmp1);
    tb4->AddTool(ID_SampleItem+23, "Item 2", tb4_bmp1);
    tb4->SetToolSticky(ID_SampleItem+23, true);
    tb4->AddTool(ID_SampleItem+24, "Disabled", tb4_bmp1);
    tb4->EnableTool(ID_SampleItem+24, false); // Just to show disabled items look
    tb4->AddTool(ID_SampleItem+25, "Item 4", tb4_bmp1);
    tb4->AddSeparator();
    tb4->AddTool(ID_SampleItem+26, "Item 5", tb4_bmp1);
    tb4->AddTool(ID_SampleItem+27, "Item 6", tb4_bmp1);
    tb4->AddTool(ID_SampleItem+28, "Item 7", tb4_bmp1);
    tb4->AddTool(ID_SampleItem+29, "Item 8", tb4_bmp1);
    tb4->SetToolDropDown(ID_DropDownToolbarItem, true);
    tb4->SetCustomOverflowItems(prepend_items, append_items);
    wxChoice* choice = new wxChoice(tb4, ID_SampleItem+35);
    choice->AppendString("One choice");
    choice->AppendString("Another choice");
    tb4->AddControl(choice);
    tb4->Realize();


    wxAuiToolBar* tb5 = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                         wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_OVERFLOW | wxAUI_TB_VERTICAL);
    tb5->AddTool(ID_SampleItem+30, "Test", wxArtProvider::GetBitmapBundle(wxART_ERROR));
    tb5->AddSeparator();
    tb5->AddTool(ID_SampleItem+31, "Test", wxArtProvider::GetBitmapBundle(wxART_QUESTION));
    tb5->AddTool(ID_SampleItem+32, "Test", wxArtProvider::GetBitmapBundle(wxART_INFORMATION));
    tb5->AddTool(ID_SampleItem+33, "Test", wxArtProvider::GetBitmapBundle(wxART_WARNING));
    tb5->AddTool(ID_SampleItem+34, "Test", wxArtProvider::GetBitmapBundle(wxART_MISSING_IMAGE));
    tb5->SetCustomOverflowItems(prepend_items, append_items);
    tb5->Realize();

    // add a bunch of panes
    m_mgr.AddPane(CreateSizeReportCtrl(), wxAuiPaneInfo().
                  Name("test1").Caption("Pane Caption").
                  Top());

    m_mgr.AddPane(CreateSizeReportCtrl(), wxAuiPaneInfo().
                  Name("test2").Caption("Client Size Reporter").
                  Bottom().Position(1).
                  CloseButton(true).MaximizeButton(true));

    m_mgr.AddPane(CreateSizeReportCtrl(), wxAuiPaneInfo().
                  Name("test3").Caption("Client Size Reporter").
                  Bottom().
                  CloseButton(true).MaximizeButton(true));

    m_mgr.AddPane(CreateSizeReportCtrl(), wxAuiPaneInfo().
                  Name("test4").Caption("Pane Caption").
                  Left());

    m_mgr.AddPane(CreateSizeReportCtrl(), wxAuiPaneInfo().
                  Name("test5").Caption("No Close Button").
                  Right().CloseButton(false));

    m_mgr.AddPane(CreateSizeReportCtrl(), wxAuiPaneInfo().
                  Name("test6").Caption("Client Size Reporter").
                  Right().Row(1).
                  CloseButton(true).MaximizeButton(true));

    m_mgr.AddPane(CreateSizeReportCtrl(), wxAuiPaneInfo().
                  Name("test7").Caption("Client Size Reporter").
                  Left().Layer(1).
                  CloseButton(true).MaximizeButton(true));

    m_mgr.AddPane(CreateTreeCtrl(), wxAuiPaneInfo().
                  Name("test8").Caption("Tree Pane").
                  Left().Layer(1).Position(1).
                  CloseButton(true).MaximizeButton(true));

    m_mgr.AddPane(CreateSizeReportCtrl(), wxAuiPaneInfo().
                  Name("test9").Caption("Min Size 200x100").
                  BestSize(FromDIP(wxSize(200,100))).MinSize(FromDIP(wxSize(200,100))).
                  Bottom().Layer(1).
                  CloseButton(true).MaximizeButton(true));

    wxWindow* wnd10 = CreateTextCtrl("This pane will prompt the user before hiding.");

    // Give this pane an icon, too, just for testing.
    int iconSize = m_mgr.GetArtProvider()->GetMetric(wxAUI_DOCKART_CAPTION_SIZE);

    // Make it even to use 16 pixel icons with default 17 caption height.
    iconSize &= ~1;

    m_mgr.AddPane(wnd10, wxAuiPaneInfo().
                  Name("test10").Caption("Text Pane with Hide Prompt").
                  Bottom().Layer(1).Position(1).
                  Icon(wxArtProvider::GetBitmapBundle(wxART_WARNING,
                                                      wxART_OTHER,
                                                      wxSize(iconSize, iconSize))));

    m_mgr.AddPane(CreateSizeReportCtrl(), wxAuiPaneInfo().
                  Name("test11").Caption("Fixed Pane").
                  Bottom().Layer(1).Position(2).Fixed());


    m_mgr.AddPane(new SettingsPanel(this,this), wxAuiPaneInfo().
                  Name("settings").Caption("Dock Manager Settings").
                  Dockable(false).Float().Hide());

    // create some center panes

    m_mgr.AddPane(CreateGrid(), wxAuiPaneInfo().Name("grid_content").
                  CenterPane().Hide());

    m_mgr.AddPane(CreateTreeCtrl(), wxAuiPaneInfo().Name("tree_content").
                  CenterPane().Hide());

    m_mgr.AddPane(CreateSizeReportCtrl(), wxAuiPaneInfo().Name("sizereport_content").
                  CenterPane().Hide());

    m_mgr.AddPane(CreateTextCtrl(), wxAuiPaneInfo().Name("text_content").
                  CenterPane().Hide());

    m_mgr.AddPane(CreateHTMLCtrl(), wxAuiPaneInfo().Name("html_content").
                  CenterPane().Hide());

    m_mgr.AddPane(CreateNotebook(), wxAuiPaneInfo().Name("notebook_content").
                  CenterPane().PaneBorder(false));

    // add the toolbars to the manager
    m_mgr.AddPane(tb1, wxAuiPaneInfo().
                  Name("tb1").Caption("Big Toolbar").
                  ToolbarPane().Top());

    m_mgr.AddPane(tb2, wxAuiPaneInfo().
                  Name("tb2").Caption("Toolbar 2 (Horizontal)").
                  ToolbarPane().Top().Row(1));

    m_mgr.AddPane(tb3, wxAuiPaneInfo().
                  Name("tb3").Caption("Toolbar 3").
                  ToolbarPane().Top().Row(1).Position(1));

    m_mgr.AddPane(tb4, wxAuiPaneInfo().
                  Name("tb4").Caption("Sample Bookmark Toolbar").
                  ToolbarPane().Top().Row(2));

    m_mgr.AddPane(tb5, wxAuiPaneInfo().
                  Name("tb5").Caption("Sample Vertical Toolbar").
                  ToolbarPane().Left().
                  GripperTop());

    m_mgr.AddPane(new wxButton(this, wxID_ANY, _("Test Button")),
                  wxAuiPaneInfo().Name("tb6").
                  ToolbarPane().Top().Row(2).Position(1).
                  LeftDockable(false).RightDockable(false));

    // make some default perspectives

    wxString perspective_all = m_mgr.SavePerspective();

    size_t i, count;
    wxAuiPaneInfoArray& all_panes = m_mgr.GetAllPanes();
    for (i = 0, count = all_panes.GetCount(); i < count; ++i)
        if (!all_panes.Item(i).IsToolbar())
            all_panes.Item(i).Hide();
    m_mgr.GetPane("tb1").Hide();
    m_mgr.GetPane("tb6").Hide();
    m_mgr.GetPane("test8").Show().Left().Layer(0).Row(0).Position(0);
    m_mgr.GetPane("test10").Show().Bottom().Layer(0).Row(0).Position(0);
    m_mgr.GetPane("notebook_content").Show();
    wxString perspective_default = m_mgr.SavePerspective();

    m_perspectives.Add(perspective_default);
    m_perspectives.Add(perspective_all);

    // "commit" all changes made to wxAuiManager
    m_mgr.Update();
}

wxAuiDockArt* MyFrame::GetDockArt()
{
    return m_mgr.GetArtProvider();
}

void MyFrame::DoUpdate()
{
    m_mgr.Update();
}

void MyFrame::OnEraseBackground(wxEraseEvent& event)
{
    event.Skip();
}

void MyFrame::OnSize(wxSizeEvent& event)
{
    event.Skip();
}

void MyFrame::OnSettings(wxCommandEvent& WXUNUSED(evt))
{
    // show the settings pane, and float it
    wxAuiPaneInfo& floating_pane = m_mgr.GetPane("settings").Float().Show();

    if (floating_pane.floating_pos == wxDefaultPosition)
        floating_pane.FloatingPosition(GetStartPosition());

    m_mgr.Update();
}

void MyFrame::OnCustomizeToolbar(wxCommandEvent& WXUNUSED(evt))
{
    wxMessageBox(_("Customize Toolbar clicked"));
}

void MyFrame::OnGradient(wxCommandEvent& event)
{
    int gradient = 0;

    switch (event.GetId())
    {
        case ID_NoGradient:         gradient = wxAUI_GRADIENT_NONE; break;
        case ID_VerticalGradient:   gradient = wxAUI_GRADIENT_VERTICAL; break;
        case ID_HorizontalGradient: gradient = wxAUI_GRADIENT_HORIZONTAL; break;
    }

    m_mgr.GetArtProvider()->SetMetric(wxAUI_DOCKART_GRADIENT_TYPE, gradient);
    m_mgr.Update();
}

void MyFrame::OnToolbarResizing(wxCommandEvent& WXUNUSED(evt))
{
    wxAuiPaneInfoArray& all_panes = m_mgr.GetAllPanes();
    const size_t count = all_panes.GetCount();
    for (size_t i = 0; i < count; ++i)
    {
        wxAuiToolBar* toolbar = wxDynamicCast(all_panes[i].window, wxAuiToolBar);
        if (toolbar)
        {
            all_panes[i].Resizable(!all_panes[i].IsResizable());
        }
    }

    m_mgr.Update();
}

void MyFrame::OnManagerFlag(wxCommandEvent& event)
{
    unsigned int flag = 0;

#if !defined(__WXMSW__) && !defined(__WXMAC__) && !defined(__WXGTK__)
    if (event.GetId() == ID_TransparentDrag ||
        event.GetId() == ID_TransparentHint ||
        event.GetId() == ID_HintFade)
    {
        wxMessageBox("This option is presently only available on wxGTK, wxMSW and wxMac");
        return;
    }
#endif

    int id = event.GetId();

    if (id == ID_TransparentHint ||
        id == ID_VenetianBlindsHint ||
        id == ID_RectangleHint ||
        id == ID_NoHint)
    {
        unsigned int flags = m_mgr.GetFlags();
        flags &= ~wxAUI_MGR_TRANSPARENT_HINT;
        flags &= ~wxAUI_MGR_VENETIAN_BLINDS_HINT;
        flags &= ~wxAUI_MGR_RECTANGLE_HINT;
        m_mgr.SetFlags(flags);
    }

    switch (id)
    {
        case ID_AllowFloating: flag = wxAUI_MGR_ALLOW_FLOATING; break;
        case ID_TransparentDrag: flag = wxAUI_MGR_TRANSPARENT_DRAG; break;
        case ID_HintFade: flag = wxAUI_MGR_HINT_FADE; break;
        case ID_NoVenetianFade: flag = wxAUI_MGR_NO_VENETIAN_BLINDS_FADE; break;
        case ID_AllowActivePane: flag = wxAUI_MGR_ALLOW_ACTIVE_PANE; break;
        case ID_TransparentHint: flag = wxAUI_MGR_TRANSPARENT_HINT; break;
        case ID_VenetianBlindsHint: flag = wxAUI_MGR_VENETIAN_BLINDS_HINT; break;
        case ID_RectangleHint: flag = wxAUI_MGR_RECTANGLE_HINT; break;
        case ID_LiveUpdate: flag = wxAUI_MGR_LIVE_RESIZE; break;
    }

    if (flag)
    {
        m_mgr.SetFlags(m_mgr.GetFlags() ^ flag);
    }

    m_mgr.Update();
}


void MyFrame::OnNotebookFlag(wxCommandEvent& event)
{
    int id = event.GetId();

    if (id == ID_NotebookNoCloseButton ||
        id == ID_NotebookCloseButton ||
        id == ID_NotebookCloseButtonAll ||
        id == ID_NotebookCloseButtonActive)
    {
        m_notebook_style &= ~(wxAUI_NB_CLOSE_BUTTON |
                              wxAUI_NB_CLOSE_ON_ACTIVE_TAB |
                              wxAUI_NB_CLOSE_ON_ALL_TABS);

        switch (id)
        {
            case ID_NotebookNoCloseButton: break;
            case ID_NotebookCloseButton: m_notebook_style |= wxAUI_NB_CLOSE_BUTTON; break;
            case ID_NotebookCloseButtonAll: m_notebook_style |= wxAUI_NB_CLOSE_ON_ALL_TABS; break;
            case ID_NotebookCloseButtonActive: m_notebook_style |= wxAUI_NB_CLOSE_ON_ACTIVE_TAB; break;
        }
    }

    if (id == ID_NotebookAllowTabMove)
    {
        m_notebook_style ^= wxAUI_NB_TAB_MOVE;
    }
    if (id == ID_NotebookAllowTabExternalMove)
    {
        m_notebook_style ^= wxAUI_NB_TAB_EXTERNAL_MOVE;
    }
     else if (id == ID_NotebookAllowTabSplit)
    {
        m_notebook_style ^= wxAUI_NB_TAB_SPLIT;
    }
     else if (id == ID_NotebookWindowList)
    {
        m_notebook_style ^= wxAUI_NB_WINDOWLIST_BUTTON;
    }
     else if (id == ID_NotebookScrollButtons)
    {
        m_notebook_style ^= wxAUI_NB_SCROLL_BUTTONS;
    }
     else if (id == ID_NotebookTabFixedWidth)
    {
        m_notebook_style ^= wxAUI_NB_TAB_FIXED_WIDTH;
    }


    size_t i, count;
    wxAuiPaneInfoArray& all_panes = m_mgr.GetAllPanes();
    for (i = 0, count = all_panes.GetCount(); i < count; ++i)
    {
        wxAuiPaneInfo& pane = all_panes.Item(i);
        if (pane.window->IsKindOf(CLASSINFO(wxAuiNotebook)))
        {
            wxAuiNotebook* nb = (wxAuiNotebook*)pane.window;

            if (id == ID_NotebookArtGloss)
            {
                nb->SetArtProvider(new wxAuiDefaultTabArt);
                m_notebook_theme = 0;
            }
             else if (id == ID_NotebookArtSimple)
            {
                nb->SetArtProvider(new wxAuiSimpleTabArt);
                m_notebook_theme = 1;
            }


            nb->SetWindowStyleFlag(m_notebook_style);
            nb->Refresh();
        }
    }


}


void MyFrame::OnUpdateUI(wxUpdateUIEvent& event)
{
    unsigned int flags = m_mgr.GetFlags();

    switch (event.GetId())
    {
        case ID_NoGradient:
            event.Check(m_mgr.GetArtProvider()->GetMetric(wxAUI_DOCKART_GRADIENT_TYPE) == wxAUI_GRADIENT_NONE);
            break;
        case ID_VerticalGradient:
            event.Check(m_mgr.GetArtProvider()->GetMetric(wxAUI_DOCKART_GRADIENT_TYPE) == wxAUI_GRADIENT_VERTICAL);
            break;
        case ID_HorizontalGradient:
            event.Check(m_mgr.GetArtProvider()->GetMetric(wxAUI_DOCKART_GRADIENT_TYPE) == wxAUI_GRADIENT_HORIZONTAL);
            break;
        case ID_AllowToolbarResizing:
        {
            wxAuiPaneInfoArray& all_panes = m_mgr.GetAllPanes();
            const size_t count = all_panes.GetCount();
            for (size_t i = 0; i < count; ++i)
            {
                wxAuiToolBar* toolbar = wxDynamicCast(all_panes[i].window, wxAuiToolBar);
                if (toolbar)
                {
                    event.Check(all_panes[i].IsResizable());
                    break;
                }
            }
            break;
        }
        case ID_AllowFloating:
            event.Check((flags & wxAUI_MGR_ALLOW_FLOATING) != 0);
            break;
        case ID_TransparentDrag:
            event.Check((flags & wxAUI_MGR_TRANSPARENT_DRAG) != 0);
            break;
        case ID_TransparentHint:
            event.Check((flags & wxAUI_MGR_TRANSPARENT_HINT) != 0);
            break;
        case ID_LiveUpdate:
            event.Check((flags & wxAUI_MGR_LIVE_RESIZE) != 0);
            break;
        case ID_VenetianBlindsHint:
            event.Check((flags & wxAUI_MGR_VENETIAN_BLINDS_HINT) != 0);
            break;
        case ID_RectangleHint:
            event.Check((flags & wxAUI_MGR_RECTANGLE_HINT) != 0);
            break;
        case ID_NoHint:
            event.Check(((wxAUI_MGR_TRANSPARENT_HINT |
                          wxAUI_MGR_VENETIAN_BLINDS_HINT |
                          wxAUI_MGR_RECTANGLE_HINT) & flags) == 0);
            break;
        case ID_HintFade:
            event.Check((flags & wxAUI_MGR_HINT_FADE) != 0);
            break;
        case ID_NoVenetianFade:
            event.Check((flags & wxAUI_MGR_NO_VENETIAN_BLINDS_FADE) != 0);
            break;

        case ID_NotebookNoCloseButton:
            event.Check((m_notebook_style & (wxAUI_NB_CLOSE_BUTTON|wxAUI_NB_CLOSE_ON_ALL_TABS|wxAUI_NB_CLOSE_ON_ACTIVE_TAB)) != 0);
            break;
        case ID_NotebookCloseButton:
            event.Check((m_notebook_style & wxAUI_NB_CLOSE_BUTTON) != 0);
            break;
        case ID_NotebookCloseButtonAll:
            event.Check((m_notebook_style & wxAUI_NB_CLOSE_ON_ALL_TABS) != 0);
            break;
        case ID_NotebookCloseButtonActive:
            event.Check((m_notebook_style & wxAUI_NB_CLOSE_ON_ACTIVE_TAB) != 0);
            break;
        case ID_NotebookAllowTabSplit:
            event.Check((m_notebook_style & wxAUI_NB_TAB_SPLIT) != 0);
            break;
        case ID_NotebookAllowTabMove:
            event.Check((m_notebook_style & wxAUI_NB_TAB_MOVE) != 0);
            break;
        case ID_NotebookAllowTabExternalMove:
            event.Check((m_notebook_style & wxAUI_NB_TAB_EXTERNAL_MOVE) != 0);
            break;
        case ID_NotebookScrollButtons:
            event.Check((m_notebook_style & wxAUI_NB_SCROLL_BUTTONS) != 0);
            break;
        case ID_NotebookWindowList:
            event.Check((m_notebook_style & wxAUI_NB_WINDOWLIST_BUTTON) != 0);
            break;
        case ID_NotebookTabFixedWidth:
            event.Check((m_notebook_style & wxAUI_NB_TAB_FIXED_WIDTH) != 0);
            break;
        case ID_NotebookArtGloss:
            event.Check(m_notebook_style == 0);
            break;
        case ID_NotebookArtSimple:
            event.Check(m_notebook_style == 1);
            break;

    }
}

void MyFrame::OnPaneClose(wxAuiManagerEvent& evt)
{
    if (evt.pane->name == "test10")
    {
        int res = wxMessageBox("Are you sure you want to close/hide this pane?",
                               "wxAUI",
                               wxYES_NO,
                               this);
        if (res != wxYES)
            evt.Veto();
    }
}

void MyFrame::OnCreatePerspective(wxCommandEvent& WXUNUSED(event))
{
    wxTextEntryDialog dlg(this, "Enter a name for the new perspective:",
                          "wxAUI Test");

    dlg.SetValue(wxString::Format("Perspective %u", unsigned(m_perspectives.GetCount() + 1)));
    if (dlg.ShowModal() != wxID_OK)
        return;

    if (m_perspectives.GetCount() == 0)
    {
        m_perspectives_menu->AppendSeparator();
    }

    m_perspectives_menu->Append(ID_FirstPerspective + m_perspectives.GetCount(), dlg.GetValue());
    m_perspectives.Add(m_mgr.SavePerspective());
}

void MyFrame::OnCopyPerspectiveCode(wxCommandEvent& WXUNUSED(evt))
{
    wxString s = m_mgr.SavePerspective();

#if wxUSE_CLIPBOARD
    if (wxTheClipboard->Open())
    {
        wxTheClipboard->SetData(new wxTextDataObject(s));
        wxTheClipboard->Close();
    }
#endif
}

void MyFrame::OnRestorePerspective(wxCommandEvent& evt)
{
    m_mgr.LoadPerspective(m_perspectives.Item(evt.GetId() - ID_FirstPerspective));
}

void MyFrame::OnNotebookPageClose(wxAuiNotebookEvent& evt)
{
    wxAuiNotebook* ctrl = (wxAuiNotebook*)evt.GetEventObject();
    if (ctrl->GetPage(evt.GetSelection())->IsKindOf(CLASSINFO(wxHtmlWindow)))
    {
        int res = wxMessageBox("Are you sure you want to close/hide this notebook page?",
                       "wxAUI",
                       wxYES_NO,
                       this);
        if (res != wxYES)
            evt.Veto();
    }
}

void MyFrame::OnNotebookPageClosed(wxAuiNotebookEvent& evt)
{
    wxAuiNotebook* ctrl = (wxAuiNotebook*)evt.GetEventObject();
    wxUnusedVar(ctrl);

    // selection should always be a valid index
    wxASSERT_MSG( ctrl->GetSelection() < (int)ctrl->GetPageCount(),
                  wxString::Format("Invalid selection %d, only %d pages left",
                                   ctrl->GetSelection(),
                                   (int)ctrl->GetPageCount()) );

    evt.Skip();
}

void MyFrame::OnNotebookPageChanging(wxAuiNotebookEvent& evt)
{
    if ( evt.GetOldSelection() == 3 )
    {
        if ( wxMessageBox( "Are you sure you want to leave this page?\n"
                           "(This demonstrates veto-ing)",
                           "wxAUI",
                           wxICON_QUESTION | wxYES_NO,
                           this ) != wxYES )
        {
            evt.Veto();
        }
    }
}

void MyFrame::OnAllowNotebookDnD(wxAuiNotebookEvent& evt)
{
    // for the purpose of this test application, explicitly
    // allow all notebook drag and drop events
    evt.Allow();
}

wxPoint MyFrame::GetStartPosition()
{
    static int x = 0;
    x += FromDIP(20);
    wxPoint pt = ClientToScreen(wxPoint(0,0));
    return wxPoint(pt.x + x, pt.y + x);
}

void MyFrame::OnCreateTree(wxCommandEvent& WXUNUSED(event))
{
    m_mgr.AddPane(CreateTreeCtrl(), wxAuiPaneInfo().
                  Caption("Tree Control").
                  Float().FloatingPosition(GetStartPosition()).
                  FloatingSize(FromDIP(wxSize(150,300))));
    m_mgr.Update();
}

void MyFrame::OnCreateGrid(wxCommandEvent& WXUNUSED(event))
{
    m_mgr.AddPane(CreateGrid(), wxAuiPaneInfo().
                  Caption("Grid").
                  Float().FloatingPosition(GetStartPosition()).
                  FloatingSize(FromDIP(wxSize(300,200))));
    m_mgr.Update();
}

void MyFrame::OnCreateHTML(wxCommandEvent& WXUNUSED(event))
{
    m_mgr.AddPane(CreateHTMLCtrl(), wxAuiPaneInfo().
                  Caption("HTML Control").
                  Float().FloatingPosition(GetStartPosition()).
                  FloatingSize(FromDIP(wxSize(300,200))));
    m_mgr.Update();
}

void MyFrame::OnCreateNotebook(wxCommandEvent& WXUNUSED(event))
{
    m_mgr.AddPane(CreateNotebook(), wxAuiPaneInfo().
                  Caption("Notebook").
                  Float().FloatingPosition(GetStartPosition()).
                  //FloatingSize(FromDIP(wxSize(300,200))).
                  CloseButton(true).MaximizeButton(true));
    m_mgr.Update();
}

void MyFrame::OnCreateText(wxCommandEvent& WXUNUSED(event))
{
    m_mgr.AddPane(CreateTextCtrl(), wxAuiPaneInfo().
                  Caption("Text Control").
                  Float().FloatingPosition(GetStartPosition()));
    m_mgr.Update();
}

void MyFrame::OnCreateSizeReport(wxCommandEvent& WXUNUSED(event))
{
    m_mgr.AddPane(CreateSizeReportCtrl(), wxAuiPaneInfo().
                  Caption("Client Size Reporter").
                  Float().FloatingPosition(GetStartPosition()).
                  CloseButton(true).MaximizeButton(true));
    m_mgr.Update();
}

void MyFrame::OnChangeContentPane(wxCommandEvent& evt)
{
    m_mgr.GetPane("grid_content").Show(evt.GetId() == ID_GridContent);
    m_mgr.GetPane("text_content").Show(evt.GetId() == ID_TextContent);
    m_mgr.GetPane("tree_content").Show(evt.GetId() == ID_TreeContent);
    m_mgr.GetPane("sizereport_content").Show(evt.GetId() == ID_SizeReportContent);
    m_mgr.GetPane("html_content").Show(evt.GetId() == ID_HTMLContent);
    m_mgr.GetPane("notebook_content").Show(evt.GetId() == ID_NotebookContent);
    m_mgr.Update();
}

void MyFrame::OnDropDownToolbarItem(wxAuiToolBarEvent& evt)
{
    if (evt.IsDropDownClicked())
    {
        wxAuiToolBar* tb = static_cast<wxAuiToolBar*>(evt.GetEventObject());

        tb->SetToolSticky(evt.GetId(), true);

        // create the popup menu
        wxMenu menuPopup;

        // TODO: Use GetBitmapBundle() when wxMenuItem is updated to use it too.
        wxBitmap bmp = wxArtProvider::GetBitmap(wxART_QUESTION, wxART_OTHER, FromDIP(wxSize(16,16)));

        wxMenuItem* m1 =  new wxMenuItem(&menuPopup, 10001, _("Drop Down Item 1"));
        m1->SetBitmap(bmp);
        menuPopup.Append(m1);

        wxMenuItem* m2 =  new wxMenuItem(&menuPopup, 10002, _("Drop Down Item 2"));
        m2->SetBitmap(bmp);
        menuPopup.Append(m2);

        wxMenuItem* m3 =  new wxMenuItem(&menuPopup, 10003, _("Drop Down Item 3"));
        m3->SetBitmap(bmp);
        menuPopup.Append(m3);

        wxMenuItem* m4 =  new wxMenuItem(&menuPopup, 10004, _("Drop Down Item 4"));
        m4->SetBitmap(bmp);
        menuPopup.Append(m4);

        // line up our menu with the button
        wxRect rect = tb->GetToolRect(evt.GetId());
        wxPoint pt = tb->ClientToScreen(rect.GetBottomLeft());
        pt = ScreenToClient(pt);


        PopupMenu(&menuPopup, pt);


        // make sure the button is "un-stuck"
        tb->SetToolSticky(evt.GetId(), false);
    }
}


void MyFrame::OnTabAlignment(wxCommandEvent &evt)
{
    size_t i, count;
    wxAuiPaneInfoArray& all_panes = m_mgr.GetAllPanes();
    for (i = 0, count = all_panes.GetCount(); i < count; ++i)
    {
        wxAuiPaneInfo& pane = all_panes.Item(i);
        if (pane.window->IsKindOf(CLASSINFO(wxAuiNotebook)))
        {
            wxAuiNotebook* nb = (wxAuiNotebook*)pane.window;

            long style = nb->GetWindowStyleFlag();
            style &= ~(wxAUI_NB_TOP | wxAUI_NB_BOTTOM);
            if (evt.GetId() == ID_NotebookAlignTop)
                style |= wxAUI_NB_TOP;
            else if (evt.GetId() == ID_NotebookAlignBottom)
                style |= wxAUI_NB_BOTTOM;
            nb->SetWindowStyleFlag(style);

            nb->Refresh();
        }
    }
}

void MyFrame::OnExit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(_("wxAUI Demo\nAn advanced window management library for wxWidgets\n(c) Copyright 2005-2006, Kirix Corporation"), _("About wxAUI Demo"), wxOK, this);
}

wxTextCtrl* MyFrame::CreateTextCtrl(const wxString& ctrl_text)
{
    static int n = 0;

    wxString text;
    if ( !ctrl_text.empty() )
        text = ctrl_text;
    else
        text.Printf("This is text box %d", ++n);

    return new wxTextCtrl(this,wxID_ANY, text,
                          wxPoint(0,0), FromDIP(wxSize(150,90)),
                          wxNO_BORDER | wxTE_MULTILINE);
}


wxGrid* MyFrame::CreateGrid()
{
    wxGrid* grid = new wxGrid(this, wxID_ANY,
                              wxPoint(0,0),
                              FromDIP(wxSize(150,250)),
                              wxNO_BORDER | wxWANTS_CHARS);
    grid->CreateGrid(50, 20);
    return grid;
}

wxTreeCtrl* MyFrame::CreateTreeCtrl()
{
    wxTreeCtrl* tree = new wxTreeCtrl(this, wxID_ANY,
                                      wxPoint(0,0),
                                      FromDIP(wxSize(160,250)),
                                      wxTR_DEFAULT_STYLE | wxNO_BORDER);

    wxSize size(16, 16);
    wxVector<wxBitmapBundle> images;
    images.push_back(wxArtProvider::GetBitmapBundle(wxART_FOLDER, wxART_OTHER, size));
    images.push_back(wxArtProvider::GetBitmapBundle(wxART_NORMAL_FILE, wxART_OTHER, size));
    tree->SetImages(images);

    wxTreeItemId root = tree->AddRoot("wxAUI Project", 0);
    wxArrayTreeItemIds items;



    items.Add(tree->AppendItem(root, "Item 1", 0));
    items.Add(tree->AppendItem(root, "Item 2", 0));
    items.Add(tree->AppendItem(root, "Item 3", 0));
    items.Add(tree->AppendItem(root, "Item 4", 0));
    items.Add(tree->AppendItem(root, "Item 5", 0));


    size_t i, count;
    for (i = 0, count = items.Count(); i < count; ++i)
    {
        wxTreeItemId id = items.Item(i);
        tree->AppendItem(id, "Subitem 1", 1);
        tree->AppendItem(id, "Subitem 2", 1);
        tree->AppendItem(id, "Subitem 3", 1);
        tree->AppendItem(id, "Subitem 4", 1);
        tree->AppendItem(id, "Subitem 5", 1);
    }


    tree->Expand(root);

    return tree;
}

wxSizeReportCtrl* MyFrame::CreateSizeReportCtrl(const wxSize& size)
{
    wxSizeReportCtrl* ctrl = new wxSizeReportCtrl(this, wxID_ANY,
                                   wxDefaultPosition,
                                   size, &m_mgr);
    return ctrl;
}

wxHtmlWindow* MyFrame::CreateHTMLCtrl(wxWindow* parent)
{
    if (!parent)
        parent = this;

    wxHtmlWindow* ctrl = new wxHtmlWindow(parent, wxID_ANY,
                                   wxDefaultPosition,
                                   FromDIP(wxSize(400,300)));
    ctrl->SetPage(GetIntroText());
    return ctrl;
}

wxAuiNotebook* MyFrame::CreateNotebook()
{
   // create the notebook off-window to avoid flicker
   wxSize client_size = GetClientSize();

   wxAuiNotebook* ctrl = new wxAuiNotebook(this, wxID_ANY,
                                    wxPoint(client_size.x, client_size.y),
                                    FromDIP(wxSize(430,200)),
                                    m_notebook_style);
   ctrl->Freeze();

   wxBitmapBundle page_bmp = wxArtProvider::GetBitmapBundle(wxART_NORMAL_FILE, wxART_OTHER, wxSize(16,16));

   ctrl->AddPage(CreateHTMLCtrl(ctrl), "Welcome to wxAUI" , false, page_bmp);
   ctrl->SetPageToolTip(0, "Welcome to wxAUI (this is a page tooltip)");

   wxPanel *panel = new wxPanel( ctrl, wxID_ANY );
   wxFlexGridSizer *flex = new wxFlexGridSizer( 4, 2, 0, 0 );
   flex->AddGrowableRow( 0 );
   flex->AddGrowableRow( 3 );
   flex->AddGrowableCol( 1 );
   flex->Add( FromDIP(5), FromDIP(5) );   flex->Add( FromDIP(5), FromDIP(5) );
   flex->Add( new wxStaticText( panel, -1, "wxTextCtrl:" ), 0, wxALL|wxALIGN_CENTRE, FromDIP(5) );
   flex->Add( new wxTextCtrl( panel, -1, "", wxDefaultPosition, FromDIP(wxSize(100,-1))),
                1, wxALL|wxALIGN_CENTRE, FromDIP(5) );
   flex->Add( new wxStaticText( panel, -1, "wxSpinCtrl:" ), 0, wxALL|wxALIGN_CENTRE, FromDIP(5) );
   flex->Add( new wxSpinCtrl( panel, -1, "5", wxDefaultPosition, wxDefaultSize,
                wxSP_ARROW_KEYS, 5, 50, 5 ), 0, wxALL|wxALIGN_CENTRE, FromDIP(5) );
   flex->Add( FromDIP(5), FromDIP(5) );   flex->Add( FromDIP(5), FromDIP(5) );
   panel->SetSizer( flex );
   ctrl->AddPage( panel, "wxPanel", false, page_bmp );


   ctrl->AddPage( new wxTextCtrl( ctrl, wxID_ANY, "Some text",
                wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxNO_BORDER) , "wxTextCtrl 1", false, page_bmp );

   ctrl->AddPage( new wxTextCtrl( ctrl, wxID_ANY, "Some more text",
                wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxNO_BORDER) , "wxTextCtrl 2" );

   ctrl->AddPage( new wxTextCtrl( ctrl, wxID_ANY, "Some more text",
                wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxNO_BORDER) , "wxTextCtrl 3" );

   ctrl->AddPage( new wxTextCtrl( ctrl, wxID_ANY, "Some more text",
                wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxNO_BORDER) , "wxTextCtrl 4" );

   ctrl->AddPage( new wxTextCtrl( ctrl, wxID_ANY, "Some more text",
                wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxNO_BORDER) , "wxTextCtrl 5" );

   ctrl->AddPage( new wxTextCtrl( ctrl, wxID_ANY, "Some more text",
                wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxNO_BORDER) , "wxTextCtrl 6" );

   ctrl->AddPage( new wxTextCtrl( ctrl, wxID_ANY, "Some more text",
                wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxNO_BORDER) , "wxTextCtrl 7 (longer title)" );
   ctrl->SetPageToolTip(ctrl->GetPageCount()-1,
                        "wxTextCtrl 7: and the tooltip message can be even longer!");

   ctrl->AddPage( new wxTextCtrl( ctrl, wxID_ANY, "Some more text",
                wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxNO_BORDER) , "wxTextCtrl 8" );

   ctrl->Thaw();
   return ctrl;
}

wxString MyFrame::GetIntroText()
{
    const char* text =
        "<html><body>"
        "<h3>Welcome to wxAUI</h3>"
        "<br/><b>Overview</b><br/>"
        "<p>wxAUI is an Advanced User Interface library for the wxWidgets toolkit "
        "that allows developers to create high-quality, cross-platform user "
        "interfaces quickly and easily.</p>"
        "<p><b>Features</b></p>"
        "<p>With wxAUI, developers can create application frameworks with:</p>"
        "<ul>"
        "<li>Native, dockable floating frames</li>"
        "<li>Perspective saving and loading</li>"
        "<li>Native toolbars incorporating real-time, &quot;spring-loaded&quot; dragging</li>"
        "<li>Customizable floating/docking behaviour</li>"
        "<li>Completely customizable look-and-feel</li>"
        "<li>Optional transparent window effects (while dragging or docking)</li>"
        "<li>Splittable notebook control</li>"
        "</ul>"
        "<p><b>What's new in 0.9.4?</b></p>"
        "<p>wxAUI 0.9.4, which is bundled with wxWidgets, adds the following features:"
        "<ul>"
        "<li>New wxAuiToolBar class, a toolbar control which integrates more "
        "cleanly with wxAuiFrameManager.</li>"
        "<li>Lots of bug fixes</li>"
        "</ul>"
        "<p><b>What's new in 0.9.3?</b></p>"
        "<p>wxAUI 0.9.3, which is now bundled with wxWidgets, adds the following features:"
        "<ul>"
        "<li>New wxAuiNotebook class, a dynamic splittable notebook control</li>"
        "<li>New wxAuiMDI* classes, a tab-based MDI and drop-in replacement for classic MDI</li>"
        "<li>Maximize/Restore buttons implemented</li>"
        "<li>Better hinting with wxGTK</li>"
        "<li>Class rename.  'wxAui' is now the standard class prefix for all wxAUI classes</li>"
        "<li>Lots of bug fixes</li>"
        "</ul>"
        "<p><b>What's new in 0.9.2?</b></p>"
        "<p>The following features/fixes have been added since the last version of wxAUI:</p>"
        "<ul>"
        "<li>Support for wxMac</li>"
        "<li>Updates for wxWidgets 2.6.3</li>"
        "<li>Fix to pass more unused events through</li>"
        "<li>Fix to allow floating windows to receive idle events</li>"
        "<li>Fix for minimizing/maximizing problem with transparent hint pane</li>"
        "<li>Fix to not paint empty hint rectangles</li>"
        "<li>Fix for 64-bit compilation</li>"
        "</ul>"
        "<p><b>What changed in 0.9.1?</b></p>"
        "<p>The following features/fixes were added in wxAUI 0.9.1:</p>"
        "<ul>"
        "<li>Support for MDI frames</li>"
        "<li>Gradient captions option</li>"
        "<li>Active/Inactive panes option</li>"
        "<li>Fix for screen artifacts/paint problems</li>"
        "<li>Fix for hiding/showing floated window problem</li>"
        "<li>Fix for floating pane sizing problem</li>"
        "<li>Fix for drop position problem when dragging around center pane margins</li>"
        "<li>LF-only text file formatting for source code</li>"
        "</ul>"
        "<p>See README.txt for more information.</p>"
        "</body></html>";

    return wxString::FromAscii(text);
}
using MainWnd = MyFrame;
//#else

#include <wx/splitter.h>
#include <wx/treectrl.h>

#include <wx/filedlg.h>
#include <wx/wfstream.h>
#include <wx/aboutdlg.h>

#include "wxwin16x16.xpm"
#include "new.xpm"
#include "fileopen.xpm"
#include "filesave.xpm"
#include "htmfoldr.xpm"

#include "cut.xpm"
#include "copy.xpm"
#include "find.xpm"

namespace ftk
{
    class Label : public wxStaticText
    {
        wxDECLARE_DYNAMIC_CLASS(Label);
    public:
        Label() {}
        Label(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxStaticTextNameStr)
        {
            Create(parent, id, label, pos, size, style, name);
        };

    protected:
        void OnLeftMouseButtonDown(wxMouseEvent& event);
        wxDECLARE_EVENT_TABLE();
    };

    wxBEGIN_EVENT_TABLE(Label, wxStaticText)
        EVT_LEFT_DOWN(Label::OnLeftMouseButtonDown)
    wxEND_EVENT_TABLE()

    wxIMPLEMENT_DYNAMIC_CLASS(Label, wxStaticText)

    //----------------------------------------------------------------------------
    void Label::OnLeftMouseButtonDown(wxMouseEvent& event)
    //----------------------------------------------------------------------------
    {
        wxCommandEvent e(wxEVT_COMMAND_BUTTON_CLICKED, GetId());
        e.SetEventObject(this);
        ProcessEvent(e);
    }


#define ID_LABEL_CLICK 0
#define ID_ROLL_OUT    1
    class Panel : public wxPanel
    {
    public:
        Panel(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = L"panel");
    protected:
        wxDECLARE_EVENT_TABLE();
    };

    class NamedPanel : public Panel
    {
    public:
        NamedPanel(wxWindow* parent, wxWindowID id = -1, bool CloseButton = false, bool HideTitle = false);
        ~NamedPanel() override;

        void Add(wxWindow* window, int option = 0, int flag = wxEXPAND, int border = 0);
        void Add(wxSizer* sizer, int option = 0, int flag = wxEXPAND, int border = 0);

        bool Remove(wxWindow* window);
        bool Remove(wxSizer* sizer);

        void SetTitle(const wxString& label);
        void SetTitleColor(wxColour* color = NULL);

    protected:
        wxPanel* m_Top;
        Label* m_Label;
        wxColour   m_Color;

        wxDECLARE_EVENT_TABLE();
    };

    class ScrolledPanel : public wxScrolledWindow
    {
    public:
        ScrolledPanel(wxWindow* parent, wxWindowID id = -1);
        ~ScrolledPanel() override;

        void Add(wxWindow* window, int option = 0, int flag = wxEXPAND, int border = 0);
        void Add(wxSizer* sizer, int option = 0, int flag = wxEXPAND, int border = 0);

        bool Remove(wxWindow* window);
        bool Remove(wxSizer* sizer);

        //Adjust the ScrollBar Settings. Must be called explicitly if the size of the contents change at runtime.
        //void FitInside();

    protected:
        wxDECLARE_EVENT_TABLE();
    };

    class MutexPanel : public Panel
    {
    public:
        MutexPanel(wxWindow* parent, wxWindowID id);
        ~MutexPanel() override;

        /** Put the gui onto the panel removing the previous GUI. */
        bool Put(wxWindow* win);

        /** Remove the current gui. */
        bool RemoveCurrentClient();

        /** retrieve the current gui on panel. */
        wxWindow* GetCurrentClient();

    protected:
        //wxWindow *m_Client;
        wxDECLARE_EVENT_TABLE();
    };

    class PanelStack : public Panel
    {
    public:
        PanelStack(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = "panel");
        ~PanelStack() override;

        bool Put(Panel* p);
        bool Remove(Panel* p);

        void Push(Panel* p);
        void Pop();

    protected:
        void OnSize(wxSizeEvent& event);
        void DoLayout();

        Panel* m_CurrentPanel;

        wxDECLARE_EVENT_TABLE();
    };

    class RollOutPanel : public Panel
    {
    public:
        RollOutPanel(wxWindow* parent_window, const wxString& title, wxWindow* rolled_window_panel, int id = -1, bool rollOutOpen = true);
        ~RollOutPanel() override;

        void RollOut(bool open = true);

    protected:
        /** Answer the click on the Picture button to open/close the roll out gui.*/
        void OnRollOut(wxCommandEvent& event);

        wxWindow* m_ParentWindow; ///< Gui on which will be plugged the RollOut gui.
        wxWindow* m_RolledWindow; ///< Child gui that will be rolled out.
        wxBitmapButton* m_RollOutButton; ///< Picture button representing the icon to open/close the roll out gui.
        wxDECLARE_EVENT_TABLE();
    };

    class NamedScrolledPanel : public NamedPanel
    {
    public:
        NamedScrolledPanel(wxWindow* parent, wxWindowID id, bool CloseButton = false, bool HideTitle = false);
        ~NamedScrolledPanel() override;

        //wxPanel* GetPanel() {return (wxPanel*)m_ScrolledPanel;}

        bool     PutWindow(wxWindow* window);
        bool     RemoveCurrentWindow();
        wxWindow* GetCurrentWindow();

    protected:
        ScrolledPanel* m_ScrolledPanel;

        wxDECLARE_EVENT_TABLE();
    };


    wxWindowBase* mafGetFrame() { return nullptr; }
    wxBEGIN_EVENT_TABLE(Panel, wxPanel)
        wxEND_EVENT_TABLE()
        //----------------------------------------------------------------------------
        Panel::Panel(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name) : wxPanel(parent, id, pos, size, style, name)
        //----------------------------------------------------------------------------
    {
    }


    wxBEGIN_EVENT_TABLE(NamedPanel, Panel)
        wxEND_EVENT_TABLE()
        //----------------------------------------------------------------------------
#define NamedPanelStyle wxNO_BORDER | wxCLIP_CHILDREN
  //----------------------------------------------------------------------------
        NamedPanel::NamedPanel(wxWindow* parent, wxWindowID id, bool CloseButton, bool HideTitle) :
        Panel(parent, id, wxDefaultPosition, wxDefaultSize, NamedPanelStyle),
        m_Top(nullptr),
        m_Label(nullptr),
        //m_Color(133,162,185)
        //m_Color(190,190,190)
        //m_Color(255,255,255)
        m_Color(110, 150, 200)
        //----------------------------------------------------------------------------
    {
        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

        if (!HideTitle)
        {
            //m_Top = new wxStaticBox( this, -1,"",wxDefaultPosition,wxSize(-1,30));
            m_Top = new wxPanel(this, -1);
            m_Label = new Label(m_Top, ID_LABEL_CLICK, "Panel Title:");
            wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);

            wxFont font = wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
#if WIN32
            font.SetPointSize(9);
#endif
            //font.SetWeight(wxBOLD);
            m_Label->SetFont(font);
            topSizer->Add(m_Label, 1, wxEXPAND);

            if (CloseButton)
            {
                /*mafGUIPicButton *b = new mafGUIPicButton(m_Top, "CLOSE_SASH",ID_CLOSE_SASH);
                b->SetEventId(ID_CLOSE_SASH);  //SIL. 7-4-2005:
                m_TopSizer->Add(b, 0, wxRIGHT, 2);*/
            }

            m_Top->SetSizer(topSizer);
            topSizer->SetSizeHints(m_Top);

            sizer->Add(m_Top, 0, wxEXPAND | wxTOP, 2);
        }

        SetTitleColor(&m_Color);

        SetSizer(sizer);
        sizer->SetSizeHints(this);
    }
    //----------------------------------------------------------------------------
    NamedPanel::~NamedPanel()
        //----------------------------------------------------------------------------
    {
    }
    //----------------------------------------------------------------------------
    void NamedPanel::Add(wxWindow* window, int option, int flag, int border)
        //----------------------------------------------------------------------------
    {
        window->Reparent(this);
        if (wxSizer* wsizer = GetSizer())
            wsizer->Add(window, option, flag, border);
    }
    //----------------------------------------------------------------------------
    void NamedPanel::Add(wxSizer* sizer, int option, int flag, int border)
        //----------------------------------------------------------------------------
    {
        if (wxSizer* wsizer = GetSizer())
            wsizer->Add(sizer, option, flag, border);
    }
    //----------------------------------------------------------------------------
    bool NamedPanel::Remove(wxWindow* window)
        //----------------------------------------------------------------------------
    {
        if (wxSizer* wsizer = GetSizer())
            return wsizer->Detach(window);
        return false;
    }
    //----------------------------------------------------------------------------
    bool NamedPanel::Remove(wxSizer* sizer)
        //----------------------------------------------------------------------------
    {
        if (wxSizer* wsizer = GetSizer())
            return wsizer->Detach(sizer);
        return false;
    }
    //----------------------------------------------------------------------------
    void NamedPanel::SetTitle(const wxString& label)
        //----------------------------------------------------------------------------
    {
        if (m_Label)
            m_Label->SetLabel(label);
    }
    //----------------------------------------------------------------------------
    void NamedPanel::SetTitleColor(wxColour* color)
        //----------------------------------------------------------------------------
    {
        if (color)
            m_Color = *color;
        if (m_Top)
            m_Top->SetBackgroundColour(m_Color);
        if (m_Label)
            m_Label->SetBackgroundColour(m_Color);
    }

    //----------------------------------------------------------------------------
    // mafGUIScrolledPanel
    //----------------------------------------------------------------------------
    wxBEGIN_EVENT_TABLE(ScrolledPanel, wxScrolledWindow)
        //EVT_BUTTON (ID_EPANEL, wxEPanel::OnButton)
        wxEND_EVENT_TABLE()
        //----------------------------------------------------------------------------
        ScrolledPanel::ScrolledPanel(wxWindow* parent, wxWindowID id) :
        wxScrolledWindow(parent, id, wxDefaultPosition, wxDefaultSize, wxNO_BORDER)
        //----------------------------------------------------------------------------
    {
        SetScrollbars(0, 10, 0, 100);
        EnableScrolling(0, 1);

        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
        SetSizer(sizer);
        sizer->SetSizeHints(this);
    }
    //----------------------------------------------------------------------------
    ScrolledPanel::~ScrolledPanel()
        //----------------------------------------------------------------------------
    {
    }
    //----------------------------------------------------------------------------
    void ScrolledPanel::Add(wxWindow* window, int option, int flag, int border)
        //----------------------------------------------------------------------------
    {
        if (wxSizer* sizer = GetSizer())
            sizer->Add(window, option, flag, border);
        FitInside();
    }
    //----------------------------------------------------------------------------
    void ScrolledPanel::Add(wxSizer* sizer, int option, int flag, int border)
        //----------------------------------------------------------------------------
    {
        if (wxSizer* sizer = GetSizer())
            sizer->Add(sizer, option, flag, border);
        FitInside();
    }
    //----------------------------------------------------------------------------
    bool ScrolledPanel::Remove(wxWindow* window)
        //----------------------------------------------------------------------------
    {
        if (wxSizer* sizer = GetSizer())
            return sizer->Detach(window);
        return false;
    }
    //----------------------------------------------------------------------------
    bool ScrolledPanel::Remove(wxSizer* sizer)
        //----------------------------------------------------------------------------
    {
        if (wxSizer* sizer = GetSizer())
            return sizer->Detach(sizer);
        return false;
        FitInside();
    }
    //----------------------------------------------------------------------------
    /*void ScrolledPanel::FitInside()
    //----------------------------------------------------------------------------
    {
      // Paolo 13-08-2007: fix resizing scrollbars to start from the
      // scrolled position and not from 0; this avoid jumps to the
      // scrollbar.
      wxSizer *sizer = GetSizer();
      if(!sizer)
        return;
      int pos_old = GetScrollPos(wxVERTICAL);
      int range_old = GetScrollRange(wxVERTICAL);
      int range_new = sizer->GetMinSize().GetHeight() / 10;
      int pos_new = (pos_old * range_new) / range_old;
      SetScrollbars(0, 10, 0, sizer->GetMinSize().GetHeight() / 10, 0, pos_new);
      Layout();
    }*/


    wxBEGIN_EVENT_TABLE(MutexPanel, Panel)
        //EVT_BUTTON (ID_EPANEL, wxEPanel::OnButton)
        wxEND_EVENT_TABLE()
        //----------------------------------------------------------------------------
        MutexPanel::MutexPanel(wxWindow* parent, wxWindowID id) :
        Panel(parent, id)
        //----------------------------------------------------------------------------
    {
        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
        SetSizer(sizer);
        //m_Sizer->SetSizeHints(this);
    }
    //----------------------------------------------------------------------------
    MutexPanel::~MutexPanel()
        //----------------------------------------------------------------------------
    {
    }
    //----------------------------------------------------------------------------
    bool MutexPanel::Put(wxWindow* win)
        //----------------------------------------------------------------------------
    {
        if (win == GetCurrentClient())
            return false;

        RemoveCurrentClient();
        if (win == nullptr)
            return false;

        //win->FitGui(); // solo in mafGUI
        win->Reparent(this);
        if (wxSizer* sizer = GetSizer())
            sizer->Add(win, 1, wxEXPAND);
        win->Show(true);
        win->Update();

        //m_Sizer->SetSizeHints(this);
        Layout();

        return true;
    }
    //----------------------------------------------------------------------------
    bool MutexPanel::RemoveCurrentClient()
        //----------------------------------------------------------------------------
    {
        wxWindow* current_gui = GetCurrentClient();
        if (current_gui == nullptr)
            return false;

        if (wxSizer* sizer = GetSizer())
            sizer->Detach(current_gui);
        current_gui->Show(false);
        current_gui->Reparent(nullptr);
        Layout();
        return true;
    }
    //----------------------------------------------------------------------------
    wxWindow* MutexPanel::GetCurrentClient()
        //----------------------------------------------------------------------------
    {
        size_t num_children = this->GetChildren().GetCount();
        assert(num_children <= 1);

        if (num_children == 1)
        {
            auto node = this->GetChildren().GetFirst();
            return (wxWindow*)node->GetData();
        }
        return nullptr;
    }

    wxBEGIN_EVENT_TABLE(PanelStack, Panel)
        EVT_SIZE(PanelStack::OnSize)
        wxEND_EVENT_TABLE()
        //----------------------------------------------------------------------------
        PanelStack::PanelStack(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name) :
        Panel(parent, id, pos, size, style, name)
        //----------------------------------------------------------------------------
    {
        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

        m_CurrentPanel = new Panel(this, -1);

        Push(new Panel(this, -1));

        SetSizer(sizer);
        sizer->SetSizeHints(this);
    }
    //----------------------------------------------------------------------------
    PanelStack::~PanelStack()
        //----------------------------------------------------------------------------
    {
    }
    //----------------------------------------------------------------------------
    bool PanelStack::Put(Panel* p)
        //----------------------------------------------------------------------------
    {
        Push(p);
        return true;
    }
    //----------------------------------------------------------------------------
    bool PanelStack::Remove(Panel* p)
        //----------------------------------------------------------------------------
    {
        assert(p);
        assert(p == m_CurrentPanel);
        Pop();
        return true;
    }
    //----------------------------------------------------------------------------
    void PanelStack::Push(Panel* p)
        //----------------------------------------------------------------------------
    {
        if (m_CurrentPanel == p)
            return;

        assert(p);
        assert(m_CurrentPanel);
        m_CurrentPanel->Reparent(mafGetFrame());
        m_CurrentPanel->Show(false);
        if (wxSizer* sizer = GetSizer())
            sizer->Detach(m_CurrentPanel);

        //p->m_NextPanel = m_CurrentPanel;
        m_CurrentPanel = p;

        m_CurrentPanel->Show(true);
        m_CurrentPanel->Reparent(this);
        if (wxSizer* sizer = GetSizer())
            sizer->Add(m_CurrentPanel, 1, wxEXPAND);

        DoLayout();
    }
    //----------------------------------------------------------------------------
    void PanelStack::Pop()
        //----------------------------------------------------------------------------
    {
        assert(m_CurrentPanel);

        m_CurrentPanel->Show(false);
        m_CurrentPanel->Reparent(mafGetFrame());
        if (wxSizer* sizer = GetSizer())
            sizer->Detach(m_CurrentPanel);

        //m_CurrentPanel= m_CurrentPanel->m_NextPanel;
        assert(m_CurrentPanel);

        m_CurrentPanel->Show(true);
        m_CurrentPanel->Reparent(this);
        if (wxSizer* sizer = GetSizer())
            sizer->Add(m_CurrentPanel, 1, wxEXPAND);

        DoLayout();
    }
    //----------------------------------------------------------------------------
    void PanelStack::OnSize(wxSizeEvent& WXUNUSED(event))
        //----------------------------------------------------------------------------
    {
        DoLayout();
    }
    //----------------------------------------------------------------------------
    void PanelStack::DoLayout()
        //----------------------------------------------------------------------------
    {
        //wxSizeEvent e;
        //wxPanel::
          //wxScrolledWindow::OnSize(e);
    }


    wxBEGIN_EVENT_TABLE(RollOutPanel, Panel)
        //EVT_COMMAND_RANGE(WIDGETS_START, WIDGETS_END, wxEVT_COMMAND_BUTTON_CLICKED, RollOutPanel::OnRollOut)
        EVT_COMMAND(ID_ROLL_OUT, wxEVT_COMMAND_BUTTON_CLICKED, RollOutPanel::OnRollOut)
        wxEND_EVENT_TABLE()

        static const char* ROLLOUT_CLOSE_xpm[] = {
          "16 16 3 1",
          " 	c None",
          ".	c #EFEDDE",
          "+	c #4D4DA6",
          "................",
          "....+...........",
          "....++..........",
          "....+++.........",
          "....++++........",
          "....+++++.......",
          "....++++++......",
          "....+++++++.....",
          "....++++++......",
          "....+++++.......",
          "....++++........",
          "....+++.........",
          "....++..........",
          "....+...........",
          "................",
          "................" };
    static const char* ROLLOUT_OPEN_xpm[] = {
      "16 16 3 1",
      " 	c None",
      ".	c #EFEDDE",
      "+	c #4D4DA6",
      "................",
      "................",
      "................",
      "................",
      ".+++++++++++++..",
      "..+++++++++++...",
      "...+++++++++....",
      "....+++++++.....",
      ".....+++++......",
      "......+++.......",
      ".......+........",
      "................",
      "................",
      "................",
      "................",
      "................" };


    //----------------------------------------------------------------------------
    RollOutPanel::RollOutPanel(wxWindow* parent_window, const wxString& title, wxWindow* rolled_window, int id, bool rollOutOpen) :
        Panel(parent_window, id)
        //----------------------------------------------------------------------------
    {
        wxBoxSizer* topsizer = new wxBoxSizer(wxHORIZONTAL);

        const char** b_xpm = rollOutOpen ? ROLLOUT_OPEN_xpm : ROLLOUT_CLOSE_xpm;
        m_RollOutButton = new wxBitmapButton(this, ID_ROLL_OUT, wxBitmap(b_xpm));
        //m_RollOutButton->SetEventId(ID_ROLL_OUT);
        topsizer->Add(m_RollOutButton, 0, wxLEFT, 2);

        wxStaticText* lab = new wxStaticText(this, -1, title);
        wxFont font = wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
#if WIN32
        font.SetPointSize(9);
#endif
        font.SetWeight(wxFONTWEIGHT_BOLD);
        lab->SetFont(font);
        topsizer->Add(lab, 1, wxEXPAND | wxALL);

        SetSizer(topsizer);
        topsizer->SetSizeHints(this);

        m_ParentWindow = parent_window;
        m_RolledWindow = rolled_window;

        //m_ParentWindow->Add(this);
        Reparent(m_ParentWindow);
        if (wxSizer* sizer = m_ParentWindow->GetSizer())
            sizer->Add(this);
        //m_MainGui->AddGui(m_RolledWindow);
        m_RolledWindow->Show(rollOutOpen);
        //m_ParentWindow->FitGui();
    }
    //----------------------------------------------------------------------------
    RollOutPanel::~RollOutPanel()
        //----------------------------------------------------------------------------
    {
    }
    //----------------------------------------------------------------------------
    void RollOutPanel::OnRollOut(wxCommandEvent& event)
        //----------------------------------------------------------------------------
    {
        int widget_id = this->GetId();
        RollOut(!m_RolledWindow->IsShown());
        bool isOpen = m_RolledWindow->IsShown();
        //mafEventMacro(mafEvent(this, widget_id, isOpen));
    }
    //----------------------------------------------------------------------------
    void RollOutPanel::RollOut(bool open)
        //----------------------------------------------------------------------------
    {
        m_RolledWindow->Show(open);
        const char** b_xpm = m_RolledWindow->IsShown() ? ROLLOUT_OPEN_xpm : ROLLOUT_CLOSE_xpm;
        m_RollOutButton->SetBitmapLabel(wxBitmap(b_xpm));
        m_ParentWindow->Update();
        //m_RolledWindow->FitGui();
        wxWindow* parent = m_ParentWindow->GetParent();
        if (parent)
        {
            // Refresh the scrollbar (if exists) when open/close
            // the roll out GUI.
            parent->FitInside();
        }
    }

    //----------------------------------------------------------------------------
    BEGIN_EVENT_TABLE(NamedScrolledPanel, NamedPanel)
        //EVT_BUTTON (ID_EPANEL, wxEPanel::OnButton)
        END_EVENT_TABLE()

        //----------------------------------------------------------------------------
        NamedScrolledPanel::NamedScrolledPanel(wxWindow* parent, wxWindowID id, bool CloseButton, bool HideTitle) :
        NamedPanel(parent, id, CloseButton, HideTitle)
        //----------------------------------------------------------------------------
    {
        SetTitle("module controls area:");
        m_ScrolledPanel = new ScrolledPanel(this, -1);
        Add(m_ScrolledPanel, 1, wxEXPAND);
    }
    //----------------------------------------------------------------------------
    NamedScrolledPanel::~NamedScrolledPanel()
        //----------------------------------------------------------------------------
    {
    }
    //----------------------------------------------------------------------------
    bool NamedScrolledPanel::PutWindow(wxWindow* window)
        //----------------------------------------------------------------------------
    {
        if (window == GetCurrentWindow())
            return false;

        RemoveCurrentWindow();
        if (window == nullptr)
            return false;
        //window->FitGui();
        window->Reparent(m_ScrolledPanel);
        m_ScrolledPanel->Add(window, 1, wxEXPAND);
        window->Show(true);
        window->Update();

        m_ScrolledPanel->Layout();
        window->SetFocus();
        return true;
    }
    //----------------------------------------------------------------------------
    bool NamedScrolledPanel::RemoveCurrentWindow()
        //----------------------------------------------------------------------------
    {
        wxWindow* current_window = GetCurrentWindow();
        if (current_window == nullptr)
            return false;  //nothing to remove

        m_ScrolledPanel->Remove(current_window);
        current_window->Show(false);
        current_window->Reparent(mafGetFrame());
        //  current_gui->Show(false);
        m_ScrolledPanel->Layout();
        return true;
    }
    //----------------------------------------------------------------------------
    wxWindow* NamedScrolledPanel::GetCurrentWindow()
        //----------------------------------------------------------------------------
    {
        size_t num_children = m_ScrolledPanel->GetChildren().GetCount();
        assert(num_children <= 1);

        if (num_children == 1)
        {
            auto node = m_ScrolledPanel->GetChildren().GetFirst();
            return (wxWindow*)node->GetData();
        }
        return nullptr;
    }
}
#endif
namespace ftk
{
    class Navigator : public wxNotebook
    {
    public:
        enum IDs
        {
            PGID = 1,
            ID_INFO_TREE = 5152
        };
        const bool DOUBLE_NOTEBOOK = true;
        Navigator(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxNotebookNameStr)
            : wxNotebook(parent, id, pos, size, style, name)
        {
            //m_node = Node::New_();
            //m_node->SetName("First node");
            
            
                SetFont(wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT)));
            wxSplitterWindow* splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER/* | wxSP_3DSASH | wxSP_FULLSASH*//*wxSP_THIN_SASH*/);

            auto treePanel = new wxPanel(splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxCLIP_CHILDREN);
            auto sizer = new wxBoxSizer(wxVERTICAL);
            treePanel->SetSizerAndFit(sizer);

            //panel->SetSizer(sizer);
//            sizer->SetSizeHints(panel);

            //m_InfoTree = new wxTreeCtrl(panel, ID_INFO_TREE, wxPoint(50, 50)/*wxDefaultPosition*/, wxSize(100,100));//, wxNO_BORDER | wxTR_HAS_BUTTONS);
  
            
            auto infoTree = new wxTreeCtrl(treePanel, ID_INFO_TREE, wxDefaultPosition, wxDefaultSize, wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT | wxTR_SINGLE);
            sizer->Add(infoTree, 1, wxEXPAND);
            //panel->SetSize(parent->FromDIP(wxSize(-1, 300)));
            //panel->SetSize(FromDIP(wxSize(250, 300)));
            //panel->SetTitle(" vme hierarchy: ");
            //infoTree->AddRoot(L"Root");
            AddPage(splitter, _("data tree"), true);

            if (DOUBLE_NOTEBOOK)
            {
                auto nodeNotebook = new wxNotebook(splitter, wxID_ANY);
                //nodeNotebook->SetFont(wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT)));
                const char* panelNames[] = { "vme output" , "visual props", "vme" };
                std::vector<wxPanel*> panels;
                for (auto name : panelNames)
                {
                    panels.push_back(new NamedScrolledPanel(nodeNotebook, wxID_ANY, false, true));
                    nodeNotebook->AddPage(panels.back(), name);
                }
				wxPropertyGrid* pg = new wxPropertyGrid(panels.back(), PGID, wxDefaultPosition, wxSize(400, 400),
					wxPG_SPLITTER_AUTO_CENTER |
					wxPG_BOLD_MODIFIED);
				//m_pg = pg;

                //auto p = new wxStringProperty("String Property", wxPG_LABEL, m_node->GetName());
                //p->O
				//pg->Append(p);
				pg->Append(new wxIntProperty("Int Property", wxPG_LABEL));
				pg->Append(new wxBoolProperty("Bool Property", wxPG_LABEL));
                splitter->SetMinimumPaneSize(50);
                splitter->SplitHorizontally(treePanel, nodeNotebook);
            }
            else
            {
                auto panel = new wxPanel(this, wxID_ANY);// , false, true);
                AddPage(panel, _("vme"));

                auto panel2 = new wxPanel(splitter, wxID_ANY);// , false, true);
                panel2->Show(false);
                splitter->SetMinimumPaneSize(5);
                splitter->SplitHorizontally(treePanel, panel2);
            }

            //return;

            /*m_VmeOutputPanel = new mafGUIHolder(m_VmeNotebook,-1,false,true);
            m_VmeNotebook->AddPage(m_VmeOutputPanel,_(" vme output "));
            m_VmePipePanel = new mafGUIHolder(m_VmeNotebook,-1,false,true);
            m_VmeNotebook->AddPage(m_VmePipePanel,_(" visual props "));
            m_VmePanel = new mafGUIHolder(m_VmeNotebook,-1,false,true);
            m_VmeNotebook->AddPage(m_VmePanel,_("vme"));*/
            //wxPanel *m_VmeOutputPanel = new wxPanel(m_VmeNotebook,-1, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxCLIP_CHILDREN);
            //wxPanel *m_VmeOutputPanel = new fbg::NamedPanel(m_VmeNotebook, -1, false, true);
            //wxPanel *m_VmeOutputPanel = new fbg::RollOutPanel(m_VmeNotebook, L"Test", new fbg::NamedPanel(m_VmeNotebook));
            //m_VmeNotebook->AddPage(m_VmeOutputPanel,_(" vme output "));
            //wxPanel *m_VmePipePanel = new wxPanel(m_VmeNotebook,-1, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxCLIP_CHILDREN);
            //m_VmeNotebook->AddPage(m_VmePipePanel,_(" visual props "));
            //wxPanel *m_VmePanel = new wxPanel(m_VmeNotebook,-1, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxCLIP_CHILDREN);
            //m_VmeNotebook->AddPage(m_VmePanel,_("vme"));
            /*wxPanel*/

            //m_VmeOutputPanel->PutWindow(new fbg::NamedPanel(m_VmeNotebook));




            //wxTreeItemId root = m_InfoTree->AddRoot(m_NodeManager->GetRoot()->GetName().c_str());
            //wxTreeItemId root1 = m_InfoTree->AddRoot(m_NodeManager->GetRoot()->GetChild(0)->GetName().c_str());
            //mafNode::Pointer node = m_NodeManager->GetRoot();
            //FillTree(m_InfoTree, root, m_NodeManager->GetRoot());
            //wxTreeItemId item = root;
            //for()
            //m_InfoTree->AppendItem(root, _("Item 1"));
            //m_InfoTree->AppendItem(root, _("Item 2"));
            //m_InfoTree->AppendItem(root, _("Item 3"));
            //m_InfoTree->Expand(root);

        }
        
        ~Navigator() override
        {
            
        }
        
        void OnPropertyGridChange( wxPropertyGridEvent& event )
        {
            wxPGProperty* property = event.GetProperty();

            const wxString& name = property->GetName();

            // Properties store values internally as wxVariants, but it is preferred
            // to use the more modern wxAny at the interface level
            wxAny value = property->GetValue();
            if(name == "String Property")
            {
                //m_node->SetName(value.As<wxString>().ToStdString());
            }
        }
        void OnPropertyGridChanging( wxPropertyGridEvent& event ){;}
        void OnPropertyGridSelect( wxPropertyGridEvent& event ){;}
        void OnPropertyGridHighlight( wxPropertyGridEvent& event ){;}
        void OnPropertyGridItemRightClick( wxPropertyGridEvent& event ){;}
        void OnPropertyGridItemDoubleClick( wxPropertyGridEvent& event ){;}
        void OnPropertyGridPageChange( wxPropertyGridEvent& event ){;}
        void OnPropertyGridButtonClick( wxCommandEvent& event ){;}
        void OnPropertyGridTextUpdate( wxCommandEvent& event ){;}
        void OnPropertyGridKeyEvent( wxKeyEvent& event ){;}
        void OnPropertyGridItemCollapse( wxPropertyGridEvent& event ){;}
        void OnPropertyGridItemExpand( wxPropertyGridEvent& event ){;}
        void OnPropertyGridLabelEditBegin( wxPropertyGridEvent& event ){;}
        void OnPropertyGridLabelEditEnding( wxPropertyGridEvent& event ){;}
        void OnPropertyGridColBeginDrag( wxPropertyGridEvent& event ){;}
        void OnPropertyGridColDragging( wxPropertyGridEvent& event ){;}
        void OnPropertyGridColEndDrag( wxPropertyGridEvent& event ){;}

    protected:
        wxDECLARE_EVENT_TABLE();
    private:
        //std::unique_ptr<Node> m_node;
    };


    wxBEGIN_EVENT_TABLE(Navigator, wxNotebook)
        EVT_PG_CHANGED( PGID, Navigator::OnPropertyGridChange )
    wxEND_EVENT_TABLE()


    class AppSettings {
    public:
        AppSettings()
        {
            m_Check1 = true;
            m_Check2 = false;
        }
        bool m_Check1;
        bool m_Check2;
    };

    class PrefStorage
    {
    public:
        virtual const AppSettings& GetSettings() const = 0;
        virtual void UpdateSettings(const AppSettings& settings) = 0;
    };

    class PrefsPageGeneralPanel : public wxPanel
    {
    public:
        PrefsPageGeneralPanel(wxWindow* parent, PrefStorage& storage) : wxPanel(parent), m_storage(storage)
        {
            m_useMarkdown = new wxCheckBox(this, wxID_ANY, "Use Markdown syntax");
            m_spellcheck = new wxCheckBox(this, wxID_ANY, "Check spelling");

            wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
            sizer->Add(m_useMarkdown, wxSizerFlags().Border());
            sizer->Add(m_spellcheck, wxSizerFlags().Border());

            SetSizerAndFit(sizer);

            m_useMarkdown->Bind(wxEVT_CHECKBOX,
                &PrefsPageGeneralPanel::ChangedUseMarkdown,
                this);
            m_spellcheck->Bind(wxEVT_CHECKBOX,
                &PrefsPageGeneralPanel::ChangedSpellcheck,
                this);
        }

        bool TransferDataToWindow() wxOVERRIDE
        {
            m_settingsCurrent = m_storage.GetSettings();
            m_useMarkdown->SetValue(m_settingsCurrent.m_Check1);
            m_spellcheck->SetValue(m_settingsCurrent.m_Check2);
            return true;
        }

        bool TransferDataFromWindow() wxOVERRIDE
        {
            // Called on platforms with modal preferences dialog to save and apply
            // the changes.
            m_storage.UpdateSettings(m_settingsCurrent);
            return true;
        }

    private:
        void UpdateSettingsIfNecessary()
        {
            // On some platforms (OS X, GNOME), changes to preferences are applied
            // immediately rather than after the OK or Apply button is pressed, so
            // we need to take them into account as soon as they happen. On others
            // (MSW), we need to wait until the changes are accepted by the user by
            // pressing the "OK" button. To reuse the same code for both cases, we
            // always update m_settingsCurrent object under all platforms, but only
            // update the real application settings if necessary here.
            if (wxPreferencesEditor::ShouldApplyChangesImmediately())
            {
                m_storage.UpdateSettings(m_settingsCurrent);
            }
        }

        void ChangedUseMarkdown(wxCommandEvent& e)
        {
            m_settingsCurrent.m_Check1 = e.IsChecked();
            UpdateSettingsIfNecessary();
        }

        void ChangedSpellcheck(wxCommandEvent& e)
        {
            m_settingsCurrent.m_Check2 = e.IsChecked();
            UpdateSettingsIfNecessary();
        }

        wxCheckBox* m_useMarkdown;
        wxCheckBox* m_spellcheck;

        // Settings corresponding to the current values in this dialog.
        AppSettings m_settingsCurrent;
        PrefStorage& m_storage;
    };

    class PrefsPageGeneral : public wxStockPreferencesPage
    {
    public:
        PrefsPageGeneral(PrefStorage& storage) : wxStockPreferencesPage(Kind_General), m_storage(storage) {}

        wxWindow* CreateWindow(wxWindow* parent) wxOVERRIDE
        {
            return new PrefsPageGeneralPanel(parent, m_storage);
        }
        PrefStorage& m_storage;
    };


    class PrefsPageTopicsPanel : public wxPanel
    {
    public:
        PrefsPageTopicsPanel(wxWindow* parent) : wxPanel(parent)
        {
            wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
            sizer->Add(new wxStaticText(this, wxID_ANY, "Search in these topics:"), wxSizerFlags().Border());
            wxListBox* box = new wxListBox(this, wxID_ANY);
            box->SetMinSize(wxSize(400, 300));
            sizer->Add(box, wxSizerFlags(1).Border().Expand());
            m_fulltext = new wxCheckBox(this, wxID_ANY, "Automatically build fulltext index");
            sizer->Add(m_fulltext, wxSizerFlags().Border());

            SetSizerAndFit(sizer);

            if (wxPreferencesEditor::ShouldApplyChangesImmediately())
            {
                m_fulltext->Bind(wxEVT_CHECKBOX,
                    &PrefsPageTopicsPanel::ChangedFulltext,
                    this);
            }
        }

        bool TransferDataToWindow() wxOVERRIDE
        {
            // This is the place where you can initialize values, e.g. from wxConfig.
            // For demonstration purposes, we just set hardcoded values.
            m_fulltext->SetValue(true);
            // TODO: handle the listbox
            return true;
        }

        bool TransferDataFromWindow() wxOVERRIDE
        {
            // Called on platforms with modal preferences dialog to save and apply
            // the changes.
            wxCommandEvent dummy;
            ChangedFulltext(dummy);
            // TODO: handle the listbox
            return true;
        }

    private:
        void ChangedFulltext(wxCommandEvent& WXUNUSED(e))
        {
            // save new m_fulltext value and apply the change to the app
        }

        wxCheckBox* m_fulltext;
    };

    class PrefsPageTopics : public wxPreferencesPage
    {
    public:
	    wxString GetName() const wxOVERRIDE { return "Topics"; }

	    wxBitmapBundle GetIcon() const wxOVERRIDE
        {
            return wxArtProvider::GetBitmapBundle(wxART_HELP, wxART_TOOLBAR);
        }

	    wxWindow* CreateWindow(wxWindow* parent) wxOVERRIDE
        {
            return new PrefsPageTopicsPanel(parent);
        }
    };


    class NewMDIChildFrame : public wxMDIChildFrame
    {
		static const int PH = 13; //panel height
    public:
        NewMDIChildFrame(wxMDIParentFrame* parent)
            : wxMDIChildFrame(parent, wxID_ANY, "")
        {
			m_AllowCloseFlag = true;
			m_EnableResize = true;

			this->Show(true);
//			m_Win = m_View->GetWindow();
//			m_Win->Reparent(this);
//			m_Win->Show(true);

//			SetIcon(mafPictureFactory::GetPictureFactory()->GetIcon(_R("MDICHILD_ICON")));
//			SetTitle(wxStripMenuCodes(m_View->GetLabel().toWx()));
        }

		~NewMDIChildFrame() override
		{
        }

        void SetAllowCloseWindow(bool allow_close)
        {
            m_AllowCloseFlag = allow_close;
        }

        void EnableResize(bool enable = true)
		{
			m_EnableResize = enable;
		}
    protected:
        void OnSize(wxSizeEvent& event)
        {
			if (!m_EnableResize)
			{
				return;
			}

			int w, h;
			//don't initialize w & h using the event - use GetClientSize instead
			this->GetClientSize(&w, &h);

//			if (h < PH || w < PH) return;

	//		m_Win->Move(0, 0);
		//	m_Win->SetSize(w, h);
			//m_Win->Layout();
#ifndef WIN32
			//if (m_View)
			{
				//m_View->SetWindowSize(w, h);
			}
#endif
			//m_View->CameraUpdate();
        }

        void OnSelect(wxCommandEvent& event)
        {
			Activate(); // allow activation with the RMouse too
        }

        void OnCloseWindow(wxCloseEvent& event)
		{
			if (!m_AllowCloseFlag) { return; }

			// VIEW_DELETE must be sent from here and not from the destructor
			  // otherwise VIEW_DELETE is sent also on the closing of the application
			  // when the listener (the ViewManager) has been already destroyed

			//mafEventMacro(mafEvent(this, VIEW_DELETE, m_View));
			Destroy();
			//m_View = NULL;
		}


        void OnActivate(wxActivateEvent& event)
		{
//			if (event.GetActive() && m_View)
	//		{
		//		mafEventMacro(mafEvent(this, VIEW_SELECT, m_View, (wxWindow*)NULL));
			//	Layout();
			//}
		}

        void OnMaximize(wxMaximizeEvent& event)
		{
//			if (m_View)
	//		{
		//		mafString msg = _R("MaximizeSelectedView");
			//	mafEventMacro(mafEvent(this, VIEW_MAXIMIZE, &msg));
			//}
		}

        bool         m_AllowCloseFlag;
        bool         m_EnableResize;

        DECLARE_EVENT_TABLE()
    };
	BEGIN_EVENT_TABLE(NewMDIChildFrame, wxMDIChildFrame)
		EVT_CLOSE(NewMDIChildFrame::OnCloseWindow)
		EVT_SIZE(NewMDIChildFrame::OnSize)
		//EVT_BUTTON(VIEW_CLICKED, NewMDIChildFrame::OnSelect)
		EVT_ACTIVATE(NewMDIChildFrame::OnActivate)
		EVT_MAXIMIZE(NewMDIChildFrame::OnMaximize)
		END_EVENT_TABLE()


    const long DefaultFrameStyle = wxDEFAULT_FRAME_STYLE;
    //using BaseFrame = wxFrame;
    using BaseFrame = wxMDIParentFrame;
    //using BaseFrame = wxAuiMDIParentFrame;
    //using NewFrame = NewFrameT<wxFrame, wxDEFAULT_FRAME_STYLE>;
    //using NewFrame = NewFrameT<wxMDIParentFrame, wxDEFAULT_FRAME_STYLE>;
    //using NewFrame = NewFrameT<wxAuiMDIParentFrame, wxDEFAULT_FRAME_STYLE>;
    //template<class BaseFrame, long DefaultStyle>
    class NewFrame : public BaseFrame
    {
    public:
        class PreferencesStorage : public PrefStorage
        {
        public:
            PreferencesStorage(NewFrame& frame) :m_frame(frame) {}

            const AppSettings& GetSettings() const override
            {
                return m_frame.GetSettings();
            }

            void UpdateSettings(const AppSettings& settings) override
            {
                m_frame.UpdateSettings(settings);
            }
            NewFrame& m_frame;
        };
        const AppSettings& GetSettings() const { return m_settings; }
        void UpdateSettings(const AppSettings& settings)
        {
            m_settings = settings;
            //propagate m_settings;
        }

        AppSettings m_settings;
		std::unique_ptr<wxPreferencesEditor> m_prefEditor;
        std::unique_ptr<PreferencesStorage> m_preferences;


        void ShowPreferencesEditor()
        {
            if (!m_prefEditor)
            {
                m_prefEditor.reset(new wxPreferencesEditor);
                m_prefEditor->AddPage(new PrefsPageGeneral(*m_preferences));
                m_prefEditor->AddPage(new PrefsPageTopics());
            }
            m_prefEditor->Show(this);
        }

        void DismissPreferencesEditor()
        {
            if (m_prefEditor)
                m_prefEditor->Dismiss();
        }

        enum MENU_ID_ENUM
        {
            MENU_FILE_QUIT = wxID_EXIT,
            MENU_FILE_OPEN = wxID_OPEN,
            MENU_FILE_NEW = wxID_NEW,
            // it is important for the id corresponding to the "About" command to have
            // this standard value as otherwise it won't be handled properly under Mac
            // (where it is special and put into the "Apple" menu)
            MENU_ABOUT = wxID_ABOUT,
            ID_APP_SETTINGS = wxID_PREFERENCES,
            ID_START = wxID_LAST,
            ID_STANDARD_TOOLBAR = ID_START,
            ID_ADDITIONAL_TOOLBAR,
            ID_NOTEBOOK,
            ID_NAVIGATOR,
            //ID_INFO_TREE,
            ID_STATUSBAR,
            ID_LOGTEXTCTRL,
            ID_END,
            ID_LOAD_LAYOUT,
            ID_SAVE_LAYOUT,
            ID_OP_START,
            ID_OP_END = ID_OP_START + 1000,
            ID_VIEW_START = ID_OP_END,
            ID_VIEW_END = ID_VIEW_START + 1000
        };

        NewFrame() {}

        NewFrame(wxWindow* parent,
                 wxWindowID id,
                 const wxString& title,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = DefaultFrameStyle,
                 const wxString& name = wxASCII_STR(wxFrameNameStr))
            : BaseFrame()
        {
            Init();

            Create(parent, id, title, pos, size, style, name);
        }

        bool Create(wxWindow* parent,
                    wxWindowID id,
                    const wxString& title,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = DefaultFrameStyle,
                    const wxString& name = wxASCII_STR(wxFrameNameStr))
        {
            if (!BaseFrame::Create(parent, id, title, pos, size, style, name))
                return false;
            CreateControls();
            //Bind(wxEVT_MENU, &NewFrame::OnPreferences, this, wxID_PREFERENCES);

            return true;
        }
    protected:
        void OnAbout(wxCommandEvent& e)
        {
            wxAboutDialogInfo info;
            info.SetName("FTK Demo App");
            info.SetCopyright("(C) 2024 FRAMOSOFT");
            info.SetVersion("v1.0");
            info.SetWebSite("framosoft.com");
            info.AddDeveloper("Fedor Moiseev");
            wxAboutBox(info);
        }

        void OnPreferences(wxCommandEvent& e)
        {
            ShowPreferencesEditor();
        }

        void OnCloseWindow(wxCloseEvent& e)
        {
            DismissPreferencesEditor();
            e.Skip();
        }

        void OnDropFile(wxDropFilesEvent& e)
        {
        }

        void OnIdle(wxIdleEvent& event)
        {
#ifdef WIN32
            MEMORYSTATUS ms;
            GlobalMemoryStatus(&ms);
            wxString s;
            s << "free mem: " << ms.dwAvailPhys / (1024 * 1024) << " mb";
            if(GetStatusBar() != nullptr)
            {
                SetStatusText(s, 5);
            }
#endif
        }

        void OnLayout(wxCommandEvent& e)
        {
        }

        //void OnSashDrag(wxSashEvent& event){}
        void OnSize(wxSizeEvent& e) 
        {
        }

        void OnMenu(wxCommandEvent& e)
        {
			if (e.GetId() == ID_VIEW_START)
			{
				auto c = new NewMDIChildFrame(this);
				//c->SetWindowStyleFlag(m_ChildFrameStyle);
				//c->SetListener(m_ViewManager);
				return;
			}
            if (e.GetId() == MENU_FILE_QUIT)
            {
                Close();
                return;
            }
            wxAuiPaneInfoArray& all_panes = m_auiManager.GetAllPanes();
            size_t count = all_panes.GetCount();
            for (size_t i = 0; i < count; i++)
            {
                if (e.GetId() == all_panes[i].window->GetId())
                {
                    all_panes[i].Show(!all_panes[i].IsShown());
                    m_auiManager.Update();
                    return;
                }
            }

            if (e.GetId() == ID_STATUSBAR)
            {
                if (wxStatusBar* statusBar = GetStatusBar())
                {
                    SetStatusBar(nullptr);
                    statusBar->Destroy();
                }
                else
                {
                    CreateStatus();
                }
                m_auiManager.Update();
                return;
            }
        }

        void OnUpdateUI(wxUpdateUIEvent& e)
        {
            wxAuiPaneInfoArray& all_panes = m_auiManager.GetAllPanes();
            size_t count = all_panes.GetCount();
            for (size_t i = 0; i < count; i++)
            {
                if (e.GetId() == all_panes[i].window->GetId())
                {
                    e.Check(all_panes[i].IsShown());
                }
            }
            if (e.GetId() == ID_STATUSBAR)
            {
                e.Check(GetStatusBar() != nullptr);
            }
        }

        void OnToggleStatusbar(wxCommandEvent& e)
        {
            wxStatusBar* statusBar = GetStatusBar();
            if (statusBar != nullptr)
            {
                SetStatusBar(nullptr);
                statusBar->Destroy();
            }
            else
            {
                CreateStatusBar();
            }
            m_auiManager.Update();
        }

        void OnLoadLayout(wxCommandEvent& e)
        {
            wxFileDialog dlg(this, "Choose a file", wxEmptyString, wxEmptyString,
                "wxAUI Layout (*.layout)|*.layout", wxFD_OPEN);
            if (dlg.ShowModal() == wxID_OK)
            {
                LoadLayout(dlg.GetPath());
            }
        }
        void OnSaveLayout(wxCommandEvent& e)
        {
            wxFileDialog dlg(this, "Choose a file", wxEmptyString, wxEmptyString,
                "wxAUI Layout (*.layout)|*.layout", wxFD_SAVE);
            if (dlg.ShowModal() == wxID_OK)
            {
                SaveLayout(dlg.GetPath());
            }
        }

        void LoadLayout(const wxString& filename)
        {
            if (!wxFileExists(filename)) return;
            wxFileInputStream stream(filename);
            if (!stream.Ok()) return;
            size_t cnt = stream.GetLength() / sizeof(wxChar);
            wxChar* tmp = new wxChar[cnt + 1];
            stream.Read(tmp, stream.GetLength());
            tmp[cnt] = wxChar(0);
            wxString perspective(tmp);
            delete[] tmp;
            m_auiManager.LoadPerspective(perspective);
        }

        void SaveLayout(const wxString& filename)
        {
            wxString perspective = m_auiManager.SavePerspective();
            wxFileOutputStream stream(filename);
            if (!stream.Ok()) return;
            stream.Write(perspective.GetData(), perspective.Length() * sizeof(wxChar));
        }


    private:
        void Init()
        {
            m_preferences = std::make_unique<PreferencesStorage>(*this);
        }

        void CreateControls()
        {
            CreateMenuBar();
            CreateStatus();

            m_auiManager.SetManagedWindow(this);
            AddPane(CreateStdToolBar(), wxAuiPaneInfo().ToolbarPane().Caption("Standard").Name("Standard Toolbar").Top().Floatable(false));
            AddPane(CreateAddToolBar(), wxAuiPaneInfo().ToolbarPane().Caption("Additional").Name("Additional Toolbar").Top().Position(2).Floatable(false));
            AddPane(CreateNavigator(), wxAuiPaneInfo().Right().Layer(1)/*.PinButton().MinimizeButton().MaximizeButton()*/.Name("Information").Caption("Information").MinSize(BaseFrame::FromDIP(wxSize(240, 450))));
            AddPane(CreateLogWindow(), wxAuiPaneInfo().Bottom().Name("Help").Caption(_("Help Window")));

            //m_AuiManager.AddPane(m_AddToolBar, wxAuiPaneInfo().ToolbarPane().Name(wxT("Additional Toolbar")).Top().Floatable(false));//Layer(2)

            //m_Notebook = new wxAuiNotebook(this, ID_NOTEBOOK, wxDefaultPosition, wxSize(600, 450), wxAUI_NB_DEFAULT_STYLE|wxNO_BORDER);
            //m_Page1 = new wxPanel(m_Notebook, wxID_ANY);
            //m_Page2 = new wxPanel(m_Notebook, wxID_ANY);
            //m_Notebook->AddPage(m_Page1, _("Page1"));
            //m_Notebook->AddPage(m_Page2, _("Page2"));
            //m_Page1 = new wxPanel(GetNotebook(), wxID_ANY);
            //m_Page2 = new wxPanel(GetNotebook(), wxID_ANY);
            //GetNotebook()->AddPage(m_Page1, _("Page1"));
            //GetNotebook()->AddPage(m_Page2, _("Page2"));


            //m_AuiManager->AddPane(m_Notebook, wxAuiPaneInfo().CenterPane());
            //m_AuiManager->AddPane(m_StdToolBar, wxAuiPaneInfo().ToolbarPane().Name(wxT("Standard Toolbar")).Top().Floatable(false));
            //m_AuiManager->AddPane(m_AddToolBar, wxAuiPaneInfo().ToolbarPane().Name(wxT("Additional Toolbar")).Top().Floatable(false));//Layer(2)
            //m_AuiManager->AddPane(m_InfoTree, wxAuiPaneInfo().Left().Layer(1).PinButton().MinimizeButton().MaximizeButton().Name(wxT("Information")).Caption(wxT("Information")));
//            AddPane(m_StdToolBar, wxAuiPaneInfo().ToolbarPane().Caption(wxT("Standard")).Name(wxT("Standard Toolbar")).Top().Floatable(false), wxT("&View"), wxT("Toolbars"));
//            AddPane(m_AddToolBar, wxAuiPaneInfo().ToolbarPane().Caption(wxT("Additional")).Name(wxT("Additional Toolbar")).Top().Floatable(false), wxT("&View"), wxT("Toolbars"));//Layer(2)
//            AddPane(m_SideNotebook, wxAuiPaneInfo().Right().Layer(1)/*.PinButton().MinimizeButton().MaximizeButton()*/.Name(wxT("Information")).Caption(wxT("Information")).MinSize(240, 450));

            //AddPane(GetStatusBar(), wxAuiPaneInfo().Bottom().Layer(1).Name(wxT("Statusbar")).Caption(wxT("Statusbar")));

            //RemovePane(m_StdToolBar);
            //delete m_StdToolBar;

            //m_AuiManager->AddPane(GetNotebook(), wxAuiPaneInfo().Name(wxT("CenterPane")).CenterPane());
            //m_AuiManager->AddPane(m_LOGTextCtrl, wxAuiPaneInfo().Bottom().Name(wxT("Help")).Caption(_("Help Window")));
//            AddPane(m_LOGTextCtrl, wxAuiPaneInfo().Bottom().Name(wxT("Help")).Caption(_("Help Window")));


            //m_Page1 = new wxAuiMDIChildFrame(this, wxID_ANY, _("Page1"));
            //m_Page2 = new wxAuiMDIChildFrame(this, wxID_ANY, _("Page2"));
            //GetNotebook()->AddPage(m_Page1, _("Page1"));
            //GetNotebook()->AddPage(m_Page2, _("Page2"));
            //GetNotebook()->SetSelection(0);


            /*m_AuiManager->AddPane(m_Notebook, wxAuiPaneInfo().CenterPane());
            m_AuiManager->AddPane(m_StdToolBar, wxAuiPaneInfo().ToolbarPane().Top().Floatable(false));
            m_AuiManager->AddPane(m_AddToolBar, wxAuiPaneInfo().ToolbarPane().Top().Position(2).Floatable(false));
            m_AuiManager->AddPane(m_InfoTree, wxAuiPaneInfo().Left().Layer(1).PinButton().MinimizeButton().MaximizeButton().Caption(wxT("Information")));*/

            /*m_AuiManager->GetArtProvider()->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE,0 );
            m_AuiManager->GetArtProvider()->SetColor(wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR, m_AuiManager->GetArtProvider()->GetColor(wxAUI_DOCKART_ACTIVE_CAPTION_COLOUR));
            m_AuiManager->GetArtProvider()->SetColor(wxAUI_DOCKART_INACTIVE_CAPTION_GRADIENT_COLOUR, m_AuiManager->GetArtProvider()->GetColor(wxAUI_DOCKART_ACTIVE_CAPTION_GRADIENT_COLOUR));
            m_AuiManager->GetArtProvider()->SetColor(wxAUI_DOCKART_INACTIVE_CAPTION_TEXT_COLOUR, m_AuiManager->GetArtProvider()->GetColor(wxAUI_DOCKART_ACTIVE_CAPTION_TEXT_COLOUR));*/

            m_auiManager.Update();
        }

        void CreateMenuBar()
        {
            auto menuBar = new wxMenuBar;

            auto fileMenu = new wxMenu;
            fileMenu->Append(wxID_NEW);
            fileMenu->Append(wxID_OPEN);
            fileMenu->Append(wxID_SAVE);
            fileMenu->AppendSeparator();
            fileMenu->Append(MENU_FILE_QUIT);

            auto layoutMenu = new wxMenu;
            layoutMenu->Append(ID_LOAD_LAYOUT, _("Load"));
            layoutMenu->Append(ID_SAVE_LAYOUT, _("Save"));

            auto viewMenu = new wxMenu;
			viewMenu->Append(ID_VIEW_START, _("Add view"));
            wxMenuItem* layoutItem = viewMenu->Append(wxID_ANY, _("Layout"), layoutMenu);

            auto opMenu = new wxMenu;

            auto toolsMenu = new wxMenu;
            toolsMenu->Append(ID_APP_SETTINGS);

            auto helpMenu = new wxMenu;
            helpMenu->Append(MENU_ABOUT, _("About..."));

            menuBar->Append(fileMenu, _("File"));
            menuBar->Append(viewMenu, _("View"));
            menuBar->Append(opMenu, _("Operation"));
            menuBar->Append(toolsMenu, _("Tools"));
            menuBar->Append(helpMenu, _("Help"));
            BaseFrame::SetMenuBar(menuBar);
        }

        wxStatusBar* CreateStatus()
        {
            if (auto statusBar = BaseFrame::CreateStatusBar(1, wxSTB_DEFAULT_STYLE, ID_STATUSBAR))
            {
                int widths[] = { -1,60,60,60,210,120 };
                std::transform(std::begin(widths), std::end(widths), std::begin(widths), [this](int w) {return BaseFrame::FromDIP(w); });
                BaseFrame::GetStatusBar()->SetFieldsCount(6);
                BaseFrame::SetStatusWidths(6, widths);
                BaseFrame::SetStatusText(_("welcome"), 0);
                BaseFrame::SetStatusText(" ", 1);
                BaseFrame::SetStatusText(" ", 2);
                BaseFrame::SetStatusText(" ", 3);

                wxMenuBar* menubar = GetMenuBar();
                if (!menubar)
                    return statusBar;
                int idx = menubar->FindMenu("View");
                if (idx == wxNOT_FOUND)
                    return statusBar;
                wxMenu* viewMenu = menubar->GetMenu(idx);
                int ids = viewMenu->FindItem("Layout");
                if (ids != wxNOT_FOUND)
                {
                    size_t pos = 0;
                    wxMenuItem* sm = viewMenu->FindChildItem(ids, &pos);
                    viewMenu->Insert(pos, statusBar->GetId(), "Status bar", "", wxITEM_CHECK);
                }
                else
                {
                    viewMenu->Append(statusBar->GetId(), "Status bar", "", wxITEM_CHECK);
                }
            }
            return BaseFrame::GetStatusBar();
        }

        wxWindow* CreateStdToolBar()
        {
            auto toolBar = new wxAuiToolBar(this, ID_STANDARD_TOOLBAR);// , wxDefaultPosition, wxDefaultSize, wxBORDER_NONE | wxTB_HORIZONTAL | wxTB_NODIVIDER | wxTB_FLAT);
            //auto toolBar = new wxToolBar(this, ID_STANDARD_TOOLBAR, wxDefaultPosition,
            //    wxDefaultSize, wxBORDER_NONE | wxTB_HORIZONTAL | wxTB_NODIVIDER | wxTB_FLAT);
            //hdpi m_StdToolBar->SetToolBitmapSize(wxSize(16, 15));
			toolBar->AddTool(wxID_NEW, wxGetStockLabel(wxID_NEW, wxSTOCK_WITH_ACCELERATOR |
				wxSTOCK_WITH_MNEMONIC), wxBitmap(new_xpm), wxGetStockHelpString(wxID_NEW));
            toolBar->AddTool(wxID_OPEN, _("Open"), wxBitmap(fileopen_xpm));
            toolBar->AddTool(wxID_SAVE, _("Save"), wxBitmap(filesave_xpm));
            toolBar->AddSeparator();
            toolBar->AddTool(wxID_ABOUT, _("About..."), wxBitmap(htmfoldr_xpm));
            toolBar->Realize();
            return toolBar;
        }

        wxWindow* CreateAddToolBar()
        {
            auto toolBar = new wxAuiToolBar(this, ID_ADDITIONAL_TOOLBAR);// , wxDefaultPosition, wxDefaultSize, wxBORDER_NONE | wxTB_HORIZONTAL | wxTB_NODIVIDER | wxTB_FLAT);
            //auto toolBar = new wxToolBar(this, ID_ADDITIONAL_TOOLBAR, wxDefaultPosition,
            //    wxDefaultSize, wxBORDER_NONE | wxTB_HORIZONTAL | wxTB_NODIVIDER | wxTB_FLAT);
            //hdpi m_AddToolBar->SetToolBitmapSize(wxSize(16, 15));
            //toolBar->AddSeparator();
            toolBar->AddTool(wxID_CUT, _("Cut"), wxBitmap(cut_xpm));
            toolBar->AddTool(wxID_COPY, _("Copy"), wxBitmap(copy_xpm));
            toolBar->AddTool(wxID_FIND, _("Find"), wxBitmap(find_xpm));
            toolBar->Realize();
            return toolBar;
        }

        wxWindow* CreateNavigator() 
        {
            return new Navigator(this, ID_NAVIGATOR, wxDefaultPosition, BaseFrame::FromDIP(wxSize(200, 500)));
        }

        wxWindow* CreateLogWindow()
        {
            return new wxTextCtrl(this, ID_LOGTEXTCTRL, wxEmptyString, wxDefaultPosition, BaseFrame::FromDIP(wxSize(250, 100)), wxTE_MULTILINE | wxTE_READONLY | wxNO_BORDER);
        }

        bool AddPane(wxWindow* window, wxAuiPaneInfo& pane_info)
        {
            if (!m_auiManager.AddPane(window, pane_info))
            {
                return false;
            }
            
            if (pane_info.caption.IsEmpty())
            {
                return true;
            }

            wxMenuBar* menubar = GetMenuBar();
            if (!menubar)
                return true;
            int idx = menubar->FindMenu("View");
            if (idx == wxNOT_FOUND)
                return true;
            wxMenu* viewMenu = menubar->GetMenu(idx);
            if (pane_info.IsToolbar())
            {
                wxMenu* toolbarsMenu = nullptr;
                int ids = viewMenu->FindItem("Toolbars");
                if (ids == wxNOT_FOUND)
                {
                    toolbarsMenu = new wxMenu;
                    wxMenuItem* toolbarsItem = viewMenu->Insert(0, wxID_ANY, "Toolbars", toolbarsMenu);
                }
                else
                {
                    wxMenuItem* sm = viewMenu->FindItem(ids);
                    toolbarsMenu = sm->GetSubMenu();
                }
                if (toolbarsMenu)
                {
                    toolbarsMenu->Append(window->GetId(), pane_info.caption, "", wxITEM_CHECK);
                }
            }
            else
            {
                int ids = viewMenu->FindItem("Layout");
                if (ids != wxNOT_FOUND)
                {
                    size_t pos = 0;
                    wxMenuItem* sm = viewMenu->FindChildItem(ids, &pos);
                    viewMenu->Insert(pos, window->GetId(), pane_info.caption, "", wxITEM_CHECK);
                }
                else
                {
                    viewMenu->Append(window->GetId(), pane_info.caption, "", wxITEM_CHECK);
                }
            }
            return true;
        }

        wxWindow* RemovePane(const wxString& pane_name)
        {
            wxAuiPaneInfo& pi = m_auiManager.GetPane(pane_name);
            if (pi.IsOk())
            {
                return RemovePane(pi);
            }
            return nullptr;
        }

        wxWindow* RemovePane(wxWindow* window)
        {
            wxAuiPaneInfo& pi = m_auiManager.GetPane(window);
            if (pi.IsOk())
            {
                return RemovePane(pi);
            }
            return nullptr;
        }

        wxWindow* RemovePane(wxAuiPaneInfo& pane_info)
        {
            wxMenuBar* menubar = GetMenuBar();
            int idx = menubar->FindMenu("View");
            if (idx != wxNOT_FOUND)
            {
                wxMenu* viewMenu = menubar->GetMenu(idx);
                if (pane_info.IsToolbar())
                {
                    int ids = viewMenu->FindItem("Toolbars");
                    wxMenuItem* sm = viewMenu->FindItem(ids);
                    wxMenu* toolbarsMenu = sm->GetSubMenu();
                    delete toolbarsMenu->Remove(pane_info.window->GetId());
                    if (toolbarsMenu->GetMenuItemCount() == 0)
                    {
                        delete viewMenu->Remove(sm);
                    }
                }
                else
                {
                    delete viewMenu->Remove(pane_info.window->GetId());
                }
            }
            wxWindow* w = pane_info.window;
            if (!m_auiManager.DetachPane(pane_info.window))
                return nullptr;
            return w;
        }

        wxAuiManager m_auiManager;
        wxDECLARE_EVENT_TABLE();
    };

    BEGIN_EVENT_TABLE(NewFrame, BaseFrame)
        EVT_SIZE(NewFrame::OnSize)
        EVT_CLOSE(NewFrame::OnCloseWindow)
        EVT_DROP_FILES(NewFrame::OnDropFile)
        EVT_IDLE(NewFrame::OnIdle)
        EVT_MENU(MENU_FILE_QUIT, NewFrame::OnMenu)
        EVT_MENU(MENU_FILE_OPEN, NewFrame::OnMenu)
        EVT_MENU(MENU_FILE_NEW, NewFrame::OnMenu)
        EVT_MENU(MENU_ABOUT, NewFrame::OnAbout)
        EVT_MENU(ID_APP_SETTINGS, NewFrame::OnPreferences)
        EVT_MENU_RANGE(wxID_FILE1, wxID_FILE9, NewFrame::OnMenu)
        EVT_MENU_RANGE(ID_START, ID_END, NewFrame::OnMenu)
		EVT_MENU(ID_VIEW_START, NewFrame::OnMenu)
        EVT_UPDATE_UI_RANGE(ID_START, ID_END, NewFrame::OnUpdateUI)
        EVT_MENU(ID_LOAD_LAYOUT, NewFrame::OnLoadLayout)
        EVT_MENU(ID_SAVE_LAYOUT, NewFrame::OnSaveLayout)
    END_EVENT_TABLE()

}

namespace ftk
{
    wxFrame* CreateFrame_(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos,
        const wxSize& size, long style, const wxString& name)
    {
        wxSize sz = (size == wxDefaultSize) ? wxWindow::FromDIP(wxSize(800, 600), nullptr) : size;
        auto frame = new NewFrame(parent, id, "FTK Demo App", pos, sz, style, name);
        return frame;
    }
}
