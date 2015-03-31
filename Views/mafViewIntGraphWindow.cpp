/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewIntGraphWindow.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 12:13:22 $
  Version:   $Revision: 1.2 $
  Authors:   Fedor Moiseev / Vladik Aranov
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafViewIntGraphWindow.h"
#include "wx/file.h" 
#include "wx/colordlg.h" 

#include <stdio.h>
#include <set>

#include "mafViewIntGraph.h"
#include "mafGUI.h"


//----------------------------------------------------------------------------
// constants
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// macros
//----------------------------------------------------------------------------
#define round(val)     (((val) - floor(val) > ceil(val) - (val)) ? ceil(val) : floor(val))
#ifndef DIM
#define DIM(a)  (sizeof((a)) / sizeof(*(a)))
#endif
#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

//----------------------------------------------------------------------------
#define HALFMARK_SIZE (2)

#define SPLITS_NUMBER (10)
#define SUBSPLITS_NUMBER (5)

#define MAX_SPLITS (15)
#define MIN_SPLITS (2)
#define DEFAULT_SPLITS (4)


#define SIGNIFICANT_DIGITS_ONRANGES (2)
#define SIGNIFICANT_DIGITS_ONMARKS (SIGNIFICANT_DIGITS_ONRANGES )

#define AXES_COLOR (wxColor(0,0,255))
#define SMALL_GRID_COLOR (wxColor(200,200,200))
#define BIG_GRID_COLOR (wxColor(127,127,127))
#define GRAPH_COLOR (wxColor(255,0,255))
#define CAPTION_COLOR (wxColor(0,0,0))
#define MARKER_COLOR (wxColor(0,0,0))
#define MARKER_RADIUS (4)
#define NUMBERS_COLOR (wxColor(0,0,0))
#define SPACING_Y_COEFF 1.5
#define DERIV_ABS_MIN 0.001

#define ID__CLOSE            40023
#define ID__RECALCRANGES     40024
#define ID__GRIDONOFF        40025
#define ID__PRECGRIDONOFF    40026
#define ID__SAVEIMAGE        40027
#define ID__SAVECSV          40028
#define ID__ADJUSTCURVE      40029
#define ID__FIRSTAUTOMENU    40031

#define MAXTEXTLEN           1024 

#ifdef WIN32
#define LONG2POINT(l, pt)  ((pt).x = (SHORT)LOWORD(l), (pt).y = (SHORT)HIWORD(l))
#else
#define LONG2POINT(l, pt)  ((pt).x = (INT)LOWORD(l), (pt).y = (INT)HIWORD(l))
#endif
static wxColor _GraphColors[] = 
{
  wxColor(rand(),rand(),rand()),
  wxColor(rand(),rand(),rand()),
  wxColor(rand(),rand(),rand()),
  wxColor(rand(),rand(),rand()),
  wxColor(rand(),rand(),rand()),
  wxColor(rand(),rand(),rand()),
  wxColor(rand(),rand(),rand()),
  wxColor(rand(),rand(),rand()),
  wxColor(rand(),rand(),rand())//{RGB(255,0,255)};
};

enum
{
  ID_TITLE_FONTSIZE = MINID,
  ID_TITLE_FONTFAMILY ,
  ID_TITLE_FONTSTYLE  ,
  ID_TITLE_FONTWEIGHT ,
  ID_TICK_FONTSIZE    ,
  ID_TICK_FONTFAMILY  ,
  ID_TICK_FONTSTYLE   ,
  ID_TICK_FONTWEIGHT  ,
  ID_THICK_CURVE      ,
  ID_THICK_AXIS       ,
  ID_THICK_GRID       ,
  ID_SHOW_ROUGH_GRID  ,
  ID_SHOW_PRECISE_GRID
};

//----------------------------------------------------------------------------
// compulsory definitions
//----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(mafViewIntGraphWindow, wxWindow)

//----------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------

//struct for data of control window
//must be used only in this file, so it is defined here
//----------------------------------------------------------------------------
int mafViewIntGraphWindow::GetIntX(const wxRect *prc, double rX)
//----------------------------------------------------------------------------
{
  return ((int)(rX * (prc->GetRight() - prc->GetLeft() - 10 * HALFMARK_SIZE) + 5 * HALFMARK_SIZE));
}

//----------------------------------------------------------------------------
int mafViewIntGraphWindow::GetIntY(const wxRect *prc, double rY)
//----------------------------------------------------------------------------
{
  return ((int)(rY * (prc->GetBottom() - prc->GetTop()) + prc->GetTop()));
}

//----------------------------------------------------------------------------
int mafViewIntGraphWindow::Log10Abs(double rValue)
//----------------------------------------------------------------------------
{
  if(fabs(rValue) > 0)
  {
    return ((int)log10(fabs(rValue)));
  }
  return (0);
}

//----------------------------------------------------------------------------
int mafViewIntGraphWindow::GetOptimalSplits(double rMin, double rMax, double rCoef)
//----------------------------------------------------------------------------
{
  int nSplitsNumber = SPLITS_NUMBER;
  int nMin,nMax;

  nMin = (int)(round(rMin / rCoef));
  nMax = (int)(round(rMax / rCoef));
  for(nSplitsNumber = MAX_SPLITS;nSplitsNumber > MIN_SPLITS; nSplitsNumber--)
  {
    if((nMax-nMin) % nSplitsNumber == 0)
    {
      break;
    }
  }
  if(nSplitsNumber == MIN_SPLITS)       
  {
    nSplitsNumber = DEFAULT_SPLITS;
  }
  return (nSplitsNumber);
}

//----------------------------------------------------------------------------
mafStringSet *mafViewIntGraphWindow::SaveSettings(void) const
//----------------------------------------------------------------------------
{
  wxString sParam("");
  wxInt32  nSize = 1 + 3 * m_ColorTableNumber + 4 + 4 + 3 + 2;
  mafStringSet *pSave = new mafStringSet(nSize);
  wxInt32  nI;
  wxInt32  nCount = 0;

  // 1, save plot colors number
  sParam.Printf("%d", m_ColorTableNumber);
  pSave->GetData()[nCount++] = _strdup(sParam.GetData());

  // 3 * m_ColorTableNumber; save table itself
  for(nI = 0; nI < m_ColorTableNumber; nI++)
  {
    sParam.Printf("%d", (wxInt32)m_ColorTable[nI].Red());
    pSave->GetData()[nCount++] = _strdup(sParam.GetData());
    sParam.Printf("%d", (wxInt32)m_ColorTable[nI].Green());
    pSave->GetData()[nCount++] = _strdup(sParam.GetData());
    sParam.Printf("%d", (wxInt32)m_ColorTable[nI].Blue());
    pSave->GetData()[nCount++] = _strdup(sParam.GetData());
  }
  // 4, title font settings
  sParam.Printf("%d", GetTitleFontSize());
  pSave->GetData()[nCount++] = _strdup(sParam.GetData());
  sParam.Printf("%d", GetTitleFontFamily());
  pSave->GetData()[nCount++] = _strdup(sParam.GetData());
  sParam.Printf("%d", GetTitleFontStyle());
  pSave->GetData()[nCount++] = _strdup(sParam.GetData());
  sParam.Printf("%d", GetTitleFontWeight());
  pSave->GetData()[nCount++] = _strdup(sParam.GetData());

  // 4, tick font settings
  sParam.Printf("%d", GetTickFontSize());
  pSave->GetData()[nCount++] = _strdup(sParam.GetData());
  sParam.Printf("%d", GetTickFontFamily());
  pSave->GetData()[nCount++] = _strdup(sParam.GetData());
  sParam.Printf("%d", GetTickFontStyle());
  pSave->GetData()[nCount++] = _strdup(sParam.GetData());
  sParam.Printf("%d", GetTickFontWeight());
  pSave->GetData()[nCount++] = _strdup(sParam.GetData());

  // 3, lines settings
  sParam.Printf("%d", GetCurveThickness());
  pSave->GetData()[nCount++] = _strdup(sParam.GetData());
  sParam.Printf("%d", GetAxisThickness());
  pSave->GetData()[nCount++] = _strdup(sParam.GetData());
  sParam.Printf("%d", GetGridThickness());
  pSave->GetData()[nCount++] = _strdup(sParam.GetData());

  // 2, grids
  sParam.Printf("%d", m_RoughGrid);
  pSave->GetData()[nCount++] = _strdup(sParam.GetData());
  sParam.Printf("%d", m_PreciseGrid);
  pSave->GetData()[nCount++] = _strdup(sParam.GetData());

  wxASSERT(nSize == nCount);
  return (pSave);
}          
//----------------------------------------------------------------------------
void mafViewIntGraphWindow::LoadSettings(mafStringSet const *pSettings)
//----------------------------------------------------------------------------
{
  wxString sParam("");
  wxInt32  nI;
  wxInt32  nCount = 0;
  wxInt32  nRed, nGreen, nBlue;

  // 1, load plot colors number
  if(pSettings->GetStringNumber() > nCount)
    m_ColorTableNumber = atoi(pSettings->GetData()[nCount++]);
 
  // 3 * m_ColorTableNumber; load table itself
  for(nI = 0; nI < m_ColorTableNumber; nI++)
  {
    if(pSettings->GetStringNumber() > nCount)
      nRed   = atoi(pSettings->GetData()[nCount++]);
    if(pSettings->GetStringNumber() > nCount)
      nGreen = atoi(pSettings->GetData()[nCount++]);
    if(pSettings->GetStringNumber() > nCount)
      nBlue  = atoi(pSettings->GetData()[nCount++]);
    m_ColorTable[nI].Set(nRed, nGreen, nBlue);
  }
  // 4, title font settings
  if(pSettings->GetStringNumber() > nCount)
    SetTitleFontSize(atoi(pSettings->GetData()[nCount++]));
  if(pSettings->GetStringNumber() > nCount)
    SetTitleFontFamily(mafViewIntGraphFontFamily(atoi(pSettings->GetData()[nCount++])));
  if(pSettings->GetStringNumber() > nCount)
    SetTitleFontStyle(atoi(pSettings->GetData()[nCount++]));
  if(pSettings->GetStringNumber() > nCount)
    SetTitleFontWeight(atoi(pSettings->GetData()[nCount++]));

  // 4, tick font settings
  if(pSettings->GetStringNumber() > nCount)
    SetTickFontSize(atoi(pSettings->GetData()[nCount++]));
  if(pSettings->GetStringNumber() > nCount)
    SetTickFontFamily(mafViewIntGraphFontFamily(atoi(pSettings->GetData()[nCount++])));
  if(pSettings->GetStringNumber() > nCount)
    SetTickFontStyle(atoi(pSettings->GetData()[nCount++]));
  if(pSettings->GetStringNumber() > nCount)
    SetTickFontWeight(atoi(pSettings->GetData()[nCount++]));

  // 3, lines settings
  if(pSettings->GetStringNumber() > nCount)
    SetCurveThickness(atoi(pSettings->GetData()[nCount++]));
  if(pSettings->GetStringNumber() > nCount)
    SetAxisThickness(atoi(pSettings->GetData()[nCount++]));
  if(pSettings->GetStringNumber() > nCount)
    SetGridThickness(atoi(pSettings->GetData()[nCount++]));

  // 2, grids
  if(pSettings->GetStringNumber() > nCount)
    m_RoughGrid  =  (atoi(pSettings->GetData()[nCount++]) != 0);
  if(pSettings->GetStringNumber() > nCount)
    m_PreciseGrid = (atoi(pSettings->GetData()[nCount++]) != 0);
}
//----------------------------------------------------------------------------
void mafViewIntGraphWindow::DrawXAxis(wxDC *pDC, wxRect *prc, double rXMin, double rXMax, double rXCoef, bool bGrid, bool bPreciseGrid)
//----------------------------------------------------------------------------
{
  double rX,rY,rStepX;
  int nX,nY,nI;
  int nSplitsNumber    = SPLITS_NUMBER;
  int nSubSplitsNumber = SUBSPLITS_NUMBER;
  bool  bXUp;
  char sValue[MAXTEXTLEN + 1];
  wxRect  rTextRect;
  wxPoint szTextSize;
  wxPen   *pPen, *pBigPen, *pSmallPen;

//  rY = (cpgtrRanges->rYMax - cpgtrRanges->rYAxis)/(cpgtrRanges->rYMax - cpgtrRanges->rYMin);
  rY = 1;
  nY = GetIntY(prc,rY);

  bXUp = (rY <= 0.5f);

  pPen      = new wxPen(AXES_COLOR      , m_AxisThickness, wxSOLID);
  pBigPen   = new wxPen(BIG_GRID_COLOR  , m_GridThickness, wxDOT_DASH);
  pSmallPen = new wxPen(SMALL_GRID_COLOR, m_GridThickness, wxDOT);
  pDC->SetPen(*pPen);

  pDC->DrawLine(GetIntX(prc,0.f), nY, GetIntX(prc,1.f), nY);
  
  nSplitsNumber = GetOptimalSplits(rXMin, rXMax, rXCoef);

  rStepX = (rXMax - rXMin) / (nSplitsNumber * nSubSplitsNumber);
  for(nI = 0; nI <= nSplitsNumber * nSubSplitsNumber; nI++)
  {
    rX = rXMin + rStepX * (nI);
    sprintf(sValue," %.1f", round(rX / rXCoef * 1000) / 1000);
    rX  = (rX - rXMin)/(rXMax - rXMin);
    nX = GetIntX(prc,rX);
    if(nI % nSubSplitsNumber == 0)
    {
      if(bGrid && nI != 0)
      {
        pDC->SetPen(*pBigPen);
        pDC->DrawLine(nX, GetIntY(prc, 1.0f), nX, GetIntY(prc, 0.f));
        pDC->SetPen(*pPen);
      }
      pDC->DrawLine(nX, nY + 2 * HALFMARK_SIZE, nX, nY - 2 * HALFMARK_SIZE);
      if(nI != 0)
      {
        pDC->GetTextExtent(sValue, &szTextSize.x, &szTextSize.y);
        if(bXUp)
        {
          rTextRect.SetLeft  (nX - szTextSize.x / 2);
          rTextRect.SetTop   (nY - 2 * HALFMARK_SIZE - szTextSize.y);
          rTextRect.SetRight (nX + szTextSize.x / 2 + 1);
          rTextRect.SetBottom(nY - 2 * HALFMARK_SIZE);
        }
        else
        {
          rTextRect.SetLeft  (nX - szTextSize.x / 2);
          rTextRect.SetTop   (nY + 2 * HALFMARK_SIZE);
          rTextRect.SetRight (nX + szTextSize.x / 2 + 1);
          rTextRect.SetBottom(nY + 2 * HALFMARK_SIZE + szTextSize.y);
        }
        pDC->DrawText(sValue, rTextRect.GetLeft(), rTextRect.GetTop());
      }
    }
    else
    {
      if(bPreciseGrid && bGrid)
      {
        pDC->SetPen(*pSmallPen);
        pDC->DrawLine(nX, GetIntY(prc, 1.0f), nX, GetIntY(prc, 0.f));
        pDC->SetPen(*pPen);
      }
      pDC->DrawLine(nX, nY + HALFMARK_SIZE, nX, nY - HALFMARK_SIZE);
    }
  }
  pDC->SetPen(wxNullPen);
  cppDEL(pPen);
  cppDEL(pBigPen);
  cppDEL(pSmallPen);
  return;
}

//----------------------------------------------------------------------------
void mafViewIntGraphWindow::DrawYAxis(wxDC *pDC, wxRect *prc, double rYMin, double rYMax, double rYCoef, bool bGrid, bool bPreciseGrid)
//----------------------------------------------------------------------------
{
  double   rX,rY,rStepY;
  int     nX,nY,nI;
  int     nSplitsNumber = SPLITS_NUMBER;
  int     nSubSplitsNumber = SUBSPLITS_NUMBER;
  bool    bYLeft;
  char    sValue[MAXTEXTLEN + 1];
  wxRect  rTextRect;
  wxPoint szTextSize;
  wxPen   *pPen, *pBigPen, *pSmallPen;

//  rX = (cpgtrRanges->rXAxis - cpgtrRanges->rXMin) / (cpgtrRanges->rXMax - cpgtrRanges->rXMin);
  rX = 0;
  nX = GetIntX(prc,rX);

  bYLeft = (rX <= 0.5f);

  pPen      = new wxPen(AXES_COLOR      , m_AxisThickness, wxSOLID);
  pBigPen   = new wxPen(BIG_GRID_COLOR  , m_GridThickness, wxDOT_DASH);
  pSmallPen = new wxPen(SMALL_GRID_COLOR, m_GridThickness, wxDOT);
  pDC->SetPen(*pPen);
  
  pDC->DrawLine(nX, GetIntY(prc,0.f), nX, GetIntY(prc,1.f));

  nSplitsNumber = GetOptimalSplits(rYMin, rYMax, rYCoef);

  rStepY = (rYMax - rYMin) / (nSplitsNumber * nSubSplitsNumber);
  for(nI = 0;nI <= nSplitsNumber * nSubSplitsNumber; nI++)
  {
    rY  = rYMin + rStepY * (nI);
    sprintf(sValue," %.1f", round(rY / rYCoef * 1000) / 1000);
    rY  = (rYMax - rY)/(rYMax - rYMin);
    nY  = GetIntY(prc,rY);
    if(nI % nSubSplitsNumber == 0)
    {
      if(bGrid && nI != 0)
      {
        pDC->SetPen(*pBigPen);
        pDC->DrawLine(GetIntX(prc, 1.0f), nY, GetIntX(prc, 0.f), nY);
        pDC->SetPen(*pPen);
      }
      pDC->DrawLine(nX + 2 * HALFMARK_SIZE, nY, nX - 2 * HALFMARK_SIZE, nY);
      if(nI != 0)
      {
        pDC->GetTextExtent(sValue, &szTextSize.x, &szTextSize.y);
        if(bYLeft)
        {
          rTextRect.SetLeft  (nX + 2 * HALFMARK_SIZE);
          rTextRect.SetTop   (nY - szTextSize.y / 2);
          rTextRect.SetRight (nX + 2 * HALFMARK_SIZE + szTextSize.x);
          rTextRect.SetBottom(nY + szTextSize.y / 2);
        }
        else
        {
          rTextRect.SetLeft  (nX - 2 * HALFMARK_SIZE - szTextSize.x);
          rTextRect.SetTop   (nY - szTextSize.y / 2);
          rTextRect.SetRight (nX - 2 * HALFMARK_SIZE);
          rTextRect.SetBottom(nY + szTextSize.y / 2);
        }
        pDC->DrawText(sValue, rTextRect.GetLeft(), rTextRect.GetTop());
      }
    }
    else
    {
      if(bPreciseGrid && bGrid)
      {
        pDC->SetPen(*pSmallPen);
        pDC->DrawLine(GetIntX(prc, 1.0f), nY, GetIntX(prc, 0.f), nY);
        pDC->SetPen(*pPen);
      }
      pDC->DrawLine(nX + HALFMARK_SIZE, nY, nX - HALFMARK_SIZE, nY);
    }
  }
  pDC->SetPen(wxNullPen);
  cppDEL(pPen);
  cppDEL(pBigPen);
  cppDEL(pSmallPen);
  return;
}

//----------------------------------------------------------------------------
void mafViewIntGraphWindow::DrawAxes(wxDC *pDC, wxRect *prc, double rXMin, double rXMax, double rYMin, double rYMax, double rXCoef, double rYCoef, bool bGrid, bool bPreciseGrid)
{
  wxColor cTextColor;

  cTextColor = pDC->GetTextForeground();
  pDC->SetTextForeground(NUMBERS_COLOR);

  DrawXAxis(pDC, prc, rXMin, rXMax, rXCoef, bGrid, bPreciseGrid);
  DrawYAxis(pDC, prc, rYMin, rYMax, rYCoef, bGrid, bPreciseGrid);

  pDC->SetTextForeground(cTextColor);
  return;
}

//----------------------------------------------------------------------------
void mafViewIntGraphWindow::AdoptMinMaxRange(double& vMin, double& vMax, double& vPow10Marks)
//----------------------------------------------------------------------------
{
  int    nLowLog,nHiLog,nLog;
  double rPow10Ranges;

  nLowLog = Log10Abs(vMin);
  nHiLog  = Log10Abs(vMax);

  nLog = (nHiLog > nLowLog) ? nHiLog : nLowLog;

  rPow10Ranges = pow(10.0, nLog - SIGNIFICANT_DIGITS_ONRANGES + 1);

  vPow10Marks  = pow(10.0, nLog - SIGNIFICANT_DIGITS_ONMARKS + 1);

  vMax /= rPow10Ranges;
  vMax  = ceil(vMax);
  vMax *= rPow10Ranges;

  vMin /= rPow10Ranges;
  vMin  = floor(vMin);
  vMin *= rPow10Ranges;
}

void mafViewIntGraphWindow::UpdateRanges()
{
  bool   xRangeInit = m_RangeInit;
  double GrowStep    = 5.0;
  double RangeSpace  = 0.000001;

  if(m_XGraph)
  {
    for(unsigned j = 0; j < m_XGraph->GetSize(); j++)
    {
      for(unsigned k = 0; k < m_XGraph->GetXVarNum(); k++)
      {
        double curVal = m_XGraph->GetValue(j, m_XGraph->GetIndexX(k), m_XGraph->GetXDer(k));
        if(!xRangeInit)
        {
          m_XMin = curVal;
          m_XMax = m_XMin;
          m_XMin -= RangeSpace;
          m_XMax += RangeSpace;
          xRangeInit = true;
        }
        else
        {
          double addGrow = (m_XMax - m_XMin) * GrowStep * 0.01;
          if(curVal > m_XMax)
          {
            m_XMax   = curVal + addGrow;
          }
          if(curVal < m_XMin)
          {
            m_XMin = curVal - addGrow;
          }
        }
      }
    }
  }
  else
  {
    for(unsigned i = 0; i < m_Graphs.size(); i++)
    {
      for(unsigned j = 0; j < m_Graphs[i]->GetSize(); j++)
      {
        double curVal = m_Graphs[i]->GetValue(j, 0, 0);
        if(!xRangeInit)
        {
          m_XMin = curVal;
          m_XMax = m_XMin;
          m_XMin -= RangeSpace;
          m_XMax += RangeSpace;
          xRangeInit = true;
        }
        else
        {
          double addGrow = (m_XMax - m_XMin) * GrowStep * 0.01;
          if(curVal > m_XMax)
          {
            m_XMax   = curVal + addGrow;
          }
          if(curVal < m_XMin)
          {
            m_XMin = curVal - addGrow;
          }
        }
      }
    }
  }
  double oldymin    = m_YMin;
  double oldymax    = m_YMax;
  bool   yRangeInit = m_RangeInit;

  for(unsigned i = 0; i < m_Graphs.size(); i++)
  {
    for(unsigned j = 0; j < m_Graphs[i]->GetYVarNum(); j++)
    {
      double VMin, VMax;
      bool   localYInit = m_RangeInit;
      if(localYInit)
      {
        VMin       = oldymin;
        VMax       = oldymax;
        localYInit = true;
      }
      for(unsigned k = 0; k < m_Graphs[i]->GetSize(); k++)
      {
        double curVal;
        if(m_Graphs[i]->GetYDer(j) == 0)
          curVal = m_Graphs[i]->GetValue(k, m_Graphs[i]->GetIndexY(j), m_Graphs[i]->GetYDer(j));
        else
        {
          bool ok = m_Graphs[i]->GetValueByParam(curVal, m_Graphs[i]->GetValue(k, 0, 0), m_Graphs[i]->GetIndexY(j), m_Graphs[i]->GetYDer(j));
          if(!ok)
            continue;
        }
        if(m_Graphs[i]->GetYDer(j) > 0)
        {
          double derX = 1.0;
          if(m_XGraph != NULL)
          {
            double tpar = m_Graphs[i]->GetValue(k, 0, 0);
            if(!m_XGraph->GetValueByParam(derX, tpar, m_XGraph->GetIndexX(0), 1))
              continue;
            derX *= m_XGraph->GetDerivCoef(m_XGraph->GetIndexX(0));
          }
          /*if(fabs(derX) < DERIV_ABS_MIN)
          continue;*/
          curVal /= derX;
        }

        if(!localYInit)
        {
          VMin   = curVal;
          VMax  = VMin;
          VMin -= RangeSpace;
          VMax += RangeSpace;
          localYInit = true;
        }
        else
        {
          double addGrow = (VMax - VMin) * GrowStep * 0.01;
          if(curVal > VMax)
          {
            VMax   = curVal + addGrow;
          }
          if(curVal < VMin)
          {
            VMin = curVal - addGrow;
          }
        }
      }
      if(localYInit)
      {
        if(!yRangeInit || VMin < m_YMin)
          m_YMin = VMin;
        if(!yRangeInit || VMax > m_YMax)
          m_YMax = VMax;
        yRangeInit = true;
      }
    }
  }
  m_RangeInit = xRangeInit && yRangeInit;
}



/*
 * Function draws graph attached to window in specified DC
 *
 * @memo    
 * @param   hWnd    handle window for which we are painting
 * @param   hDC     DC where to paint
 * @return          nothing
 * @author  Earnol
 * @see     Nothing
 */
#ifdef ORIGGGGRRRRRRRRR
//----------------------------------------------------------------------------
void mafViewIntGraphWindow::DrawGraph(wxDC *pCompatDC)
//----------------------------------------------------------------------------
{
  int          nClHeight, nClWidth;
  wxRect       rTextRect;
  wxPoint      szTextSize;
  unsigned int nI;
  int          nX,nY;
  unsigned int nSize,nBreakBegin,nBreakEnd;
  double       rX,rY;
  double       valX, valY, derX;
  char         sExp[MAXTEXTLEN + 1];
  char         sXIDDesc[MAXTEXTLEN + 1];
  char         sYIDDesc[MAXTEXTLEN + 1];
  unsigned int nDimX,nDimY,nJ,nYNumber,nXNumber, nXDer, nYDer;
  wxPoint      pt;
  wxRect       rcGraphRect;
  double       rXPow10Marks,rYPow10Marks;
  double       XMin, XMax, YMin, YMax;
  wxColor      cOldColor;
  int          nStartX, nStartY;
  wxPen        *pPen;
  wxFont       *pFont;

  pCompatDC->GetSize(&nClWidth, &nClHeight);
  pPen = new wxPen(wxColor(255,255,255), 1, wxSOLID);
  pCompatDC->SetPen(*pPen);
  pCompatDC->DrawRectangle(0, 0, nClWidth, nClHeight);
  pCompatDC->SetPen(wxNullPen);
  cppDEL(pPen);

  // do nothing in case we do not need anything
  if(m_Graphs.size() == 0 || m_Graphs[0] == NULL)
  {
    pPen = new wxPen(wxColor(0,0,0), 1, wxSOLID);
    pCompatDC->SetPen(*pPen);
    //pCompatDC->DrawLine(0, 0, nClWidth, nClHeight);
    //pCompatDC->DrawLine(nClWidth, 0, 0, nClHeight);
    pCompatDC->SetPen(wxNullPen);
    cppDEL(pPen);
    return;
  }

  //access
  nDimX       = m_Graphs[0]->GetXVarNum();
  nDimY       = m_Graphs[0]->GetYVarNum();
  nSize       = m_Graphs[0]->GetSize();
  nBreakBegin = m_Graphs[0]->GetBreakBegin();
  nBreakEnd   = m_Graphs[0]->GetBreakEnd();
  
  //do not draw if have not enough points
  if((nSize < 2)||(nDimX == 0)||(nDimY == 0))
  {
    pPen = new wxPen(wxColor(0,0,0), 1, wxSOLID);
    pCompatDC->SetPen(*pPen);
    //pCompatDC->DrawLine(0, 0, nClWidth, nClHeight);
    //pCompatDC->DrawLine(nClWidth, 0, 0, nClHeight);
    pCompatDC->SetPen(wxNullPen);
    cppDEL(pPen);
    return;
  }

  UpdateRanges();

  XMin = m_XMin;
  XMax = m_XMax;
  YMin = m_YMin;
  YMax = m_YMax;

  AdoptMinMaxRange(XMin, XMax, rXPow10Marks);
  AdoptMinMaxRange(YMin, YMax, rYPow10Marks);

  pFont = new wxFont(m_TitleFontSize, GetFontFamily(m_TitleFontFamily), wxNORMAL, wxLIGHT, false, wxEmptyString, wxFONTENCODING_SYSTEM) ;
  pCompatDC->SetFont(*pFont);
  m_TitleFntHeight = pCompatDC->GetCharHeight();
  rcGraphRect.SetLeft(0);
  rcGraphRect.SetRight(nClWidth);
  //set it to N*1.5 graphs 
  rcGraphRect.SetTop((wxInt32)(SPACING_Y_COEFF * m_TitleFntHeight * nDimY));
  
  sXIDDesc[0]='\0';
  sYIDDesc[0]='\0';
  
  pCompatDC->SetFont(wxNullFont);
  cppDEL(pFont);
  pFont = new wxFont(m_TickFontSize, GetFontFamily(m_TickFontFamily), wxNORMAL, wxLIGHT, false, wxEmptyString, wxFONTENCODING_SYSTEM) ;
  pCompatDC->SetFont(*pFont);
  rcGraphRect.SetBottom(nClHeight - (int)round(1.2 * pCompatDC->GetCharHeight()));

  //drawing axes
  DrawAxes(pCompatDC, &rcGraphRect, XMin, XMax, YMin, YMax, rXPow10Marks, rYPow10Marks, m_RoughGrid != 0, m_PreciseGrid != 0);

  
  pCompatDC->SetFont(wxNullFont);
  cppDEL(pFont);
  pFont = new wxFont(m_TitleFontSize, GetFontFamily(m_TitleFontFamily), wxNORMAL, wxLIGHT, false, wxEmptyString, wxFONTENCODING_SYSTEM) ;
  pCompatDC->SetFont(*pFont);

  //drawing graph
  pt.x = 0;
  pt.y = 0;
  for(nJ = 0; nJ < nDimY; nJ++)
  {
    pPen = new wxPen(m_ColorTable[nJ % m_ColorTableNumber], m_CurveThickness, wxSOLID);
    pCompatDC->SetPen(*pPen);
    cOldColor = pCompatDC->GetTextForeground();
    pCompatDC->SetTextForeground(m_ColorTable[nJ % m_ColorTableNumber]);
    nXNumber = m_Graphs[0]->GetIndexX(0);
    nXDer    = m_Graphs[0]->GetXDer(0);
    nYNumber = m_Graphs[0]->GetIndexY(nJ);
    nYDer    = m_Graphs[0]->GetYDer(nJ);
    //writing captions
    {
      m_Graphs[0]->GetIDDesc(nXNumber, nXDer, sXIDDesc, MAXTEXTLEN);
      m_Graphs[0]->GetIDDesc(nYNumber, nYDer, sYIDDesc, MAXTEXTLEN);
      if((int)rYPow10Marks != 1 && (int)rXPow10Marks != 1)
      {
        sprintf(sExp,"%s, %1.0e (%s, %1.0e) ", sYIDDesc, rYPow10Marks, sXIDDesc, rXPow10Marks);
      }
      else  if((int)rYPow10Marks == 1 && (int)rXPow10Marks != 1)
      {
        sprintf(sExp,"%s (%s, %1.0e) ", sYIDDesc, sXIDDesc, rXPow10Marks);
      }
      else  if((int)rYPow10Marks != 1 && (int)rXPow10Marks == 1)
      {
        sprintf(sExp,"%s, %1.0e (%s) ", sYIDDesc, rYPow10Marks, sXIDDesc);
      }
      else
      {
        sprintf(sExp,"%s (%s) ", sYIDDesc, sXIDDesc);
      }
      pCompatDC->GetTextExtent(sExp, &szTextSize.x, &szTextSize.y);
      rTextRect.SetLeft(pt.x + 35);
      rTextRect.SetTop(pt.y);
      rTextRect.SetRight(pt.x + szTextSize.x + 35);
      rTextRect.SetBottom(m_TitleFntHeight);
      pCompatDC->DrawText(sExp, rTextRect.GetLeft(), rTextRect.GetTop());
      //pt.x += szTextSize.x;
      pt.y += round(m_TitleFntHeight * SPACING_Y_COEFF);
    }
    unsigned int nStart  = 0;
    unsigned int nFinish = nBreakBegin;
    for(int count = 0; count < 2; count++)
    {
      if(nFinish - nStart > 1)
      {
        bool previnit = false;
        for(nI = nStart; nI < nFinish; nI++)
        {
          valX = m_Graphs[0]->GetValue(nI,nXNumber, nXDer);
          valY = m_Graphs[0]->GetValue(nI,nYNumber, nYDer);
          if(nYDer == 1)
          {
            derX  = m_Graphs[0]->GetValue(nI,nXNumber, nYDer) * m_Graphs[0]->GetDerivCoef(nXNumber);
            /*if(fabs(derX) < DERIV_ABS_MIN)
              continue;*/
            valY /= derX;
            //valY  = fabs(valY);
          }
          rX   = (valX - XMin)/(XMax - XMin);
          rY   = (YMax - valY)/(YMax - YMin);
          nX   = GetIntX(&rcGraphRect,rX);
          nY   = GetIntY(&rcGraphRect,rY);
          if(previnit)
          {
            pCompatDC->DrawLine(nStartX, nStartY, nX, nY);
          }
          nStartX = nX;
          nStartY = nY;
          previnit = true;
        }
      }
      nStart  = nBreakEnd;
      nFinish = nSize;
    }
    pCompatDC->SetTextForeground(cOldColor);
    pCompatDC->SetPen(wxNullPen);
    cppDEL(pPen);

    pPen = new wxPen(MARKER_COLOR, 0, wxSOLID);
    pCompatDC->SetPen(*pPen);
    pCompatDC->DrawEllipse(nX - MARKER_RADIUS, nY - MARKER_RADIUS, 2 * MARKER_RADIUS, 2 * MARKER_RADIUS);
    pCompatDC->SetPen(wxNullPen);
    cppDEL(pPen);
  }
  pCompatDC->SetFont(wxNullFont);
  cppDEL(pFont);
  return;
} // end of _saRedraw
#endif



//----------------------------------------------------------------------------
void mafViewIntGraphWindow::DrawGraph(wxDC *pCompatDC)
//----------------------------------------------------------------------------
{
  int          nClHeight, nClWidth;
  wxRect       rTextRect;
  wxPoint      szTextSize;
  char         sExp[MAXTEXTLEN + 1];
  char         sXIDDesc[MAXTEXTLEN + 1];
  char         sYIDDesc[MAXTEXTLEN + 1];
  unsigned int nDimX,nDimY,nMaxSize;
  wxPoint      pt;
  wxRect       rcGraphRect;
  double       rXPow10Marks,rYPow10Marks;
  double       XMin, XMax, YMin, YMax;
  wxColor      cOldColor;
  int          nStartX, nStartY;
  wxPen        *pPen;
  wxFont       *pFont;

  pCompatDC->GetSize(&nClWidth, &nClHeight);
  pPen = new wxPen(wxColor(255,255,255), 1, wxSOLID);
  pCompatDC->SetPen(*pPen);
  pCompatDC->DrawRectangle(0, 0, nClWidth, nClHeight);
  pCompatDC->SetPen(wxNullPen);
  cppDEL(pPen);

  // do nothing in case we do not need anything
  if(m_Graphs.size() == 0)
    return;

  //access
  nDimX       = 1;//m_Graphs[0]->GetXVarNum();
  nDimY       = 0;//m_Graphs[0]->GetYVarNum();
  nMaxSize    = 0;
  for(unsigned i = 0; i < m_Graphs.size(); i++)
  {
    //nDimX   += m_Graphs[i]->GetXVarNum();
    nDimY   += m_Graphs[i]->GetYVarNum();
    nMaxSize = max(nMaxSize, m_Graphs[i]->GetSize());
  }
  //nSize       = m_Graphs[0]->GetSize();

  //do not draw if have not enough points
  if((nMaxSize < 2)||(nDimX == 0||nDimY == 0))
  {
    return;
  }

  UpdateRanges();

  XMin = m_XMin;
  XMax = m_XMax;
  YMin = m_YMin;
  YMax = m_YMax;

  AdoptMinMaxRange(XMin, XMax, rXPow10Marks);
  AdoptMinMaxRange(YMin, YMax, rYPow10Marks);

  pFont = new wxFont(m_TitleFontSize, GetFontFamily(m_TitleFontFamily), wxNORMAL, wxLIGHT, false, wxEmptyString, wxFONTENCODING_SYSTEM) ;
  pCompatDC->SetFont(*pFont);
  m_TitleFntHeight = pCompatDC->GetCharHeight();
  rcGraphRect.SetLeft(0);
  rcGraphRect.SetRight(nClWidth);
  //set it to N*1.5 graphs 
  rcGraphRect.SetTop((wxInt32)(SPACING_Y_COEFF * m_TitleFntHeight * nDimY));

  sXIDDesc[0]='\0';
  sYIDDesc[0]='\0';

  pCompatDC->SetFont(wxNullFont);
  cppDEL(pFont);
  pFont = new wxFont(m_TickFontSize, GetFontFamily(m_TickFontFamily), wxNORMAL, wxLIGHT, false, wxEmptyString, wxFONTENCODING_SYSTEM) ;
  pCompatDC->SetFont(*pFont);
  rcGraphRect.SetBottom(nClHeight - (int)round(1.2 * pCompatDC->GetCharHeight()));

  //drawing axes
  DrawAxes(pCompatDC, &rcGraphRect, XMin, XMax, YMin, YMax, rXPow10Marks, rYPow10Marks, m_RoughGrid != 0, m_PreciseGrid != 0);


  pCompatDC->SetFont(wxNullFont);
  cppDEL(pFont);
  pFont = new wxFont(m_TitleFontSize, GetFontFamily(m_TitleFontFamily), wxNORMAL, wxLIGHT, false, wxEmptyString, wxFONTENCODING_SYSTEM) ;
  pCompatDC->SetFont(*pFont);

  //drawing graph
  pt.x = 0;
  pt.y = 0;
  unsigned varIndex = 0;
  for(unsigned i = 0; i < m_Graphs.size(); i++)
  {
    for(unsigned j = 0; j < m_Graphs[i]->GetYVarNum(); j++,varIndex++)
    {
      pPen = new wxPen(m_ColorTable[varIndex % m_ColorTableNumber], m_CurveThickness, wxSOLID);
      pCompatDC->SetPen(*pPen);
      cOldColor = pCompatDC->GetTextForeground();
      pCompatDC->SetTextForeground(m_ColorTable[varIndex % m_ColorTableNumber]);
      unsigned nYNumber = m_Graphs[i]->GetIndexY(j);
      unsigned nYDer    = m_Graphs[i]->GetYDer(j);
      //writing captions
      {
        if(m_XGraph)
          m_XGraph->GetIDDesc(m_XGraph->GetIndexX(0), 0, sXIDDesc, MAXTEXTLEN);
        else
          m_Graphs[i]->GetIDDesc(0, 0, sXIDDesc, MAXTEXTLEN);
        m_Graphs[i]->GetIDDesc(nYNumber, nYDer, sYIDDesc, MAXTEXTLEN);
        if((int)rYPow10Marks != 1 && (int)rXPow10Marks != 1)
        {
          sprintf(sExp,"%s, %1.0e (%s, %1.0e) ", sYIDDesc, rYPow10Marks, sXIDDesc, rXPow10Marks);
        }
        else  if((int)rYPow10Marks == 1 && (int)rXPow10Marks != 1)
        {
          sprintf(sExp,"%s (%s, %1.0e) ", sYIDDesc, sXIDDesc, rXPow10Marks);
        }
        else  if((int)rYPow10Marks != 1 && (int)rXPow10Marks == 1)
        {
          sprintf(sExp,"%s, %1.0e (%s) ", sYIDDesc, rYPow10Marks, sXIDDesc);
        }
        else
        {
          sprintf(sExp,"%s (%s) ", sYIDDesc, sXIDDesc);
        }
        pCompatDC->GetTextExtent(sExp, &szTextSize.x, &szTextSize.y);
        rTextRect.SetLeft(pt.x + 35);
        rTextRect.SetTop(pt.y);
        rTextRect.SetRight(pt.x + szTextSize.x + 35);
        rTextRect.SetBottom(m_TitleFntHeight);
        pCompatDC->DrawText(sExp, rTextRect.GetLeft(), rTextRect.GetTop());
        //pt.x += szTextSize.x;
        pt.y += round(m_TitleFntHeight * SPACING_Y_COEFF);
      }
      pCompatDC->SetTextForeground(cOldColor);
      pCompatDC->SetPen(wxNullPen);
      cppDEL(pPen);
    }
  }

  bool previnit = false;
  if(m_XGraph != NULL)
  {
    varIndex = 0;
    for(unsigned i = 0; i < m_Graphs.size(); i++)
    {
      for(int nJ = 0; nJ < m_Graphs[i]->GetYVarNum(); nJ++, varIndex++)
      {
        int nX, nY;
        pPen = new wxPen(m_ColorTable[varIndex % m_ColorTableNumber], m_CurveThickness, wxSOLID);
        pCompatDC->SetPen(*pPen);
        cOldColor = pCompatDC->GetTextForeground();
        pCompatDC->SetTextForeground(m_ColorTable[varIndex % m_ColorTableNumber]);

        unsigned nStart  = m_XGraph->GetBreakBegin();
        unsigned nFinish = m_XGraph->GetBreakEnd();
        for(int count = 0; count < 2; count++)
        {
          if(nFinish - nStart > 0)
            previnit = false;
          for(int nI = nStart; nI < nFinish; nI++)
          {
            double valX   = m_XGraph->GetValue(nI, m_XGraph->GetIndexX(0), m_XGraph->GetXDer(0));
            double paramt = m_XGraph->GetValue(nI, 0, 0);
            double valY;
            unsigned int der = m_Graphs[i]->GetYDer(nJ);
            if(m_Graphs[i]->GetValueByParam(valY, paramt, m_Graphs[i]->GetIndexY(nJ), der))
            {
              if(der == 1)
              {
                double derX = m_XGraph->GetValue(nI, m_XGraph->GetIndexX(0), 1);
                derX *= m_XGraph->GetDerivCoef(m_XGraph->GetIndexX(0));
                /*if(fabs(derX) < DERIV_ABS_MIN)
                continue;*/
                valY /= derX;
              }

              double rX   = (valX - XMin) /( XMax - XMin);
              double rY   = (YMax - valY) / (YMax - YMin);
              nX      = GetIntX(&rcGraphRect, rX);
              nY      = GetIntY(&rcGraphRect, rY);
              if(previnit)
              {
                pCompatDC->DrawLine(nStartX, nStartY, nX, nY);
              }
              nStartX = nX;
              nStartY = nY;
              previnit = true;
            }
          }
          nStart = 0;
          nFinish = m_XGraph->GetBreakBegin();
        }
        pCompatDC->SetTextForeground(cOldColor);
        pCompatDC->SetPen(wxNullPen);
        cppDEL(pPen);

        if(previnit)
        {
          pPen = new wxPen(MARKER_COLOR, 0, wxSOLID);
          pCompatDC->SetPen(*pPen);
          pCompatDC->DrawEllipse(nX - MARKER_RADIUS, nY - MARKER_RADIUS, 2 * MARKER_RADIUS, 2 * MARKER_RADIUS);
          pCompatDC->SetPen(wxNullPen);
          cppDEL(pPen);
        }
      }
    }
  }
  else
  {
    varIndex = 0;
    for(unsigned i = 0; i < m_Graphs.size(); i++)
    {
      for(unsigned j = 0; j < m_Graphs[i]->GetYVarNum(); j++, varIndex++)
      {
        pPen = new wxPen(m_ColorTable[varIndex % m_ColorTableNumber], m_CurveThickness, wxSOLID);
        pCompatDC->SetPen(*pPen);
        cOldColor = pCompatDC->GetTextForeground();
        pCompatDC->SetTextForeground(m_ColorTable[varIndex % m_ColorTableNumber]);
        unsigned nYNumber = m_Graphs[i]->GetIndexY(j);
        unsigned nYDer    = m_Graphs[i]->GetYDer(j);
        //writing captions
        unsigned int nStart  = 0;
        unsigned int nFinish = m_Graphs[i]->GetBreakBegin();
        int nX, nY;
        for(int count = 0; count < 2; count++)
        {
          if(nFinish - nStart > 0)
            previnit = false;
          if(nFinish - nStart <= 1)
          {
            nStart  = m_Graphs[i]->GetBreakEnd();
            nFinish = m_Graphs[i]->GetSize();
            continue;
          }
          for(unsigned t = nStart; t < nFinish; t++)
          {
            double valX;
            double valY;
            valX = m_Graphs[i]->GetValue(t, 0, 0);
            if(m_Graphs[i]->GetValueByParam(valY, valX, nYNumber, nYDer))
            {
              /*if(nYDer == 1)
              {
              double derX = 1.0;
              valY /= derX;
              }*/

              double rX   = (valX - XMin) /( XMax - XMin);
              double rY   = (YMax - valY) / (YMax - YMin);
              nX      = GetIntX(&rcGraphRect, rX);
              nY      = GetIntY(&rcGraphRect, rY);
              if(previnit)
              {
                pCompatDC->DrawLine(nStartX, nStartY, nX, nY);
              }
              nStartX = nX;
              nStartY = nY;
              previnit = true;
            }
          }
          nStart  = m_Graphs[i]->GetBreakEnd();
          nFinish = m_Graphs[i]->GetSize();
        }
        pCompatDC->SetTextForeground(cOldColor);
        pCompatDC->SetPen(wxNullPen);
        cppDEL(pPen);
        if(previnit)
        {
          pPen = new wxPen(MARKER_COLOR, 0, wxSOLID);
          pCompatDC->SetPen(*pPen);
          pCompatDC->DrawEllipse(nX - MARKER_RADIUS, nY - MARKER_RADIUS, 2 * MARKER_RADIUS, 2 * MARKER_RADIUS);
          pCompatDC->SetPen(wxNullPen);
          cppDEL(pPen);
        }
      }
    }
  }

  pCompatDC->SetFont(wxNullFont);
  cppDEL(pFont);
  return;
} // end of _saRedraw



//----------------------------------------------------------------------------
void mafViewIntGraphWindow::AdjustCurveAppearance(unsigned int nCurve)
//----------------------------------------------------------------------------
{
  wxColor cNewColor;

  cNewColor = ::wxGetColourFromUser(this, m_ColorTable[nCurve % m_ColorTableNumber]);
  if(cNewColor.Ok())
  {
    m_ColorTable[nCurve % m_ColorTableNumber] = cNewColor;
  }
  return;
}

//----------------------------------------------------------------------------
wxBitmap mafViewIntGraphWindow::GetBitmap()
//----------------------------------------------------------------------------
{
  int        nHeight, nWidth;
  wxMemoryDC compatDC;
  GetClientSize(&nWidth, &nHeight);
  wxBitmap   pBitMap(nWidth, nHeight);
  compatDC.SelectObject(pBitMap);
  DrawGraph(&compatDC);
  compatDC.SelectObject(wxNullBitmap);
  return pBitMap;
}

/*
 * handler for WM_COMMAND message
 * @memo    
 * @param   event   actual event received
 * @author  Earnol
 */
//----------------------------------------------------------------------------
void mafViewIntGraphWindow::OnCommand(wxCommandEvent& event)
//----------------------------------------------------------------------------
{
  if(m_Graphs.size() == 0 || m_Graphs[0] == NULL)
  {
    return;
  }
  if(event.GetId() == ID__RECALCRANGES)
  {
    m_RangeInit = false;
    UpdateRanges();
  }
  else if(event.GetId() == ID__GRIDONOFF)
  {
    m_RoughGrid = !m_RoughGrid;
  }
  else if(event.GetId() == ID__PRECGRIDONOFF)
  {
    m_PreciseGrid = !m_PreciseGrid;
  }
  else if(event.GetId() == ID__SAVEIMAGE)
  {
    wxString fileName = ::wxFileSelector("Select image file name", "", "image", ".bmp", "BMP files (*.bmp)|*.bmp|All files (*.*)|*.*", 
                                 wxSAVE | wxOVERWRITE_PROMPT | wxHIDE_READONLY, this);
    if(fileName != "")
    {
      wxBitmap pBitMap = GetBitmap();
      pBitMap.SaveFile(fileName, wxBITMAP_TYPE_BMP, NULL);
    }
  }
  else if(event.GetId() == ID__SAVECSV)
  {
    wxString CSVFileName = ::wxFileSelector("Select Excel CSV file name", "", "data", ".csv", "CSV files (*.csv)|*.csv|All files (*.*)|*.*", 
                                   wxSAVE | wxOVERWRITE_PROMPT | wxHIDE_READONLY, this);
    if(CSVFileName != "")
    {
      if(!SaveGraphAsCSV(CSVFileName))
      {
        wxMessageBox("Error: Data was not saved", "Error.", wxOK | wxCENTER | wxICON_HAND, this);
      }
    }
  }
  else if(event.GetId() == ID__ADJUSTCURVE)
  {
    AdjustCurveAppearance(m_pointed);
  }
  /*else 
  {
    if(event.GetId() < ID__FIRSTAUTOMENU)
    {
      //not our event go away
      return;
    }
    m_Desc->ProcessMenu(m_Graphs[0], event.GetId() - ID__FIRSTAUTOMENU);
  }*/
  Refresh(false, NULL);
  return;
} // end of SaIntGraphWndDoCommandProc

//----------------------------------------------------------------------------
void mafViewIntGraphWindow::OnLeftMouseButtonDown(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  unsigned int nYNumber;
  unsigned int nGraphIndex, nID;
  unsigned int numVars = 0;
  for(unsigned i = 0; i < m_Graphs.size(); i++)
    numVars += m_Graphs[i]->GetYVarNum();

  if(numVars == 0)
    return;
  if(0 <= event.GetY() && event.GetY() <= SPACING_Y_COEFF * m_TitleFntHeight * (numVars - 0.5))
  {
    nYNumber = max(event.GetY() - (SPACING_Y_COEFF - 1.0)* m_TitleFntHeight * 0.5, 0) / (SPACING_Y_COEFF * m_TitleFntHeight);
    for(nGraphIndex = 0; nGraphIndex < m_Graphs.size(); nGraphIndex++)
    {
      if(nYNumber < m_Graphs[nGraphIndex]->GetYVarNum())
        break;
      nYNumber -= m_Graphs[nGraphIndex]->GetYVarNum();
    }
    nID      = m_Graphs[nGraphIndex]->GetIndexY(nYNumber);
  }
  return;
} // end of mafViewIntGraphWindow::OnLeftMouseButtonDown

/*
 * handler for WM_RBUTTONDOWN message
 *
 * @memo    
 * @param   hWnd    standart
 * @param   nMsg    windows
 * @param   wParam  params
 * @param   lParam  and
 * @return          return value for this message
 * @author  Earnol
 */
//----------------------------------------------------------------------------
void mafViewIntGraphWindow::OnRightMouseButtonDown(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  wxMenu       *pPopupMenu;//, *pManageMenu;
  wxPoint      pt;
  char         *sCaption;

  pt = event.GetPosition();
  pPopupMenu = new wxMenu();
  sCaption = (char*)malloc((MAXTEXTLEN + 1) * sizeof(char));
  /*pManageMenu = m_Desc->GenerateMenu(m_Graphs[0], ID__FIRSTAUTOMENU);
  if(pManageMenu)
    pPopupMenu->Append(0, "Manage variables", pManageMenu);*/
  pPopupMenu->Append(ID__RECALCRANGES,"Update axes\' ranges");
  if(m_RoughGrid)
  {
    pPopupMenu->Append(ID__GRIDONOFF,"Turn grid off");
  }
  else
  {
    pPopupMenu->Append(ID__GRIDONOFF,"Turn grid on");
  }
  if(m_PreciseGrid)
  {
    pPopupMenu->Append(ID__PRECGRIDONOFF,"Turn small grid off");
  }
  else
  {
    pPopupMenu->Append(ID__PRECGRIDONOFF,"Turn small grid on");
  }
  pPopupMenu->Append(ID__SAVEIMAGE, "Save image");
  pPopupMenu->Append(ID__SAVECSV,   "Save Excel csv");

  unsigned NumGr = 0;
  for(unsigned i = 0; i < m_Graphs.size(); i++)
    NumGr += m_Graphs[i]->GetYVarNum();
  if(0 <= event.GetY() && event.GetY() <= SPACING_Y_COEFF * m_TitleFntHeight * (NumGr - 0.5))
  {
    m_pointed = max(event.GetY() - (SPACING_Y_COEFF - 1.0)* m_TitleFntHeight * 0.5, 0) / (SPACING_Y_COEFF * m_TitleFntHeight);
    pPopupMenu->Append(ID__ADJUSTCURVE, "Adjust curve");
  }
  free(sCaption);
  PopupMenu(pPopupMenu, pt);
  cppDEL(pPopupMenu);
  return;
} // end of mafViewIntGraphWindow::OnRightMouseButtonDown

/*
 * handler for WM_PAINT message
 *
 * @memo    
 * @param   event   paint event
 * @return          return value for this message
 * @author  Earnol
 * @see     Nothing
 */
//----------------------------------------------------------------------------
void mafViewIntGraphWindow::OnCloseWindow(wxCloseEvent& event)
//----------------------------------------------------------------------------
{ 
  Hide();
  Destroy();
}

/*
 * handler for WM_PAINT message
 *
 * @memo    
 * @param   event   paint event
 * @return          return value for this message
 * @author  Earnol
 * @see     Nothing
 */
//----------------------------------------------------------------------------
void mafViewIntGraphWindow::OnPaint(wxPaintEvent &event)
//----------------------------------------------------------------------------
{
  wxPaintDC pDC(this);
  int   nHeight, nWidth;
  wxMemoryDC compatDC;

  if(this == NULL)
  {
    //in case of weird destroys
    return;
  }
  GetClientSize(&nWidth, &nHeight);
  wxBitmap   pBitMap(nWidth, nHeight);
  compatDC.SelectObject(pBitMap);  
  DrawGraph(&compatDC);  
  pDC.Blit(0, 0, nWidth, nHeight, &compatDC,0,0, wxCOPY);
  compatDC.SelectObject(wxNullBitmap);
  return;
} // end of SaIntGraphWndDoPaintProc

/*
 * handler for WM_SIZE message
 *
 * @memo    
 * @param   hWnd    standart
 * @param   nMsg    windows
 * @param   wParam  params
 * @param   lParam  and
 * @return          return value for this message
 * @author  Earnol
 * @see     Nothing
 */
//----------------------------------------------------------------------------
void mafViewIntGraphWindow::OnSize(wxSizeEvent &event)
//----------------------------------------------------------------------------
{
  Refresh(false, NULL);

  return;
} // end of mafViewIntGraphWindow::OnSize


/*
 * handler for IGWM_SETGRAPH message
 *
 * @memo    
 * @param   hWnd    handle to window that received this message
 * @param   nMsg    message code
 * @param   wParam  params
 * @param   lParam  and
 * @return          return value for this message
 * @author  Earnol
 * @see     Nothing
 */
//----------------------------------------------------------------------------
void  mafViewIntGraphWindow::AddGraphData(const mafGraphData *pGraphData)
//----------------------------------------------------------------------------
{
  m_Graphs.push_back(pGraphData);
  Refresh(false, NULL);
  return;
} // end of SaIntGraphWndDoSetGraphProc

/*
* handler for IGWM_SETGRAPH message
*
* @memo    
* @param   hWnd    handle to window that received this message
* @param   nMsg    message code
* @param   wParam  params
* @param   lParam  and
* @return          return value for this message
* @author  Earnol
* @see     Nothing
*/
//----------------------------------------------------------------------------
void  mafViewIntGraphWindow::SetGraphData(int idx, const mafGraphData *pGraphData)
//----------------------------------------------------------------------------
{
  int sz = m_Graphs.size();
  if(sz < idx + 1)
  {
    m_Graphs.resize(idx + 1);
    for(int i = sz; i < idx + 1; i++)
      m_Graphs[i] = NULL;
  }
  m_Graphs[idx] = pGraphData;
  Refresh(false, NULL);
  return;
} // end of SaIntGraphWndDoSetGraphProc
/*
* handler for IGWM_SETGRAPH message
*
* @memo    
* @param   hWnd    handle to window that received this message
* @param   nMsg    message code
* @param   wParam  params
* @param   lParam  and
* @return          return value for this message
* @author  Earnol
* @see     Nothing
*/
//----------------------------------------------------------------------------
void  mafViewIntGraphWindow::RemGraphData(const mafGraphData *pGraphData)
//----------------------------------------------------------------------------
{
  for(std::vector<const mafGraphData *>::iterator it = m_Graphs.begin(); it != m_Graphs.end(); ++it)
  {
    if((*it) == pGraphData)
    {
      m_Graphs.erase(it);
      break;
    }
  }
  Refresh(false, NULL);
  return;
} // end of SaIntGraphWndDoSetGraphProc

//----------------------------------------------------------------------------
void mafViewIntGraphWindow::CreateGui()
//----------------------------------------------------------------------------
{
  static mafString choices[] = {wxString("Default"),wxString("Decorative"), wxString("Roman"), wxString("Script"), wxString("Swiss"), wxString("Modern")};
  m_Gui = new mafGUI(this);

  m_Gui->Bool   (ID_SHOW_ROUGH_GRID  ,"Rough Grid"       , &(m_RoughGrid),0);
  m_Gui->Bool   (ID_SHOW_PRECISE_GRID,"Precise Grid"     , &(m_PreciseGrid),0);

  m_Gui->Integer(ID_TITLE_FONTSIZE , "Legend font size"  , &m_TitleFontSize, 3, 50); 
  m_Gui->Combo  (ID_TICK_FONTFAMILY, "Legend font family", (int *)&m_TitleFontFamily, mafFF_LAST, choices);

  m_Gui->Integer(ID_TITLE_FONTSIZE , "Tick font size"  , &m_TickFontSize, 3, 50); 
  m_Gui->Combo  (ID_TICK_FONTFAMILY, "Tick font family", (int *)&m_TickFontFamily, mafFF_LAST, choices);

  m_Gui->Slider (ID_THICK_CURVE    , "Curves thickness", &m_CurveThickness, 1, 5);
  m_Gui->Slider (ID_THICK_AXIS     , "Axis   thickness", &m_AxisThickness , 1, 5);
  m_Gui->Slider (ID_THICK_GRID     , "Grid   thickness", &m_GridThickness , 1, 5);
  m_Gui->Update();
}

//----------------------------------------------------------------------------
wxInt32 mafViewIntGraphWindow::GetFontFamily(mafViewIntGraphFontFamily fFamily)
//----------------------------------------------------------------------------
{
  wxInt32 nFamily = wxDEFAULT;
  switch(fFamily)
  {
    case mafFF_DEFAULT    :
    {
      nFamily = wxDEFAULT;
      break;
    }
    case mafFF_DECORATIVE:
    {
      nFamily = wxDECORATIVE;
      break;
    }
    case mafFF_ROMAN:
    {
      nFamily = wxROMAN;
      break;
    }
    case mafFF_SCRIPT:
    {
      nFamily = wxSCRIPT;
      break;
    }
    case mafFF_SWISS:
    {
      nFamily = wxSWISS;
      break;
    }
    case mafFF_MODERN:
    {
      nFamily = wxMODERN;
      break;
    }
    default:
    {
      //DIASSERT_ALOOF_CODE();
    }
  }
  return nFamily;
}

//----------------------------------------------------------------------------
void mafViewIntGraphWindow::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case ID_TITLE_FONTSIZE:
      case ID_TITLE_FONTFAMILY:
      case ID_TITLE_FONTSTYLE:
      case ID_TITLE_FONTWEIGHT:
      case ID_TICK_FONTSIZE:
      case ID_TICK_FONTFAMILY:
      case ID_TICK_FONTSTYLE:
      case ID_TICK_FONTWEIGHT:
      case ID_THICK_CURVE:
      case ID_THICK_AXIS:
      case ID_THICK_GRID:
      case ID_SHOW_ROUGH_GRID:
      case ID_SHOW_PRECISE_GRID:
        Update();
        break;
    }
  }
}
//----------------------------------------------------------------------------

/*
 * handler for IGWM_UPDATE message
 *
 * @memo    
 * @return          return value for this message
 * @author  Earnol
 * @see     Nothing
 */
//----------------------------------------------------------------------------
void  mafViewIntGraphWindow::Update(void)
//----------------------------------------------------------------------------
{
  Refresh(false, NULL);
} // end of SaIntGraphWndDoUpdateProc


/*
 * Save entire data from window in CSV form for Excel handling
 *
 * @memo    
 * @return false if any error happened and file not created
 * @author  Earnol
 * @see     Nothing
 */
//----------------------------------------------------------------------------
bool mafViewIntGraphWindow::SaveGraphAsCSV(wxString const &sFileName)
//----------------------------------------------------------------------------
{
  wxFile         *pFile = NULL;
  wxString       sStr("");  
  wxString       sWriteStr("");  
  wxChar const   *pSeparatorStr = GetListSeparator();
  wxChar const   *pDecStr = GetDecimalSeparator();

  // do nothing in case we do not need anything
  if(m_Graphs.size() == 0 || m_Graphs[0] == NULL)
    return false;

  pFile = new wxFile(sFileName, wxFile::write);
  if(pFile == NULL)
  {
    return (false);
  }
  if(!pFile->IsOpened())
  {
    cppDEL(pFile);
    return (false);
  }

  char nameBuffer[MAXTEXTLEN + 1];
  sWriteStr = "";
  if(m_XGraph)
  {
    m_XGraph->GetIDDesc(m_XGraph->GetIndexX(0), 0, nameBuffer, MAXTEXTLEN);
    sWriteStr += nameBuffer;
    sWriteStr += ";";
  }
  else if(m_Graphs.size() > 0)
  {
    m_Graphs[0]->GetIDDesc(0, 0, nameBuffer, MAXTEXTLEN);
    sWriteStr += nameBuffer;
    sWriteStr += ";";
  }

  for(unsigned i = 0; i < m_Graphs.size(); i++)
  {
    for(int j = 0; j < m_Graphs[i]->GetYVarNum(); j++)
    {
      m_Graphs[i]->GetIDDesc(m_Graphs[i]->GetIndexY(j), m_Graphs[i]->GetYDer(j), nameBuffer, 1000);
      sWriteStr += nameBuffer;
      sWriteStr += ";";
    }
  }

  int strln = sWriteStr.length();
  if(strln > 0)
    sWriteStr[strln - 1] = '\n';
  pFile->Write(sWriteStr);


  if(m_XGraph != NULL)
  {
    unsigned nStart  = m_XGraph->GetBreakEnd();
    unsigned nFinish = m_XGraph->GetSize();
    for(int count = 0; count < 2; count++)
    {
      for(int nI = nStart; nI < nFinish; nI++)
      {
        //save all X values
        double xValue = m_XGraph->GetValue(nI, m_XGraph->GetIndexX(0), m_XGraph->GetXDer(0));
        double paramt = m_XGraph->GetValue(nI, 0, 0);
        sWriteStr = "";
        sStr.Printf("%14.10f", xValue);
        sStr.Replace(".", pDecStr, true);
        sWriteStr += sStr;
        sWriteStr += pSeparatorStr;

        for(unsigned i = 0; i < m_Graphs.size(); i++)
        {
          for(int nJ = 0; nJ < m_Graphs[i]->GetYVarNum(); nJ++)
          {
            double yValue;
            unsigned int der = m_Graphs[i]->GetYDer(nJ);
            if(m_Graphs[i]->GetValueByParam(yValue, paramt, m_Graphs[i]->GetIndexY(nJ), der))
            {
              if(der == 1)
              {
                double derX;
                if(m_XGraph->GetValueByParam(derX, paramt, m_XGraph->GetIndexX(0), 1))
                {
                  derX *= m_XGraph->GetDerivCoef(m_XGraph->GetIndexX(0));
                  /*if(fabs(derX) < DERIV_ABS_MIN)
                  continue;*/
                  yValue /= derX;
                  sStr.Printf("%14.10f", yValue);
                  sStr.Replace(".", pDecStr, true);
                }
                else
                {
                  sStr.Printf("              ");
                }
              }
              else
              {
                sStr.Printf("%14.10f", yValue);
                sStr.Replace(".", pDecStr, true);
              }
            }
            else
            {
            }
            sWriteStr += sStr;
            sWriteStr += pSeparatorStr;
          }
        }
        //write to file
        strln = sWriteStr.length();
        if(strln > 0)
          sWriteStr[strln - 1] = '\n';
        pFile->Write(sWriteStr);
      }
      nStart = 0;
      nFinish = m_XGraph->GetBreakBegin();
    }
    pFile->Flush();
    pFile->Close();
    cppDEL(pFile);
    return true;
  }

  std::set<double> tts;
  for(unsigned i = 0; i < m_Graphs.size(); i++)
  {
    for(unsigned j = 0; j < m_Graphs[i]->GetSize(); j++)
    {
      tts.insert(m_Graphs[i]->GetValue(j , 0, 0));
    }
  }

  for (std::set<double>::iterator it = tts.begin(); it!=tts.end(); ++it)
  {
    sWriteStr = "";
    sStr.Printf("%14.10f", (*it));
    sStr.Replace(".", pDecStr, true);
    sWriteStr += sStr;
    sWriteStr += pSeparatorStr;
    for(unsigned i = 0; i < m_Graphs.size(); i++)
    {
      unsigned nStart  = m_Graphs[i]->GetBreakEnd();
      unsigned nFinish = m_Graphs[i]->GetSize();
      bool     exported = false;
      for(int count = 0; count < 2; count++)
      {
        for(int nI = nStart; nI < nFinish; nI++)
        {
          double ts = m_Graphs[i]->GetValue(nI, 0, 0);
          if(ts == (*it))
          {
            exported = true;
            for(int nJ = 0; nJ < m_Graphs[i]->GetYVarNum(); nJ++)
            {
              unsigned int der = m_Graphs[i]->GetYDer(nJ);
              double yValue;
              if(m_Graphs[i]->GetValueByParam(yValue, ts, m_Graphs[i]->GetIndexY(nJ), der))
              {
                //derX    = 1.0;
                //yValue /= 1.0;
                sStr.Printf("%14.10f", yValue);
                sStr.Replace(".", pDecStr, true);
                sWriteStr += sStr;
                sWriteStr += pSeparatorStr;
              }
              else
              {
                sStr.Printf("              ");
                sWriteStr += sStr;
                sWriteStr += pSeparatorStr;
              }
            }
            count = 2;
            break;
          }
        }
        nStart = 0;
        nFinish = m_Graphs[i]->GetBreakBegin();
      }
      if(!exported)
      {
        for(int nJ = 0; nJ < m_Graphs[i]->GetYVarNum(); nJ++)
        {
          sStr.Printf("              ");
          sWriteStr += sStr;
          sWriteStr += pSeparatorStr;
        }
      }
    }
    //write to file
    strln = sWriteStr.length();
    if(strln > 0)
      sWriteStr[strln - 1] = '\n';
    pFile->Write(sWriteStr);
  }
  pFile->Flush();
  pFile->Close();

  cppDEL(pFile);
  return true;
}

BEGIN_EVENT_TABLE(mafViewIntGraphWindow, wxWindow)
  EVT_PAINT      (mafViewIntGraphWindow::OnPaint)
  EVT_CLOSE      (mafViewIntGraphWindow::OnCloseWindow)
  EVT_RIGHT_DOWN (mafViewIntGraphWindow::OnRightMouseButtonDown)
  EVT_LEFT_DOWN  (mafViewIntGraphWindow::OnLeftMouseButtonDown)
  EVT_SIZE       (mafViewIntGraphWindow::OnSize)
  EVT_COMMAND_RANGE(0, 0xFFFF, wxEVT_COMMAND_MENU_SELECTED, mafViewIntGraphWindow::OnCommand)
END_EVENT_TABLE()

/**
 * Actual constructor
 *
 * @author  Earnol
 * @see     Nothing
 */
//----------------------------------------------------------------------------
mafViewIntGraphWindow::mafViewIntGraphWindow(const wxString& label, mafBaseEventHandler *listener):
wxWindow(mafGetFrame(), -1, wxDefaultPosition, wxDefaultSize, 0, label)
//----------------------------------------------------------------------------
{
  SetListener(listener);
  
  m_XGraph           = NULL;

  m_RoughGrid        = false;
  m_PreciseGrid      = false;
  m_pointed          = 0;

  m_BaseID           = 0;

  m_TitleFontSize    = 10;
  m_TitleFontFamily  = mafFF_DEFAULT; 
  m_TitleFontStyle   = wxNORMAL;
  m_TitleFontWeight  = wxLIGHT;

  m_TickFontSize     = 10;
  m_TickFontFamily   = mafFF_DEFAULT;
  m_TickFontStyle    = wxNORMAL;
  m_TickFontWeight   = wxLIGHT;

  m_CurveThickness   = 1;
  m_AxisThickness    = 1;
  m_GridThickness    = 1;

  m_ColorTable       = new wxColor[DIM(_GraphColors)];
  m_ColorTableNumber = DIM(_GraphColors);
  for(int i = 0; i < m_ColorTableNumber; i++)
  {
    m_ColorTable[i] = _GraphColors[i];
  }

  m_TitleFntHeight   = 20;

  m_RangeInit        = false;
  m_Gui              = NULL;
  CreateGui();
}


/*
 * Just a destructor
 *
 * @memo    
 * @return  Nothing
 * @author  Earnol
 * @see     Nothing
 */
//----------------------------------------------------------------------------
mafViewIntGraphWindow::~mafViewIntGraphWindow()
//----------------------------------------------------------------------------
{
  delete[] m_ColorTable;
  m_ColorTable       = NULL;
  m_ColorTableNumber = 0;
}


//#if !defined UNIX 
//#include <windows.h>
//#endif

//----------------------------------------------------------------------------
wxChar const *GetListSeparator()
//----------------------------------------------------------------------------
{
  static wxChar _caListSparator[2] = {',','\0'};

  #if !defined UNIX 

  HKEY  hKey;
  char  szListSeparator[2];
  DWORD dwBufLen = 2;
  LONG  lRet;

  lRet = RegOpenKeyEx( HKEY_CURRENT_USER, "Control Panel\\International", 0, KEY_QUERY_VALUE, &hKey);
  if(lRet != ERROR_SUCCESS)
     return _caListSparator;

  lRet = RegQueryValueEx(hKey, "Slist", NULL, NULL, (LPBYTE)szListSeparator, &dwBufLen);
  if((lRet != ERROR_SUCCESS) || (dwBufLen > 2) )
     return _caListSparator;

  RegCloseKey(hKey);
  strncpy(_caListSparator, szListSeparator, 2);
  #endif

  return _caListSparator;
}

//----------------------------------------------------------------------------
wxChar const *GetDecimalSeparator()
//----------------------------------------------------------------------------
{
  static wxChar _caDecSparator[2] = {'.','\0'};

  #if !defined UNIX 

  HKEY  hKey;
  char  szDecSeparator[2];
  DWORD dwBufLen = 2;
  LONG  lRet;

  lRet = RegOpenKeyEx( HKEY_CURRENT_USER, "Control Panel\\International", 0, KEY_QUERY_VALUE, &hKey);
  if(lRet != ERROR_SUCCESS)
     return _caDecSparator;

  lRet = RegQueryValueEx(hKey, "sDecimal", NULL, NULL, (LPBYTE)szDecSeparator, &dwBufLen);
  if((lRet != ERROR_SUCCESS) || (dwBufLen > 2) )
     return _caDecSparator;

  RegCloseKey(hKey);
  strncpy(_caDecSparator, szDecSeparator, 2);
  #endif

  return _caDecSparator;
}
