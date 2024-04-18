/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpExporterCSVGraph.cpp,v $
  Language:  C++
  Date:      $Date: 2009/05/19 14:29:53 $
  Version:   $Revision: 1.1.1.1 $
  Authors:   Matteo Giacomoni
==========================================================================
  Copyright (c) 2009
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "lhpOpExporterCSVGraph.h"

#include "wx/busyinfo.h"

#include "mafDecl.h"
#include "mafGUI.h"

#include "mmuTimeSet.h"
#include "mafSmartPointer.h"
#include "mafVME.h"
#include "vtkSmartPointer.h"
#include "mafVMEGroup.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mafVMESurface.h"
#include "mafVMEVector.h"
#include "medVMEAnalog.h"
#include "mafVMEOutputScalarMatrix.h"
#include "mafTagArray.h"
#include "mafNodeIterator.h"

#include <vtkCubeSource.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTransform.h>
#include "vtkSmartPointer.h"
#include "vtkCellArray.h"
#include <vtkPoints.h>
#include <vtkPolyData.h>

//#include "C3D_Reader.h"

#include <fstream>
#include <string>
#include <vnl\vnl_matrix.h>

#include <iostream>
#include <fstream>

//----------------------------------------------------------------------------
mafCxxTypeMacro(lhpOpExporterCSVGraph);
//----------------------------------------------------------------------------
mafViewIntGraph          *lhpOpExporterCSVGraph::m_ViewIntGraph;

//----------------------------------------------------------------------------
lhpOpExporterCSVGraph::lhpOpExporterCSVGraph(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType = OPTYPE_EXPORTER;
  m_Canundo = true;
  m_File = _R("");
  m_FileDir = _R("");
  m_Input   = NULL;
  m_GlobalPos = true;
  m_Subtree   = false;
}
//----------------------------------------------------------------------------
lhpOpExporterCSVGraph::~lhpOpExporterCSVGraph()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
bool lhpOpExporterCSVGraph::Accept(mafNode *node)   
//----------------------------------------------------------------------------
{ 
  return (node != NULL);
}
enum C3D_EXPORTER_ID
{
  ID_LOAD_PSC_SCRIPT = MINID,
};
//----------------------------------------------------------------------------
void lhpOpExporterCSVGraph::OpRun()   
//----------------------------------------------------------------------------
{
  m_Gui = new mafGUI(this);
  m_Gui->FileOpen(ID_LOAD_PSC_SCRIPT, _R("Script"), &m_PscScriptFileName, _R("*.psc"));
  m_Gui->OkCancel();
  m_Gui->Enable(wxOK, false);
  m_Gui->Enable(wxCANCEL, true);
  ShowGui();
}

namespace
{
  struct Line
  {
    std::string data;
    operator std::string const&() const { return data; }
  };
  std::istream& operator>>(std::istream& s, Line& dst)
  {
    return std::getline(s, dst.data);
  }
}

//----------------------------------------------------------------------------
void lhpOpExporterCSVGraph::LoadScript()
{
  m_shown_flags.clear();
  m_pipe_config.clear();
  std::ifstream ifs(m_PscScriptFileName.GetCStr());
  Line s;
  ifs >> s;
  std::istringstream iss(s);
  std::copy(std::istream_iterator<bool>(iss), std::istream_iterator<bool>(), std::back_inserter(m_shown_flags));
  std::copy(std::istream_iterator<Line>(ifs), std::istream_iterator<Line>(), std::back_inserter(m_pipe_config));
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void lhpOpExporterCSVGraph::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafViewIntGraph::SafeDownCast((mafObject*)maf_event->GetSender()))
    return;
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
    case ID_LOAD_PSC_SCRIPT:
    {
      LoadScript();
      m_Gui->Enable(wxOK, !m_PscScriptFileName.IsEmpty());
      break;
    }
    case wxOK:
      {
        int result = OP_RUN_CANCEL;
        m_Gui->Enable(wxOK, false);
        m_Gui->Enable(wxCANCEL, false);

        assert(m_Input);
        mafString proposed = mafGetApplicationDirectory() + _R("/Data/External/");
        proposed += m_Input->GetName();
        proposed += _R(".csv");
        mafString wildc = _R("csv file (*.csv)|*.csv");

        mafString f = mafGetSaveFile(proposed, wildc);
        if (!f.IsEmpty())
        {
          m_File = f;
          ExportGraphs();
          result = OP_RUN_OK;
        }
        OpStop(result);
    }
      break;
    case wxCANCEL:
      OpStop(OP_RUN_CANCEL);
      break;
    default:
      Superclass::OnEvent(maf_event);
      break;
    }
  }
}


//----------------------------------------------------------------------------
void lhpOpExporterCSVGraph::ExportGraphs()
  //----------------------------------------------------------------------------
{
  wxBusyInfo *wait;
  if(!m_TestMode)
  {
    wait = new wxBusyInfo("Please wait, exporting...");
  }
  mafViewIntGraph *vgraph = (mafViewIntGraph *)m_ViewIntGraph->Copy(this);
  vgraph->m_shown_flags = m_shown_flags;
  vgraph->m_pipe_config = m_pipe_config;

  if (mafNode * root = m_Input->GetRoot())
  {
    mafNodeIterator *iter = root->NewIterator(); // iterate over inserted vme
    for (mafNode *vme = iter->GetFirstNode(); vme; vme = iter->GetNextNode())
      vgraph->VmeAdd(vme); // Add them in the specified view
    iter->Delete();
  }
  vgraph->VmeSelect(m_Input, true);
  vgraph->loadPlot(false);
  vgraph->GetRenderWindow()->SaveGraphAsCSV(m_File.toWx());
  if(!m_TestMode)
  {
    delete wait;
  }
}

//----------------------------------------------------------------------------
mafOp* lhpOpExporterCSVGraph::Copy()   
//----------------------------------------------------------------------------
{
  lhpOpExporterCSVGraph *cp = new lhpOpExporterCSVGraph(GetLabel());
  cp->m_Canundo      = m_Canundo;
  cp->m_OpType       = m_OpType;
  cp->SetListener(GetListener());
  cp->m_Next         = NULL;
  cp->m_File         = m_File;
  cp->m_Input        = m_Input;
  cp->m_FileDir      = m_FileDir;
  cp->m_GlobalPos    = m_GlobalPos;
  cp->m_Subtree      = m_Subtree;
  return cp;
}

