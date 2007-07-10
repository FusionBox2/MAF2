/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoTimeReduce.h,v $
  Language:  C++
  Date:      $Date: 2007-07-10 22:35:05 $
  Version:   $Revision: 1.1 $
  Authors:   Fedor Moiseev
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#ifndef __mmoTimeReduce_H__
#define __mmoTimeReduce_H__

#ifdef __GNUG__
    #pragma interface "mmoTimeReduce.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMEAFRefSys;
class mafOp;
class mafGui;
class mafEvent;
class mafIntGraphHyer;
class mafVMELandmarkCloud;
class vtkPoints;

//----------------------------------------------------------------------------
// mmoRefSys :
//----------------------------------------------------------------------------
/** */
class mmoTimeReduce: public mafOp
{
public:
  mmoTimeReduce(const wxString& label);
 ~mmoTimeReduce(); 

  virtual void OnEvent(mafEventBase *maf_event);
  mafOp* Copy();

  bool Accept(mafNode* vme);   
  void OpRun();
  void OpDo();
  void OpUndo();
  void CreateGui();

protected: 

  void OpStop(int result);
private:
  int m_delete;
  int m_number;
};
#endif
