/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoINPExporter.cpp,v $
  Language:  C++
  Date:      $Date: 2007-08-22 14:01:40 $
  Version:   $Revision: 1.1 $
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

#include "mmoINPExporter.h"

#include "mafDecl.h"
#include "mmgGui.h"

#include "mafINPWriter.h"
#include "mafVME.h"
#include "mafVMEOutputSurface.h"
#include "mafTransformBase.h"

#include "vtkMAFSmartPointer.h"
#include "vtkTriangleFilter.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkTransform.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mmoINPExporter);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mmoINPExporter::mmoINPExporter(const wxString &label) : mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType        = OPTYPE_EXPORTER;
  m_Canundo       = true;
  m_File          = "";
  m_Binary        = 1;
  m_ABSMatrixFlag = 1;
  m_FileDir       = mafGetApplicationDirectory().c_str();
}
//----------------------------------------------------------------------------
mmoINPExporter::~mmoINPExporter()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
bool mmoINPExporter::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && ((mafVME *)node)->GetOutput()->IsMAFType(mafVMEOutputSurface));
}
//----------------------------------------------------------------------------
// constants
//----------------------------------------------------------------------------
enum STL_EXPORTER_ID
{
  ID_STL_BINARY_FILE = MINID,
  ID_ABS_MATRIX_TO_STL,
  ID_CHOOSE_FILENAME,
};
//----------------------------------------------------------------------------
void mmoINPExporter::OpRun()   
//----------------------------------------------------------------------------
{
  mafString wildc = "INP file (*.inp)|*.inp";

  m_Gui = new mmgGui(this);
  //m_Gui->FileSave(ID_CHOOSE_FILENAME,"stl file", &m_File, wildc,"Save As...");
  m_Gui->Label("file type",true);
  m_Gui->Bool(ID_STL_BINARY_FILE,"binary",&m_Binary,0);
  m_Gui->Label("absolute matrix",true);
  m_Gui->Bool(ID_ABS_MATRIX_TO_STL,"apply",&m_ABSMatrixFlag,0);
  m_Gui->OkCancel();
  //m_Gui->Enable(wxOK,m_File != "");

  m_Gui->Divider();

  ShowGui();
}
//----------------------------------------------------------------------------
void mmoINPExporter::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
    case wxOK:
      {
        mafString FileDir = mafGetApplicationDirectory().c_str();
        FileDir << "\\";
        FileDir << m_Input->GetName();
        FileDir << ".inp";
        mafString wildc = "INP (*.inp)|*.inp";
        m_File = mafGetSaveFile(FileDir.GetCStr(), wildc.GetCStr()).c_str();
        if(m_File!="")
        {
          ExportSurface();
          OpStop(OP_RUN_OK);
        }
        else
          OpStop(OP_RUN_CANCEL);
      }
      break;
    case ID_CHOOSE_FILENAME:
      m_Gui->Enable(wxOK,m_File != "");
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
/*void mmoINPExporter::OpStop(int result)
//----------------------------------------------------------------------------
{
  HideGui();
  mafEventMacro(mafEvent(this,result));        
}*/
//----------------------------------------------------------------------------
void mmoINPExporter::ExportSurface()
//----------------------------------------------------------------------------
{
  mafVMEOutputSurface *out_surface = mafVMEOutputSurface::SafeDownCast(((mafVME *)m_Input)->GetOutput());
  out_surface->Update();

  vtkMAFSmartPointer<vtkTriangleFilter>triangles;
  vtkMAFSmartPointer<vtkTransformPolyDataFilter> v_tpdf;
  triangles->SetInput(out_surface->GetSurfaceData());
  triangles->Update();

  v_tpdf->SetInput(triangles->GetOutput());
  v_tpdf->SetTransform(out_surface->GetAbsTransform()->GetVTKTransform());
  v_tpdf->Update();

  vtkMAFSmartPointer<mafINPWriter> writer;
  mafEventMacro(mafEvent(this,BIND_TO_PROGRESSBAR,writer));
  writer->SetFileName(m_File.GetCStr());
  if(this->m_ABSMatrixFlag)
    writer->SetInput(v_tpdf->GetOutput());
  else
    writer->SetInput(triangles->GetOutput());
  if(this->m_Binary)
    writer->SetFileTypeToBinary();
  else
    writer->SetFileTypeToASCII();
  writer->Update();
}
//----------------------------------------------------------------------------
mafOp* mmoINPExporter::Copy()   
//----------------------------------------------------------------------------
{
  mmoINPExporter *cp = new mmoINPExporter(m_Label);
  cp->m_File = m_File;
  return cp;
}
