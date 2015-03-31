/*=========================================================================

 Program: MAF2
 Module: mafGUIRangeSlider
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafGUIRangeSlider_H__
#define __mafGUIRangeSlider_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafBaseEventHandler.h"
#include "mafEventSender.h"

/**
  class name: mafGUIRangeSlider 
  Class that handles a slider for visualising or changing range.
*/
class mafGUIRangeSlider : public wxControl , public mafEventSender
{
public:
  /** constructor */
  mafGUIRangeSlider(wxWindow *parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxPanelNameStr);

  /** Get the 'min', 'max' or 'mid' value according to 'i'. */
	double GetValue(int i) const { return this->m_Value[i]; }

  /** Set the 'min', 'max' or 'mid' value according to 'i'. */
  void SetValue(int i, double value);

  /** Get the range of the slider. */
	const double *GetRange() const { return this->m_Range; }

  /** Set the range for the slider. */
  void SetRange(double *range);

  /** Set the range for the slider. */
  void SetRange(double minR, double maxR) { double range[2] = {minR, maxR}; this->SetRange(range); }

  /** Set the number of widgets for the slider. */
  void SetNumberOfWidgets(int num) { if (num == this->m_NumberOfWidgets || num < 1 || num > 3) return; this->m_NumberOfWidgets = num; this->Refresh(); }
  
	/** Enable/Disable the center cursor for the slider. */
  void EnableCenterWidget(bool enable) { this->m_NumberOfWidgets = (enable) ? 3 : 2; this->Refresh(); }

  /** Hide / show the circles on the boundary of the slider.*/
  void EnableBoundaryHandles(bool enable) {this->m_BorderWidgetRadius = (enable) ? 3 : 0; this->Refresh();}
  
	/** Return true if one of the cursors are moving. */
  bool IsDragging() const { return this->m_SelectedTriangleWidget != -1 || this->m_SelectedRangeWidget != -1; }

protected:
	/** Initialize the values for the slider. */
  void Initialize();
	
	/** Refresh the slider shape. */
  void OnPaint(wxPaintEvent &event);
	
	/** Move the cursors on mouse event. */
  void OnMouse(wxMouseEvent &event);

  /** Intercept slider events and forward to the listener.*/
  void OnRangeModified(wxCommandEvent &event);
	
	/** Redraw the widgets in the new position. */
  void RedrawWidgets(wxDC &dc, bool eraseWidgets = false);

  /** Draw a label according to the mouse position.*/
  void DrawLabel(int x, int y, double val);

  double m_Range[2];
  double m_Value[3];

  int m_NumberOfWidgets;

private:
  // ui coordinates
  int m_LineStartX;
  int m_LineStartY;
  int m_LineWidth;
  int m_BorderWidgetRadius;
  wxPoint m_TrianglePosition[3];

  static wxPoint m_TriangleWidgetPoints[3];
  static wxPoint m_TriangleWidgetCenter;
  static int     m_TriangleWidgetRadius;

  wxPoint m_PrevMousePosition;
  int     m_SelectedTriangleWidget;
  int     m_SelectedRangeWidget;
  wxString m_ValueString;
  wxStaticText *m_RangeValueLabel;
  /** declaring event table macro */
  DECLARE_EVENT_TABLE()
};
#endif // _mafGUIRangeSlider_H_
