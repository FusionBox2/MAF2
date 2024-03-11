/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpImporterC3DFused.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 11:14:48 $
  Version:   $Revision: 1.2 $
  Authors:   Paolo Quadrani      - porting Daniele Giunchi
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __lhpOpImporterC3DFused_H__
#define __lhpOpImporterC3DFused_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMEGroup.h"
#include "mafVMESurface.h"
#include "mafVMEInfoText.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafGUI;
class mafEvent;
class mafGUIDialog;
class vtkPoints;

//----------------------------------------------------------------------------
// lhpOpImporterC3DFused :
//----------------------------------------------------------------------------
/** */
class lhpOpImporterC3DFused: public mafOp
{
public:
  lhpOpImporterC3DFused(const mafString& label = _L("Import C3d with auto fuse"));
 ~lhpOpImporterC3DFused(); 
  virtual void OnEvent(mafEventBase *maf_event);
  
  mafTypeMacro(lhpOpImporterC3DFused, mafOp);
  
  mafOp* Copy();

  /** Return true for the acceptable vme type. */
  bool Accept(mafNode* node);   

  /** Builds operation's interface. */
  void OpRun();

  /** Execute the operation. */
  void OpDo();

  /** Makes the undo for the operation. */
  void OpUndo();

  static bool ClosedCloudAccept(mafNode* node) {if(node != NULL && node->IsA("mafVMELandmarkCloud") && !((mafVMELandmarkCloud*)node)->IsOpen())return true;return false;}

  bool ReadLMDictionary(mafString *fileName);
protected:
  void ParseString(wxString &pFirstLine, wxString &sOne, wxString &sTwo);

  /** Method called to extract matching point between source and target.*/
  int ExtractMatchingPoints(mafVMELandmarkCloud *src, mafVMELandmarkCloud *trg, double time = -1);
  
  /** Register the source  on the target  according 
  to the registration method selected: rigid, similar or affine. */
  double RegisterPoints(mafVMELandmarkCloud *src, mafVMELandmarkCloud *trg, mafVMELandmarkCloud *reg, double currTime = -1);

   /** Check the correctness of the vme's type. */
  bool CheckMultiTime(mafNode *vme);

  bool RegistrationProcedure();

  bool ProcessNode(mafVMELandmarkCloud *src, mafVMELandmarkCloud *trg, mafVMELandmarkCloud *registered, bool multiTime);

  mafVME*      m_Source;

  std::vector<mafVME*> m_Imported;
  std::vector<mafVME*> m_Registered;

  mafString   m_SourceName;
  
  std::vector<std::pair<wxString, wxString> >  m_LMDict;
  mafString                                    m_ListFName;

  std::vector<mafString>         m_C3DInputFileNameFullPaths;

  mafString                      m_DictionaryFileName;
  mafString                      m_LMRenameFileName;
  mafString                      m_PscScriptFileName;
  bool                           m_CSVExported;
  vtkPoints *m_PointsSource;
  vtkPoints *m_PointsTarget;
  
  int m_RegistrationMode; 

};
#endif
