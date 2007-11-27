/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: lhpOpUploadVME.h,v $
Language:  C++
Date:      $Date: 2007-11-27 09:36:57 $
Version:   $Revision: 1.2 $
Authors:   Daniele Giunchi
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

//----------------------------------------------------------------------------
// lhpOpUploadVME :
//----------------------------------------------------------------------------
/**Convert a Parametric Surface into a Normal Surface*/
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

  /** */
  void SetMsfDir(mafString msfDir){m_MsfDir = msfDir;}

protected:

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	virtual void OpStop(int result);

  /** This method create on filesystem a cache with msf and binary data that must be uploaded */
  bool CreateCache();

  /** This method check if process exists and if there is a lock file */
  bool ExistsRunningProcess();
  
  mafString m_CacheDir;
  mafString m_PythonUploadFullPath;
  mafString m_FileName;
  mafString m_PythonExe;
  mafString m_MsfDir;
  static mafString m_CacheSubdir;
  mafString m_CurrentCache;

 static long m_Pid;
};
#endif
