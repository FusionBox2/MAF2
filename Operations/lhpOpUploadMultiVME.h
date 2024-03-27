/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: lhpOpUploadMultiVME.h,v $
Language:  C++
Date:      $Date: 2009-04-06 16:10:13 $
Version:   $Revision: 1.13.2.10 $
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

#ifndef __lhpOpUploadMultiVME_H__
#define __lhpOpUploadMultiVME_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"
#include "mafNode.h"


//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafUser;
class mafNode;
class lhpOpUploadVME;

//----------------------------------------------------------------------------
// lhpOpUploadMultiVME :
//----------------------------------------------------------------------------
/**Upload multiple VME using lhpOpUploadVME.*/
// TODO: REFACTOR THIS 
// BEWARE Heavy Refactoring in progress!!!!!!!!!!!!

class lhpOpUploadMultiVME: public mafOp
{
public:

	lhpOpUploadMultiVME(const mafString& label = _R("Upload Multi Vme"));
	~lhpOpUploadMultiVME(); 

	mafTypeMacro(lhpOpUploadMultiVME, mafOp);

	mafOp* Copy();

	/** Return true for the acceptable vme type. */
  bool Accept(mafNode* vme);

	/** Builds operation's interface by calling CreateOpDialog() method. */
	void OpRun();

	/** Execute the operation. */
	virtual void OpDo();

  /** Set Current Working Msf Directory*/
  void SetMsfDir(mafString msfDir){m_MsfDir = msfDir;};


protected:
  
  /** check if lhpbuilder software version is up to date in order to a allow vme uploading */
  bool IsSoftwareVersionUpToDate();

  /** Try to handle auto tags through tags factory and convert unhandled 
  to manual tags is to be filled by the user*/
  void HandleAutoTagsTroughFactory();

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	virtual void OpStop(int result);

  mafString m_CacheDir; //>cache superdirectory
  static mafString m_CacheSubdir; //>cache subdirectory

  mafString m_VMEUploaderDownloaderDir; //>directory where the scripts are
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

  mafUser *m_User;

private:
  mafString GetXMLDictionaryFileName(mafString dictionaryFileNamePrefix);

  /** Upload one or more than one VME chosen from a check list box */
  int UploadMultiVME(mafNode *node, bool isLast);

  /** Upload of node and all its children*/
  int UploadTree(mafNode *node);

  /** Upload linked VME */
  int UploadVMELinks(mafNode *derived);   

  /** Search for python uploader error */
  bool GetUploadError();   

  /** Write a file with URI information about VME link uploaded */
  int SaveLinkURIFile(mafNode *node, std::vector<mafString> linkURI);

  /** Write a file with URI information about VME children uploaded */
  int SaveChildURIFile(mafNode* node, mafString URI);

  /** Edit VME tag with VME link URI */
  int SetVMELinks(mafNode *node);

  /** Check if a binary data is associated to the VME */
  bool isBinaryDataPresent(mafNode *node);

  /** Remove uploaded reosurces in case of error during msf uploading */
  bool RemoveResources(std::vector<mafString> vectorURI);  

  void SaveConnectionConfigurationFile();

  int AssembleDictionaries();

  bool m_WithChild;
  bool m_DebugMode;
 
  mafString m_ServiceURL;
  mafString m_MsfFile;

  lhpOpUploadVME *m_UploadVME;
  std::vector<mafNode*> m_NodeVector;
  std::vector<mafNode*> m_EmptyNodeVector;
  std::vector<mafNode*> m_UploadedNodeVector;
  std::vector<mafString> m_UploadedURIVector;
  std::vector<mafString> m_FileCreatedVector;
  std::vector<int> m_NodeDerivedId;

  mafNode *m_UploadingNode;
  int m_NodeCounter;
  int m_SubdictionaryId;



};
#endif