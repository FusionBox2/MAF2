/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpExporterCSVGraph.h,v $
  Language:  C++
  Date:      $Date: 2009/05/19 14:29:53 $
  Version:   $Revision: 1.1.1.1 $
  Authors:   Matteo Giacomoni
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __lhpOpExporterCSVGraph_H__
#define __lhpOpExporterCSVGraph_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"
#include "btkAcquisitionFileWriter.h"
#include "btkForcePlatformsExtractor.h"
#include "btkGroundReactionWrenchFilter.h"
#include "btkForcePlatformWrenchFilter.h"
#include <map>
#include "mafViewIntGraph.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;
class mafVMEMesh;
class mafEvent;
class mafVMELandmarkCloud;
class mafVMESurface;
class mafVMEVector;
class medVMEAnalog;
class mafVMEGroup;

//----------------------------------------------------------------------------
// lhpOpExporterCSVGraph :
//----------------------------------------------------------------------------
class lhpOpExporterCSVGraph : public mafOp
{
public:
  lhpOpExporterCSVGraph(const mafString& label = _R("CSV Graph Exporter"));
  ~lhpOpExporterCSVGraph() override; 

  mafTypeMacro(lhpOpExporterCSVGraph, mafOp);

  void OnEvent(mafEventBase *maf_event) override;

  mafOp* Copy() override;

  /** Return true for the acceptable vme type. */
  bool Accept(mafNode *node) override;

  /** Build the interface of the operation, i.e the dialog that let choose the name of the output file. */
  void OpRun() override;

  /** Export landmarks contained into a mafVMELandmarkCloud.*/
  void ExportGraphs();

  /** Set the filename for the .stl to export */
  void SetFileName(const mafString& file_name) {m_File = file_name;};

  void SetPSCFileName(const mafString& filename){ m_PscScriptFileName = filename; LoadScript(); }

  static mafViewIntGraph          *m_ViewIntGraph;
protected:
  void LoadScript();

  std::vector<bool>        m_shown_flags;
  std::vector<std::string> m_pipe_config;
  mafString                m_PscScriptFileName;


  mafString	m_File;
  mafString	m_FileDir;
  int       m_GlobalPos;
  int       m_Subtree;
};
#endif
