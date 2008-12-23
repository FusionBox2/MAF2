/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: lhpOpEditTag.h,v $
Language:  C++
Date:      $Date: 2008-12-23 12:49:50 $
Version:   $Revision: 1.11.2.11 $
Authors:   Roberto Mucci , Stefano Perticoni 
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

#ifndef __lhpOpEditTag_H__
#define __lhpOpEditTag_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class lhpUser;

//----------------------------------------------------------------------------
// lhpOpEditTag :
//----------------------------------------------------------------------------
/**Operation to edit VME tags*/
class lhpOpEditTag: public mafOp
{
public:

	lhpOpEditTag(wxString label = "Edit VME Tag");
	~lhpOpEditTag(); 

	mafTypeMacro(lhpOpEditTag, mafOp);

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

protected:
  
  /** Propagate tags to other vmes choosed through a tree checkbox */
  void PropagateTagsToChoosedVMES();

  /** Generate auto tags and manual tags list from XML editor input dictionary*/
  int GeneratesTagsListsFromXMLDictionary();

  /** Build the input XML dictionary for the editor */
  int BuildXMLEditorInputDictionary(mafString &outputFileName);
 
  /* Base Cache creation directory*/
  bool CreateBaseCacheDirectories();

  /** This method creates on filesystem a cache with manual tag cvs file */
  bool CreateCache();

  int AppendChildDictionary(const char *sourceXMLDictionaryFileName, const char *d2aFN, const char *pythonString, const char *outputXMLFN);

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	virtual void OpStop(int result);

  /** Save information information about VME link*/
  void SaveLinkInfo();

  /** Import Edited MSF*/
  int CopyEditorTagsIntoOriginalMSF();

  /** Edit VME tags*/
  int EditTags();
  
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

  lhpUser  *m_User;
  
private:
  mafString GetXMLDictionaryFileName(mafString dictionaryFileNamePrefix);
  void CreateGui();

  void LoadUsedDictionariesFromTags();

  void StoreUsedDictionariesToTags();

  int m_SubdictionaryId;
  wxArrayString m_AutoTagsList;
  wxArrayString m_ManualTagsList;
  wxArrayString m_UnhandledAutoTagsListFromFactory; ///< the factory was not able to handle these tags
  wxArrayString m_HandledAutoTagsListFromFactory; ///< tags handled by the factory  
  wxString m_CsvName;
  wxString m_NodeName;
  mafNode *m_Parent;
  bool m_HasLink;
  bool m_DebugMode;

  std::vector<mafNode*> m_LinkNode;
  std::vector<mafString> m_LinkName;
  
  /** id identifying the metadata editor:
  0: lhpMetadataEditor
  1: CSV Editor
  */
  int m_MetadataEditorId;

  /**id identifying if FA subontology should be added to the meta data
  0: no (Default)
  1: yes
  */
  int m_UseFADictionary;

  mafString m_InputDictionaryFileName;
  mafString m_DictionaryToProcessFileName  ;

  int m_UseDicomSubdictionary;
  int m_UseFASubdictionary;
  int m_UseMASubdictionary;
  int m_UseMicroCTSubdictionary;
};
#endif