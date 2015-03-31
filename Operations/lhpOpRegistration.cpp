/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpRegistration.cpp,v $
  Language:  C++
  Date:      $Date: 2007-10-24 11:11:29 $
  Version:   $Revision: 1.2 $
  Authors:   Fedor Moiseev
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "lhpOpRegistration.h"
#include "wx/busyinfo.h"

#include "mafDecl.h"
#include "mafEvent.h"
#include "mafGUI.h"
#include "mafTransform.h"
#include "mafTransformFrame.h"

#include "mafSmartPointer.h"
#include "mafMatrix3x3.h"
#include "mafDataVector.h"
#include "mafMatrixVector.h"
#include "mafVMEAFRefSys.h"
#include "mafVMEGroup.h"
#include "mafVMELandmarkCloud.h"
#include "mafJointAnalysis.h"
#include "mafPlotMath.h"
#include "nr.h"

//----------------------------------------------------------------------------
// Required for MSVC
//----------------------------------------------------------------------------
#ifdef _MSC_FULL_VER
#pragma warning (disable: 4786)
#endif

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Forward Refs
//----------------------------------------------------------------------------



class RegData
{
public:
  RegData();
  virtual ~RegData();
  virtual bool SetValuesList(int ID,
    const std::vector<double> &px, const std::vector<double> &py, const std::vector<double> &pz, 
    const std::vector<double> &rx, const std::vector<double> &ry, const std::vector<double> &rz);

  virtual bool IsIDValid(int ID) = 0;

  virtual bool IsDimValid(int ID, unsigned dim) = 0;

  virtual bool GetValues(int ID, double &px, double &py, double &pz, double &rx, double &ry, double &rz)= 0;

  virtual bool ExtractFreeValues(int ID, double &px, double &py, double &pz, double &rx, double &ry, double &rz, std::vector<double> &values, std::vector<double>& deltas) = 0;
  virtual bool GetValues(int ID, std::vector<double>::iterator &values, double &px, double &py, double &pz, double &rx, double &ry, double &rz) = 0;

  void SetLeftSize (double size);
  void SetRightSize(double size);

  double GetLeftSize (){return m_LeftSize;}
  double GetRightSize(){return m_RightSize;}
protected:
  virtual bool OnSetVectors(int ID) = 0;
protected:
  class _vecs
  {
  public:
    std::vector<double>& operator[](int i){return m_vecs[i];}
    const std::vector<double>& operator[](int i)const{return m_vecs[i];}
  private:
    std::vector<double> m_vecs[6];
  };

  double               m_LeftSize,  m_RightSize;
  std::map<int, _vecs> m_Data;
private:
  RegData(const mafVMERegData&); // Not implemented
  void operator=(const RegData&); // Not implemented
};

class RegDataRef : public RegData
{
public:
  RegDataRef();
  virtual ~RegDataRef();
  virtual bool IsIDValid(int ID){return (mafVMEAFRefSys::ID_AFS_NOTDEFINED != ID && mafVMEAFRefSys::ID_AFS_PELVIS !=  ID);}
  virtual bool IsDimValid(int ID, unsigned dim){if(mafVMEAFRefSys::ID_AFS_RTHIGH == ID || mafVMEAFRefSys::ID_AFS_LTHIGH == ID)return dim >= 0; return dim >=6;}
  virtual bool GetValues(int ID, double &px, double &py, double &pz, double &rx, double &ry, double &rz);
  virtual bool ExtractFreeValues(int ID, double &px, double &py, double &pz, double &rx, double &ry, double &rz, std::vector<double> &values, std::vector<double>& deltas);
  virtual bool GetValues(int ID, std::vector<double>::iterator &values, double &px, double &py, double &pz, double &rx, double &ry, double &rz);
protected:
  virtual bool OnSetVectors(int ID);
private:
  struct _regression
  {
  public:
    std::vector<double>& operator[](int i){return m_coefs[i];}
    const std::vector<double>& operator[](int i)const{return m_coefs[i];}
  private:
    std::vector<double> m_coefs[5];
  };
  std::map<int, _regression> m_RegCoefs;

  RegDataRef(const RegDataRef&); // Not implemented
  void operator=(const RegDataRef&); // Not implemented
};

class RegDataSph : public RegData
{
public:
  RegDataSph();
  virtual ~RegDataSph();
  virtual bool IsIDValid(int ID){return (mafVMEAFRefSys::ID_AFS_NOTDEFINED != ID && mafVMEAFRefSys::ID_AFS_PELVIS !=  ID);}
  virtual bool IsDimValid(int ID, unsigned dim){if(mafVMEAFRefSys::ID_AFS_RTHIGH == ID || mafVMEAFRefSys::ID_AFS_LTHIGH == ID)return dim >= 0; return dim >=6;}
  virtual bool GetValues(int ID, double &px, double &py, double &pz, double &rx, double &ry, double &rz);
  virtual bool ExtractFreeValues(int ID, double &px, double &py, double &pz, double &rx, double &ry, double &rz, std::vector<double> &values, std::vector<double>& deltas);
  virtual bool GetValues(int ID, std::vector<double>::iterator &values, double &px, double &py, double &pz, double &rx, double &ry, double &rz);
protected:
  virtual bool OnSetVectors(int ID);
private:

  RegDataSph(const RegDataSph&); // Not implemented
  void operator=(const RegDataSph&); // Not implemented
};


//-------------------------------------------------------------------------
RegData::RegData()
//-------------------------------------------------------------------------
{
  m_LeftSize   = 0.0;
  m_RightSize  = 0.0;
}

//-------------------------------------------------------------------------
RegData::~RegData()
//-------------------------------------------------------------------------
{
}


bool RegData::SetValuesList(int ID,
                            const std::vector<double> &px, const std::vector<double> &py, const std::vector<double> &pz, 
                            const std::vector<double> &rx, const std::vector<double> &ry, const std::vector<double> &rz)
{
  if(!IsIDValid(ID))
    return false;
  unsigned dim = px.size();
  if(!IsDimValid(ID, dim))
    return false;
  if(py.size() != dim || pz.size() != dim || rx.size() != dim || ry.size() != dim || rz.size() != dim)
    return false;

  _vecs vectors;
  m_Data[ID] = vectors;
  _vecs &vecs = m_Data.find(ID)->second;
  vecs[0] = px;
  vecs[1] = py;
  vecs[2] = pz;
  vecs[3] = rx;
  vecs[4] = ry;
  vecs[5] = rz;
  return OnSetVectors(ID);
}

void RegData::SetLeftSize (double size)
{
  m_LeftSize  = size;
}
void RegData::SetRightSize(double size)
{
  m_RightSize = size;
}



//-------------------------------------------------------------------------
RegDataRef::RegDataRef()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
RegDataRef::~RegDataRef()
//-------------------------------------------------------------------------
{
}

static void funcs(const DP x, Vec_O_DP &afunc)
{
  int i;
  int dim = afunc.size();

  afunc[dim - 1] = 1.0;
  for(i = dim - 1; i > 0; i--)
    afunc[i - 1] = afunc[i] * x;
}

static int leastsq(std::vector<double>& XVec, std::vector<double>& YVec, std::vector<double>& ResCoefs, int ResDeg)
{
  assert(XVec.size() == YVec.size());
  int VecSize = XVec.size();
  int NPT=VecSize,NTERM=ResDeg + 1;
  int i;
  DP chisq;
  Vec_BOOL ia(NTERM);
  Vec_DP   a(NTERM),x(NPT),y(NPT),sig(NPT);
  Mat_DP   covar(NTERM,NTERM);

  for (i=0;i<NPT;i++) {
    x[i]   = XVec[i];
    y[i]   = YVec[i];
    sig[i] = 1.0;
  }
  for (i=0;i<NTERM;i++) 
    ia[i]=true;
  NR::lfit(x,y,sig,a,ia,covar,chisq,funcs);
  ResCoefs.resize(ResDeg + 1);
  for(i = 0; i <= ResDeg; i++)
    ResCoefs[i] = a[i];
  return 0;
}

static double _polynom(const std::vector<double>& coef, double value)
{
  int nI;
  int degree = coef.size() - 1;
  double res = 0.0f;
  for(nI = 0; nI <= degree; nI++)
  {
    res *= value;
    res += coef[nI];
  }
  return res;
}
bool RegDataRef::OnSetVectors(int ID)
{
  if(ID == mafVMEAFRefSys::ID_AFS_NOTDEFINED)
    return false;
  if(ID == mafVMEAFRefSys::ID_AFS_PELVIS || ID == mafVMEAFRefSys::ID_AFS_LTHIGH || ID == mafVMEAFRefSys::ID_AFS_RTHIGH)
    return true;


  _vecs &vecs = m_Data.find(ID)->second;

  _regression regr;
  m_RegCoefs[ID] = regr;
  _regression &regCoefs = m_RegCoefs.find(ID)->second;

  for(unsigned i = 0; i < 5; i++)
  {
    std::vector<double> coefs;
    leastsq(vecs[5], vecs[i], coefs, 5);
    regCoefs[i] = coefs;
  }
  return true;
}

bool RegDataRef::GetValues(int ID, double &px, double &py, double &pz, double &rx, double &ry, double &rz)
{
  if(ID == mafVMEAFRefSys::ID_AFS_NOTDEFINED)
    return false;
  if(ID == mafVMEAFRefSys::ID_AFS_PELVIS)
    return true;
  if(ID == mafVMEAFRefSys::ID_AFS_LTHIGH || ID == mafVMEAFRefSys::ID_AFS_RTHIGH)
  {
    px = 0.0;
    py = 0.0;
    pz = 0.0;
    return true;
  }
  std::map<int, _regression>::iterator it =  m_RegCoefs.find(ID);
  if(it == m_RegCoefs.end())
    return false;
  px = _polynom(it->second[0], rz);
  py = _polynom(it->second[1], rz);
  pz = _polynom(it->second[2], rz);
  rx = _polynom(it->second[3], rz);
  ry = _polynom(it->second[4], rz);
  return true;
}

bool RegDataRef::ExtractFreeValues(int ID, double &px, double &py, double &pz, double &rx, double &ry, double &rz, std::vector<double> &values, std::vector<double>& deltas)
{
  //values.clear();
  //deltas.clear();
  if(ID == mafVMEAFRefSys::ID_AFS_NOTDEFINED)
    return true;
  if(ID == mafVMEAFRefSys::ID_AFS_PELVIS)
    return true;
  if(ID == mafVMEAFRefSys::ID_AFS_LTHIGH || ID == mafVMEAFRefSys::ID_AFS_RTHIGH)
  {
    values.push_back(rx);
    deltas.push_back(10.0);
    values.push_back(ry);
    deltas.push_back(10.0);
    values.push_back(rz);
    deltas.push_back(10.0);
    return true;
  }
  values.push_back(rz);
  deltas.push_back(10.0);
  return true;
}
bool RegDataRef::GetValues(int ID, std::vector<double>::iterator &values, double &px, double &py, double &pz, double &rx, double &ry, double &rz)
{
  if(ID == mafVMEAFRefSys::ID_AFS_NOTDEFINED)
    return false;
  if(ID == mafVMEAFRefSys::ID_AFS_PELVIS)
    return false;
  if(ID == mafVMEAFRefSys::ID_AFS_LTHIGH || ID == mafVMEAFRefSys::ID_AFS_RTHIGH)
  {
    rx = *(values++);
    ry = *(values++);
    rz = *(values++);
  }
  else
  {
    rz = *(values++);
  }
  return GetValues(ID, px, py, pz, rx, ry, rz);
}

//-------------------------------------------------------------------------
RegDataSph::RegDataSph()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
RegDataSph::~RegDataSph()
//-------------------------------------------------------------------------
{
}

bool RegDataSph::OnSetVectors(int ID)
{
  if(ID == mafVMEAFRefSys::ID_AFS_NOTDEFINED)
    return false;
  //if(ID == mafVMEAFRefSys::ID_AFS_PELVIS || ID == mafVMEAFRefSys::ID_AFS_LTHIGH || ID == mafVMEAFRefSys::ID_AFS_RTHIGH)
  return true;
}

bool RegDataSph::GetValues(int ID, double &px, double &py, double &pz, double &rx, double &ry, double &rz)
{
  if(ID == mafVMEAFRefSys::ID_AFS_NOTDEFINED)
    return false;
  if(ID == mafVMEAFRefSys::ID_AFS_PELVIS)
    return true;
  //if(ID == mafVMEAFRefSys::ID_AFS_LTHIGH || ID == mafVMEAFRefSys::ID_AFS_RTHIGH)
  {
    px = 0.0;
    py = 0.0;
    pz = 0.0;
    return true;
  }
  return true;
}

bool RegDataSph::ExtractFreeValues(int ID, double &px, double &py, double &pz, double &rx, double &ry, double &rz, std::vector<double> &values, std::vector<double>& deltas)
{
  //values.clear();
  //deltas.clear();
  if(ID == mafVMEAFRefSys::ID_AFS_NOTDEFINED)
    return true;
  if(ID == mafVMEAFRefSys::ID_AFS_PELVIS)
    return true;
  //if(ID == mafVMEAFRefSys::ID_AFS_LTHIGH || ID == mafVMEAFRefSys::ID_AFS_RTHIGH)
  {
    values.push_back(rx);
    deltas.push_back(10.0);
    values.push_back(ry);
    deltas.push_back(10.0);
    values.push_back(rz);
    deltas.push_back(10.0);
    return true;
  }
  return true;
}
bool RegDataSph::GetValues(int ID, std::vector<double>::iterator &values, double &px, double &py, double &pz, double &rx, double &ry, double &rz)
{
  if(ID == mafVMEAFRefSys::ID_AFS_NOTDEFINED)
    return false;
  if(ID == mafVMEAFRefSys::ID_AFS_PELVIS)
    return false;
  //if(ID == mafVMEAFRefSys::ID_AFS_LTHIGH || ID == mafVMEAFRefSys::ID_AFS_RTHIGH)
  {
    rx = *(values++);
    ry = *(values++);
    rz = *(values++);
  }
  return GetValues(ID, px, py, pz, rx, ry, rz);
}

//----------------------------------------------------------------------------
bool FillRegData(std::map<int, mafVMELandmarkCloud*>& mp, mafTimeStamp tsRef, RegData *regData)
//----------------------------------------------------------------------------
{
  mafVMELandmarkCloud *plv = NULL;
  mafVMELandmarkCloud *lft = NULL;
  mafVMELandmarkCloud *rft = NULL;

  for(std::map<int, mafVMELandmarkCloud*>::iterator it = mp.begin(); it != mp.end(); ++it)
  {
    if(it->first == mafVMEAFRefSys::ID_AFS_PELVIS)
    {
      plv = it->second;
      continue;
    }
    else if(it->first == mafVMEAFRefSys::ID_AFS_RFOOT)
    {
      rft = it->second;
    }
    else if(it->first == mafVMEAFRefSys::ID_AFS_LFOOT)
    {
      lft = it->second;
    }
    std::vector<mafTimeStamp> stamps;
    it->second->GetTimeStamps(stamps);
    std::vector<double> px, py, pz, rx, ry, rz;
    for(unsigned i = 0; i < stamps.size(); i++)
    {
      if(stamps[i] == tsRef)
        continue;
      V4d<double> ps, rt;
      OVP_GES(it->second, stamps[i], tsRef, &ps, &rt, NULL, NULL);
      px.push_back(ps.x);
      py.push_back(ps.y);
      pz.push_back(ps.z);
      rx.push_back(rt.x * mafMatrix3x3::RadiansToDegrees());
      ry.push_back(rt.y * mafMatrix3x3::RadiansToDegrees());
      rz.push_back(rt.z * mafMatrix3x3::RadiansToDegrees());
    }
    regData->SetValuesList(it->first, px, py, pz, rx, ry, rz);
  }

  if(plv == NULL || rft == NULL || lft == NULL)
    return true;

  mafVMEAFRefSys *plRS = GetAFRefSys(plv);
  mafVMEAFRefSys *rfRS = GetAFRefSys(rft);
  mafVMEAFRefSys *lfRS = GetAFRefSys(lft);

  if(plRS == NULL || rfRS == NULL || lfRS == NULL)
    return true;

  V3d<double> vRIAC, vLIAC;
  V3d<double> vRFCC, vLFCC;

  if(plRS->GetVector("RIAC", tsRef, vRIAC) && plRS->GetVector("LIAC", tsRef, vLIAC) &&
     rfRS->GetVector("RFCC", tsRef, vRFCC) && lfRS->GetVector("LFCC", tsRef, vLFCC))
  {
    regData->SetLeftSize(sqrt(vLIAC | vLFCC));
    regData->SetRightSize(sqrt(vRIAC | vRFCC));
  }
  return true;
}


//----------------------------------------------------------------------------
lhpOpRegistration::lhpOpRegistration(const wxString& label) : mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType      = OPTYPE_OP;
  m_Canundo     = true;
  m_RegTarget   = NULL;
  m_RegTrgName  = "";
  m_RefStampTrg = 0.0;
  m_RefStampSrc = 0.0;

  m_Scale       = true;
  m_Primary     = true;
  m_Advanced    = true;
  m_Regression  = true;
  m_Result      = NULL;
}

//----------------------------------------------------------------------------
lhpOpRegistration::~lhpOpRegistration()
//----------------------------------------------------------------------------
{
  mafDEL(m_Result);
}

//----------------------------------------------------------------------------
mafOp* lhpOpRegistration::Copy()   
//----------------------------------------------------------------------------
{
  return new lhpOpRegistration(m_Label);
}

static bool fillMap(mafVMELandmarkCloud *lmc, int ID, std::map<int, mafVMELandmarkCloud*>& clouds)
{
  if(lmc == NULL)
    return false;
  mafVMEAFRefSys *sys = GetAFRefSys(lmc);
  std::vector<int> childrenIDs;

  if(ID == mafVMEAFRefSys::ID_AFS_NOTDEFINED)
    return false;
  
  if(sys == NULL || sys->GetBoneID() != ID)
    return false;

  for(int childID = mafVMEAFRefSys::ID_AFS_FIRST; childID < mafVMEAFRefSys::ID_AFS_LAST; childID++)
  {
    if(FindParentID(childID) == ID)
      childrenIDs.push_back(childID);
  }

  for(unsigned j = 0; j < childrenIDs.size(); j++)
  {
    for(unsigned i = 0; i < lmc->GetNumberOfChildren(); i++)
    {
      mafVMELandmarkCloud *lmcChild = mafVMELandmarkCloud::SafeDownCast(lmc->GetChild(i));
      if(lmcChild == NULL)
        continue;
      mafVMEAFRefSys *childSys = GetAFRefSys(lmcChild);
      if(childSys == NULL || childSys->GetBoneID() != childrenIDs[j])
        continue;
      if(!fillMap(lmcChild, childrenIDs[j], clouds))
        return false;
    }
  }
  clouds[ID] = lmc;
  return true;
}

//----------------------------------------------------------------------------
bool lhpOpRegistration::Accept(mafNode* vme)
//----------------------------------------------------------------------------
{
  mafVMELandmarkCloud *lmc = mafVMELandmarkCloud::SafeDownCast(vme);
  if(!lmc) 
    return false;
  mafVMEAFRefSys *sys = GetAFRefSys(lmc);
  if(sys == NULL || sys->GetBoneID() == mafVMEAFRefSys::ID_AFS_NOTDEFINED)
    return false;
  std::map<int, mafVMELandmarkCloud*> mp;
  if(!fillMap(lmc, sys->GetBoneID(), mp))
    return false;
  return true;
}

//----------------------------------------------------------------------------
void lhpOpRegistration::OpRun()   
//----------------------------------------------------------------------------
{
  CreateGui();
}

//----------------------------------------------------------------------------
void lhpOpRegistration::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new mafGUI(this);
  m_Gui->SetListener(this);
  m_Gui->Button(ID_REGTRG, "Reg target", "", "Press to select." );  
  m_Gui->Label("Wnd d.VME",&m_RegTrgName);
  m_Gui->Bool(ID_SCALE, "Scale", &m_Scale);
  m_Gui->Bool(ID_PRIMARY, "Primary", &m_Primary);
  m_Gui->Bool(ID_ADVANCED, "Advanced", &m_Advanced);
  m_Gui->Bool(ID_REGRESSION, "Regression", &m_Regression);
  m_Gui->OkCancel();
  ShowGui();
}

bool lhpOpRegistration::StampsIdentical(std::map<int, mafVMELandmarkCloud*>& mp, std::vector<mafTimeStamp>& stamps)

{
  std::map<int, mafVMELandmarkCloud*>::iterator it = mp.begin();
  if (it == mp.end())
    return false;//???
  it->second->GetTimeStamps(stamps);
  ++it;
  for(;it != mp.end(); ++it)
  {
    std::vector<mafTimeStamp> stampsNew;
    it->second->GetTimeStamps(stampsNew);
    if(stamps.size() != stampsNew.size())
      return false;
    for(unsigned jj = 0; jj < stampsNew.size(); jj++)
    {
      if(stamps[jj] != stampsNew[jj])
        return false;
    }
  }
  return true;
}

void GetOVPSpecial(mafVME *vme, mafTimeStamp ts, mafTimeStamp tsRef, 
                   V3d<double>& rplv, V3d<double>& rthg, V3d<double>& lplv, V3d<double>& lthg, 
                   V4d<double> *vOVPPos, V4d<double> *vOVPRot)
{
  V4d<double> vOVPPosInt;
  V4d<double> vOVPRotInt;

  V4d<double> *vOVPPosOut = (vOVPPos == NULL) ? &vOVPPosInt : vOVPPos;
  V4d<double> *vOVPRotOut = (vOVPRot == NULL) ? &vOVPRotInt : vOVPRot;
  vOVPPosOut->x = 0.0;vOVPPosOut->y = 0.0;vOVPPosOut->z = 0.0;vOVPPosOut->w = 0.0;
  vOVPRotOut->x = 0.0;vOVPRotOut->y = 0.0;vOVPRotOut->z = 0.0;vOVPRotOut->w = 0.0;
  if(vme == NULL)
    return;


  mafMatrix vmeMatr;
  mafMatrix vmeMatrRef;
  vme->GetOutput()->GetMatrix(vmeMatr, ts);
  vme->GetOutput()->GetMatrix(vmeMatrRef, tsRef);

  DiMatrix mLTM;
  V4d<double> vTm;
  mflMatrixToDi(vmeMatr.GetVTKMatrix(), &mLTM);
  mafTransfInverseTransformUpright(&mLTM, &vTm, vOVPRotOut);

  mafVMEAFRefSys *vmeSys = GetAFRefSys(vme);
  if(vmeSys == NULL || vmeSys->GetBoneID() == mafVMEAFRefSys::ID_AFS_NOTDEFINED)
    return;
  if(vmeSys->GetBoneID() == mafVMEAFRefSys::ID_AFS_PELVIS)
  {
    DiV4dCopy(&vTm, vOVPPosOut);
    return;
  }
  mafVME *parent = vme->GetParent();
  if(parent == NULL)
    return;
  mafVMEAFRefSys *parentSys = GetAFRefSys(parent);
  if(parentSys == NULL || parentSys->GetBoneID() == mafVMEAFRefSys::ID_AFS_NOTDEFINED || parentSys->GetBoneID() != FindParentID(vmeSys->GetBoneID()))
    return;


  if(vmeSys->GetBoneID() == mafVMEAFRefSys::ID_AFS_LTHIGH || vmeSys->GetBoneID() == mafVMEAFRefSys::ID_AFS_RTHIGH)
  {
    V3d<double> pelvisPnt;
    V3d<double> thighPnt;
    if(vmeSys->GetBoneID() == mafVMEAFRefSys::ID_AFS_RTHIGH)
    {
      pelvisPnt = rplv;
      thighPnt  = rthg;
    }
    else
    {
      pelvisPnt = lplv;
      thighPnt  = lthg;
    }
    double mltp[4];
    mltp[0] = thighPnt.x;
    mltp[1] = thighPnt.y;
    mltp[2] = thighPnt.z;
    mltp[3] = 1.0;

    vmeMatr.MultiplyPoint(mltp, mltp);
    thighPnt.x = mltp[0];
    thighPnt.y = mltp[1];
    thighPnt.z = mltp[2];

    V3d<double> res = thighPnt - pelvisPnt;
    vOVPPosOut->x = res.x;
    vOVPPosOut->y = res.y;
    vOVPPosOut->z = res.z;
  }
  else
  {
    mafMatrix multRes, tmp;
    tmp = vmeMatrRef;
    tmp.Invert();
    mafMatrix::Multiply4x4(vmeMatr, tmp, multRes);

    double transl[4];
    transl[0] = 0.0;
    transl[1] = 0.0;
    transl[2] = 0.0;
    transl[3] = 1.0;

    multRes.MultiplyPoint(transl, transl);

    vOVPPosOut->x = transl[0];
    vOVPPosOut->y = transl[1];
    vOVPPosOut->z = transl[2];
  }
}

void SetOVPSpecial(mafVME *vme, mafTimeStamp ts, mafTimeStamp tsRef, 
                   V3d<double>& rplv, V3d<double>& rthg, V3d<double>& lplv, V3d<double>& lthg, 
                   DiV4d *vOVPPos, DiV4d *vOVPRot)
{
  /*DiV4d vOVPPosInt;
  DiV4d vOVPRotInt;

  DiV4d *vOVPPosOut = &vOVPPosInt;
  DiV4d *vOVPRotOut = &vOVPRotInt;
  vOVPPosOut->x = 0.0;vOVPPosOut->y = 0.0;vOVPPosOut->z = 0.0;vOVPPosOut->w = 0.0;
  vOVPRotOut->x = 0.0;vOVPRotOut->y = 0.0;vOVPRotOut->z = 0.0;vOVPRotOut->w = 0.0;*/
  if(vme == NULL)
    return;

  DiMatrix  mLocNew;
  mafMatrix mLocalNew;
  DiV4d     vPosComp, vRotComp;
  vPosComp.x = vOVPPos->x;vPosComp.y = vOVPPos->y;vPosComp.z = vOVPPos->z;vPosComp.w = 1.0;
  vRotComp.x = vOVPRot->x;vRotComp.y = vOVPRot->y;vRotComp.z = vOVPRot->z;vRotComp.w = 1.0;
  mafTransfTransformUpright(&vPosComp, &vRotComp, &mLocNew);
  DiMatrixToVTK(&mLocNew, mLocalNew.GetVTKMatrix());
  mLocalNew.SetTimeStamp(ts);
  vme->SetMatrix(mLocalNew);
  vme->Modified();

  mafMatrix vmeMatr;
  mafMatrix vmeMatrRef;
  vme->GetOutput()->GetMatrix(vmeMatr, ts);
  vme->GetOutput()->GetMatrix(vmeMatrRef, tsRef);

  /*DiMatrix mLTM;
  DiV4d    vTm;
  mflMatrixToDi(vmeMatr.GetVTKMatrix(), &mLTM);
  mafTransfInverseTransformUpright(&mLTM, &vTm, vOVPRotOut);*/

  mafVMEAFRefSys *vmeSys = GetAFRefSys(vme);
  if(vmeSys == NULL || vmeSys->GetBoneID() == mafVMEAFRefSys::ID_AFS_NOTDEFINED)
    return;
  if(vmeSys->GetBoneID() == mafVMEAFRefSys::ID_AFS_PELVIS)
    return;
  mafVME *parent = vme->GetParent();
  if(parent == NULL)
    return;
  mafVMEAFRefSys *parentSys = GetAFRefSys(parent);
  if(parentSys == NULL || parentSys->GetBoneID() == mafVMEAFRefSys::ID_AFS_NOTDEFINED || parentSys->GetBoneID() != FindParentID(vmeSys->GetBoneID()))
    return;

  V3d<double> targetPos;
  targetPos.x = vOVPPos->x;
  targetPos.y = vOVPPos->y;
  targetPos.z = vOVPPos->z;

  if(vmeSys->GetBoneID() == mafVMEAFRefSys::ID_AFS_LTHIGH || vmeSys->GetBoneID() == mafVMEAFRefSys::ID_AFS_RTHIGH)
  {
    V3d<double> pelvisPnt;
    V3d<double> thighPnt;
    if(vmeSys->GetBoneID() == mafVMEAFRefSys::ID_AFS_RTHIGH)
    {
      pelvisPnt = rplv;
      thighPnt  = rthg;
    }
    else
    {
      pelvisPnt = lplv;
      thighPnt  = lthg;
    }
    double mltp[4];
    mltp[0] = thighPnt.x;
    mltp[1] = thighPnt.y;
    mltp[2] = thighPnt.z;
    mltp[3] = 0.0;

    vmeMatr.MultiplyPoint(mltp, mltp);
    thighPnt.x = mltp[0];
    thighPnt.y = mltp[1];
    thighPnt.z = mltp[2];

    V3d<double> transl = targetPos + pelvisPnt - thighPnt;

    vPosComp.x = transl.x;vPosComp.y = transl.y;vPosComp.z = transl.z;vPosComp.w = 1.0;
    vRotComp.x = vOVPRot->x;vRotComp.y = vOVPRot->y;vRotComp.z = vOVPRot->z;vRotComp.w = 1.0;
    mafTransfTransformUpright(&vPosComp, &vRotComp, &mLocNew);
    DiMatrixToVTK(&mLocNew, mLocalNew.GetVTKMatrix());
    mLocalNew.SetTimeStamp(ts);
    vme->SetMatrix(mLocalNew);
    vme->Modified();
  }
  else
  {
    mafMatrix multRes, tmp;
    tmp = vmeMatrRef;
    tmp.Invert();
    //mafMatrix::Multiply4x4(vmeMatr, tmp, multRes);

    double trl[4];
    trl[0] = 0.0;
    trl[1] = 0.0;
    trl[2] = 0.0;
    trl[3] = 1.0;

    tmp.MultiplyPoint(trl, trl);
    trl[3] = 0.0; 
    vmeMatr.MultiplyPoint(trl, trl);
    V3d<double> trlVec;
    trlVec.x = trl[0];
    trlVec.y = trl[1];
    trlVec.z = trl[2];

    V3d<double> transl = targetPos /*+ pnt that is equal to zero*/ - trlVec;

    vPosComp.x = transl.x;vPosComp.y = transl.y;vPosComp.z = transl.z;vPosComp.w = 1.0;
    vRotComp.x = vOVPRot->x;vRotComp.y = vOVPRot->y;vRotComp.z = vOVPRot->z;vRotComp.w = 1.0;
    mafTransfTransformUpright(&vPosComp, &vRotComp, &mLocNew);
    DiMatrixToVTK(&mLocNew, mLocalNew.GetVTKMatrix());
    mLocalNew.SetTimeStamp(ts);
    vme->SetMatrix(mLocalNew);
    vme->Modified();
  }
}


bool PrimaryRegistration(std::map<int, mafVMELandmarkCloud*>& src, mafTimeStamp tsRefSrc, RegData *rd, std::map<int, mafVMELandmarkCloud*>& trg, mafTimeStamp tsRefTrg)
{
  std::map<int, mafVMELandmarkCloud*>::iterator plv = trg.find(mafVMEAFRefSys::ID_AFS_PELVIS);
  std::map<int, mafVMELandmarkCloud*>::iterator rth = trg.find(mafVMEAFRefSys::ID_AFS_RTHIGH);
  std::map<int, mafVMELandmarkCloud*>::iterator lth = trg.find(mafVMEAFRefSys::ID_AFS_LTHIGH);

  if(plv == trg.end() || rth == trg.end() || lth == trg.end())
    return false;

  mafVMEAFRefSys *plvRS = GetAFRefSys(plv->second);
  mafVMEAFRefSys *rthRS = GetAFRefSys(rth->second);
  mafVMEAFRefSys *lthRS = GetAFRefSys(lth->second);

  mafMatrix plvMtr;
  mafMatrix rthMtr;
  mafMatrix lthMtr;

  plv->second->GetOutput()->GetAbsMatrix(plvMtr, tsRefTrg);
  rth->second->GetOutput()->GetAbsMatrix(rthMtr, tsRefTrg);
  lth->second->GetOutput()->GetAbsMatrix(lthMtr, tsRefTrg);
  plvMtr.Invert();
  rthMtr.Invert();
  lthMtr.Invert();


  if(plvRS == NULL || rthRS == NULL || lthRS == NULL)
    return false;

  V3d<double> vRIAC, vRFCH;
  V3d<double> vLIAC, vLFCH;

  if(!plvRS->GetVector("RIAC", tsRefTrg, vRIAC) || !rthRS->GetVector("RFCH", tsRefTrg, vRFCH))
    return false;
  if(!plvRS->GetVector("LIAC", tsRefTrg, vLIAC) || !lthRS->GetVector("LFCH", tsRefTrg, vLFCH))
    return false;

  double mltpl[4];
  mltpl[0] = vRIAC.x;mltpl[1] = vRIAC.y;mltpl[2] = vRIAC.z;mltpl[3] = 1.0;
  plvMtr.MultiplyPoint(mltpl, mltpl);
  vRIAC.x = mltpl[0];vRIAC.y = mltpl[1];vRIAC.z = mltpl[2];

  mltpl[0] = vLIAC.x;mltpl[1] = vLIAC.y;mltpl[2] = vLIAC.z;mltpl[3] = 1.0;
  plvMtr.MultiplyPoint(mltpl, mltpl);
  vLIAC.x = mltpl[0];vLIAC.y = mltpl[1];vLIAC.z = mltpl[2];

  mltpl[0] = vRFCH.x;mltpl[1] = vRFCH.y;mltpl[2] = vRFCH.z;mltpl[3] = 1.0;
  rthMtr.MultiplyPoint(mltpl, mltpl);
  vRFCH.x = mltpl[0];vRFCH.y = mltpl[1];vRFCH.z = mltpl[2];

  mltpl[0] = vLFCH.x;mltpl[1] = vLFCH.y;mltpl[2] = vLFCH.z;mltpl[3] = 1.0;
  lthMtr.MultiplyPoint(mltpl, mltpl);
  vLFCH.x = mltpl[0];vLFCH.y = mltpl[1];vLFCH.z = mltpl[2];

  std::map<int, mafVMELandmarkCloud*>::iterator itSrc, itTrg;
  for(itSrc = src.begin(), itTrg = trg.begin(); itSrc != src.end() && itTrg != trg.end(); ++itSrc, ++itTrg)
  {
    std::vector<mafTimeStamp> stamps;
    itSrc->second->GetTimeStamps(stamps);

    /*if(itSrc->first == mafVMEAFRefSys::ID_AFS_PELVIS)
      continue;*/

    for(unsigned i = 0; i < stamps.size(); i++)
    {
      if(stamps[i] == tsRefSrc || stamps[i] == tsRefTrg)
        continue;
      DiV4d ps, rt;
      double px, py, pz, rx, ry, rz;
      OVP_GES(itSrc->second, stamps[i], tsRefSrc, &ps, &rt, NULL, NULL);
      px = ps.x;
      py = ps.y;
      pz = ps.z;
      rx = rt.x * mafMatrix3x3::RadiansToDegrees();
      ry = rt.y * mafMatrix3x3::RadiansToDegrees();
      rz = rt.z * mafMatrix3x3::RadiansToDegrees();
      if(rd->GetValues(itSrc->first, px, py, pz, rx, ry, rz))
      {
        ps.x = px;
        ps.y = py;
        ps.z = pz;
        rt.x = rx * mafMatrix3x3::DegreesToRadians();
        rt.y = ry * mafMatrix3x3::DegreesToRadians();
        rt.z = rz * mafMatrix3x3::DegreesToRadians();
        //SetOVP(itTrg->second, stamps[i], tsRefTrg, &ps, &rt);
        SetOVPSpecial(itTrg->second, stamps[i], tsRefTrg, vRIAC, vRFCH, vLIAC, vLFCH, &ps, &rt);
      }
    }
  }
  return true;
}


static V3d<double>                         _samplePos;
static V3d<double>                         _sampleOri;
static V3d<double>                         _vRPV, _vLPV;
static V3d<double>                         _vRTH, _vLTH;
static V3d<double>                         _vRFT, _vLFT;
static bool                                _right;
static mafTimeStamp                        _curStamp, _refStamp;
static RegData                             *_regData;
static std::map<int, mafVMELandmarkCloud*> *_cloudsMap;
static std::vector<double>                 _values;
static std::vector<double>                 _deltas;


static bool GetOriPosR(std::map<int, mafVMELandmarkCloud*>& mp, mafTimeStamp ts, V3d<double>& vRP, V3d<double>& vRO)
{
  std::map<int, mafVMELandmarkCloud*>::iterator it = mp.begin();
  if(it == mp.end())
    return false;

  DiMatrix rightMtr;

  for(unsigned ii = 0; ii < 3; ii++)
  {
    vRP[ii] = 0.0;
    vRO[ii] = 0.0;
  }

  std::map<int, mafVMELandmarkCloud*>::iterator plv = mp.find(mafVMEAFRefSys::ID_AFS_PELVIS);
  std::map<int, mafVMELandmarkCloud*>::iterator rft = mp.find(mafVMEAFRefSys::ID_AFS_RFOOT);

  if(plv == mp.end() || rft == mp.end())
    return false;

  mafVMEAFRefSys *plvRS = GetAFRefSys(plv->second);
  mafVMEAFRefSys *rftRS = GetAFRefSys(rft->second);

  if(plvRS == NULL || rftRS == NULL)
    return false;

  mafMatrix Mt;
  DiMatrix  Matr, MatrInv, tmp;

  DiMatrixIdentity(&rightMtr);

  //pelvic data processing
  plvRS->GetOutput()->GetAbsMatrix(Mt, ts);
  mflMatrixToDi(Mt.GetVTKMatrix(), &tmp);
  DiMatrixInvert(&tmp, &MatrInv);
  DiMatrixMultiply(&rightMtr, &MatrInv, &tmp);
  DiMatrixCopy(&tmp, &rightMtr);

  V3d<double> vRIAC;
  if(plvRS->GetVector("RIAC", ts, vRIAC))
    vRP -= vRIAC;

  //rfoot data processing
  rftRS->GetOutput()->GetAbsMatrix(Mt, ts);
  mflMatrixToDi(Mt.GetVTKMatrix(), &Matr);
  DiMatrixMultiply(&Matr, &rightMtr, &tmp);
  DiMatrixCopy(&tmp, &rightMtr);

  V3d<double> vRFCC;
  if(rftRS->GetVector("RFCC", ts, vRFCC))
    vRP += vRFCC;

  DiV4d pos, rot;
  mafTransfInverseTransformUpright(&rightMtr, &pos, &rot);
  vRO.x = rot.x * mafMatrix3x3::RadiansToDegrees();
  vRO.y = rot.y * mafMatrix3x3::RadiansToDegrees();
  vRO.z = rot.z * mafMatrix3x3::RadiansToDegrees();

  return true;
}
static bool GetOriPosL(std::map<int, mafVMELandmarkCloud*>& mp, mafTimeStamp ts, V3d<double>& vLP, V3d<double>& vLO)
{
  std::map<int, mafVMELandmarkCloud*>::iterator it = mp.begin();
  if(it == mp.end())
    return false;

  DiMatrix leftMtr;

  for(unsigned ii = 0; ii < 3; ii++)
  {
    vLP[ii] = 0.0;
    vLO[ii] = 0.0;
  }

  std::map<int, mafVMELandmarkCloud*>::iterator plv = mp.find(mafVMEAFRefSys::ID_AFS_PELVIS);
  std::map<int, mafVMELandmarkCloud*>::iterator lft = mp.find(mafVMEAFRefSys::ID_AFS_LFOOT);

  if(plv == mp.end() || lft == mp.end())
    return false;

  mafVMEAFRefSys *plvRS = GetAFRefSys(plv->second);
  mafVMEAFRefSys *lftRS = GetAFRefSys(lft->second);

  if(plvRS == NULL || lftRS == NULL)
    return false;

  {
    mafMatrix Mt;
    DiMatrix  Matr, MatrInv, tmp;

    DiMatrixIdentity(&leftMtr);

    //pelvic data processing
    plvRS->GetOutput()->GetAbsMatrix(Mt, ts);
    mflMatrixToDi(Mt.GetVTKMatrix(), &tmp);
    DiMatrixInvert(&tmp, &MatrInv);
    DiMatrixMultiply(&leftMtr, &MatrInv, &tmp);
    DiMatrixCopy(&tmp, &leftMtr);

    V3d<double> vLIAC;
    if(plvRS->GetVector("LIAC", ts, vLIAC))
      vLP -= vLIAC;
    //lfoot data processing

    lftRS->GetOutput()->GetAbsMatrix(Mt, ts);
    mflMatrixToDi(Mt.GetVTKMatrix(), &Matr);
    DiMatrixMultiply(&Matr, &leftMtr, &tmp);
    DiMatrixCopy(&tmp, &leftMtr);

    V3d<double> vLFCC;
    if(lftRS->GetVector("LFCC", ts, vLFCC))
      vLP += vLFCC;

    DiV4d pos, rot;
    mafTransfInverseTransformUpright(&leftMtr, &pos, &rot);
    vLO.x = rot.x * mafMatrix3x3::RadiansToDegrees();
    vLO.y = rot.y * mafMatrix3x3::RadiansToDegrees();
    vLO.z = rot.z * mafMatrix3x3::RadiansToDegrees();
  }
  return true;
}

static bool GetOriPosRSpecial(std::map<int, mafVMELandmarkCloud*>& mp, mafTimeStamp ts, V3d<double>& vRPV, V3d<double>& vRFT, V3d<double>& vRP, V3d<double>& vRO)
{
  std::map<int, mafVMELandmarkCloud*>::iterator it = mp.begin();
  if(it == mp.end())
    return false;

  for(unsigned ii = 0; ii < 3; ii++)
  {
    vRP[ii] = 0.0;
    vRO[ii] = 0.0;
  }

  std::map<int, mafVMELandmarkCloud*>::iterator plv = mp.find(mafVMEAFRefSys::ID_AFS_PELVIS);
  std::map<int, mafVMELandmarkCloud*>::iterator rft = mp.find(mafVMEAFRefSys::ID_AFS_RFOOT);

  if(plv == mp.end() || rft == mp.end())
    return false;

  mafVMEAFRefSys *plvRS = GetAFRefSys(plv->second);
  mafVMEAFRefSys *rftRS = GetAFRefSys(rft->second);

  if(plvRS == NULL || rftRS == NULL)
    return false;

  mafMatrix   plvMatrix;
  mafMatrix   rftMatrix;
  mafMatrix   mult;
  DiMatrix    dimult;
  double      rftPnt[4];
  rftPnt[0] = vRFT.x;rftPnt[1] = vRFT.y;rftPnt[2] = vRFT.z;rftPnt[3] = 1.0;

  plv->second->GetOutput()->GetAbsMatrix(plvMatrix, ts);
  rft->second->GetOutput()->GetAbsMatrix(rftMatrix, ts);
  plvMatrix.Invert();
  mafMatrix::Multiply4x4(plvMatrix, rftMatrix, mult);
  mult.MultiplyPoint(rftPnt, rftPnt);
  vRP.x = rftPnt[0];vRP.y = rftPnt[1];vRP.z = rftPnt[2];
  vRP -= vRPV;

  mflMatrixToDi(mult.GetVTKMatrix(), &dimult);
  DiV4d pos, rot;
  mafTransfInverseTransformUpright(&dimult, &pos, &rot);
  vRO.x = rot.x * mafMatrix3x3::RadiansToDegrees();
  vRO.y = rot.y * mafMatrix3x3::RadiansToDegrees();
  vRO.z = rot.z * mafMatrix3x3::RadiansToDegrees();
  return true;
}
static bool GetOriPosLSpecial(std::map<int, mafVMELandmarkCloud*>& mp, mafTimeStamp ts, V3d<double>& vLPV, V3d<double>& vLFT, V3d<double>& vLP, V3d<double>& vLO)
{
  std::map<int, mafVMELandmarkCloud*>::iterator it = mp.begin();
  if(it == mp.end())
    return false;

  for(unsigned ii = 0; ii < 3; ii++)
  {
    vLP[ii] = 0.0;
    vLO[ii] = 0.0;
  }

  std::map<int, mafVMELandmarkCloud*>::iterator plv = mp.find(mafVMEAFRefSys::ID_AFS_PELVIS);
  std::map<int, mafVMELandmarkCloud*>::iterator lft = mp.find(mafVMEAFRefSys::ID_AFS_LFOOT);

  if(plv == mp.end() || lft == mp.end())
    return false;

  mafVMEAFRefSys *plvRS = GetAFRefSys(plv->second);
  mafVMEAFRefSys *lftRS = GetAFRefSys(lft->second);

  if(plvRS == NULL || lftRS == NULL)
    return false;

  mafMatrix   plvMatrix;
  mafMatrix   lftMatrix;
  mafMatrix   mult;
  DiMatrix    dimult;
  double      lftPnt[4];
  lftPnt[0] = vLFT.x;lftPnt[1] = vLFT.y;lftPnt[2] = vLFT.z;lftPnt[3] = 1.0;

  plv->second->GetOutput()->GetAbsMatrix(plvMatrix, ts);
  lft->second->GetOutput()->GetAbsMatrix(lftMatrix, ts);
  plvMatrix.Invert();
  mafMatrix::Multiply4x4(plvMatrix, lftMatrix, mult);
  mult.MultiplyPoint(lftPnt, lftPnt);
  vLP.x = lftPnt[0];vLP.y = lftPnt[1];vLP.z = lftPnt[2];
  vLP -= vLPV;

  mflMatrixToDi(mult.GetVTKMatrix(), &dimult);
  DiV4d pos, rot;
  mafTransfInverseTransformUpright(&dimult, &pos, &rot);
  vLO.x = rot.x * mafMatrix3x3::RadiansToDegrees();
  vLO.y = rot.y * mafMatrix3x3::RadiansToDegrees();
  vLO.z = rot.z * mafMatrix3x3::RadiansToDegrees();
  return true;
}



DP evalFunc(Vec_I_DP &inp)
{
  std::vector<double> given;
  given.resize(_values.size());

  DP   result = 0.0; 
  for(int i = 0; i < _values.size(); i++)
  {
    given[i] = inp[i];
    if(fabs(inp[i] - _values[i]) > _deltas[i])
    {
      DP dlt = fabs(inp[i] - _values[i]) - _deltas[i];
      result += 1000.0 * dlt * dlt;
    }
  }

  int rSegms[] = {mafVMEAFRefSys::ID_AFS_RTHIGH, mafVMEAFRefSys::ID_AFS_RSHANK, mafVMEAFRefSys::ID_AFS_RFOOT};
  int lSegms[] = {mafVMEAFRefSys::ID_AFS_LTHIGH, mafVMEAFRefSys::ID_AFS_LSHANK, mafVMEAFRefSys::ID_AFS_LFOOT};
  int *segms;

  segms = (_right) ? rSegms : lSegms;

  std::vector<double>::iterator iter = given.begin();
  for(int kk = 0; kk < 3; kk++)
  {
    std::map<int, mafVMELandmarkCloud*>::iterator it = _cloudsMap->find(segms[kk]);
    double px, py, pz, rx, ry, rz;
    DiV4d pos, rot;
    _regData->GetValues(it->first, iter, px, py, pz, rx, ry, rz);
    pos.x = px;
    pos.y = py;
    pos.z = pz;
    rot.x = rx * mafMatrix3x3::DegreesToRadians();
    rot.y = ry * mafMatrix3x3::DegreesToRadians();
    rot.z = rz * mafMatrix3x3::DegreesToRadians();
    SetOVPSpecial(it->second, _curStamp, _refStamp, _vRPV, _vRTH, _vLPV, _vLTH, &pos, &rot);
  }


  V3d<double> cPos, cOri;
  if(_right)
    GetOriPosRSpecial(*_cloudsMap, _curStamp, _vRPV, _vRFT, cPos, cOri);
  else
    GetOriPosLSpecial(*_cloudsMap, _curStamp, _vLPV, _vLFT, cPos, cOri);
  return result + sqrt(cPos | _samplePos) + sqrt(cOri | _sampleOri);
}



bool AdvancedRegistration(std::map<int, mafVMELandmarkCloud*>& mp, mafTimeStamp tsRef, 
                          std::vector<V3d<double> >& rp, std::vector<V3d<double> >& ro, 
                          std::vector<V3d<double> >& lp, std::vector<V3d<double> >& lo, RegData *rd)
{
  std::map<int, mafVMELandmarkCloud*>::iterator plv = mp.find(mafVMEAFRefSys::ID_AFS_PELVIS);
  std::map<int, mafVMELandmarkCloud*>::iterator rth = mp.find(mafVMEAFRefSys::ID_AFS_RTHIGH);
  std::map<int, mafVMELandmarkCloud*>::iterator lth = mp.find(mafVMEAFRefSys::ID_AFS_LTHIGH);
  std::map<int, mafVMELandmarkCloud*>::iterator rft = mp.find(mafVMEAFRefSys::ID_AFS_RFOOT);
  std::map<int, mafVMELandmarkCloud*>::iterator lft = mp.find(mafVMEAFRefSys::ID_AFS_LFOOT);

  if(plv == mp.end() || rth == mp.end() || lth == mp.end() || rft == mp.end() || lft == mp.end())
    return false;

  mafVMEAFRefSys *plvRS = GetAFRefSys(plv->second);
  mafVMEAFRefSys *rthRS = GetAFRefSys(rth->second);
  mafVMEAFRefSys *lthRS = GetAFRefSys(lth->second);
  mafVMEAFRefSys *rftRS = GetAFRefSys(rft->second);
  mafVMEAFRefSys *lftRS = GetAFRefSys(lft->second);

  mafMatrix plvMtr;
  mafMatrix rthMtr;
  mafMatrix lthMtr;
  mafMatrix rftMtr;
  mafMatrix lftMtr;

  plv->second->GetOutput()->GetAbsMatrix(plvMtr, tsRef);
  rth->second->GetOutput()->GetAbsMatrix(rthMtr, tsRef);
  lth->second->GetOutput()->GetAbsMatrix(lthMtr, tsRef);
  rft->second->GetOutput()->GetAbsMatrix(rftMtr, tsRef);
  lft->second->GetOutput()->GetAbsMatrix(lftMtr, tsRef);
  plvMtr.Invert();
  rthMtr.Invert();
  lthMtr.Invert();
  rftMtr.Invert();
  lftMtr.Invert();


  if(plvRS == NULL || rthRS == NULL || lthRS == NULL || rftRS == NULL || lftRS == NULL)
    return false;

  V3d<double> vRIAC, vRFCH, vRFCC;
  V3d<double> vLIAC, vLFCH, vLFCC;

  if(!plvRS->GetVector("RIAC", tsRef, vRIAC) || !rthRS->GetVector("RFCH", tsRef, vRFCH) || !rftRS->GetVector("RFCC", tsRef, vRFCC))
    return false;
  if(!plvRS->GetVector("LIAC", tsRef, vLIAC) || !lthRS->GetVector("LFCH", tsRef, vLFCH) || !lftRS->GetVector("LFCC", tsRef, vLFCC))
    return false;

  double mltpl[4];
  mltpl[0] = vRIAC.x;mltpl[1] = vRIAC.y;mltpl[2] = vRIAC.z;mltpl[3] = 1.0;
  plvMtr.MultiplyPoint(mltpl, mltpl);
  vRIAC.x = mltpl[0];vRIAC.y = mltpl[1];vRIAC.z = mltpl[2];

  mltpl[0] = vLIAC.x;mltpl[1] = vLIAC.y;mltpl[2] = vLIAC.z;mltpl[3] = 1.0;
  plvMtr.MultiplyPoint(mltpl, mltpl);
  vLIAC.x = mltpl[0];vLIAC.y = mltpl[1];vLIAC.z = mltpl[2];

  mltpl[0] = vRFCH.x;mltpl[1] = vRFCH.y;mltpl[2] = vRFCH.z;mltpl[3] = 1.0;
  rthMtr.MultiplyPoint(mltpl, mltpl);
  vRFCH.x = mltpl[0];vRFCH.y = mltpl[1];vRFCH.z = mltpl[2];

  mltpl[0] = vLFCH.x;mltpl[1] = vLFCH.y;mltpl[2] = vLFCH.z;mltpl[3] = 1.0;
  lthMtr.MultiplyPoint(mltpl, mltpl);
  vLFCH.x = mltpl[0];vLFCH.y = mltpl[1];vLFCH.z = mltpl[2];

  mltpl[0] = vRFCC.x;mltpl[1] = vRFCC.y;mltpl[2] = vRFCC.z;mltpl[3] = 1.0;
  rftMtr.MultiplyPoint(mltpl, mltpl);
  vRFCC.x = mltpl[0];vRFCC.y = mltpl[1];vRFCC.z = mltpl[2];

  mltpl[0] = vLFCC.x;mltpl[1] = vLFCC.y;mltpl[2] = vLFCC.z;mltpl[3] = 1.0;
  lftMtr.MultiplyPoint(mltpl, mltpl);
  vLFCC.x = mltpl[0];vLFCC.y = mltpl[1];vLFCC.z = mltpl[2];



  _vLPV = vLIAC;
  _vRPV = vRIAC;
  _vRTH = vRFCH;
  _vLTH = vLFCH;
  _vRFT = vRFCC;
  _vLFT = vLFCC;


  int rSegms[] = {mafVMEAFRefSys::ID_AFS_RTHIGH, mafVMEAFRefSys::ID_AFS_RSHANK, mafVMEAFRefSys::ID_AFS_RFOOT};
  int lSegms[] = {mafVMEAFRefSys::ID_AFS_LTHIGH, mafVMEAFRefSys::ID_AFS_LSHANK, mafVMEAFRefSys::ID_AFS_LFOOT};
  std::map<int, mafVMELandmarkCloud*>::iterator it = mp.begin();
  if(it == mp.end())
    return false;

  std::vector<mafTimeStamp> stamps;
  it->second->GetTimeStamps(stamps);

  for(unsigned i = 0; i < stamps.size(); i++)
  {
    if(stamps[i] == tsRef)
      continue;
    DiV4d pos, rot;

    //right size
    _values.clear();
    {
      for(int kk = 0; kk < 3; kk++)
      {
        it = mp.find(rSegms[kk]);
        double px, py, pz, rx, ry, rz;
        GetOVPSpecial(it->second, stamps[i], tsRef, vRIAC, vRFCH, vLIAC, vLFCH, &pos, &rot);
        px = pos.x;
        py = pos.y;
        pz = pos.z;
        rx = rot.x * mafMatrix3x3::RadiansToDegrees();
        ry = rot.y * mafMatrix3x3::RadiansToDegrees();
        rz = rot.z * mafMatrix3x3::RadiansToDegrees();
        rd->ExtractFreeValues(it->first, px, py, pz, rx, ry, rz, _values, _deltas);
      }


      int iterCount;
      DP  val;

      Vec_DP input(_values.size());
      Mat_DP mat_inp(_values.size(), _values.size());
      for(int ss = 0; ss < _values.size(); ss++)
      {
        for(int tt = 0; tt < _values.size(); tt++)
        {
          mat_inp[ss][tt] = 0;
        }
      }
      for(int ss = 0; ss < _values.size(); ss++)
      {
        input[ss] = _values[ss];
        mat_inp[ss][ss] = 1.0;
      }

      _regData   = rd;
      _curStamp  = stamps[i];
      _refStamp  = tsRef;
      _samplePos = rp[i];
      _sampleOri = ro[i];
      _cloudsMap = &mp;
      _right     = true;

      NR::powell(input, mat_inp, 0.0000001, iterCount, val, evalFunc);
      for(int ss = 0; ss < _values.size(); ss++)
      {
        _values[ss] = input[ss];
      }


      std::vector<double>::iterator iter = _values.begin();
      for(int kk = 0; kk < 3; kk++)
      {
        it = mp.find(rSegms[kk]);
        double px, py, pz, rx, ry, rz;
        rd->GetValues(it->first, iter, px, py, pz, rx, ry, rz);
        pos.x = px;
        pos.y = py;
        pos.z = pz;
        rot.x = rx * mafMatrix3x3::DegreesToRadians();
        rot.y = ry * mafMatrix3x3::DegreesToRadians();
        rot.z = rz * mafMatrix3x3::DegreesToRadians();
        SetOVPSpecial(it->second, stamps[i], tsRef, vRIAC, vRFCH, vLIAC, vLFCH, &pos, &rot);
      }
    }

    //left size
    _values.clear();
    {
      for(int kk = 0; kk < 3; kk++)
      {
        it = mp.find(lSegms[kk]);
        double px, py, pz, rx, ry, rz;
        GetOVPSpecial(it->second, stamps[i], tsRef, vRIAC, vRFCH, vLIAC, vLFCH, &pos, &rot);
        px = pos.x;
        py = pos.y;
        pz = pos.z;
        rx = rot.x * mafMatrix3x3::RadiansToDegrees();
        ry = rot.y * mafMatrix3x3::RadiansToDegrees();
        rz = rot.z * mafMatrix3x3::RadiansToDegrees();
        rd->ExtractFreeValues(it->first, px, py, pz, rx, ry, rz, _values, _deltas);
      }

      int iterCount;
      DP  val;

      Vec_DP input(_values.size());
      Mat_DP mat_inp(_values.size(), _values.size());
      for(int ss = 0; ss < _values.size(); ss++)
      {
        for(int tt = 0; tt < _values.size(); tt++)
        {
          mat_inp[ss][tt] = 0;
        }
      }
      for(int ss = 0; ss < _values.size(); ss++)
      {
        input[ss] = _values[ss];
        mat_inp[ss][ss] = 1.0;
      }

      _regData   = rd;
      _curStamp  = stamps[i];
      _refStamp  = tsRef;
      _samplePos = lp[i];
      _sampleOri = lo[i];
      _cloudsMap = &mp;
      _right     = false;

      NR::powell(input, mat_inp, 0.0000001, iterCount, val, evalFunc);
      for(int ss = 0; ss < _values.size(); ss++)
      {
         _values[ss] = input[ss];
      }

      std::vector<double>::iterator iter = _values.begin();
      for(int kk = 0; kk < 3; kk++)
      {
        it = mp.find(lSegms[kk]);
        double px, py, pz, rx, ry, rz;
        rd->GetValues(it->first, iter, px, py, pz, rx, ry, rz);
        pos.x = px;
        pos.y = py;
        pos.z = pz;
        rot.x = rx * mafMatrix3x3::DegreesToRadians();
        rot.y = ry * mafMatrix3x3::DegreesToRadians();
        rot.z = rz * mafMatrix3x3::DegreesToRadians();
        SetOVPSpecial(it->second, stamps[i], tsRef, vRIAC, vRFCH, vLIAC, vLFCH, &pos, &rot);
      }
    }


  }
  return true;
}

bool lhpOpRegistration::ScaleLMC(mafVMELandmarkCloud *lmc, double scale)
{
  std::vector<mafTimeStamp> stamps;
  lmc->GetMatrixTimeStamps(stamps);
  for(unsigned j = 0; j < stamps.size(); j++)
  {
    mafMatrix mtr;
    double    tmp;
    lmc->GetOutput()->GetMatrix(mtr, stamps[j]);

    tmp = mtr.GetVTKMatrix()->GetElement(0, 3) * scale;
    mtr.GetVTKMatrix()->SetElement(0, 3, tmp);
    tmp = mtr.GetVTKMatrix()->GetElement(1, 3) * scale;
    mtr.GetVTKMatrix()->SetElement(1, 3, tmp);
    tmp = mtr.GetVTKMatrix()->GetElement(2, 3) * scale;
    mtr.GetVTKMatrix()->SetElement(2, 3, tmp);
    mtr.SetTimeStamp(stamps[j]);
    lmc->SetMatrix(mtr);
  }

  lmc->GetDataTimeStamps(stamps);
  for(unsigned j = 0; j < stamps.size(); j++)
  {
    for(unsigned i = 0; i < lmc->GetNumberOfLandmarks(); i++)
    {
      V3d<double> tmp;
      lmc->GetLandmark(i, tmp.x, tmp.y, tmp.z, stamps[j]);
      tmp *= scale;
      lmc->SetLandmark(i, tmp.x, tmp.y, tmp.z, stamps[j]);
    }
  }
  return true;
}

//----------------------------------------------------------------------------
bool lhpOpRegistration::RegistrationProcedure()
//----------------------------------------------------------------------------
{
  mafNode *inputCopy = m_Input->CopyTree();
  inputCopy->Register(this);
  //mafEventMacro(mafEvent(this, VME_ADD, inputCopy));
  mafVMELandmarkCloud *src = mafVMELandmarkCloud::SafeDownCast(inputCopy);
  mafVMELandmarkCloud *trg = mafVMELandmarkCloud::SafeDownCast(m_RegTarget);
  if(!src || !trg)
    return false;

  mafVMEAFRefSys *sysSrc = GetAFRefSys(src);
  mafVMEAFRefSys *sysTrg = GetAFRefSys(trg);
  if(sysSrc == NULL || sysSrc->GetBoneID() == mafVMEAFRefSys::ID_AFS_NOTDEFINED)
    return false;
  if(sysTrg == NULL || sysTrg->GetBoneID() == mafVMEAFRefSys::ID_AFS_NOTDEFINED)
    return false;
  if(sysSrc->GetBoneID() != sysTrg->GetBoneID())
    return false;

  bool fullProcReady = (sysSrc->GetBoneID() == mafVMEAFRefSys::ID_AFS_PELVIS);

  std::map<int, mafVMELandmarkCloud*> mpSrc;
  std::map<int, mafVMELandmarkCloud*> mpTrg;
  if(!fillMap(src, sysSrc->GetBoneID(), mpSrc))
    return false;

  if(!fillMap(trg, sysTrg->GetBoneID(), mpTrg))
    return false;

  std::vector<mafTimeStamp> tsList;
  if(fullProcReady)
  {
    if(!StampsIdentical(mpSrc, tsList))
      fullProcReady = false;
  }

  std::vector<V3d<double> > lpos;
  std::vector<V3d<double> > rpos;
  std::vector<V3d<double> > lori;
  std::vector<V3d<double> > rori;
  double lsize, rsize;
  if(fullProcReady)
  {
    rpos.resize(tsList.size());
    lpos.resize(tsList.size());
    rori.resize(tsList.size());
    lori.resize(tsList.size());
    for(unsigned i = 0; i < tsList.size(); i++)
    {
      GetOriPosR(mpSrc, tsList[i], rpos[i], rori[i]);
      GetOriPosL(mpSrc, tsList[i], lpos[i], lori[i]);
    }
    V3d<double> ps, or;
    GetOriPosL(mpSrc, m_RefStampSrc, ps, or);
    lsize = sqrt(ps.length2());
    GetOriPosR(mpSrc, m_RefStampSrc, ps, or);
    rsize = sqrt(ps.length2());
  }
  RegData *regData;
  if(m_Regression) 
    regData = new RegDataRef();
  else
    regData = new RegDataSph();
  FillRegData(mpTrg, m_RefStampTrg, regData);


  double rscale = regData->GetRightSize() / rsize;
  double lscale = regData->GetLeftSize() / lsize;
  double ascale = (rscale + lscale) / 2.0;
  if(m_Scale)
  {
    int segms[] = {mafVMEAFRefSys::ID_AFS_RTHIGH, mafVMEAFRefSys::ID_AFS_RSHANK, mafVMEAFRefSys::ID_AFS_RFOOT, 
      mafVMEAFRefSys::ID_AFS_LTHIGH, mafVMEAFRefSys::ID_AFS_LSHANK, mafVMEAFRefSys::ID_AFS_LFOOT, 
      mafVMEAFRefSys::ID_AFS_PELVIS};
    double sc[] = {rscale, rscale, rscale, lscale, lscale, lscale, ascale};

    for(unsigned i = 0; i < tsList.size(); i++)
    {
      rpos[i] *= rscale;
      lpos[i] *= lscale;
    }
    std::map<int, mafVMELandmarkCloud*>::iterator it;
    for(unsigned j = 0; j < 7; j++)
    {
      it = mpSrc.find(segms[j]);
      if(it != mpSrc.end())
        ScaleLMC(it->second, sc[j]);
    }
  }
  mafVMEGroup *grp;
  mafNEW(grp);
  mafNode *trgCopy = NULL;
  grp->SetName("DSRegistration_result");
  m_Result = grp;
  if(m_Primary)
  {
    trgCopy = m_RegTarget->CopyTree();
    trgCopy->Register(this);

    mafVMELandmarkCloud *trgCopyLMC = mafVMELandmarkCloud::SafeDownCast(trgCopy);
    if(trgCopyLMC == NULL)
      return true;

    std::map<int, mafVMELandmarkCloud*> mpt;
    fillMap(mafVMELandmarkCloud::SafeDownCast(trgCopy), mafVMEAFRefSys::ID_AFS_PELVIS, mpt);

    for(std::map<int, mafVMELandmarkCloud*>::iterator it_mpt = mpt.begin(); it_mpt != mpt.end(); ++it_mpt)
    {
      mafMatrixVector *mv = it_mpt->second->GetMatrixVector();
      mafDataVector   *dv = it_mpt->second->GetDataVector();
      std::vector<mafTimeStamp> kframes;
      it_mpt->second->GetLocalTimeStamps(kframes);
      for(int i = 0; i < kframes.size(); i++)
      {
        if(kframes[i] != m_RefStampTrg)
        {
          mafMatrixVector::Iterator itm = mv->FindItem(kframes[i]);
          mafDataVector::Iterator   itd = dv->FindItem(kframes[i]);
          if(itm != mv->End())
            mv->RemoveItem(itm);
          if(itd != dv->End())
            dv->RemoveItem(itd);
        }
      }
      mafVMEAFRefSys *sysIt = GetAFRefSys(it_mpt->second);
      if(sysIt == NULL)
        continue;
      mafMatrix sysItMtr;
      DiMatrix  sysItMatrix;
      mafMatrix itMtr;
      DiMatrix  itMatrix;
      mafMatrix itGlobMtr;
      mafMatrix newInv;
      mafMatrix modif;
      DiMatrix  itGlobMatrix;
      it_mpt->second->GetOutput()->GetMatrix(itMtr, m_RefStampTrg);
      it_mpt->second->GetOutput()->GetAbsMatrix(itGlobMtr, m_RefStampTrg);
      sysIt->GetOutput()->GetAbsMatrix(sysItMtr, m_RefStampTrg);
      newInv = sysItMtr;
      newInv.Invert();
      mafMatrix::Multiply4x4(newInv, itGlobMtr, modif);
      mflMatrixToDi(itMtr.GetVTKMatrix(), &itMatrix);
      mflMatrixToDi(itGlobMtr.GetVTKMatrix(), &itGlobMatrix);
      mflMatrixToDi(sysItMtr.GetVTKMatrix(), &sysItMatrix);
      for(unsigned lmi = 0; lmi < it_mpt->second->GetNumberOfLandmarks(); lmi++)
      {
        double lmpos[4];
        double lmposnew[4];
        it_mpt->second->GetLandmark(lmi, lmpos, m_RefStampTrg);
        lmpos[3] = 1.0;
        modif.MultiplyPoint(lmpos, lmposnew);
        it_mpt->second->SetLandmark(lmi, lmposnew[0], lmposnew[1], lmposnew[2], m_RefStampTrg);
      }
      it_mpt->second->SetAbsMatrix(sysItMtr, m_RefStampTrg);
      for(unsigned chi = 0; chi < it_mpt->second->GetNumberOfChildren(); chi++)
      {
        mafVME *chld = mafVME::SafeDownCast(it_mpt->second->GetChild(chi));
        if(chld == NULL)
          continue;
        mafMatrix curMtr;
        mafMatrix newMtr;
        chld->GetOutput()->GetMatrix(curMtr, m_RefStampTrg);
        mafMatrix::Multiply4x4(modif, curMtr, newMtr);
        newMtr.SetTimeStamp(m_RefStampTrg);
        chld->SetMatrix(newMtr);
      }
    }

    PrimaryRegistration(mpSrc, m_RefStampSrc, regData, mpt, m_RefStampTrg);
    if(fullProcReady && m_Advanced)
      AdvancedRegistration(mpt, m_RefStampTrg, rpos, rori, lpos, lori, regData);
  }
  if(trgCopy)
    trgCopy->ReparentTo(m_Result);
  else
    inputCopy->ReparentTo(m_Result);
  delete regData;
  mafDEL(inputCopy);
  mafDEL(trgCopy);
  return true;
}

//----------------------------------------------------------------------------
void lhpOpRegistration::OpStop(int result)
//----------------------------------------------------------------------------
{
  if (result == OP_RUN_CANCEL)
  {
    HideGui();
    mafEventMacro(mafEvent(this,result));
    return;
  }
  if(m_RegTarget == NULL)
  {
    wxMessageBox("Not all data defined for operation.","Alert", wxOK , NULL);
    return;
  }
  HideGui();
  wxBusyInfo wait(_("Please wait, working..."));
  if(RegistrationProcedure())
  {
    mafEventMacro(mafEvent(this,result));
  }
  else
  {
    mafEventMacro(mafEvent(this,OP_RUN_CANCEL));
  }
}

//----------------------------------------------------------------------------
void lhpOpRegistration::SetNodeName(mafVME *pVME, mafString *pName) 
//----------------------------------------------------------------------------
{
  *pName = pVME->GetName();
  if(pVME->GetParent() != NULL)
  {
    *pName = *pName + " parent:";
    *pName = *pName + pVME->GetParent()->GetName().GetCStr();
  }
}

//----------------------------------------------------------------------------
void lhpOpRegistration::OnEvent(mafEventBase *maf_event) 
//----------------------------------------------------------------------------
{ 
  switch(maf_event->GetId())
  {
    case wxOK:          
    { 
      OpStop(OP_RUN_OK);
    }
    break;
    case ID_SCALE:
    case ID_PRIMARY:
    case ID_ADVANCED:
    case ID_REGRESSION:
      break;
    case wxCANCEL:
    {    
      OpStop(OP_RUN_CANCEL);
    }
    break;
    case ID_REGTRG:
    {
      mafString s("Choose reg data");
      mafEvent e(this,VME_CHOOSE, &s);
      mafEventMacro(e);
      if(e.GetVme() == NULL)
      {
        return;
      }


      {
        mafVMELandmarkCloud *lmc = mafVMELandmarkCloud::SafeDownCast(e.GetVme());
        if(lmc) 
        {
          mafVMEAFRefSys *sys    = GetAFRefSys(lmc);
          mafVMEAFRefSys *sysInp = GetAFRefSys((mafVME*)m_Input);
          if(sys    != NULL && sys->GetBoneID()    != mafVMEAFRefSys::ID_AFS_NOTDEFINED &&
             sysInp != NULL && sysInp->GetBoneID() != mafVMEAFRefSys::ID_AFS_NOTDEFINED &&
             sys->GetBoneID() == sysInp->GetBoneID())
          {
            std::map<int, mafVMELandmarkCloud*> mp;
            if(fillMap(lmc, sys->GetBoneID(), mp))
            {
              m_RegTarget = (mafVME*)e.GetVme();
              SetNodeName(m_RegTarget, &m_RegTrgName);
              m_Gui->Update();
              break;
            }
          }
        }
      }
      {
        wxMessageBox("Selected structure should correspond to input.","Warning", wxOK|wxICON_WARNING , NULL);
        return;
      }
    }
    default:
    {
      mafEventMacro(*maf_event); 
    }
    break;
  }
}




//----------------------------------------------------------------------------
void lhpOpRegistration::OpDo()
//----------------------------------------------------------------------------
{
  mafEventMacro(mafEvent(this, VME_ADD, m_Result));
}
//----------------------------------------------------------------------------
void lhpOpRegistration::OpUndo()
//----------------------------------------------------------------------------
{
  mafEventMacro(mafEvent(this, VME_REMOVE, m_Result));
}
