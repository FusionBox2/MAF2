/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: lhpOpUploadMultiVMERefactor.h,v $
Language:  C++
Date:      $Date: 2009-05-20 08:10:55 $
Version:   $Revision: 1.1.2.10 $
Authors:   Roberto Mucci
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

#ifndef __lhpOpUploadMultiVMERefactor_H__
#define __lhpOpUploadMultiVMERefactor_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"
#include "mafNode.h"


//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class lhpUser;
class mafNode;
class lhpOpUploadVMERefactor;

//----------------------------------------------------------------------------
// lhpOpUploadMultiVMERefactor :
//----------------------------------------------------------------------------
/**Upload multiple VME using lhpOpUploadVMERefactor.*/
class lhpOpUploadMultiVMERefactor: public mafOp
{
public:

	lhpOpUploadMultiVMERefactor(wxString label = "Upload Multi Vme");
	~lhpOpUploadMultiVMERefactor(); 

	mafTypeMacro(lhpOpUploadMultiVMERefactor, mafOp);

	mafOp* Copy();

	/** Return true for the acceptable vme type. */
  bool Accept(mafNode* vme);

	/** Builds operation's interface by calling CreateOpDialog() method. */
	void OpRun();

	/** Execute the operation. */
	virtual void OpDo();

  void Upload();
  
  /** Load VMEs to upload from a txt file containing their IDs */
  int LoadInputVMEsIdsFile(const char *fileName);

protected:

  /** Set Current Working Msf Directory*/
  void SetMsfDir(mafString msfDir){m_MsfDir = msfDir;};

  /** Try to handle auto tags through tags factory and convert unhandled 
  to manual tags is to be filled by the user*/
  void HandleAutoTagsTroughFactory();

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	virtual void OpStop(int result);

  mafObserver *GetListener() {return m_Listener;};
  
  mafString m_CacheDir; //>cache superdirectory
  static mafString m_CacheSubdir; //>cache subdirectory

  mafString m_VMEUploaderDownloaderABSFolder; //>directory where the scripts are
  mafString m_PythonExe; //>python  executable
  mafString m_MsfDir; //>directory of original msf
  mafString m_MasterXMLDictionaryFileName;
  mafString m_SubXMLDictionaryFilePrefix;
  mafString m_SubXMLDictionaryFileName;
  mafString m_AssembledXMLDictionaryFileName;
  mafString m_SubDictionaryBuildingCommand;
  
 
  mafString m_ConnectionConfigurationFileName;
  mafString m_ProxyURL;
  mafString m_ProxyPort;

  lhpUser *m_User;

private:
  mafString GetXMLDictionaryFileName(mafString dictionaryFileNamePrefix);

  /** Upload one or more than one VME chosen from a check list box */
  int UploadVMEWithItsLinks(mafNode *vme, bool isLast);

  /** Upload of node and all its children*/
  int UploadVMEWithItsChildren(mafNode *vme);

  /** TODO: REFACTOR TO PYTHON  
  Upload linked VME */
  int UploadVMELinks(mafNode *vme);   

  /** TODO: REFACTOR TO PYTHON 
  Search for python uploader error */
  bool GetUploadError();   

  /** TODO: REFACTOR TO PYTHON 
  Write a file with URI information about VME link uploaded */
  int SaveLinksURIFile(mafNode *node, std::vector<mafString> linkURI);

  /** TODO: REFACTOR TO PYTHON  
  Write a file with URI information about VME children uploaded */
  int SaveChildrenURIFile(mafNode* node, mafString URI);

  /** TODO: REFACTOR TO PYTHON  
  Add VME links URI to derived vme metadata */
  int AddLinksURIToDerivedVMEMetadata(mafNode *derived);
  
  /**  TODO: REFACTOR TO PYTHON  
  Remove uploaded reosurces in case of error during whole msf uploading */
  bool RemoveAlreadyUploadedXMLResources(std::vector<mafString> xmlUploadedResourcesVectorURI);  

  void SaveConnectionConfigurationFile();
  bool m_WithChild;
  bool m_DebugMode;
 
  mafString m_ServiceURL;
  mafString m_OpenMSFFileNameFullPath; ///< full path of the .msf file

  lhpOpUploadVMERefactor *m_OpUploadVME;
  std::vector<mafNode*> m_VMEToBeUploadedVector; ///< vme to be uploaded from gui
  std::vector<mafNode*> m_EmptyNodeVector;
  std::vector<mafNode*> m_AlreadyUploadedVMEVector;
  std::vector<mafString> m_AlreadyUploadedXMLURIVector;
  std::vector<mafString> m_FileCreatedVector;
  std::vector<int> m_DerivedVMEsIdVector;
  std::vector<int> m_VMEsToUploadIdsVector;

  mafNode *m_UploadingNode;
  int m_NodeCounter;
  int m_SubdictionaryId;

  /** test friend */
  friend class lhpOpUploadMultiVMERefactorTest;

};
#endif