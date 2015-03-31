/*=========================================================================

 Program: MAF2
 Module: mafGUILutEditor
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafGUILutEditor_H__
#define __mafGUILutEditor_H__

#include "mafDecl.h"
#include "mafEvent.h"
#include "mafBaseEventHandler.h"
#include "mafEventSender.h"
#include "mafLUTLibrary.h"

#include "mafGUILutWidget.h"
#include "mafGUILutSwatch.h"
#include "mafGUIColorWidget.h"

#include "vtkLookupTable.h"
/** mafGUILutEditor : vtkLookupTable Editor

  USAGE:
  - call the static member mafGUILutEditor::ShowLutDialog(vtkLookupTable *lut)

  FEATURES:
  - 20 preset available @sa mafLutPreset
  - editable number of LUT entries - limited to [1..256]
  - scalars range editing  --- ( 4 programmers - Set and Get it by calling Set/GetRange on your vtkLookupTable )
  - LUT Entry selection:
    - single selection -- mouse left click
    - range selection  -- mouse left click + drag
    - sparse selection -- mouse right click
  - Selected entries can be "shaded" interpolating first and last color both in RGBA and HSV space.

@sa mafColor mafGUIColorSwatch mafGUIColorWidget mafGUILutPreset mafGUILutSwatch mafGUILutWidget
*/
class mafGUILutEditor: public wxPanel, public mafBaseEventHandler, public mafEventSender
{
public:
  mafGUILutEditor(wxWindow* parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition, 
               const wxSize& size = wxSize(300,800), long style = wxTAB_TRAVERSAL);
  virtual ~mafGUILutEditor(); 

  void OnEvent(mafEventBase *maf_event);

  /** Assign the external lookup table to the widget.*/
  void SetLut(vtkLookupTable *lut);

  /** Show the dialog.*/
  static void ShowLutDialog(vtkLookupTable *lut, mafBaseEventHandler *listener = NULL, int id = MINID);

protected:

  void UpdateInfo();
  
  /** Update the internal Lookup Table according to the preset selected from the combo box.*/
  void UpdateLut();

  void UpdateWidgetsOnLutChange();
  
  /** Copy the external Lookup Table given by the user to the internal one.*/
  void CopyLut(vtkLookupTable *from, vtkLookupTable *to);
	
  //void OnComboSelection(wxCommandEvent &event);

  int          m_Preset; ///< Index of lookup table preset.
  int          m_UserPreset;

  int          m_NumEntry; ///< Number of colors of the current lookup table.
  double       m_ValueRange[2]; ///< Value range of the current lookup table.
  wxString     m_Info; // selected indexes
  wxString     m_NewUserLutName;
  wxString m_UserLutLibraryDir;

  mafGUILutSwatch   *m_LutSwatch;
  mafGUILutWidget   *m_LutWidget;
  mafGUIColorWidget *m_ColorWidget;

  mafLUTLibrary *m_UserLutLibrary;
  wxComboBox   *m_UserPresetCombo;
	wxComboBox   *m_PresetCombo;
  vtkLookupTable *m_ExternalLut;  ///< Given lut that will be modified by "ok" or "apply"
  vtkLookupTable *m_Lut;          ///< Internal lut -- initialized in SetLut
  DECLARE_EVENT_TABLE()
};
#endif
