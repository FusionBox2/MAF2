/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpImporterRSScan.h,v $
  Language:  C++
  Date:      $Date: 2008-03-06 11:55:06 $
  Version:   $Revision: 1.1 $
  Authors:   Stefano Perticoni   
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __lhpOpImporterRSScan_H__
#define __lhpOpImporterRSScan_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;
class mafVMEMesh;
class mafVMEGroup;
class mafEvent;

//----------------------------------------------------------------------------
// lhpOpImporterRSScan :
//----------------------------------------------------------------------------
/** */
class lhpOpImporterRSScan: public mafOp
{
public:
	lhpOpImporterRSScan(const wxString &label = "RSScanImporter");
	~lhpOpImporterRSScan(); 
	
  mafTypeMacro(lhpOpImporterRSScan, mafOp);

  virtual void OnEvent(mafEventBase *maf_event);

  mafOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode *node);

	/** Set the filename for the mesh to import */
  void SetFileName(const char *file_name);

  /** Set/Get nodes file name*/
  void SetNodesFileName(const char *name)   {m_PlateParamsFileName = name;}
  const char *GetNodesFileName() {return m_PlateParamsFileName;}

  /** Set/Get elements file name*/
  void SetElementsFileName(const char *name)   {m_DataFileName = name;}
  const char *GetElementsFileName() {return m_DataFileName;}

  /** Builds operation's interface. */
  void OpRun();

  /** Import the mesh*/
  int Read();

protected:
  /** Create the dialog interface for the importer. */
  virtual void CreateGui();

  mafVMEGroup *m_ImportedVmeMesh;
  mafString   m_FileDir;
  double      m_Scale;
  double      m_TimeShift;

  /** Nodes file name*/
  mafString m_PlateParamsFileName;

  /** Elements file name*/
  mafString m_DataFileName;
};
#endif
