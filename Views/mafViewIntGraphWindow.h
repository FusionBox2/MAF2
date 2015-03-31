/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewIntGraphWindow.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 12:13:22 $
  Version:   $Revision: 1.2 $
  Authors:   Fedor Moiseev / Vladik Aranov
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#ifndef __mafViewIntGraphWindow_H___
#define __mafViewIntGraphWindow_H___
    
//----------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------
#include "mafString.h"
#include "mafMemGraph.h"
#include "mafStringSet.h"
#include "mafGUI.h"

//----------------------------------------------------------------------------
// forward references
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// defines
//----------------------------------------------------------------------------
#define mafDEFINE_GET_SET_MEMBER(type, name)       \
protected:                                         \
  type  m_##name;                                  \
public:                                            \
  type const& Get##name() const {return m_##name;}             \
  type  Set##name(type const& name) {type old_##name = m_##name; m_##name = name; return old_##name;}

enum  mafViewIntGraphFontFamily
{
  mafFF_DEFAULT = 0,
  mafFF_DECORATIVE ,
  mafFF_ROMAN      ,
  mafFF_SCRIPT     ,
  mafFF_SWISS      ,
  mafFF_MODERN     ,
  mafFF_LAST     ,
  mafFF_FORCED_DWORD = 0x7fffffff     
};

class mafGraphData
{
public:
  //virtual const double& operator()(unsigned int point,unsigned int coord, unsigned int deriv = 0) const = 0;
  virtual double   GetValue(unsigned int point,unsigned int coord, unsigned int deriv = 0) const = 0;
  virtual bool     GetValueByParam(double& res, double param,unsigned int coord, unsigned int deriv = 0) const = 0;
  virtual unsigned GetVarNum() const  = 0;
  virtual unsigned GetXVarNum() const = 0;
  virtual unsigned GetYVarNum() const = 0;
  virtual unsigned GetXDer(unsigned index) const = 0;
  virtual unsigned GetYDer(unsigned index) const = 0;
  virtual unsigned GetIndexX(unsigned index) const = 0;
  virtual unsigned GetIndexY(unsigned index) const = 0;
  virtual unsigned GetSize() const = 0;
  virtual unsigned GetBreakBegin() const = 0;
  virtual unsigned GetBreakEnd()  const = 0;
  virtual double   GetDerivCoef(unsigned index) const = 0;
  virtual void     GetIDDesc(unsigned index, unsigned int deriv, char *sDescript,unsigned int nLength) const = 0;
};

//----------------------------------------------------------------------------
// mafViewIntGraphWindow
//----------------------------------------------------------------------------
class mafViewIntGraphWindow: public wxWindow, public mafObserver
{
  DECLARE_DYNAMIC_CLASS(mafViewIntGraphWindow)
public:
  mafViewIntGraphWindow(const wxString& label = "", mafObserver *listener = NULL);
  ~mafViewIntGraphWindow(void);

  int            &GetRoughGrid(void)   {return m_RoughGrid;}
  int            &GetPreciseGrid(void) {return m_PreciseGrid;}
  wxBitmap       GetBitmap();
  void           AddGraphData(const mafGraphData *pGraphData);
  void           SetGraphData(int idx, const mafGraphData *pGraphData);
  void           RemGraphData(const mafGraphData *pGraphData);
  const mafGraphData *GetXParam(){return m_XGraph;}
  void           SetXParam(const mafGraphData *pXGraph){m_XGraph = pXGraph;m_RangeInit = false;}
  void           Update(void);
  bool           SaveGraphAsCSV(wxString const &sFileName);
  /** Return User Interface */
  mafGUI         *GetGui() {return m_Gui;};
  void           CreateGui();
  void           OnEvent(mafEventBase *maf_event);
  void           SetListener(mafObserver *listener) {m_Listener = listener;};
  static wxInt32 GetFontFamily(mafViewIntGraphFontFamily fFamily);
  /** Tune curve parameters */
  void           AdjustCurveAppearance(unsigned int nCurve);
  /** Save window settings in text form*/
  mafStringSet   *SaveSettings(void) const;
  /** Restore window settings from text form*/
  void           LoadSettings(mafStringSet const *pSettings);

protected:

  mafObserver                       *m_Listener;
  std::vector<const mafGraphData *> m_Graphs;
  const mafGraphData                *m_XGraph;
  unsigned                          m_XIndex;
  int                               m_RoughGrid;
  int                               m_PreciseGrid;
  wxInt32                           m_TitleFntHeight;
  unsigned                          m_pointed;
  mafGUI                            *m_Gui;

  void           OnCommand(wxCommandEvent& event);
  void           OnPaint(wxPaintEvent &event);
  void           OnSize(wxSizeEvent &event);
  void           OnCloseWindow  (wxCloseEvent& event);
  void           OnRightMouseButtonDown(wxMouseEvent &event);
  void           OnLeftMouseButtonDown(wxMouseEvent &event);

  //must have for table hook
  DECLARE_EVENT_TABLE();

  wxInt32  m_BaseID;           
  //tuning parameters
  mafDEFINE_GET_SET_MEMBER(wxInt32, TitleFontSize)
  mafDEFINE_GET_SET_MEMBER(mafViewIntGraphFontFamily, TitleFontFamily)
  mafDEFINE_GET_SET_MEMBER(wxInt32, TitleFontStyle)
  mafDEFINE_GET_SET_MEMBER(wxInt32, TitleFontWeight)

  mafDEFINE_GET_SET_MEMBER(wxInt32, TickFontSize)
  mafDEFINE_GET_SET_MEMBER(mafViewIntGraphFontFamily, TickFontFamily)
  mafDEFINE_GET_SET_MEMBER(wxInt32, TickFontStyle)
  mafDEFINE_GET_SET_MEMBER(wxInt32, TickFontWeight)

  mafDEFINE_GET_SET_MEMBER(wxInt32, CurveThickness)
  mafDEFINE_GET_SET_MEMBER(wxInt32, AxisThickness )
  mafDEFINE_GET_SET_MEMBER(wxInt32, GridThickness )

private:
  int       GetIntX(const wxRect *prc, double rX);
  int       GetIntY(const wxRect *prc, double rY);
  int       Log10Abs(double rValue);
  int       GetOptimalSplits(double rMin, double rMax, double rCoef);
  void      AdoptMinMaxRange(double& vMin, double& vMax, double& vPow10Marks);
  void      DrawGraph(wxDC *pCompatDC);
  void      DrawAxes(wxDC *pDC, wxRect *prc, double rXMin, double rXMax, double rYMin, double rYMax, double rXCoef, double rYCoef, bool bGrid, bool bPreciseGrid);
  void      DrawXAxis(wxDC *pDC, wxRect *prc, double rXMin, double rXMax, double rXCoef, bool bGrid, bool bPreciseGrid);
  void      DrawYAxis(wxDC *pDC, wxRect *prc, double rYMin, double rYMax, double rYCoef, bool bGrid, bool bPreciseGrid);
  void      Init();
  void      UpdateRanges();

  wxColor   *m_ColorTable;
  wxInt32   m_ColorTableNumber;
  double    m_XMin, m_XMax, m_YMin, m_YMax;
  bool      m_RangeInit;
};

wxChar const *GetListSeparator();
wxChar const *GetDecimalSeparator();

#endif