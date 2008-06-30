/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: lhpOpUploadVME.h,v $
Language:  C++
Date:      $Date: 2008-06-30 14:59:45 $
Version:   $Revision: 1.23 $
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

#ifndef __lhpOpUploadVME_H__
#define __lhpOpUploadVME_H__

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
// lhpOpUploadVME :
//----------------------------------------------------------------------------
/**Upload VME*/
class lhpOpUploadVME: public mafOp
{
public:

	lhpOpUploadVME(wxString label = "Upload Vme");
	~lhpOpUploadVME(); 

	mafTypeMacro(lhpOpUploadVME, mafOp);

	mafOp* Copy();

  /** Class for handle mafEvent*/
  virtual void OnEvent(mafEventBase *maf_event);

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode* vme);

	/** Builds operation's interface by calling CreateOpDialog() method. */
	void OpRun();

	/** Execute the operation. */
	virtual void OpDo();

  /** Set Current Working Msf Directory*/
  void SetMsfDir(mafString msfDir){m_MsfDir = msfDir;};

  /** Set subDictionray. */
  void SetDictionary(int subDictionary);   

  /** This method use MAF Api for set the login in order to make operation like upload or download*/
  bool CheckLogin();

  /** Load configuration file for connection*/
  void SaveConnectionConfigurationFile();


  /** Upload a single VME*/
  int UploadVME(mafString &binaryURI, bool isBinaryDataPresent, bool returnURI = false);

  /** Save information information about VME link*/
  void SaveLinkInfo();

  /** Used with proxy authentication*/
  void SetProxyURL(mafString proxyURL) {m_ProxyURL = proxyURL;};
  mafString GetProxyURL() {return m_ProxyURL;};

  /** Used with proxy authentication*/
  void SetProxyPort(mafString proxyPort) {m_ProxyPort = proxyPort;};
  mafString GetProxyPort() {return m_ProxyPort;};

protected:
  
  /** check if lhbbuilder software version is up to date in order to a allow vme uploading */
  bool IsLHPBuilderVersionUpToDate();

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

  /** This method checks if process exists and if there is a lock file */
  bool ExistsRunningProcess();

  /** Import Edited MSF*/
  int ImportMSF();

  /** Return binary URI of the VME upload*/
  mafString GetBinaryURI();


  
  mafString m_CacheDir; //>cache superdirectory
  static mafString m_CacheSubdir; //>cache subdirectory
  mafString m_CurrentCache; //>current cache directory
  
  mafString m_OutgoingDir; //directoyr for xml and binary to send

  mafString m_PythonUploadFullPath; //>directory where the scripts are
  mafString m_FileName; //>script file name
  mafString m_PythonExe; //>python  executable
  mafString m_PythonwExe; //>pythonw  executable
  mafString m_MsfDir; //>directory of original msf
	mafString m_MsfFile; //>absolute path of original msf
  
  static long m_Pid; //> pid of the server process

  mafString m_MasterXMLDictionaryFilePrefix; 
  mafString m_MasterXMLDictionaryFileName;
  mafString m_SubXMLDictionaryFilePrefix;
  mafString m_SubXMLDictionaryFileName;
  mafString m_AssembledXMLDictionaryFileName;
  mafString m_SubDictionaryBuildingCommand;

  mafString m_HandledAutoTagsFileName;
  mafString m_AutoTagsListFromXMLDictionaryFileName; 
  mafString m_ManualTagsListFromXMLDictionaryFileName;
  mafString m_UnhandledPlusManualTagsFileName;

  static lhpUser  m_User;
  
  mafString m_ConnectionConfigurationFileName;
  mafString m_ProxyURL;
  mafString m_ProxyPort;

private:
  mafString GetXMLDictionaryFileName(mafString dictionaryFileNamePrefix);
  void CreateGui();
  int AssembleDictionaries();
  mafVME *m_CacheVme;

  int m_SubdictionaryId;
  wxArrayString m_AutoTagsList;
  wxArrayString m_ManualTagsList;
  wxArrayString m_UnhandledAutoTagsListFromFactory; ///< the factory was not able to handle these tags
  wxArrayString m_HandledAutoTagsListFromFactory; ///< tags handled by the factory  
  wxString m_CsvName;
  wxString m_NodeName;
  FILE *m_ProxyFile;
  mafNode *m_Parent;
  mafNode *m_TemporaryNode;
  bool m_HasLink;
  bool m_HasChild;

  std::vector<mafNode*> m_LinkNode;
  std::vector<mafString> m_LinkName;

  
};
#endif
