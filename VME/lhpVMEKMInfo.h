/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpVMEKMInfo.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 07:05:59 $
  Version:   $Revision: 1.9 $
  Authors:   Marco Petrone , Stefano Perticoni
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __lhpVMEKMInfo_h
#define __lhpVMEKMInfo_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVMEGenericAbstract.h"
#include "mafVMEOutputNULL.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafGUI;
class mafTransform;

/** lhpVMEKMInfo - a VME use to create hierarchical assemblies of VME's.
  lhpVMEKMInfo is a specialized VME inheriting the VME-Generic features to internally
  store pose matrices, used to create assemblies of VMEs and direct cinematic. 
  @sa mafVME mafMatrixVector */
class MAF_EXPORT lhpVMEKMInfo : public mafVMEGenericAbstract
{
public:
  mafTypeMacro(lhpVMEKMInfo,mafVMEGenericAbstract);

  /** Precess events coming from other objects */ 
  virtual void OnEvent(mafEventBase *maf_event);

  /** Return the right type of output.*/  
  mafVMEOutputNULL *GetVTKOutput() {return (mafVMEOutputNULL *)GetOutput();}

  /** Return the output. This create the output object on demand. */  
  virtual mafVMEOutput *GetOutput();

  /** return icon */
  static char** GetIcon();
  
  /** override superclass */
  void SetMatrix(const mafMatrix &mat);

  /** override superclass */
  void Print(std::ostream& os, const int tabs);

  /** return icon */
  bool Equals(mafVME *vme);

  /** return icon */
  int DeepCopy(mafNode *a);

  double GetValue(int index){return m_values[index];}
  void SetValue(int index, double val){m_values[index] = val;}

protected:
  lhpVMEKMInfo();
  virtual ~lhpVMEKMInfo();

  /** Internally used to create a new instance of the GUI.*/
  virtual mafGUI *CreateGui();

  mafTransform *m_Transform;

  void InternalStore(mafStorageElementBuilder& parent) override;
  void InternalRestore(const mafStorageElement& node) override;

  double m_values[200];
private:
  lhpVMEKMInfo(const lhpVMEKMInfo&); // Not implemented
  void operator=(const lhpVMEKMInfo&); // Not implemented
};

#endif
