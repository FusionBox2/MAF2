/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpExporterOBJ.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 12:19:11 $
  Version:   $Revision: 1.3 $
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

#include "mafOpExporterOBJ.h"

#include "mafDecl.h"
#include "mafGUI.h"

#include "vtkOBJWriter.h"
#include "mafVME.h"
#include "mafVMEOutputSurface.h"
#include "mafTransformBase.h"

#include "vtkSmartPointer.h"
#include "vtkTriangleFilter.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkTransform.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpExporterOBJ);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpExporterOBJ::mafOpExporterOBJ(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType        = OPTYPE_EXPORTER;
  m_Canundo       = true;
  m_File          = _R("");
  m_ABSMatrixFlag = 1;
  m_FileDir       = _R("");
}
//----------------------------------------------------------------------------
mafOpExporterOBJ::~mafOpExporterOBJ()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
bool mafOpExporterOBJ::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node != NULL);
}
//----------------------------------------------------------------------------
// constants
//----------------------------------------------------------------------------
enum OBJ_EXPORTER_ID
{
  ID_ABS_MATRIX_TO_OBJ = MINID,
  ID_CHOOSE_FILENAME,
};
//----------------------------------------------------------------------------
void mafOpExporterOBJ::OpRun()   
//----------------------------------------------------------------------------
{
  mafString wildc = _R("OBJ file (*.obj)|*.obj");

  m_Gui = new mafGUI(this);
  m_Gui->Label(_R("absolute matrix"),true);
  m_Gui->Bool(ID_ABS_MATRIX_TO_OBJ,_R("apply"),&m_ABSMatrixFlag,0);
  m_Gui->OkCancel();
  m_Gui->Divider();

  ShowGui();
}
//----------------------------------------------------------------------------
void mafOpExporterOBJ::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
    case wxOK:
      {
        if(mafVME::SafeDownCast(m_Input)->GetOutput()->IsMAFType(mafVMEOutputSurface))
        {
          mafString FileDir = mafGetApplicationDirectory();
          FileDir += _R("\\");
          FileDir += m_Input->GetName();
          FileDir += _R(".obj");
          mafString wildc = _R("OBJ (*.obj)|*.obj");
          m_File = mafGetSaveFile(FileDir, wildc);
          if(!m_File.IsEmpty())
          {
            ExportSurface();
            OpStop(OP_RUN_OK);
          }
          else
            OpStop(OP_RUN_CANCEL);
        }
        else
        {
          mafString f = mafGetDirName(mafGetApplicationDirectory());

          if(!f.IsEmpty()) 
          {
            m_FileDir = f;
            ExportSurface();
            OpStop(OP_RUN_OK);
          }
          else
            OpStop(OP_RUN_CANCEL);
        }
      }
      break;
    case ID_CHOOSE_FILENAME:
      m_Gui->Enable(wxOK,!m_File.IsEmpty());
      break;
    case wxCANCEL:
      OpStop(OP_RUN_CANCEL);
      break;
    default:
      e->Log();
      break;
    }
  }
}
//----------------------------------------------------------------------------
/*void mafOpExporterOBJ::OpStop(int result)
//----------------------------------------------------------------------------
{
  HideGui();
  mafEventMacro(mafEvent(this,result));        
}*/



void mafOpExporterOBJ::ExportOneSurface(const char *filename, mafVMEOutputSurface* surf)
{
  mafVMEOutputSurface *out_surface = surf;
  out_surface->Update();

  vtkMAFSmartPointer<vtkTriangleFilter>triangles;
  vtkMAFSmartPointer<vtkTransformPolyDataFilter> v_tpdf;
  triangles->SetInputConnection(out_surface->GetVTKOutputPort());
  triangles->Update();

  v_tpdf->SetInputConnection(triangles->GetOutputPort());
  v_tpdf->SetTransform(out_surface->GetAbsTransform()->GetVTKTransform());
  v_tpdf->Update();

  vtkMAFSmartPointer<vtkOBJWriter> writer;
  mafEventMacro(mafEvent(this,BIND_TO_PROGRESSBAR,writer));
  writer->SetFileName(filename);
  if(this->m_ABSMatrixFlag)
    writer->SetInputConnection(v_tpdf->GetOutputPort());
  else
    writer->SetInputConnection(triangles->GetOutputPort());
  writer->Update();
}

void mafOpExporterOBJ::ExportingTraverse(const char *dirName, mafNode* node)
{
  if(mafVME::SafeDownCast(node)->GetOutput()->IsMAFType(mafVMEOutputSurface))
  {
    mafString fn = _R(dirName);
    fn += _R("\\");
    fn += node->GetName();
    fn += _R(".inp");
    ExportOneSurface(fn.GetCStr(), mafVMEOutputSurface::SafeDownCast(mafVME::SafeDownCast(node)->GetOutput()));
  }
  int numberChildren = node->GetNumberOfChildren();
  mafString fn = _R(dirName);
  fn += _R("\\");
  fn += node->GetName();
  if(numberChildren > 0)
    mafDirMake(fn);
  for (int i= 0; i< numberChildren; i++)
  {
    mafNode *child = node->GetChild(i);
    ExportingTraverse(fn.GetCStr(), child);
  }
}
//----------------------------------------------------------------------------
void mafOpExporterOBJ::ExportSurface()
//----------------------------------------------------------------------------
{
  if(mafVME::SafeDownCast(m_Input)->GetOutput()->IsMAFType(mafVMEOutputSurface))
  {
    ExportOneSurface(m_File.GetCStr(), mafVMEOutputSurface::SafeDownCast(mafVME::SafeDownCast(m_Input)->GetOutput()));
  }
  else
  {
    ExportingTraverse(m_FileDir.GetCStr(), m_Input);
  }
}
//----------------------------------------------------------------------------
mafOp* mafOpExporterOBJ::Copy()   
//----------------------------------------------------------------------------
{
  mafOpExporterOBJ *cp = new mafOpExporterOBJ(GetLabel());
  cp->m_File = m_File;
  return cp;
}
