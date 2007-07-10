/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoHelAxis.h,v $
  Language:  C++
  Date:      $Date: 2007-07-10 20:43:13 $
  Version:   $Revision: 1.1 $
  Authors:   Fedor Moiseev / Vladik Aranov
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#ifndef __mmoHelAxis_H__
#define __mmoHelAxis_H__

#ifdef __GNUG__
    #pragma interface "mmoHelAxis.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "mafVME.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMEHelAxis;
class mafOp;
class mafGui;
class mafEvent;
class mafIntGraphHyer;
class mafVMELandmarkCloud;
class vtkPoints;

class mmoHelAxis: public mafOp
{
public:
  mmoHelAxis(wxString label);
 ~mmoHelAxis(); 

  virtual void OnEvent(mafEventBase *maf_event);
  mafOp* Copy();

  bool Accept(mafNode* vme);   
  void OpRun();
  void OpDo();
  void OpUndo();
  void CreateGui();

  static double RegisterPoints(vtkPoints *pointsSource, vtkPoints *pointsTarget, int &numPoints, mafVMELandmarkCloud *src, float time1, float time2, vtkMatrix4x4 *res_matrix);
protected: 

  void OpStop(int result);

  mafVMEHelAxis   *m_HelicalSys;
  mafIntGraphHyer *m_Hierarchy;
  wxString         m_DictionaryFName;
};
#endif
