/*=========================================================================

 Program: MAF2
 Module: mafGUILutWidget
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafGUILutWidget_H__
#define __mafGUILutWidget_H__

#include "mafDecl.h"
#include "mafEvent.h"
#include "mafColor.h"
#include "vtkLookupTable.h"
//----------------------------------------------------------------------------
/** mafGUILutWidget : Helper class for mafLutEditor
@sa mafLutEditor
*/
class mafGUILutWidget: public wxPanel, public mafObserver
{
public:
  mafGUILutWidget(wxWindow* parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition, 
           const wxSize& size = wxSize(300,290), long style = wxTAB_TRAVERSAL /*| wxSUNKEN_BORDER */);

  virtual void SetListener(mafObserver *Listener) {m_Listener = Listener;};
  void OnEvent( mafEventBase *event );

  void     SetSelectionColor(mafColor col);
  mafColor GetSelectionColor(); // return the color of the first selected entry
  void     ShadeSelectionInRGB();    
  void     ShadeSelectionInHSV();    
  void     GetSelection(int *min, int *max, int *num=NULL); 
  
  struct LutEntry {
    mafColor      m_Color;
    bool          m_Selected;
  } m_LutEntry[257];

  void SetNumEntry( int num );

  //void ReadLut(vtkLookupTable *lut);
  //void WriteLut(vtkLookupTable *lut);
  void SetLut(vtkLookupTable *lut);

protected:
  mafObserver     *m_Listener;
  vtkLookupTable  *m_Lut;
  
  bool     m_Dragging;
  int      m_DragBeginIdx;

  int m_SelectionRowMin;
  int m_SelectionColMin;
  int m_SelectionRowMax;
  int m_SelectionColMax; //selection rectangle expressed using row/cols coord -- used during drag operation

  wxBitmap m_Bmp;      //the bitmap with the LUT
  wxBitmap m_Bmp2;     //the bitmap with the LUT and the Selection Indicator
  wxPoint  m_BmpPosition;       //position of the bitmap
  wxSize   m_BmpSize;       //size of the bitmap

  int m_NumEntry;
  int m_EntryW;
  int m_EntryH;
  int m_EntryM; // margin
  int m_EntryPerRow;
  
  void InitBitmaps();        
  void DrawEntries();      
  void DrawEntry(int idx); 
  void DrawSelection(); 
  void OnEraseBackground(wxEraseEvent& event) {};  // overriden to prevent flickering
  void OnPaint(wxPaintEvent &event);                

  int  MouseToIndex(wxMouseEvent &event);
  void OnLeftMouseButtonDown(wxMouseEvent &event);
  void OnLeftMouseButtonUp(wxMouseEvent &event);
  void OnMouseMotion(wxMouseEvent &event);
  
  DECLARE_EVENT_TABLE()
};
#endif
