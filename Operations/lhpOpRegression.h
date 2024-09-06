/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpRegression.h,v $
  Language:  C++
  Date:      $Date: 2007-08-22 14:01:40 $
  Version:   $Revision: 1.1 $
  Authors:   Fedor Moiseev / Vladik Aranov
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#ifndef __lhpOpRegression_H__
#define __lhpOpRegression_H__

//----------------------------------------------------------------------------
// Includes:
//----------------------------------------------------------------------------
#include "mafOp.h"
#include "mafVectors.h"
#include "mafDynamicLoader.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;
class mafGUI;
class mafEvent;
class mafVMELandmarkCloud;

//----------------------------------------------------------------------------
// lhpOpRegression :
//----------------------------------------------------------------------------
/** */
class lhpOpRegression: public mafOp
{
public:
  static bool Config(LibHandle handle);
  mafTypeMacro(lhpOpRegression, mafOp)
  lhpOpRegression(const mafString& label= _R(""));
 ~lhpOpRegression() override;

  void OnEvent(mafEventBase *maf_event) override;
  mafOp* Copy() override;

  bool Accept(mafNode* vme) override;   
  void OpRun() override;
  void OpDo() override;
  void OpUndo() override;
  void CreateGui();

  enum SPLINE_WIDGET_ID
  {
    ID_BONETYPE = MINID,
    ID_APPROX,
    ID_LAST
  };


protected: 

  void OpStop(int result) override;
  bool Regression();
  bool RegressionPelvis(bool right = true);
  bool RegressionFemur(bool right = true);
  bool RegressionHumerus(bool right = true);
  bool RegressionScapula(bool right = true);
  bool RegressionClavicle(bool right = true);
  void CreateGuiBase();
  void CreateGuiPelvis();
  void CreateGuiFemur();
  void CreateGuiHumerus();
  void CreateGuiScapula();
  void CreateGuiClavicle();
  void AddSphere(const mafString& name, double *center, double radius);
  void AddPlane(const mafString& name, double *center, double *normal);
  void AddArrow(const mafString& name, double *center, double *normal);
  void AddEllipsoid(const mafString& name, const mafMatrix &mtr, const double *radius);

  /// limb cloud from motion: animated, we will insert stick tip here
  mafVMELandmarkCloud  *m_Cloud;

private:
  /// index of new landmark for undo operation 
  wxInt32                                 m_BoneType;
  wxInt32                                 m_Level;
  wxInt32                                 m_Approximate[10];
  std::vector<mafVME*>                    m_Added;
  double                                  m_Length;
  struct LMAdding
  {
    mafString   m_Name;
    V3d<double> m_Pos;
    int         m_Index;
    bool        m_Pelvic;
  };
  std::vector<LMAdding> m_LMAdd;

  static bool m_Inited;
  static bool (*Regr_4_Als)(double glob[], double loc[], double rad[], double transf[], double p1[], double p2[], double p3[], double p4[], bool pelvic, bool left);
  static bool (*Regr_3_Als)(double V1[], double V2[], double V3[], double V4[], double FTC[], double FME[], double FLE[], bool left);

};
#endif
