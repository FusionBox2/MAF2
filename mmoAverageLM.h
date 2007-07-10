/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoAverageLM.h,v $
  Language:  C++
  Date:      $Date: 2007-07-10 22:35:24 $
  Version:   $Revision: 1.2 $
  Authors:   Fedor Moiseev / Vladik Aranov
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#ifndef __mmoAverageLM_H__
#define __mmoAverageLM_H__

#ifdef __GNUG__
    #pragma interface "mmoAverageLM.cpp"
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
class mmoAverageLM: public mafOp
{
public:
  mmoAverageLM(const wxString& label);
 ~mmoAverageLM(); 

  virtual void OnEvent(mafEventBase *maf_event);
  mafOp* Copy();

  bool Accept(mafNode* vme);   
  void OpRun();
  void OpDo();
  void OpUndo();
  void CreateGui();

protected: 

  void OpStop(int result);

  /// limb cloud from motion: animated, we will insert stick tip here
  mafVMELandmarkCloud  *m_LimbCloud;

private:
  /// index of new landmark for undo opration 
  wxInt32                       m_NewIndex;
};
#endif
