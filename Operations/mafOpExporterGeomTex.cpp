/*=========================================================================

Program: MAF2
Module: mafOpExporterGeomTex


Copyright (c) B3C
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafOpExporterGeomTex.h"

#include "mafDecl.h"
#include "mafGUI.h"

#include "mafVME.h"
#include "mafVMEOutputSurface.h"
#include "mafTransformBase.h"

#include "vtkMAFSmartPointer.h"
#include "vtkSTLWriter.h"
#include "vtkTriangleFilter.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkTransform.h"
#include "vtkBMPWriter.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpExporterGeomTex);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpExporterGeomTex::mafOpExporterGeomTex(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
	m_OpType = OPTYPE_EXPORTER;
	m_Canundo = true;
	m_File = _R("");

	m_Binary = 1;
	m_ABSMatrixFlag = 1;

	m_FileDir = _R("");//mafGetApplicationDirectory();
}
//----------------------------------------------------------------------------
mafOpExporterGeomTex::~mafOpExporterGeomTex()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
bool mafOpExporterGeomTex::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
	return (node && ((mafVME *)node)->GetOutput()->IsMAFType(mafVMEOutputSurface));
}
//----------------------------------------------------------------------------
// constants
//----------------------------------------------------------------------------
enum GeomTex_EXPORTER_ID
{
	ID_GeomTex_BINARY_FILE = MINID,
	ID_ABS_MATRIX_TO_GeomTex,
	ID_CHOOSE_FILENAME,
};
//----------------------------------------------------------------------------
void mafOpExporterGeomTex::OpRun()
//----------------------------------------------------------------------------
{
	mafString wildc = _R("Stereo Litography (*.stl)|*.stl");

	m_Gui = new mafGUI(this);
	m_Gui->FileSave(ID_CHOOSE_FILENAME, _R("stl file"), &m_File, wildc, _R("Save As..."));
	m_Gui->Label(_R("file type"), true);
	m_Gui->Bool(ID_GeomTex_BINARY_FILE, _R("binary"), &m_Binary, 0);
	m_Gui->Label(_R("absolute matrix"), true);
	m_Gui->Bool(ID_ABS_MATRIX_TO_GeomTex, _R("apply"), &m_ABSMatrixFlag, 0);
	m_Gui->OkCancel();
	m_Gui->Enable(wxOK, !m_File.IsEmpty());

	m_Gui->Divider();

	ShowGui();
}
//----------------------------------------------------------------------------
void mafOpExporterGeomTex::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		switch (e->GetId())
		{
		case wxOK:
			/*{
			//mafString FileDir = mafGetApplicationDirectory().c_str();
			//FileDir<<"\\";
			mafString name = m_Input->GetName();
			if (name.FindChr('\\') != -1 || name.FindChr('/') != -1 || name.FindChr(':') != -1 ||
			name.FindChr('?')  != -1 || name.FindChr('"') != -1 || name.FindChr('<') != -1 ||
			name.FindChr('>')  != -1 || name.FindChr('|') != -1 )
			{
			mafMessage("Node name contains invalid chars.\nA node name can not contain chars like \\ / : * ? \" < > |");
			m_File = "";
			}
			else
			{
			m_FileDir << this->m_Input->GetName();
			m_FileDir << ".stl";
			mafString wildc = "STL (*.stl)|*.stl";
			m_File = mafGetSaveFile(m_FileDir.GetCStr(), wildc.GetCStr());
			}

			if(m_File.IsEmpty())
			{
			OpStop(OP_RUN_CANCEL);
			}
			else
			{
			ExportSurface();
			OpStop(OP_RUN_OK);
			}
			}*/
			ExportSurface();
			ExportTexture();
			OpStop(OP_RUN_OK);
			break;
		case ID_CHOOSE_FILENAME:
			m_Gui->Enable(wxOK, !m_File.IsEmpty());
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
void mafOpExporterGeomTex::OpStop(int result)
//----------------------------------------------------------------------------
{
	HideGui();
	mafEventMacro(mafEvent(this, result));
}
//----------------------------------------------------------------------------
void mafOpExporterGeomTex::ExportTexture()
//----------------------------------------------------------------------------
{
	
	mafVMEOutputSurface *out_surface = mafVMEOutputSurface::SafeDownCast(((mafVME *)m_Input)->GetOutput());
	out_surface->Update();
	
	vtkMAFSmartPointer<vtkBMPWriter> exporter;
	
	exporter->SetInput(out_surface->GetTexture());
	exporter->SetFileName("C:\\texture.bmp") ;
	exporter->Write();
	exporter->Delete();
}

void mafOpExporterGeomTex::ExportSurface()
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

	vtkMAFSmartPointer<vtkSTLWriter> writer;
	mafEventMacro(mafEvent(this, BIND_TO_PROGRESSBAR, writer));
	writer->SetFileName(m_File.GetCStr());
	if (this->m_ABSMatrixFlag)
		writer->SetInput(v_tpdf->GetOutput());
	else
		writer->SetInput(triangles->GetOutput());
	if (this->m_Binary)
		writer->SetFileTypeToBinary();
	else
		writer->SetFileTypeToASCII();
	writer->Update();
}
//----------------------------------------------------------------------------
mafOp* mafOpExporterGeomTex::Copy()
//----------------------------------------------------------------------------
{
	mafOpExporterGeomTex *cp = new mafOpExporterGeomTex(GetLabel());
	cp->m_File = m_File;
	return cp;
}