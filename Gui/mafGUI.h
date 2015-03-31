/*=========================================================================

 Program: MAF2
 Module: mafGUI
 Authors: Silvano Imboden - Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafGUI_H__
#define __mafGUI_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------

#include "wx/grid.h"
#include "wx/listctrl.h"
#include "mafDecl.h"
#include "mafGUIPanel.h"
#include "mafEvent.h"
#include "mafEventSender.h"
#include "mafBaseEventHandler.h"
#include <map>

#ifdef MAF_EXPORTS
#include "mafDllMacros.h"
EXPORT_STL_MAP(MAF_EXPORT,int,int);
#endif

//----------------------------------------------------------------------------
// class forward :
//----------------------------------------------------------------------------
class mafGUIFloatSlider;
class mafGUICheckListBox;
class mafGUICrossIncremental;
class mafGUILutSwatch;
class mafGUIRollOut;


#ifdef MAF_USE_VTK //:::::::::::::::::::::::::::::::::
class vtkLookupTable;
#endif             //:::::::::::::::::::::::::::::::::


/**  \par implementation details:
MAFWidgetId is a counter that holds the last generated widget_ID.
It is used and incremented by GetWidgetId.
\sa GetWidgetId GetModuleId MAFWidgetId m_WidgetTableID
*/
//extern "C" int MAFWidgetId;

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------
#pragma warning(push)
#pragma warning (disable:4005)

#define MININT    -2147483647-1
#define MAXINT     2147483647
#define MINFLOAT  -1.0e+38F
#define MAXFLOAT   1.0e+38F
#define MINDOUBLE -1.0e+299
#define MAXDOUBLE 1.0e+299
#pragma warning(pop)

//----------------------------------------------------------------------------
// Constants to be used with mafGUI::GetMetrics()
//----------------------------------------------------------------------------
enum GUI_CONSTANTS
{
  GUI_ROW_MARGIN,
  GUI_LABEL_MARGIN,
  GUI_WIDGET_MARGIN,
  GUI_LABEL_HEIGHT,
  GUI_BUTTON_HEIGHT,
  GUI_LABEL_WIDTH,
  GUI_WIDGET_WIDTH,
  GUI_FULL_WIDTH,
  GUI_DATA_WIDTH,
  GUI_HOLDER_WIDTH,
};
//----------------------------------------------------------------------------
// mafGUI :
/**  mafGUI is a panel with function to easily create GUI.
The user calls function like Vector,String,Color ecc.. .
mafGUI take care of: 
- creating and Layout the widgets,
- filter and validate user input, widget are constrained in range and type. 
- manage widget initialization user variables updates. 
- notify the user of gui-events by sending mafEvents to a mafObserver.

\par implementation details:
On creation of widget, the user passes an ID that will be
used by the widget when sending notification back. 
One command may cause the creation of more than one widget, ex:
the Vector create 3 wxTextCtrl, and I can't use the same ID for every widget.
So user-ID are translated into widgets-ID using 
two member functions : GetModuleID and GetWidgetId. GetWidgetId increments an internal ID_counter.
\sa GetWidgetId GetModuleId MAFWidgetId m_WidgetTableID
*/
//----------------------------------------------------------------------------
class MAF_EXPORT mafGUI: public mafGUIPanel, public mafBaseEventHandler, public mafEventSender
{
public:
           mafGUI(mafBaseEventHandler *listener);
  virtual ~mafGUI();
    
  /** Separator widget. */
	void Divider(long style = 0);

  /** Label widget. */
  void Label(const mafString& label,bool bold = false, bool multiline = false);

  /** Label widget. */
  void Label(mafString *var ,bool bold = false, bool multiline = false);

  /** Double label widget. */
	void Label(const mafString& label1,const mafString& label2, bool bold_label = false, bool bold_var = false);

  /** Double label widget. */
  void Label(const mafString& label1,mafString *var, bool bold_label = false, bool bold_var = false);

  //---------------------------------------------
  /** Label widget. */
  //void Label(mafString label,bool bold = false, bool multiline = false);

  /** Label widget. */
  //void Label(mafString *var ,bool bold = false, bool multiline = false);

  /** Double label widget. */
  //void Label(mafString label1,mafString  label2, bool bold = false);

  /** Double label widget. */
  //void Label(mafString label1,mafString *var, bool bold = false);
  //---------------------------------------------

  /** String entry widget. */
  void String(int id,const mafString& label,mafString *var, const mafString& tooltip = "", bool multiline = false, bool password = false);

  /** Integer entry widget. */
  void Integer(int id,const mafString& label,int *var, int min = MININT, int max = MAXINT, const mafString& tooltip = "", bool labelAlwaysEnable = false);

  /** Float entry widget. */
  void Float(int id,const mafString& label,float *var, float min = MINFLOAT, float max = MAXFLOAT, int flag=0, int decimal_digit = -1, const mafString& tooltip = "");

  /** Double entry widget. */
  void Double(int id,const mafString& label,double *var, double	min = MINDOUBLE, double max = MAXDOUBLE, int decimal_digit = -1, const mafString& tooltip = "", bool labelAlwaysEnable = false);

  /** Integer vector3 entry widget. */
  void Vector(int id,const mafString& label, int var[3], int min = MININT, int max = MAXINT, const mafString& tooltip = "", wxColour *bg_colour = NULL);

  /** Integer vector3 entry widget. */
  void Vector(int id,const mafString& label, int var[3], int minx, int maxx, int miny, int maxy, int minz, int maxz, const mafString& tooltip = "", wxColour *bg_colour = NULL);

  /** Float vector3 entry widget. */
  void Vector(int id,const mafString& label, float var[3], float min = MINFLOAT, float max = MAXFLOAT, int decimal_digit = -1, const mafString& tooltip = "", wxColour *bg_colour = NULL);

  /** Float vector3 entry widget. */
  void Vector(int id,const mafString& label, float var[3], float minx, float maxx, float miny, float maxy, float minz, float maxz, int decimal_digit = -1, const mafString& tooltip = "", wxColour *bg_colour = NULL);

  /** Double vector3 entry widget. */
  void Vector(int id,const mafString& label, double var[3], double min = MINFLOAT, double max = MAXFLOAT, int decimal_digit = -1, const mafString& tooltip = "", wxColour *bg_colour = NULL);

  /** Double vector3 entry widget. */
  void Vector(int id,const mafString& label, double var[3], double minx, double maxx, double miny, double maxy, double minz, double maxz, int decimal_digit = -1, const mafString& tooltip = "", wxColour *bg_colour = NULL);

  /** Double vectorN entry widget. */
  void VectorN(int id,const mafString& label, double *var, int num_elem = 3, double min = MINFLOAT, double max = MAXFLOAT, int decimal_digit = -1, const mafString& tooltip = "");
  
  /** Int vectorN entry widget. */
  void VectorN(int id,const mafString& label, int *var,int num_elem = 3,int min = MININT, int max = MAXINT, const mafString& tooltip = "");

  /** Checkbutton widget. */
  void Bool(int id, const mafString& label, int *var, int flag = 0, const mafString& tooltip = ""	);

  /** Checkbutton grid widget. */
  void BoolGrid(int numRows, int numColumns, std::vector<int> &ids, std::vector<const char*> &labelsRows,std::vector<const char*> &labelsColumns, int *var, mafString tooltip = ""	);

  /** Radiobutton widget. */
  void Radio(int id,const mafString& label, int *var, int numchoices = 0, const mafString choices[] = NULL, int dim = 1, const mafString& tooltip = "", int style = wxRA_SPECIFY_COLS);

  /** Combo widget. */
  wxComboBox *Combo(int id,const mafString& label, int *var, int numchoices = 0, const mafString choices[] = NULL, const mafString& tooltip = "");

  /** File open dialog widget. */
  void FileOpen(int id,const mafString& label,mafString *var, const mafString& wildcard = "", const mafString& tooltip = "");

  /** File save dialog widget. */
  void FileSave(int id,const mafString& label,mafString *var, const mafString& wildcard = "", const mafString& tooltip = "", bool enableTextCtrl = true);

  /** Dir Open dialog widget. */
  void DirOpen(int id,const mafString& label,mafString *var, const mafString& tooltip = "");

  /** Color dialog widget. */
  void Color(int id,const mafString& label,wxColour *var, const mafString& tooltip = "");

  #ifdef MAF_USE_VTK //:::::::::::::::::::::::::::::::::
  /** LookupTable dialog widget. */
  mafGUILutSwatch *Lut(int id,const mafString& label,vtkLookupTable *lut);
  #endif //:::::::::::::::::::::::::::::::::

  /** Button widget. */
  void Button(int id,const mafString& button_text,const mafString& label="", const mafString& tooltip=""); 
  
  /** Button widget with variable label. */
  void Button(int id,mafString *label,const mafString& button_text, const mafString& tooltip=""); 

	/** Widget with 4 buttons and a text array disposed like a cross.*/
  mafGUICrossIncremental *CrossIncremental(int id,const mafString& label, double *stepVariable, double *topBottomVariable, double *leftRightVariable, int modality ,const mafString& tooltip ="", bool boldLabel = true, bool comboStep = false, int digits = -1, mafString *buttonUpDown_text = NULL, mafString *buttonLeftRight_text = NULL);

  /** two generic Buttons widget. */
  void TwoButtons(int firstID, int secondID, const mafString& label1, const mafString& label2, int alignment = wxALL, int width = -1);

  /** Multiple Generic Buttons widget. */
  void MultipleButtons(int numButtons, int numColumns, std::vector<int> &ids, const std::vector<mafString> &labels, int alignment = wxALL );

  /** Ok-Cancel Button widget. */
  void OkCancel(int alignment = wxALL);

  /** Integer slider widget. */
	wxSlider *Slider(int id, const mafString& label, int *var,int min = MININT, int max = MAXINT, const mafString& tooltip = "",bool showText=true);

  /** Float slider widget.*/
  mafGUIFloatSlider *FloatSlider(int id, const mafString& label, double *var,double min, double max, wxSize size = wxDefaultSize, const mafString& tooltip = "", bool textBoxEnable = true);

  /** Float slider widget.*/
  mafGUIFloatSlider *FloatSlider(int id, double *var, double min, double max, const mafString& minLab, const mafString& maxLab, wxSize size = wxDefaultSize, const mafString& tooltip = "", bool textBoxEnable = true);

  /** Checked listbox widget. */
  mafGUICheckListBox *CheckList(int id, const mafString& label = "", int height = 60, const mafString& tooltip = "");

  /** Listbox widget. */
	wxListBox *ListBox(int id, const mafString& label = "", int height = 60, const mafString& tooltip = "", long lbox_style = 0, int width = -1);

  /** ListCtrl widget. */
	wxListCtrl *ListCtrl(int id, const mafString& label = "", int height = 60, const mafString& tooltip = "", long lbox_style = 0, int width = -1);

  /** Grid widget. */
	wxGrid *Grid(int id, const mafString& label = "", int height = 60, int row = 2,int cols = 2, const mafString& tooltip = "");

  /** Create a roll out gui.*/
  mafGUIRollOut *RollOut(int id, const mafString& title, mafGUI *roll_gui, bool rollOutOpen = true);

  /** Add window to gui sizer. */
  void Add(wxWindow* window,int option = 0, int flag = wxEXPAND, int border = 0)  {window->Reparent(this); window->Show(true); m_Sizer->Add(window,option,flag,border);};
  
  /** Add sizer to gui sizer. */
  void Add(wxSizer*  sizer, int option = 0, int flag = wxEXPAND, int border = 0)  {m_Sizer->Add(sizer, option,flag,border);};

  /** Add gui to gui sizer. */
  void AddGui(mafGUI*  gui, int option = 0, int flag = wxEXPAND, int border = 0);

  /** Remove window from gui sizer. */
  bool Remove(wxWindow* window) {window->Show(false); window->Reparent(mafGetFrame()); return m_Sizer->Detach(window);};

  /** Remove sizer from gui sizer. */
  bool Remove(wxSizer*  sizer ) {return m_Sizer->Detach(sizer);};

  /** Recalculate 'this' Gui Size and MinSize considering the space required by the children widgets.
      FitGui is called implicitly when a gui is inserted in a mafGUIHolder or mafGUIPanel.
      FitGui must call explicitly when children widget are changed dynamically. */
  void FitGui();

	/** Update gui widget. */
  void Update();
  
	/** Enable/Disable gui widget. */
  void Enable(int mod_id, bool enable);
  
	/** Return the font used for bold label. */
  wxFont GetBoldFont() {return m_BoldFont;}; 

  wxFont GetGuiFont() {return m_Font;}; 

  /** Return the measure used to layout the widgets - pass one of the GUI_xxx constants. */
  int GetMetrics( int id); 

  void OnEvent(mafEventBase *maf_event);

  /**  \par implementation details:
  GetWidgetId is used to obtain a new/unique widget_ID. 
  As a side effect a new pair widget_ID->module_ID is stored in m_WidgetTableID
  \sa GetWidgetId GetModuleId MAFWidgetId m_WidgetTableID
  */
  int GetWidgetId(int mod_id);

  int* GetMAFWidgetId();

  /** Turn On/Off the collaboration status. */
  void Collaborate(bool status) {m_CollaborateStatus = status;};

  void GetWidgetValue(long widget_id, WidgetDataType &widget_data);

  void SetWidgetValue(int id, WidgetDataType &widget_data);

  /** place the GUI on a different parent, and perform the required Resize/Stretch/ and Show */
  void Reparent(wxWindow *parent);

protected:
  wxBoxSizer   *m_Sizer;

  wxColour      m_BackgroundColor;
  bool          m_UseBackgroundColor;
  long          m_EntryStyle;

  /**  \par implementation details:
  m_BoldFont is the font used for the Bold labels*/
  wxFont m_BoldFont;
  wxFont m_Font;

  /**  \par implementation details:
  m_WidgetTableID is an std::map holding a table of conversion widget_id -> user_id.
  The range of widget_IDs is between MINID and MAXID (created widget_IDs are <= then MAFWidgetId)    
  To obtain the Module ID, widget_ID are shifted by MINID and used to index the array
  \sa GetWidgetId GetModuleId MAFWidgetId m_WidgetTableID
  */
  std::map<int,int> m_WidgetTableID;

  /**
  \par implementation details:
  translate a widget_ID in it's module_ID, accessing m_WidgetTableID
  \sa GetWidgetId GetModuleId MAFWidgetId m_WidgetTableID
  */
  int GetModuleId(int w_id)   {assert(w_id > 0 && w_id <= (*GetMAFWidgetId())); return  m_WidgetTableID[w_id - MINID];};

  void OnSlider			  (wxCommandEvent &event) { }//@@@ this->OnEvent(mafEvent(this, GetModuleId(event.GetWidgetId()))); }
  void OnListBox      (wxCommandEvent &event);
  void OnListCtrl      (wxCommandEvent &event);
  void OnCheckListBox (wxCommandEvent &event);
  void OnMouseWheel   (wxMouseEvent &event);

  bool m_CollaborateStatus;  ///< Flag set to know if the application is in collaborative mode or no.

DECLARE_EVENT_TABLE()
};
#endif
