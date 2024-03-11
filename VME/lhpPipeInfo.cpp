/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpPipeInfo.cpp,v $
  Language:  C++
  Date:      $Date: 2008-04-14 12:03:18 $
  Version:   $Revision: 1.6 $
  Authors:   Fedor Moiseev
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

#include "lhpPipeInfo.h"
#include "mafDecl.h"
#include "lhpViewInfo.h"
#include "lhpVMEKMInfo.h"

mafCxxTypeMacro(lhpPipeInfo);

//----------------------------------------------------------------------------
lhpPipeInfo::lhpPipeInfo()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
lhpPipeInfo::~lhpPipeInfo()
//----------------------------------------------------------------------------
{
  if (m_Node)
  {
    m_Node->RemoveObserver(this);
  }
  lhpViewInfo *igraph = lhpViewInfo::SafeDownCast(m_View);
  /*if(igraph && igraph->GetRenderWindow())
  {
    igraph->GetRenderWindow()->RemInfo(m_Node);
  }*/
}
void lhpPipeInfo::FillPage()
{
  mafString htmlbuf;
  double values[54];
  for(int i = 0; i < 54; i++)
    values[i] = 0.f;
  if(lhpVMEKMInfo *kmi = lhpVMEKMInfo::SafeDownCast(m_Node))
  {
    for(int i = 0; i < 54; i++)
      values[i] = kmi->GetValue(i);
    if(kmi->GetName().FindFirst(_R("Hand-To-Head")) != -1 || kmi->GetName().FindFirst(_R("Hand-To-Mouth")) != -1 || kmi->GetName().FindFirst(_R("Hand-To-Back")) != -1)
    {
        htmlbuf = _R("<table border=\"1\">\
        <tr><th colspan=\"2\">") + m_Node->GetName() +
          mafString::Format(
        _R("</th></tr>\
        <tr><td colspan=\"2\">Right shoulder</th></tr>\
        <tr><td style=\"width:60%%;\">Flexion(+)/Extension(-)/Speed</td><td>%.0f / %.0f / %.0f</td></tr>\
        <tr><td style=\"width:60%%;\">Adduction(+)/Abduction(-)/Speed</td><td>%.0f / %.0f / %.0f</td></tr>\
        <tr><td style=\"width:60%%;\">Internal rotation(+)/External rotation(-)/Speed</td><td>%.0f / %.0f / %.0f</td></tr>\
        <tr><td colspan=\"2\">Left shoulder</th></tr>\
        <tr><td style=\"width:60%%;\">Flexion(+)/Extension(-)/Speed</td><td>%.0f / %.0f / %.0f</td></tr>\
        <tr><td style=\"width:60%%;\">Adduction(+)/Abduction(-)/Speed</td><td>%.0f / %.0f / %.0f</td></tr>\
        <tr><td style=\"width:60%%;\">Internal rotation(+)/External rotation(-)/Speed</td><td>%.0f / %.0f / %.0f</td></tr>\
        <tr><td colspan=\"2\">Ratio shoulder (right/left)</th></tr>\
        <tr><td style=\"width:60%%;\">Flexion(+)/Extension(-)/Speed</td><td>%.0f / %.0f / %.0f</td></tr>\
        <tr><td style=\"width:60%%;\">Adduction(+)/Abduction(-)/Speed</td><td>%.0f / %.0f / %.0f</td></tr>\
        <tr><td style=\"width:60%%;\">Internal rotation(+)/External rotation(-)/Speed</td><td>%.0f / %.0f / %.0f</td></tr>\
        <tr><td colspan=\"2\">Right elbow</th></tr>\
        <tr><td style=\"width:60%%;\">Flexion(+)/Extension(-)/Speed</td><td>%.0f / %.0f / %.0f</td></tr>\
        <tr><td style=\"width:60%%;\">Adduction(+)/Abduction(-)/Speed</td><td>%.0f / %.0f / %.0f</td></tr>\
        <tr><td style=\"width:60%%;\">Internal rotation(+)/External rotation(-)/Speed</td><td>%.0f / %.0f / %.0f</td></tr>\
        <tr><td colspan=\"2\">Left elbow</th></tr>\
        <tr><td style=\"width:60%%;\">Flexion(+)/Extension(-)/Speed</td><td>%.0f / %.0f / %.0f</td></tr>\
        <tr><td style=\"width:60%%;\">Adduction(+)/Abduction(-)/Speed</td><td>%.0f / %.0f / %.0f</td></tr>\
        <tr><td style=\"width:60%%;\">Internal rotation(+)/External rotation(-)/Speed</td><td>%.0f / %.0f / %.0f</td></tr>\
        <tr><td colspan=\"2\">Ratio elbow (right/left)</th></tr>\
        <tr><td style=\"width:60%%;\">Flexion(+)/Extension(-)/Speed</td><td>%.0f / %.0f / %.0f</td></tr>\
        <tr><td style=\"width:60%%;\">Adduction(+)/Abduction(-)/Speed</td><td>%.0f / %.0f / %.0f</td></tr>\
        <tr><td style=\"width:60%%;\">Internal rotation(+)/External rotation(-)/Speed</td><td>%.0f / %.0f / %.0f</td></tr>\
        </table>"), 
        values[0], values[1], values[2],
        values[3], values[4], values[5],
        values[6], values[7], values[8],
        values[9], values[10], values[11],
        values[12], values[13], values[14],
        values[15], values[16], values[17],
        values[18], values[19], values[20],
        values[21], values[22], values[23],
        values[24], values[25], values[26],
        values[27], values[28], values[29],
        values[30], values[31], values[32],
        values[33], values[34], values[35],
        values[36], values[37], values[38],
        values[39], values[40], values[41],
        values[42], values[43], values[44],
        values[45], values[46], values[47],
        values[48], values[49], values[50],
        values[51], values[52], values[53]
      );
    }
  }
  m_PageText = htmlbuf;
}

//----------------------------------------------------------------------------
void lhpPipeInfo::Create(mafNode *node, mafView *view)
//----------------------------------------------------------------------------
{
  Superclass::Create(node, view);
  lhpViewInfo *igraph = lhpViewInfo::SafeDownCast(m_View);
  /*if(igraph)
    igraph->GetRenderWindow()->AddInfo(m_Node);*/

  FillPage();
  m_Node->AddObserver(this);
}

void lhpPipeInfo::OnEvent(mafEventBase *maf_event)
{
  if(maf_event->GetSender() == m_Node && maf_event->GetId() == VME_MODIFIED)
  {
    FillPage();
    lhpViewInfo *igraph = lhpViewInfo::SafeDownCast(m_View);
    if(igraph)
      igraph->UpdatePage();
  }
  Superclass::OnEvent(maf_event);
}


