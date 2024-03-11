/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpFingerStick.h,v $
  Language:  C++
  Date:      $Date: 2007-02-12 17:44:18 $
  Version:   $Revision: 1.1 $
  Authors:   Vladik Aranov
==========================================================================
  Copyright (c) 2002/2005
  ULB - Free University of Brussels (www.ulb.ac.be)
=========================================================================*/
#ifndef __lhpOpFingerStick_H__
#define __lhpOpFingerStick_H__

#include "mafDefines.h"


#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif


//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;
class mafOp;
class mafEvent;
class mafVMELandmarkCloud;
class vtkPoints;
class vtkMatrix4x4;
class vtkWeightedLandmarkTransform;

//----------------------------------------------------------------------------
// lhpOpRefSys :
//----------------------------------------------------------------------------
/** */
class lhpOpFingerStick: public mafOp
{
public:
  mafTypeMacro(lhpOpFingerStick, mafOp)
  lhpOpFingerStick(const mafString& label = _R(""));
 ~lhpOpFingerStick(); 

 virtual void OnEvent(mafEventBase *maf_event);
  mafOp* Copy();

  bool Accept(mafNode* vme);   
  void OpRun();
  void OpDo();
  void CreateGui();

protected: 
  bool ReadLMDictionary(mafString *fileName);
  void ParseString(wxString &pFirstLine, wxString &sOne, wxString &sTwo);
  bool ProcessSingleLM(int lmIndex, double result[3]);

  int  ExtractMatchingPoints(mafVMELandmarkCloud *src, mafVMELandmarkCloud *trg, mafTimeStamp srctime, mafTimeStamp trgtime);
  void RegisterPoints(vtkMatrix4x4 *res_matrix, std::vector<double>& devs);


  void OpStop(int result);

  ///
  mafVMELandmarkCloud  *m_PlateCalibration;
  mafString            m_PlateCalibrationName;
  ///
  mafVMELandmarkCloud  *m_BallsCalibration;
  mafString             m_BallsCalibrationName;
  ///
  mafVMELandmarkCloud  *m_PalpatorCalibration;
  mafString             m_PalpatorCalibrationName;
  ///
  mafVMELandmarkCloud  *m_PlateCloud;
  mafString             m_PlateCloudName;
  ///
  mafVMELandmarkCloud  *m_PalpatorCloud;
  mafString             m_PalpatorCloudName;


  std::vector<std::pair<wxString, wxString> >         m_LMDict;

  mafString             m_ListFName;


  mafVMELandmarkCloud*  m_Registered;
  double*               m_weight;

  int                   m_registration_mode; 

  //arrays for registration transform calculation
  vtkPoints             *m_pointsSource;
  vtkPoints             *m_pointsTarget;
  int                   m_numPoints;

  ///filename the relationship between tip and limb cloud will be stored in:
  /// 3 float numbers
  mafString        m_RelFName;
  
  ///shitoza: see name
  mafString        m_DictionaryFName;

  wxInt32          m_Method;

  /**Just a misc. stuff*/
  void SetNodeName(mafVME *pVME, mafString *pName);
  ///work variables
  double           m_LocalTip[4];
  mafString        m_NewLandmarkName;
  vtkWeightedLandmarkTransform *m_RegisterTransform;

private:
  static   mafVMELandmarkCloud *m_StickDefinitionPersistent;
  /// index of new landmark for undo opration 
  wxInt32                       m_NewIndex;
};
#endif
