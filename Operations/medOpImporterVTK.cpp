/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpImporterVTK.cpp,v $
Language:  C++
Date:      $Date: 2010-12-21 14:37:00 $
Version:   $Revision: 1.3.2.1 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)

MafMedical Library use license agreement

The software named MafMedical Library and any accompanying documentation, 
manuals or data (hereafter collectively "SOFTWARE") is property of the SCS s.r.l.
This is an open-source copyright as follows:
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution.
* Modified source versions must be plainly marked fas such, and must not be misrepresented 
as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

MafMedical is partially based on OpenMAF.
=========================================================================*/

#include "medDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medOpImporterVTK.h"
#include <wx/busyinfo.h>

#include "mafVME.h"
#include "medVMEPolylineGraph.h"

#include "mafTagArray.h"

#include "vtkDataSet.h"
#include "vtkDataSetReader.h"
#include "vtkPolyDataReader.h"
#include "vtkMAFSmartPointer.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(medOpImporterVTK);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medOpImporterVTK::medOpImporterVTK(const wxString &label) :
mafOpImporterVTK(label)
//----------------------------------------------------------------------------
{
  m_VmePolyLine = NULL;
}
//----------------------------------------------------------------------------
medOpImporterVTK::~medOpImporterVTK()
//----------------------------------------------------------------------------
{
  mafDEL(m_VmePolyLine);
}
//----------------------------------------------------------------------------
mafOp* medOpImporterVTK::Copy()   
//----------------------------------------------------------------------------
{
  medOpImporterVTK *cp = new medOpImporterVTK(m_Label);
  cp->m_File			= m_File;
  return cp;
}
//----------------------------------------------------------------------------
int medOpImporterVTK::ImportVTK()
//----------------------------------------------------------------------------
{
  bool success = false;
  if(!this->m_TestMode)
    wxBusyInfo wait(_("Loading file: ..."));

  vtkMAFSmartPointer<vtkDataSetReader> reader;
  reader->SetFileName(m_File);

  int canRead=TRUE;
  vtkDataReader *preader = NULL;
  // workaround to avoid double reading
  switch (reader->ReadOutputType())
  {
  case VTK_POLY_DATA:
    preader = vtkPolyDataReader::New();
    break;
  default:
    return mafOpImporterVTK::ImportVTK();
  }
    
  mafEventMacro(mafEvent(this,BIND_TO_PROGRESSBAR,preader));
  preader->SetFileName(m_File);
  preader->Update();

  if (preader->GetNumberOfOutputs()>0)
  {
    wxString path, name, ext;
    wxSplitPath(m_File.c_str(),&path,&name,&ext);

    vtkDataSet *data = vtkDataSet::SafeDownCast(preader->GetOutputs()[0]);
    if (data)
    {
      mafNEW(m_VmePolyLine);
      if (m_VmePolyLine->SetDataByDetaching(data,0) == MAF_OK)
      {
        m_Output = m_VmePolyLine;
      }
      else
      {
        vtkDEL(preader);
        return mafOpImporterVTK::ImportVTK();
      }

      mafTagItem tag_Nature;
      tag_Nature.SetName("VME_NATURE");
      tag_Nature.SetValue("NATURAL");
      m_Output->GetTagArray()->SetTag(tag_Nature);
      m_Output->SetName(name.c_str());

      success = true;
    }
  }
  vtkDEL(preader);
  if(!success && !this->m_TestMode)
    mafMessage(_("Error reading VTK file."), _("I/O Error"), wxICON_ERROR );

  return MAF_OK;
}
