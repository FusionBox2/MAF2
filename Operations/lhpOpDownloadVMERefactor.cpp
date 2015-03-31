/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: lhpOpDownloadVMERefactor.cpp,v $
Language:  C++
Date:      $Date: 2009-04-10 13:50:21 $
Version:   $Revision: 1.1.2.4 $
Authors:   Daniele Giunchi, Stefano Perticoni, Roberto Mucci
==========================================================================
Copyright (c) 2002/2007
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
* Modified source versions must be plainly marked as such, and must not be misrepresented 
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

#include "mafDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------
#include "lhpBuilderDecl.h"

#include <wx/process.h>
#include <wx/dir.h>
#include <wx/file.h>
#include <wx/busyinfo.h>
#include <wx/zipstrm.h>
#include <wx/zstream.h>
#include <wx/sstream.h>
#include <wx/wfstream.h>
#include <wx/fs_zip.h>

#include "lhpOpDownloadVMERefactor.h"

#include "lhpUtils.h"
#include "mafGUI.h"
#include "lhpUser.h"
#include "mafNode.h"
#include "mafNodeManager.h"
#include "mafVMEGenericAbstract.h"
#include "mafOpImporterMSF.h"
#include "mafVMELandmarkCloud.h"
#include "medVMEWrappedMeter.h"
#include "mafTagArray.h"

#include "mafVMEItem.h"
#include "mafDataVector.h"
#include "mafVMEStorage.h"
#include "mafVMERoot.h"
#include "mafVMEGroup.h"
#include "mafEventIO.h"

#include "lhpFactoryTagHandler.h"
#include "vtkPolyData.h"

#include <string>
#include <istream>
#include <ostream>

//----------------------------------------------------------------------------
mafCxxTypeMacro(lhpOpDownloadVMERefactor);
//----------------------------------------------------------------------------

//static variables
long lhpOpDownloadVMERefactor::m_Pid = -1;
mafString lhpOpDownloadVMERefactor::m_CacheSubdir = "0";

/*enum lhpOpDownloadVME_ID
{
  ID_USE_DICOM_SUBDICTIONARY = MINID, 
};*/


//----------------------------------------------------------------------------
lhpOpDownloadVMERefactor::lhpOpDownloadVMERefactor(const mafString& label, int fromSandbox) : Superclass(label)
//----------------------------------------------------------------------------
{
	m_OpType  = OPTYPE_OP;
	m_Canundo = false;
  m_FillLinkVector = false;
  m_WholeMsfDownload = false;
  m_DebugMode = false;
  m_DerivedNodeVector.clear();
  m_LinkNodeVector.clear();
  m_DownloadedURIVector.clear();
  m_DownloadedNodeVector.clear();
  m_CheckURIVector.clear();
  m_Group = NULL;
  m_RootGroup = NULL;
  m_User = NULL;
  m_DownloadCounter = 0;
  m_FromSendbox = fromSandbox;
     

  m_PythonExe = "python.exe_UNDEFINED";
  m_PythonwExe = "pythonw.exe_UNDEFINED";  

  m_CurrentCache = m_CacheDir = (lhpUtils::lhpGetApplicationDirectory() + "\\VMEUploaderDownloaderRefactor\\UploadCache\\").c_str();
  m_IncomingDir = (lhpUtils::lhpGetApplicationDirectory() + "\\VMEUploaderDownloaderRefactor\\Incoming\\").c_str();

  m_VMEUploaderDownloaderDir  = (lhpUtils::lhpGetApplicationDirectory() + "\\VMEUploaderDownloaderRefactor\\").c_str();
  m_FileName = "";
  m_ServiceURL = "https://www.biomedtown.org/biomed_town/LHDL/users/repository/lhprepository2/";
  //m_ServiceURL = "http://devel.fec.cineca.it:12680/town/biomed_town/LHDL/users/repository/lhprepository2/";

  m_MsfDir = "";
  m_IncomingCompletePath = "";
  
  m_BasketListFileName = "ToDownload.txt" ;
  m_ConnectionConfigurationFileName = "vmeUploaderConnectionConfiguration.conf" ;
  m_BinaryRealName = "";
  m_URISRBFile = "";
  m_URISRBFileSize = "0";
  m_ProxyURL = "";
  m_ProxyPort = "0";
}

//----------------------------------------------------------------------------
lhpOpDownloadVMERefactor::~lhpOpDownloadVMERefactor()
//----------------------------------------------------------------------------
{
  m_DerivedNodeVector.clear();
  m_LinkNodeVector.clear();
  m_DownloadedURIVector.clear();
  m_DownloadedNodeVector.clear();
  m_CheckURIVector.clear();
  mafDEL(m_Group);
  mafDEL(m_RootGroup);
}

//----------------------------------------------------------------------------
bool lhpOpDownloadVMERefactor::Accept(mafNode* vme)
//----------------------------------------------------------------------------
{
  lhpUser *user = NULL;
  //Get User values
  mafEvent event;
  event.SetSender(this);
  event.SetId(ID_REQUEST_USER);
  mafEventMacro(event);
  if(event.GetMafObject() != NULL) //if proxy string contains something != ""
  {
    user = (lhpUser*)event.GetMafObject();
  }
  return (user != NULL && user->IsAuthenticated() && vme != NULL);

}

//----------------------------------------------------------------------------
mafOp* lhpOpDownloadVMERefactor::Copy()
//----------------------------------------------------------------------------
{
	/** return a copy of itself, needs to put it into the undo stack */
	return new lhpOpDownloadVMERefactor(GetLabel(), m_FromSendbox);
}

//----------------------------------------------------------------------------
void lhpOpDownloadVMERefactor::OpRun()
//----------------------------------------------------------------------------
{
    
  // get python interpreters
  mafEvent eventGetPythonExe;
  eventGetPythonExe.SetSender(this);
  eventGetPythonExe.SetId(ID_REQUEST_PYTHON_EXE_INTERPRETER);
  mafEventMacro(eventGetPythonExe);

  if(eventGetPythonExe.GetString())
  {
    m_PythonExe.Erase(0);
    m_PythonExe = eventGetPythonExe.GetString()->GetCStr();
    m_PythonExe.Append(" ");
  }

  mafEvent eventGetPythonwExe;
  eventGetPythonwExe.SetSender(this);
  eventGetPythonwExe.SetId(ID_REQUEST_PYTHONW_EXE_INTERPRETER);
  mafEventMacro(eventGetPythonwExe);

  if(eventGetPythonwExe.GetString())
  {
    m_PythonwExe.Erase(0);
    m_PythonwExe = eventGetPythonwExe.GetString()->GetCStr();
    m_PythonwExe.Append(" ");
  }
  
  //Get User values
  mafEvent eventGetUser;
  eventGetUser.SetSender(this);
  eventGetUser.SetId(ID_REQUEST_USER);
  mafEventMacro(eventGetUser);
  if(eventGetUser.GetMafObject() != NULL) //if proxy string contains something != ""
  {
    m_User = (lhpUser*)eventGetUser.GetMafObject();
  }

  //Get Proxy values
  if(m_User->GetProxyFlag())
  {
    m_ProxyURL = m_User->GetProxyHost();
    m_ProxyPort = m_User->GetProxyPort();
    // load the connection configuration file:
    this->SaveConnectionConfigurationFile(); 
  }
  else
  {
    wxString oldDir = wxGetCwd();
    if (m_DebugMode)
      mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
    wxSetWorkingDirectory(m_VMEUploaderDownloaderDir.GetCStr());
    if (m_DebugMode)
      mafLogMessage( _T("Now current working directory is: '%s' "), wxGetCwd().c_str() );

    //if file exists , delete it
    if(wxFileExists(m_ConnectionConfigurationFileName.GetCStr()))
    {
      wxRemoveFile(m_ConnectionConfigurationFileName.GetCStr());
    }
    wxSetWorkingDirectory(oldDir);
  }

  int result = OP_RUN_CANCEL;

  mafString DebugPath = m_VMEUploaderDownloaderDir;
  DebugPath.Append("\\Debug.py");
  if (wxFileExists(DebugPath.GetCStr()))
  {
    ifstream debugFile;
    debugFile.open(DebugPath.GetCStr());
    if (!debugFile) 
    {
      mafLogMessage("Unable to open Debug.py file");
    }

    std::string isDebug;
    debugFile >> isDebug;
    int pos = isDebug.find_last_of('=');
    isDebug = isDebug.substr(pos+1);
    if (!isDebug.compare("1") || !isDebug.compare("True"))
    {
      m_DebugMode = true;
    }
    debugFile.close();
  }


  bool upToDate = false;
  upToDate = this->IsLHPBuilderVersionUpToDate();

  if(upToDate)
  { 
    if(this->CreateFileListFromBasket() == MAF_OK)
    {
      result = OP_RUN_OK;
      mafEventMacro(mafEvent(this,result));
    }
    else
    {
      OpStop(result);
      return;
    }
  }
  else
  {
    OpStop(result);
    return;
  }

}
//------------------------------------------------------------
void lhpOpDownloadVMERefactor::SaveConnectionConfigurationFile()
//------------------------------------------------------------
{
  wxString oldDir = wxGetCwd();
  if (m_DebugMode)
    mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  wxSetWorkingDirectory(m_VMEUploaderDownloaderDir.GetCStr());
  if (m_DebugMode)
    mafLogMessage( _T("Now current working directory is: '%s' "), wxGetCwd().c_str() );

  //if file exists , delete it
  if(wxFileExists(m_ConnectionConfigurationFileName.GetCStr()))
  {
    wxRemoveFile(m_ConnectionConfigurationFileName.GetCStr());
  }

  // open auto tags file and try to handle tags using tags factory 
  ofstream configurationFile;
  configurationFile.open(m_ConnectionConfigurationFileName.GetCStr());
  if (!configurationFile) {
    wxString message = m_ConnectionConfigurationFileName.GetCStr();
    message.Append(" not found! Unable to write configuration connection file");
    mafLogMessage(message.c_str());
  }
  else
  {
    configurationFile << m_ProxyURL;   
    configurationFile << "\n";
    configurationFile << m_ProxyPort;
     
    wxString message = m_ConnectionConfigurationFileName.GetCStr();
    message.Append("Found connection configuration file: using connection parameters");
    message.Append("m_ProxyURL: ");
    message.Append(m_ProxyURL.GetCStr());
    message.Append("m_ProxyPort: ");
    message.Append(m_ProxyPort.GetCStr());

    if (m_DebugMode)
      mafLogMessage(message.c_str());

    configurationFile.close();
  }

  wxSetWorkingDirectory(oldDir);
  if (m_DebugMode)
    mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );

}
//----------------------------------------------------------------------------
void lhpOpDownloadVMERefactor::OnEvent(mafEventBase *maf_event) 
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
    case wxOK:
      {
        this->OpStop(OP_RUN_OK);
        return;
      }
      break;

    case wxCANCEL:
      {
        this->OpStop(OP_RUN_CANCEL);
        return;
      }
      break;

    default:
      mafEventMacro(*e);
      break;
    }	
  }
}
//----------------------------------------------------------------------------
void lhpOpDownloadVMERefactor::OpDo()   
//----------------------------------------------------------------------------
{
  if(RetrieveInformationFromBasketListFile() != MAF_OK)
  {
    wxMessageBox("Unable to read what are the choosen vme");
    return;
  }

  wxInfoFrame *wait;
  if(!m_TestMode)
  {
    wait = new wxInfoFrame(NULL, "Please wait, downloading VME");
    wait->SetWindowStyleFlag(wxSTAY_ON_TOP); //to keep wait message on top
    wait->Show(true);
    wait->Refresh();
    wait->Update();
  }

  //Download VME form the basket
  if (DownloadVME(m_BasketListURI, m_Group) != MAF_OK)
  {
    if(!m_TestMode)
    {
      delete wait;
    }
    return;
  }

  if (m_ListLinkURIInTree.size() != 0)
  {
    for (int i = 0; i < m_ListLinkURIInTree.size(); i++)
    {
      //Check if VME has been already downloaded
      for (int c = 0; c < m_DownloadedURIVector.size(); c++)
      {
        mafString VMEname = m_ListLinkURIInTree[i].c_str();
        if (m_DownloadedURIVector[c].Equals(VMEname))
        {
          //Fill vector of link, with node already downloaded
          m_LinkNodeVector.push_back(m_DownloadedNodeVector[c]);
        }
      }
    }
  }


 if (m_ListLinkURI.size() != 0 || m_ListLinkURIInTree.size() != 0)
  {
    m_FillLinkVector = true;

    //Download VME link
    if (DownloadVME(m_ListLinkURI, m_Group) != MAF_OK)
    {
      if(!m_TestMode)
      {
        delete wait;
      }
      return;
    }
    int counter = 0;
    for (int n = 0; n < m_DerivedNodeVector.size(); n++)
    {
      int subId = -1;
      mafString linkName;
      for (mafNode::mafLinksMap::iterator i = m_DerivedNodeVector[n]->GetLinks()->begin(); i != m_DerivedNodeVector[n]->GetLinks()->end(); i++)
      {
        linkName = i->first;
        if (m_LinkNodeVector[counter]->IsA("mafVMELandmarkCloud") && m_LinkNodeVector[counter]->GetNumberOfChildren() == 1)
        {
          //set subId to 0, because it is the first landmark of the cloud
          subId = 0;
          m_LinkNodeVector[counter] = m_LinkNodeVector[counter]->GetFirstChild();
          
          if (m_DerivedNodeVector[n]->IsA("medVMEWrappedMeter") && linkName == m_LinkNodeVector[counter]->GetName())
          {
            ((medVMEWrappedMeter *)m_DerivedNodeVector[n])->SetMeterLink(linkName.GetCStr(), m_LinkNodeVector[counter]); 
            ((medVMEWrappedMeter *)m_DerivedNodeVector[n])->AddMidPoint(m_LinkNodeVector[counter]->GetParent());
          }
          else
          {
            m_DerivedNodeVector[n]->SetLink(linkName.GetCStr(), m_LinkNodeVector[counter]->GetParent(), subId); 
          }
          counter++;
        }
        else
        {
         if (m_DerivedNodeVector[n]->IsA("medVMEWrappedMeter") && linkName == m_LinkNodeVector[counter]->GetName())
          {
            ((medVMEWrappedMeter *)m_DerivedNodeVector[n])->SetMeterLink(linkName.GetCStr(), m_LinkNodeVector[counter]); 
            ((medVMEWrappedMeter *)m_DerivedNodeVector[n])->AddMidPoint(m_LinkNodeVector[counter]);
          }
          else
          {
            m_DerivedNodeVector[n]->SetLink(linkName.GetCStr(), m_LinkNodeVector[counter]/*->GetParent()*/); 
          }
          counter++;
        }
      }
    }
  }
  //All VME successfuly downloaded!!
  DownloadCheck();
  if(!m_TestMode)
  {
    delete wait;
  }
}
//----------------------------------------------------------------------------
int lhpOpDownloadVMERefactor::DownloadTree(mafNode *node)   
//----------------------------------------------------------------------------
{
  int result = MAF_ERROR;
  m_ListChildURI = GetChildURI(node);

  int numChild = m_ListChildURI.size();

  //No child
  if (numChild == 0)
  {
    result = MAF_OK;
    return result;
  }
  //Download all the children of "node"
  if (node->IsA("mafVMERoot"))
  {
    node = m_RootGroup;
  }

  result = DownloadVME(m_ListChildURI, node);
  return result;
}
//----------------------------------------------------------------------------
int lhpOpDownloadVMERefactor::DownloadVME(wxArrayString listVME, mafNode *parentNode)   
//----------------------------------------------------------------------------
{
  mafString isLast = "false";

  for (int i = 0; i < listVME.size(); i++)
  {
    for (int c = 0; c < m_BasketListURI.size(); c++)
    {
      if (m_BasketListURI[c].CompareTo(listVME[i].c_str()) == 0)
        m_DownloadCounter +=1;
    }

      m_CheckURIVector.push_back(listVME[i].c_str()); 
      if(!CreateIncomingDirectory())
      {
        wxMessageBox("Unable to create Incoming Directory", wxMessageBoxCaptionStr, wxSTAY_ON_TOP | wxOK);
        DownloadCheck(true);
        return MAF_ERROR;
      }

      if(!CreateIncomingCache())
      {
        wxMessageBox("Unable to create a temporary cache, remember that msf must be saved locally", wxMessageBoxCaptionStr, wxSTAY_ON_TOP | wxOK);
        DownloadCheck(true);
        return MAF_ERROR;
      }
      if(DownloadSelectedXMLFromBasket(listVME[i]) != MAF_OK)
      {
        DownloadCheck(true);
        return MAF_ERROR;
      }

      //reconstruct msf
      if(ReconstructMSF(listVME[i]) != MAF_OK)
      {
        wxMessageBox("Unable to reconstruct msf", wxMessageBoxCaptionStr, wxSTAY_ON_TOP | wxOK);
        DownloadCheck(true);
        return MAF_ERROR;
      }

      wxString oldDir = wxGetCwd();
      if (m_DebugMode)
        mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
      wxSetWorkingDirectory(m_VMEUploaderDownloaderDir.GetCStr());
      if (m_DebugMode)
        mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
      wxBusyCursor wait;


      if (m_URISRBFile.Equals("NOT PRESENT"))
        m_URISRBFile = ".";

      mafSleep(3000);

      if ( ExistsRunningProcess() )
      {
        //PROCESS EXIST, ONLY CALL CLIENT
        wxString command2execute;
        command2execute = m_PythonwExe.GetCStr();
        // script for client
        m_FileName = "Client.py ";
        command2execute.Append(m_FileName.GetCStr());
        command2execute.Append("127.0.0.1 "); //server address (localhost)
        command2execute.Append("50000 "); //port address (50000)
        command2execute.Append(wxString::Format("DOWNLOAD ")); //Download command
        command2execute.Append(wxString::Format("%s ",m_URISRBFileSize)); //file size

        //workaround to understanding directory argument
        wxString directoryWorkAround = m_IncomingCompletePath;
        directoryWorkAround.Replace(" ", "?");
        command2execute.Append(wxString::Format("%s ",directoryWorkAround)); //cache directory
        command2execute.Append(wxString::Format("%s ",m_User->GetName())); //user
        command2execute.Append(wxString::Format("%s ",m_User->GetPwd())); //pwd
        command2execute.Append(wxString::Format("%s ",m_ServiceURL.GetCStr())); //dev repository
        command2execute.Append(wxString::Format("%s ",listVME[i].c_str())); //XML URI NAME
        command2execute.Append(wxString::Format("%s ",m_URISRBFile.GetCStr())); //SRB DATA NAME

        //Code to understand if is last VME
        if (!m_WholeMsfDownload )
        {
          bool check = false;
          if (m_BasketListURI.size() == 1 && !CheckIsRoot(listVME[i].c_str()) && !m_FillLinkVector)
            check = true;

          else if (parentNode != NULL && parentNode->Equals(m_Group) && (i+1 == listVME.size()))
            check = true;

          else if ((i+1 == listVME.size()) && m_ListLinkURI.IsEmpty() && !CheckIsRoot(listVME[i].c_str()))
            check = true;

          wxArrayString URI = CheckRemoteLink(listVME[i].c_str());
          if (check && URI.IsEmpty())
            isLast = "true";

        }
        else if (parentNode != NULL && parentNode->Equals(m_RootGroup) && (i+1 == listVME.size()) && m_DownloadCounter  == m_BasketListURI.size())
        {
          wxArrayString URI = CheckRemoteLink(listVME[i].c_str());
          if (URI.IsEmpty() && !CheckRemoteChild(listVME[i].c_str()))
            isLast = "true";
          else
          {
            bool alreadyDownloaded = false;
            isLast = "true";
            //Check if VME has been already downloaded
            for (int i = 0 ; i < URI.size(); i++)
            {
              alreadyDownloaded = false;
              for (int c = 0; c < m_DownloadedURIVector.size(); c++)
              {
                mafString VMEname = URI[i].c_str();
                if (m_DownloadedURIVector[c].Equals(VMEname))
                {
                  alreadyDownloaded = true;
                  break;
                }
              }
              if (alreadyDownloaded = false)
              {
                isLast = "false";
                break;
              }
            }
          }
          
        }
        else if (m_FillLinkVector && (i+1 == listVME.size()))
        {
          wxArrayString URI = CheckRemoteLink(listVME[i].c_str());
          if (URI.IsEmpty())
            isLast = "true";
          else
          {
            bool alreadyDownloaded = false;
            isLast = "true";
            //Check if VME has been already downloaded
            for (int i = 0 ; i < URI.size(); i++)
            {
              alreadyDownloaded = false;
              for (int c = 0; c < m_DownloadedURIVector.size(); c++)
              {
                mafString VMEname = URI[i].c_str();
                if (m_DownloadedURIVector[c].Equals(VMEname))
                {
                  alreadyDownloaded = true;
                  break;
                }
              }
              if (alreadyDownloaded = false)
              {
                isLast = "false";
                break;
              }
            }
          }
        }
        command2execute.Append(wxString::Format("%s",isLast.GetCStr())); //is last VME

        if (m_DebugMode)
          mafLogMessage( _T("Executing command: '%s'"), command2execute.c_str() );
        m_Pid = wxExecute(command2execute, wxEXEC_ASYNC);

      }
      else
      {
        //PROCESS NOT EXIST, CREATE SERVER AND CALL CLIENT
        wxString command2execute;
        if (m_DebugMode)
          command2execute = m_PythonExe.GetCStr();
        else
          command2execute = m_PythonwExe.GetCStr();
        m_FileName = "ThreadedClient.py ";
        command2execute.Append(m_FileName.GetCStr());
        command2execute.Append("50000");
        if (m_DebugMode)
          mafLogMessage( _T("Executing command: '%s'"), command2execute.c_str() );

        m_Pid = wxExecute(command2execute, wxEXEC_ASYNC);

        if (m_DebugMode)
          mafLogMessage(_T("ASYNC Command process '%s' terminated with exit code %d."),
          command2execute.c_str(), m_Pid);

        mafSleep(5000);

        command2execute.clear();
        command2execute = m_PythonwExe.GetCStr();
        m_FileName = "Client.py ";
        command2execute.Append(m_FileName.GetCStr());
        command2execute.Append("127.0.0.1 "); //server address (localhost)
        command2execute.Append("50000 "); //port address (50000)
        command2execute.Append(wxString::Format("DOWNLOAD ")); //Download command
        command2execute.Append(wxString::Format("%s ",m_URISRBFileSize)); //file size

        //workaround to understanding directory argument
        wxString directoryWorkAround = m_IncomingCompletePath;
        directoryWorkAround.Replace(" ", "?");
        command2execute.Append(wxString::Format("%s ",directoryWorkAround)); //cache directory
        command2execute.Append(wxString::Format("%s ",m_User->GetName())); //user
        command2execute.Append(wxString::Format("%s ",m_User->GetPwd())); //pwd
        command2execute.Append(wxString::Format("%s ",m_ServiceURL.GetCStr())); //dev repository
        command2execute.Append(wxString::Format("%s ",listVME[i].c_str())); //XML URI NAME
        command2execute.Append(wxString::Format("%s ",m_URISRBFile.GetCStr())); //SRB DATA NAME

        //Code to understand if is last VME
        if (!m_WholeMsfDownload )
        {
          bool check = false;
          if (m_BasketListURI.size() == 1 && !CheckIsRoot(listVME[i].c_str()) && !m_FillLinkVector)
            check = true;

          else if (parentNode != NULL && parentNode->Equals(m_Group) && (i+1 == listVME.size()))
            check = true;

          else if ((i+1 == listVME.size()) && m_ListLinkURI.IsEmpty() && !CheckIsRoot(listVME[i].c_str()))
            check = true;

          wxArrayString URI = CheckRemoteLink(listVME[i].c_str());
          if (check && URI.IsEmpty())
            isLast = "true";

        }
        else if (parentNode != NULL && parentNode->Equals(m_RootGroup) && (i+1 == listVME.size()) && m_DownloadCounter  == m_BasketListURI.size())
        {
          wxArrayString URI = CheckRemoteLink(listVME[i].c_str());
          if (URI.IsEmpty() && !CheckRemoteChild(listVME[i].c_str()))
            isLast = "true";
          else
          {
            bool alreadyDownloaded = false;
            isLast = "true";
            //Check if VME has been already downloaded
            for (int i = 0 ; i < URI.size(); i++)
            {
              alreadyDownloaded = false;
              for (int c = 0; c < m_DownloadedURIVector.size(); c++)
              {
                mafString VMEname = URI[i].c_str();
                if (m_DownloadedURIVector[c].Equals(VMEname))
                {
                  alreadyDownloaded = true;
                  break;
                }
              }
              if (alreadyDownloaded = false)
              {
                isLast = "false";
                break;
              }
            }
          }

        }
        else if (m_FillLinkVector && (i+1 == listVME.size()))
        {
          wxArrayString URI = CheckRemoteLink(listVME[i].c_str());
          if (URI.IsEmpty())
            isLast = "true";
          else
          {
            bool alreadyDownloaded = false;
            isLast = "true";
            //Check if VME has been already downloaded
            for (int i = 0 ; i < URI.size(); i++)
            {
              alreadyDownloaded = false;
              for (int c = 0; c < m_DownloadedURIVector.size(); c++)
              {
                mafString VMEname = URI[i].c_str();
                if (m_DownloadedURIVector[c].Equals(VMEname))
                {
                  alreadyDownloaded = true;
                  break;
                }
              }
              if (alreadyDownloaded = false)
              {
                isLast = "false";
                break;
              }
            }
          }
        }
        command2execute.Append(wxString::Format("%s",isLast.GetCStr())); //is last VME

        if (m_DebugMode)
          mafLogMessage( _T("Executing command: '%s'"), command2execute.c_str() );
        m_Pid = wxExecute(command2execute, wxEXEC_ASYNC);
      }
      wxSetWorkingDirectory(oldDir);
      m_DownloadedURIVector.push_back(listVME[i].c_str());

      //import msf in the current tree
      if(ImportMSF(parentNode) != MAF_OK)
      {
        wxMessageBox("Unable to import msf", wxMessageBoxCaptionStr, wxSTAY_ON_TOP | wxOK);
        DownloadCheck(true);
        return MAF_ERROR;;
      }
    
  }
  return MAF_OK;
}
//----------------------------------------------------------------------------
void lhpOpDownloadVMERefactor::OpStop(int result)   
//----------------------------------------------------------------------------
{
  //HideGui();
	mafEventMacro(mafEvent(this,result));
}
//----------------------------------------------------------------------------
bool lhpOpDownloadVMERefactor::CreateIncomingCache()
//----------------------------------------------------------------------------
{
  bool result = true;
  //create cache: logic comunicate the msf directory
  mafEvent event;
  event.SetSender(this);
  event.SetId(ID_MSF_DATA_CACHE);
  mafEventMacro(event);

  wxString temp;
  temp.Append((*event.GetString()).GetCStr());
  temp = temp.BeforeLast('/');
  m_MsfDir = temp;

  wxDir dir(m_MsfDir.GetCStr());
  wxString exist = m_MsfDir.GetCStr();
  if ( !wxDirExists(exist) || !dir.IsOpened())
  {
    // deal with the error here - wxDir would already log an error message
    // explaining the exact reason of the failure
    return false;
  }

  //control cache subdir
  mafString currentSubdir;
  currentSubdir = m_IncomingDir + m_CacheSubdir.GetCStr();
  while(wxDirExists(currentSubdir))
  {
    int number = atoi(m_CacheSubdir.GetCStr());
    number += 1;
    m_CacheSubdir = "";
    m_CacheSubdir << number;
    currentSubdir = m_IncomingDir + m_CacheSubdir.GetCStr();
  }
  currentSubdir = currentSubdir + "\\";
  wxMkDir(currentSubdir);
  m_IncomingCompletePath = currentSubdir;

  return result;
  
}
//----------------------------------------------------------------------------
bool lhpOpDownloadVMERefactor::ExistsRunningProcess()
//----------------------------------------------------------------------------
{
  bool result = false;
  
  wxFile lockFile;
  wxString lockpath = m_VMEUploaderDownloaderDir;
  lockpath += "activeLock.lhp";
  if (wxFileExists(lockpath))
  {
    result = lockFile.Open(lockpath);
  }
  
  if(result)
  {
    char processId[10];
    char *pointer;
    pointer = &processId[0];

    lockFile.Read(pointer, 10);
    long pidControl = atoi(processId);

    lockFile.Close();
    m_Pid = pidControl;

    result = wxProcess::Exists(m_Pid);

    if(!result)
    {
      wxRemoveFile(lockpath);
    }
  }
  return result;
}
//----------------------------------------------------------------------------
bool lhpOpDownloadVMERefactor::CreateIncomingDirectory()
//----------------------------------------------------------------------------
{
  bool resultIncoming = false;
  
  wxString existIncoming = m_IncomingDir.GetCStr();
  if ( wxDirExists(existIncoming) )
  {
    resultIncoming = true;
  }
  else
  {
    wxMkDir(existIncoming);
    if ( wxDirExists(existIncoming) ) resultIncoming = true;
  }
  return resultIncoming;
}

//----------------------------------------------------------------------------
int lhpOpDownloadVMERefactor::CreateFileListFromBasket()
//----------------------------------------------------------------------------
{
  wxString oldDir = wxGetCwd();
  if (m_DebugMode)
    mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  wxSetWorkingDirectory(m_VMEUploaderDownloaderDir.GetCStr());
  if (m_DebugMode)
    mafLogMessage( _T("Now current working directory is: '%s' "), wxGetCwd().c_str() );

  // gui for selecting vme
  wxString command2execute;
  command2execute.Clear();
  command2execute = m_PythonwExe.GetCStr();

  command2execute.Append(" downloadSelectorApp.py ");
  command2execute.Append(m_User->GetName());
  command2execute.Append(" ");
  command2execute.Append(m_User->GetPwd());
  command2execute.Append(" ");
  command2execute.Append(wxString::Format("%d",m_FromSendbox));


  wxBusyInfo *wait;
  if(!m_TestMode)
  {
    wait = new wxBusyInfo("Please wait for your data resource listing...");
  }

  long pid = wxExecute(command2execute, wxEXEC_SYNC);

  if(!m_TestMode)
  {
    delete wait;
  }

  wxSetWorkingDirectory(oldDir);
  if (m_DebugMode)
    mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );

  return MAF_OK;
}
//-------------------------------------------------------------------
int lhpOpDownloadVMERefactor::RetrieveInformationFromBasketListFile()
//-------------------------------------------------------------------
{
  wxString oldDir = wxGetCwd();
  if (m_DebugMode)
    mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  wxSetWorkingDirectory(m_VMEUploaderDownloaderDir.GetCStr());
  if (m_DebugMode)
    mafLogMessage( _T("Now current working directory is: '%s' "), wxGetCwd().c_str() );

  // open auto tags file and try to handle tags using tags factory 
  ifstream inBasketListFile;

  inBasketListFile.open(m_BasketListFileName.GetCStr());
  if (!inBasketListFile) {
    mafLogMessage("Unable to open file");
    return MAF_ERROR; // terminate with error
  }

  std::string idName;

  //read SRB data URI
  while (inBasketListFile >> idName) 
  {
    m_BasketListURI.Add(idName.c_str());
  }
  inBasketListFile.close();

  wxSetWorkingDirectory(oldDir);
  if (m_DebugMode)
    mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );

  return MAF_OK;
}
//-------------------------------------------------------------------
int lhpOpDownloadVMERefactor::DownloadSelectedXMLFromBasket(mafString  xmlFile)
//-------------------------------------------------------------------
{
  wxString oldDir = wxGetCwd();
  if (m_DebugMode)
    mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  wxSetWorkingDirectory(m_VMEUploaderDownloaderDir.GetCStr());
  if (m_DebugMode)
    mafLogMessage( _T("Now current working directory is: '%s' "), wxGetCwd().c_str() );

  // get manual tags
  wxString command2execute;
  command2execute.Clear();
  command2execute = m_PythonExe.GetCStr();
  command2execute.Append(" downloadSingleXML.py ");
  command2execute.Append(m_User->GetName());
  command2execute.Append(" ");
  command2execute.Append(m_User->GetPwd());
  command2execute.Append(" ");
  command2execute.Append(xmlFile.GetCStr());
  command2execute.Append(" ");

  wxString directoryWorkAround = m_IncomingCompletePath;
  directoryWorkAround.Replace(" ", "?");
  command2execute.Append(directoryWorkAround);

  wxArrayString output;
  wxArrayString errors;
  long pid = -1;
  if (pid = wxExecute(command2execute, output, errors, wxEXEC_SYNC) != 0)
  {
    wxMessageBox(wxString::Format("Error in downloadSingleXML.py trying to download '%s'.\nMSF download stopped.",xmlFile.GetCStr()), wxMessageBoxCaptionStr, wxSTAY_ON_TOP | wxOK);
    return MAF_ERROR;
  }

  if (m_DebugMode)
  {
    mafLogMessage("Command Output Messages:");
    for (int i = 0; i < output.size(); i++)
    {
      mafLogMessage(output[i]);
    }

    mafLogMessage("Command Errors Messages:");
    for (int i = 0; i < errors.size(); i++)
    {
      mafLogMessage(errors[i]);
    }
  }

  if(output.size() < 2)
  {
    return MAF_ERROR;
  }

  m_URISRBFileSize = output[output.size() - 1];
  m_URISRBFile = output[output.size() - 2];

  wxSetWorkingDirectory(oldDir);
  if (m_DebugMode)
    mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );

  return MAF_OK;
}
//-------------------------------------------------------------------
int lhpOpDownloadVMERefactor::ReconstructMSF(mafString xmlFile)
//-------------------------------------------------------------------
{
  wxString oldDir = wxGetCwd();
  if (m_DebugMode)
    mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  wxSetWorkingDirectory(m_VMEUploaderDownloaderDir.GetCStr());
  if (m_DebugMode)
    mafLogMessage( _T("Now current working directory is: '%s' "), wxGetCwd().c_str() );

  wxString command2execute;
  command2execute.Clear();
  command2execute = m_PythonwExe.GetCStr();
  command2execute.Append(" msfReconstructor.py ");

  wxString directoryWorkAround = m_IncomingCompletePath;
  directoryWorkAround.Replace(" ", "?");

  command2execute.Append(directoryWorkAround);
  command2execute.Append(" ");
  command2execute.Append(xmlFile);
  command2execute.Append(" ");

  wxString directoryWorkAroundMSF = m_MsfDir;
  directoryWorkAroundMSF.Append("/");
  directoryWorkAroundMSF.Replace(" ", "?");

  command2execute.Append(directoryWorkAroundMSF);
  command2execute.Append(" ");
  if (m_DebugMode)
    mafLogMessage( _T("Executing command: '%s'"), command2execute.c_str() );

  long pid = wxExecute(command2execute, wxEXEC_SYNC);
  
  wxSetWorkingDirectory(oldDir);
  if (m_DebugMode)
    mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );

  return MAF_OK;
}

//-------------------------------------------------------------------
wxArrayString lhpOpDownloadVMERefactor::GetChildURI(mafNode *node)
//-------------------------------------------------------------------
{
  int result = MAF_ERROR;
  wxString name;
  int count;
  wxArrayString listChildURI;
  listChildURI.clear();
  mafTagItem *tagChild = node->GetTagArray()->GetTag("L0000_resource_MAF_TreeInfo_VmeChildURI1");

  if (tagChild == NULL)
    return listChildURI;

  std::string listChild = tagChild->GetValue();

  if (listChild.rfind("dataresource") != std::string::npos)
  {
    while (listChild.find_first_of(' ') != -1)
    {
      count = listChild.find_first_of(' ');
      name = (listChild.substr(0, count)).c_str();
      if (!name.IsEmpty())
      {
        name.Trim(false);
        name.Trim();
        listChildURI.Add(name);
      }
      listChild.erase(0, count+1);
    }
  }

  return listChildURI;
}

//-------------------------------------------------------------------
void lhpOpDownloadVMERefactor::GetLinkURI(mafNode *node)
//-------------------------------------------------------------------
{
  wxString name;
  int count;
  mafTagItem *linkChild = node->GetTagArray()->GetTag("L0000_resource_MAF_Procedural_VMElinkURI1");
   if (linkChild == NULL)
     return;

  std::string linkURI = linkChild->GetValue();

  if (linkURI.rfind("dataresource") != std::string::npos)
  {
    while (linkURI.find_first_of(' ') != -1)
    {
      count = linkURI.find_first_of(' ');
      name = (linkURI.substr(0, count)).c_str();
      if (!name.IsEmpty())
      {
        name.Trim(false);
        name.Trim();
        if (m_WholeMsfDownload)
          m_ListLinkURIInTree.Add(name);
        else
          m_ListLinkURI.Add(name);
      }
      linkURI.erase(0, count+1);
    }
    if (!linkURI.empty())
    {
      if (linkURI.rfind("dataresource") != std::string::npos)
      {
        if (m_WholeMsfDownload)
          m_ListLinkURIInTree.Add(linkURI.c_str());
        else
          m_ListLinkURI.Add(linkURI.c_str());
      }
    }
  }
}
//-------------------------------------------------------------------
wxArrayString lhpOpDownloadVMERefactor::CheckRemoteLink(mafString URI)
//-------------------------------------------------------------------
{
  wxString oldDir = wxGetCwd();
  wxSetWorkingDirectory(m_VMEUploaderDownloaderDir.GetCStr());
  if (m_DebugMode)
  mafLogMessage( _T("Now current working directory is: '%s' "), wxGetCwd().c_str() );

  wxString command2execute;
  command2execute.Clear();
  command2execute = m_PythonExe.GetCStr();

  command2execute.Append("lhpReadRemoteTag.py ");
  command2execute.Append(m_User->GetName());
  command2execute.Append(" ");
  command2execute.Append(m_User->GetPwd());
  command2execute.Append(" ");
  command2execute.Append(m_ServiceURL.GetCStr());
  command2execute.Append(" ");
  command2execute.Append(URI.GetCStr());
  command2execute.Append(",");
  command2execute.Append("L0000_resource_MAF_Procedural_VMElinkURI1");

  if (m_DebugMode)
  mafLogMessage( _T("Executing command: '%s'"), command2execute.c_str() );

  wxArrayString output;
  wxArrayString errors;
  long pid = -1;
  if (pid = wxExecute(command2execute, output, errors, wxEXEC_SYNC) != 0)
  {
    wxMessageBox("Error in lhpReadRemoteTag.py. Uploading stopped", wxMessageBoxCaptionStr, wxSTAY_ON_TOP | wxOK);
    if (m_DebugMode)
      mafLogMessage(_T("SYNC Command process '%s' terminated with exit code %d."),
      command2execute.c_str(), pid);
    return MAF_ERROR;
  }

  if (m_DebugMode)
  {
    mafLogMessage("Command Output Messages:");
    for (int i = 0; i < output.size(); i++)
    {
      mafLogMessage(output[i]);
    }

    mafLogMessage("Command Errors Messages:");
    for (int i = 0; i < errors.size(); i++)
    {
      mafLogMessage(errors[i]);
    }
  }

  wxString name;
  int count;
  wxArrayString URIArray;
  wxString linkURI= output[output.size() - 1];
  if (linkURI.rfind("dataresource") != std::string::npos)
  {
    while (linkURI.find_first_of(' ') != -1)
    {
      count = linkURI.find_first_of(' ');
      name = (linkURI.substr(0, count)).c_str();
      if (!name.IsEmpty())
      {
        name.Trim(false);
        name.Trim();
        URIArray.Add(name);
      }
      linkURI.erase(0, count+1);
    }
    if (!linkURI.empty())
    {
      if (linkURI.rfind("dataresource") != std::string::npos)
      {
        URIArray.Add(linkURI.c_str());
      }
    }
  }

  wxSetWorkingDirectory(oldDir);
  if (m_DebugMode)
    mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  return URIArray;
}

//-------------------------------------------------------------------
bool lhpOpDownloadVMERefactor::CheckRemoteChild(mafString URI)
//-------------------------------------------------------------------
{
  bool containChild = false;
  wxString oldDir = wxGetCwd();
  wxSetWorkingDirectory(m_VMEUploaderDownloaderDir.GetCStr());
  if (m_DebugMode)
    mafLogMessage( _T("Now current working directory is: '%s' "), wxGetCwd().c_str() );

  wxString command2execute;
  command2execute.Clear();
  command2execute = m_PythonExe.GetCStr();

  command2execute.Append("lhpReadRemoteTag.py ");
  command2execute.Append(m_User->GetName());
  command2execute.Append(" ");
  command2execute.Append(m_User->GetPwd());
  command2execute.Append(" ");
  command2execute.Append(m_ServiceURL.GetCStr());
  command2execute.Append(" ");
  command2execute.Append(URI.GetCStr());
  command2execute.Append(",");
  command2execute.Append("L0000_resource_MAF_TreeInfo_VmeChildURI1");

  if (m_DebugMode)
    mafLogMessage( _T("Executing command: '%s'"), command2execute.c_str() );

  wxArrayString output;
  wxArrayString errors;
  long pid = -1;
  if (pid = wxExecute(command2execute, output, errors, wxEXEC_SYNC) != 0)
  {
    wxMessageBox("Error in lhpReadRemoteTag.py. Uploading stopped", wxMessageBoxCaptionStr, wxSTAY_ON_TOP | wxOK);
    if (m_DebugMode)
      mafLogMessage(_T("SYNC Command process '%s' terminated with exit code %d."),
      command2execute.c_str(), pid);
    return MAF_ERROR;
  }

  if (m_DebugMode)
  {
    mafLogMessage("Command Output Messages:");
    for (int i = 0; i < output.size(); i++)
    {
      mafLogMessage(output[i]);
    }

    mafLogMessage("Command Errors Messages:");
    for (int i = 0; i < errors.size(); i++)
    {
      mafLogMessage(errors[i]);
    }
  }

  wxString child= output[output.size() - 1];
  if (child.Contains("dataresource"))
    containChild = true;

  wxSetWorkingDirectory(oldDir);
  if (m_DebugMode)
    mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  return containChild;
}
//-------------------------------------------------------------------
bool lhpOpDownloadVMERefactor::CheckIsRoot(mafString URI)
//-------------------------------------------------------------------
{
  bool isRoot = false;
  wxString oldDir = wxGetCwd();
  wxSetWorkingDirectory(m_VMEUploaderDownloaderDir.GetCStr());
  if (m_DebugMode)
    mafLogMessage( _T("Now current working directory is: '%s' "), wxGetCwd().c_str() );

  wxString command2execute;
  command2execute.Clear();
  command2execute = m_PythonExe.GetCStr();

  command2execute.Append("lhpReadRemoteTag.py ");
  command2execute.Append(m_User->GetName());
  command2execute.Append(" ");
  command2execute.Append(m_User->GetPwd());
  command2execute.Append(" ");
  command2execute.Append(m_ServiceURL.GetCStr());
  command2execute.Append(" ");
  command2execute.Append(URI.GetCStr());
  command2execute.Append(",");
  command2execute.Append("L0000_resource_MAF_VmeType");

  if (m_DebugMode)
    mafLogMessage( _T("Executing command: '%s'"), command2execute.c_str() );

  wxArrayString output;
  wxArrayString errors;
  long pid = -1;
  if (pid = wxExecute(command2execute, output, errors, wxEXEC_SYNC) != 0)
  {
    wxMessageBox("Error in lhpReadRemoteTag.py. Uploading stopped", wxMessageBoxCaptionStr, wxSTAY_ON_TOP | wxOK);
    if (m_DebugMode)
      mafLogMessage(_T("SYNC Command process '%s' terminated with exit code %d."),
      command2execute.c_str(), pid);
    return MAF_ERROR;
  }

  if (m_DebugMode)
  {
    mafLogMessage("Command Output Messages:");
    for (int i = 0; i < output.size(); i++)
    {
      mafLogMessage(output[i]);
    }

    mafLogMessage("Command Errors Messages:");
    for (int i = 0; i < errors.size(); i++)
    {
      mafLogMessage(errors[i]);
    }
  }

  wxString child= output[output.size() - 1];
  if (child.CompareTo("mafVMERoot") == 0)
    isRoot = true;


  wxSetWorkingDirectory(oldDir);
  if (m_DebugMode)
    mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  return isRoot;
}
//-------------------------------------------------------------------
int lhpOpDownloadVMERefactor::ImportMSF(mafNode *parentNode)
//-------------------------------------------------------------------
{
  //msf name is standard: outputMSF.msf
  mafString msfFileName;
  msfFileName.Append(m_IncomingCompletePath);
  msfFileName.Append("outputMAF.msf");
  msfFileName.ParsePathName();

  if (!wxFileExists(msfFileName.GetCStr()))
  {
    return MAF_ERROR;
  }

  mafVMEStorage *storage;
  mafNodeManager manager;
  storage = mafVMEStorage::New();
  storage->SetManager(&manager);
  storage->SetURL(msfFileName.GetCStr());

  int res = storage->Restore();
  if (res != MAF_OK)
  {
    // if some problems occurred during import give feedback to the user
    if (!m_TestMode)
      mafErrorMessage(_("Errors during file parsing! Look the log area for error messages."));
    return MAF_ERROR;
  }
  mafVMERoot *root = mafVMERoot::SafeDownCast(manager.GetRoot());

  m_NodeDownloaded = root->GetFirstChild();
  if (m_NodeDownloaded == NULL)
  {
    //if root has no child, node downloaded is a root
    m_WholeMsfDownload = true;
    mafNEW(m_RootGroup);
    //copy tags from Root downloaded to new group.
    m_RootGroup->GetTagArray()->DeepCopy(root->GetTagArray());
    mafString label = "MSF from repository: ";
    label.Append(root->GetName());
    m_RootGroup->SetName(label.GetCStr());
    m_RootGroup->ReparentTo(m_Input);
    m_DownloadedNodeVector.push_back(m_RootGroup);
    if (DownloadTree(root) != MAF_OK)
    {
      return MAF_ERROR;
    }

    m_WholeMsfDownload = false;
    mafDEL(storage);
    return MAF_OK;
  }

  m_DownloadedNodeVector.push_back(m_NodeDownloaded);

  if (m_NodeDownloaded->GetNumberOfLinks() != 0)
  {
    if (!m_WholeMsfDownload)
      wxMessageBox(wxString::Format("Link found! VME link will be downloaded"), wxMessageBoxCaptionStr, wxSTAY_ON_TOP | wxOK);
    GetLinkURI(m_NodeDownloaded);
    m_DerivedNodeVector.push_back(m_NodeDownloaded);  
  }

  if (m_FillLinkVector)
  {
    m_LinkNodeVector.push_back(m_NodeDownloaded);
  }

  if (m_Group == NULL && !m_WholeMsfDownload)
  {
    mafNEW(m_Group);
    m_Group->SetName("Downloaded from repository");
    m_Group->ReparentTo(m_Input);
  }
   
  if (parentNode != NULL)
  {
    m_NodeDownloaded->ReparentTo(parentNode);
    if (((mafVME *)m_NodeDownloaded)->IsAnimated())
      UpdateAnimatedBinaryFile();
    else
      UpdateBinaryFile();

    

    if (DownloadTree(m_NodeDownloaded) != MAF_OK)
    {
      return MAF_ERROR;
    }
  }
  else
  {
    m_NodeDownloaded->ReparentTo(m_Group);
    if (((mafVME *)m_NodeDownloaded)->IsAnimated())
      UpdateAnimatedBinaryFile();
    else
      UpdateBinaryFile();
  }

  mafDEL(storage);
  return MAF_OK;
}

//----------------------------------------------------------------------------
void lhpOpDownloadVMERefactor::UpdateBinaryFile()
//----------------------------------------------------------------------------
{
  if (m_NodeDownloaded->IsA("mafVMEGeneric"))
  {
    mafDataVector *dv = ((mafVMEGeneric*)m_NodeDownloaded)->GetDataVector();
    mafDataVector::DataMap::iterator it;
    mafEventIO es(this,NODE_GET_STORAGE);
    m_Input->GetRoot()->OnEvent(&es);
    mafVMEStorage *storage = mafVMEStorage::SafeDownCast(es.GetStorage());
    mafString newMSFFileName = storage->GetURL();
    wxString oldItemURL, newItemURL, tmpURL;
    wxString path, name, ext;
    wxString oldItemPath, oldItemName, oldItemExt;

   
    wxSplitPath(newMSFFileName.GetCStr(), &path, &name, &ext);
    it = dv->Begin();
    mafVMEItem *item=it->second;
    oldItemURL = item->GetURL();
    wxSplitPath(oldItemURL, &oldItemPath, &oldItemName, &oldItemExt);

    dv->UpdateVectorId();
    item->UpdateItemId();
    int newId = item->GetId();

    newItemURL = name << '.' << newId << '.' << oldItemExt;
    //item->SetURL(newItemURL);        
   
    wxString absOldItemURL = m_IncomingCompletePath.GetCStr();
    absOldItemURL += oldItemURL.c_str();
    wxString absNewItemURL = path;
    absNewItemURL += "/";
    absNewItemURL += newItemURL.c_str();

    //Call python module to copy binary data when downloaded
    wxString oldDir = wxGetCwd();
    wxSetWorkingDirectory(m_VMEUploaderDownloaderDir.GetCStr());
    if (m_DebugMode)
      mafLogMessage( _T("Now current working directory is: '%s' "), wxGetCwd().c_str() );

    wxString command2execute;
    command2execute.Clear();
    command2execute = m_PythonwExe.GetCStr();

    command2execute.Append("binaryImporter.py ");
    command2execute.Append("false"); //false if it is not animated
    command2execute.Append(" ");

    absOldItemURL.Replace(" ", "???");
    command2execute.Append(absOldItemURL);

    command2execute.Append(" ");

    absNewItemURL.Replace(" ", "???");
    command2execute.Append(absNewItemURL);

    if (m_DebugMode)
      mafLogMessage( _T("Executing command: '%s'"), command2execute.c_str() );
    m_Pid = wxExecute(command2execute, wxEXEC_ASYNC);
    wxSetWorkingDirectory(oldDir);
 
    if (m_DebugMode)
      mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  }
  
}

//----------------------------------------------------------------------------
 void lhpOpDownloadVMERefactor::UpdateAnimatedBinaryFile()
//----------------------------------------------------------------------------
{
  if (m_NodeDownloaded->IsA("mafVMEGeneric"))
  {
    mafDataVector *dv = ((mafVMEGeneric*)m_NodeDownloaded)->GetDataVector();
    mafDataVector::Iterator it;
    mafEventIO es(this,NODE_GET_STORAGE);
    m_Input->GetRoot()->OnEvent(&es);
    mafVMEStorage *storage = mafVMEStorage::SafeDownCast(es.GetStorage());
    mafString newMSFFileName = storage->GetURL();
    wxString path, name, ext;
    wxString oldArchiveURL, newArchiveURL, tmpURL;
    wxString oldItemURL, oldItemExt, oldItemPath, newItemURL;
    wxString oldArchivePath, oldArchiveName, oldArchiveExt;
    
    wxSplitPath(newMSFFileName.GetCStr(), &path, &name, &ext);
    it = dv->Begin();
    mafVMEItem *item=it->second;
    //oldArchiveURL = item->GetArchiveFileName();
    wxSplitPath(oldArchiveURL, &oldArchivePath, &oldArchiveName, &oldArchiveExt);
    dv->UpdateVectorId();
    int newId = dv->GetVectorID();

    newArchiveURL = name;
    newArchiveURL += '.';
    newArchiveURL += mafString(newId);
    newArchiveURL += '.';
    newArchiveURL += oldArchiveExt;

    //item->SetArchiveFileName(mafString(newArchiveURL.c_str()));
    

    wxString absOldArchiveURL = m_IncomingCompletePath.GetCStr();
    absOldArchiveURL += oldArchiveURL;

    wxString absNewArchiveURL = path;
    absNewArchiveURL += '/';
    absNewArchiveURL += newArchiveURL;

    for (it = dv->Begin(); it!= dv->End(); it++)
    {
      mafVMEItem *item=it->second;
      oldItemURL = item->GetURL();
      wxSplitPath(oldItemURL, &oldItemPath, &oldItemURL, &oldItemExt);
      item->UpdateItemId();
      newId = item->GetId();
      newItemURL = name;
      newItemURL += '.';
      newItemURL += mafString(newId);
      newItemURL += '.';
      newItemURL += oldItemExt;
      //item->SetURL(newItemURL);
      //item->SetArchiveFileName(mafString(newArchiveURL.c_str()));
    }

    //Call python module to copy binary data when downloaded
    wxString oldDir = wxGetCwd();
    wxSetWorkingDirectory(m_VMEUploaderDownloaderDir.GetCStr());
    if (m_DebugMode)
      mafLogMessage( _T("Now current working directory is: '%s' "), wxGetCwd().c_str() );

    wxString command2execute;
    command2execute.Clear();
    command2execute = m_PythonwExe.GetCStr();

    command2execute.Append("binaryImporter.py ");
    command2execute.Append("true"); //true if it is animated
    command2execute.Append(" ");


    absOldArchiveURL.Replace(" ", "???");
    command2execute.Append(absOldArchiveURL);
    command2execute.Append(" ");

    absNewArchiveURL.Replace(" ", "???");
    command2execute.Append(absNewArchiveURL);
    if (m_DebugMode)
      mafLogMessage( _T("Executing command: '%s'"), command2execute.c_str() );

    if (m_DebugMode)
      mafLogMessage( _T("Executing command: '%s'"), command2execute.c_str() );
    m_Pid = wxExecute(command2execute, wxEXEC_ASYNC);
    wxSetWorkingDirectory(oldDir);

    if (m_DebugMode)
      mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  }
}

//----------------------------------------------------------------------------
int lhpOpDownloadVMERefactor::DownloadCheck(bool failed)
//----------------------------------------------------------------------------
{
  mafString checkParameter;
  for (int m = 0; m < m_CheckURIVector.size(); m++)
  {
    if (m != 0)
      checkParameter.Append(":");
    if (m == m_DownloadedURIVector.size()-1 && failed)
    {
      //if failed, last VME genereted an error
      checkParameter.Append(m_CheckURIVector[m]);
      checkParameter.Append(",");
      checkParameter.Append("error");
    }
    else
    {
      checkParameter.Append(m_CheckURIVector[m]);
      checkParameter.Append(",");
      checkParameter.Append("success");
    }
  }

  wxString oldDir = wxGetCwd();
  wxSetWorkingDirectory(m_VMEUploaderDownloaderDir.GetCStr());
  if (m_DebugMode)
    mafLogMessage( _T("Now current working directory is: '%s' "), wxGetCwd().c_str() );

  wxString command2execute;
  command2execute.Clear();
  command2execute = m_PythonwExe.GetCStr();

  command2execute.Append("lhpDownloadVmeCheck.py ");
  command2execute.Append(m_User->GetName());
  command2execute.Append(" ");
  command2execute.Append(m_User->GetPwd());
  command2execute.Append(" ");
  command2execute.Append(m_ServiceURL.GetCStr());
  command2execute.Append(" ");
  command2execute.Append(checkParameter.GetCStr());
  if (m_DebugMode)
    mafLogMessage( _T("Executing command: '%s'"), command2execute.c_str() );

  long pid = -1;
  if (pid = wxExecute(command2execute, wxEXEC_SYNC) != 0)
  {
    wxMessageBox("Error in lhpDownloadVmeCheck.py", wxMessageBoxCaptionStr, wxSTAY_ON_TOP | wxOK);
    mafLogMessage(_T("SYNC Command process '%s' terminated with exit code %d."),
      command2execute.c_str(), pid);
    return MAF_ERROR;
  }

  wxSetWorkingDirectory(oldDir);
  if (m_DebugMode)
    mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );

  return MAF_OK;
}

//----------------------------------------------------------------------------
bool lhpOpDownloadVMERefactor::IsLHPBuilderVersionUpToDate()
//----------------------------------------------------------------------------
{
  wxBusyInfo("Checking if  your software is up-to-date in order to upload, please wait...");
  wxString oldDir = wxGetCwd();
  if (m_DebugMode)
    mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
  wxSetWorkingDirectory(m_VMEUploaderDownloaderDir.GetCStr());
  if (m_DebugMode)
    mafLogMessage( _T("Now current working directory is: '%s' "), wxGetCwd().c_str() );
  
  // get application name
  mafEvent eventGetApplicationName;
  eventGetApplicationName.SetSender(this);
  eventGetApplicationName.SetId(ID_REQUEST_APPLICATION_NAME);
  mafEventMacro(eventGetApplicationName);
  mafString appName = eventGetApplicationName.GetString()->GetCStr();  

  // get manual tags
  wxString command2execute;
  command2execute.Clear();
  command2execute = m_PythonwExe.GetCStr();
  command2execute.Append(" lhpDictionaryVersionChecker.py ");
  command2execute.Append(appName.GetCStr());
  command2execute.Append(" ");
  if( !m_ProxyURL.Equals("") )
  {
    command2execute.Append(m_ProxyURL.GetCStr());
    command2execute.Append(" ");
    command2execute.Append(m_ProxyPort.GetCStr());
  }

  if (m_DebugMode)
    mafLogMessage( _T("Executing command: '%s'"), command2execute.c_str() );

  wxArrayString output;
  wxArrayString errors;
  long pid = -1;
  if (pid = wxExecute(command2execute, output, errors, wxEXEC_SYNC) != 0)
  {
    wxMessageBox("Error in lhpDictionaryVersionChecker.py. Download stopped", wxMessageBoxCaptionStr, wxSTAY_ON_TOP | wxOK);
    return MAF_ERROR;
  }

  if (m_DebugMode)
  {
    mafLogMessage("Command Output Messages:");
    for (int i = 0; i < output.size(); i++)
    {
      mafLogMessage(output[i]);
    }

    mafLogMessage("Command Errors Messages:");
    for (int i = 0; i < errors.size(); i++)
    {
      mafLogMessage(errors[i]);
    }
  }
  

  wxString result = output[output.size() - 1];

  wxSetWorkingDirectory(oldDir);
  if (m_DebugMode)
    mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );

  if (result == "UpToDate")
  {
    return true;
  } 
  else
  {
    return false;
  }  
}