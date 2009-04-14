/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: lhpOpUploadVMERefactor.h,v $
Language:  C++
Date:      $Date: 2009-04-14 15:09:33 $
Version:   $Revision: 1.1.2.7 $
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

#ifndef __lhpOpUploadVMERefactor_H__
#define __lhpOpUploadVMERefactor_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class lhpUser;
class mafVME;

//----------------------------------------------------------------------------
// lhpOpUploadVMERefactor :
//----------------------------------------------------------------------------
/**Upload Single VME */
// TODO: REFACTOR THIS 
// BEWARE Heavy Refactoring in progress!!!!!!!!!!!!

class lhpOpUploadVMERefactor: public mafOp
{
public:

	lhpOpUploadVMERefactor(wxString label = "Upload Vme");
	~lhpOpUploadVMERefactor(); 

	mafTypeMacro(lhpOpUploadVMERefactor, mafOp);

	mafOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode* vme);

	/** Builds operation's interface by calling CreateOpDialog() method. */
	void OpRun();

	/** Execute the operation. */
	virtual void OpDo();

  /** Set Current Working Msf Directory*/
  void SetMsfDir(mafString msfDir){m_MsfABSFolder = msfDir;};

  /** Set debug modality */
  void SetDebugMode(bool debugMode){m_DebugMode = debugMode;};

  /** Has XML file to upload some binary data associated? Default to false */
  void SetIsBinaryDataPresent(bool present) {m_IsBinaryDataPresent = present;};
  bool GetIsBinaryDataPresent() {return m_IsBinaryDataPresent;};
  
  /** Upload also vme children? Default to false */
  void SetWithChild(bool withChild) {m_WithChild = withChild;};
  bool GetWitChild() {return m_WithChild;};

  /** File to store uploaded XML resources URI for rollback */
  void SetXMLUploadedResourcesRollBackLocalFileName(mafString file) {m_InputXMLDataResourcesRollBackFile = file;};
  mafString GetInputXMLDataResourcesRollBackFile() {return m_InputXMLDataResourcesRollBackFile;};

  /** Is this the last resource that will feed the uploader in one instance of the 
  multiple upload operation? */
  void SetIsLast(bool isLast) {m_IsLast = isLast;};
  bool GetIsLast() {return m_IsLast;};

  /** Upload */
  int Upload();

  /** Return the remote URI where the XML resource has been stored after calling Upload()*/
  mafString GetRemoteXMLResourceURI() {return m_RemoteXMLResourceURI;};


protected:
  
  /** Load configuration file for connection*/
  void SaveConnectionConfigurationFile();
  
  /** Set subDictionary. */
  void SetDictionary(int subDictionary);   

  bool m_WithChild;
  bool m_IsLast;
  mafString m_InputXMLDataResourcesRollBackFile;
  bool m_IsBinaryDataPresent;
  mafString m_RemoteXMLResourceURI;

  /** Save information information about VME link*/
  void SaveLinkInfo();

  /** check if client software version is up to date in order to a allow vme uploading */
  bool IsClientSoftwareVersionUpToDate();

  /** Generate auto tags and manual tags list from XML lhdl dictionary*/
  int GeneratesTagsListsFromXMLDictionary();

  /** Try to handle auto tags through tags factory and convert unhandled 
  to manual tags ie to be filled by the user*/
  void HandleAutoTagsTroughFactory();

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	virtual void OpStop(int result);

  /* Base Cache and Outgoing creation directory*/
  bool CreateBaseCacheAndOutgoingDirectories();

  /** This method creates on filesystem a cache with msf and binary data that must be uploaded */
  bool CreateCache();

  /** This method copies in cache the msf and binary data that must be uploaded */
  bool CopyInputVMEInCache();

  /** This method checks if process exists and if there is a lock file */
  bool ExistsRunningProcess();

  /** Import Edited MSF*/
  int ImportMSF();
  
  mafString m_CachesParentABSFolderName; //>cache subfolders parent directory
  static mafString m_CacheChildLocalFolderName; //>cache subdirectory
  wxString m_CurrentCacheChildABSFolderName; //>current cache subdirectory
  
  mafString m_OutgoingDir; //directory for xml and binary to send

  mafString m_VMEUploaderDownloaderABSFolderName; //>directory where the scripts are
  
  mafString m_PythonExe; //>python  executable
  mafString m_PythonwExe; //>pythonw  executable
  mafString m_MsfABSFolder; //>directory of original msf
	mafString m_MsfABSFileName; //>absolute path of original msf
  
  static long m_Pid; //> pid of the server process

  mafString m_MasterXMLDictionaryFilePrefix; 
  mafString m_MasterXMLDictionaryFileName;
  mafString m_SubXMLDictionaryFilePrefix;
  mafString m_SubXMLDictionaryFileName;
  mafString m_AssembledXMLDictionaryFileName;
  mafString m_SubDictionaryBuildingCommand;

  mafString m_HandledAutoTagsLocalFileName;
  mafString m_AutoTagsListFromXMLDictionaryLocalFileName; 
  mafString m_ManualTagsListFromXMLDictionaryLocalFileName;
  

  lhpUser  *m_User;
  
  mafString m_ConnectionConfigurationFileName;
  mafString m_ProxyURL;
  mafString m_ProxyPort;
  mafString m_RepositoryServiceURL;


private:
  mafString GetXMLDictionaryFileName(mafString dictionaryFileNamePrefix);
  int AssembleDictionaries();
  int m_SubId;
  mafVME *m_CacheVme;

  int m_SubdictionaryId;
  wxArrayString m_AutoTagsList;
  wxArrayString m_ManualTagsList;
  wxArrayString m_UnhandledAutoTagsListFromFactory; ///< the factory was not able to handle these tags
  wxArrayString m_HandledAutoTagsListFromFactory; ///< tags handled by the factory  
  wxString m_UnhandledPlusManualTagsLocalFileName;
  wxString m_NodeName;
  FILE *m_ProxyFile;
  bool m_HasLink;
  bool m_HasChild;
  bool m_DebugMode;

  std::vector<mafNode*> m_LinkNode;
  std::vector<mafString> m_LinkName;


};

#endif