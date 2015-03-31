/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEHelAxis.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 12:15:29 $
  Version:   $Revision: 1.2 $
  Authors:   Fedor Moiseev / Vladik Aranov
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#ifdef _MSC_FULL_VER
#pragma warning (disable: 4786)
#endif

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------
#include "mafVMEHelAxis.h"

#include "mafGUI.h"
#include "mafIndent.h"
#include "mmaMaterial.h"
#include "mafTransform.h"

#include "mafTagArray.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMEAFRefSys.h"
#include "mafVMEOutputSurface.h"
#include "mafDataPipeCustom.h"
#include "mafStorageElement.h"
#include "mafMatrix3x3.h"
#include "mafJointAnalysis.h"
#include "mafVectors.h"

#include "vtkMAFSmartPointer.h"
#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include "vtkArrowSource.h"
#include "vtkSphereSource.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkTransform.h"
#include "vtkAppendPolyData.h"
#include "vtkWeightedLandmarkTransform.h"


#include "vtkPointData.h"
#include "vtkUnsignedCharArray.h"

#include "nr.h"


namespace
{
//----------------------------------------------------------------------------
int ExtractMatchingPoints(mafVMELandmarkCloud *src, mafTimeStamp srctime, mafTimeStamp trgtime, const mafMatrix& mpar1, const mafMatrix& mpar2, vtkPoints *pSrc, vtkPoints *pTrg)
//----------------------------------------------------------------------------
{
  double lmsrc[4];
  double lmtrg[4];
  double lmsrctrf[4];
  double lmtrgtrf[4];
  int    i;
  int    npSource    = src->GetNumberOfLandmarks();
  int    npTarget    = npSource;
  int    ncp         = 0;

  mafMatrix mcur;
  mafMatrix mnxt;

  mafMatrix mparcur;
  mafMatrix mparnxt;

  mparcur = mpar1;
  mparnxt = mpar2;

  mparcur.Invert();
  mparnxt.Invert();

  src->GetOutput()->GetAbsMatrix(mcur, srctime);
  src->GetOutput()->GetAbsMatrix(mnxt, trgtime);

  pSrc->Reset();
  pTrg->Reset();

  for(i = 0; i < npSource; i++)
  {
    if(!src->GetLandmarkVisibility(i,srctime) || !src->GetLandmarkVisibility(i,trgtime))
      continue;

    //add new points to arrays if no one is zero (zero indicates that data are invalid)
    src->GetLandmark(i, lmsrc, srctime);
    src->GetLandmark(i, lmtrg, trgtime);

    lmsrc[3] = 1.0;
    lmtrg[3] = 1.0;
    mcur.MultiplyPoint(lmsrc, lmsrctrf);
    mnxt.MultiplyPoint(lmtrg, lmtrgtrf);

    mparcur.MultiplyPoint(lmsrctrf, lmsrc);
    mparnxt.MultiplyPoint(lmtrgtrf, lmtrg);

    pSrc->InsertNextPoint(lmsrc);
    pTrg->InsertNextPoint(lmtrg);
    ncp++;
  }

  return ncp;
}
//----------------------------------------------------------------------------
double RegisterPoints(vtkPoints *pSrc, vtkPoints *pTrg, vtkMatrix4x4 *res_matrix)
//----------------------------------------------------------------------------
{
  int i;
  double deviation = 0.0;
  double dx, dy, dz;
  assert(pSrc && pTrg);

  vtkWeightedLandmarkTransform *RegisterTransform = vtkWeightedLandmarkTransform::New();

  //setup transform calculator
  RegisterTransform->SetSourceLandmarks(pSrc);
  RegisterTransform->SetTargetLandmarks(pTrg);

  RegisterTransform->SetModeToRigidBody();// Similarity();
  //calculate transform
  RegisterTransform->Update();
  RegisterTransform->GetMatrix(res_matrix);

  //calculate deviation
  for(i = 0; i < pSrc->GetNumberOfPoints(); i++)
  {
    double coord[4];
    double result[4];
    double target[3];
    pSrc->GetPoint(i, coord);
    coord[3] = 1.0;

    pTrg->GetPoint(i, target);

    //transform point
    res_matrix->MultiplyPoint(coord, result);

    dx = target[0] - result[0];
    dy = target[1] - result[1];
    dz = target[2] - result[2];

    deviation += dx * dx + dy * dy + dz * dz;
  }
  if(pSrc->GetNumberOfPoints() != 0)
    deviation /= pSrc->GetNumberOfPoints();
  deviation = sqrt(deviation);

  vtkDEL(RegisterTransform);
  return deviation;
}

void findTransform(mafVMELandmarkCloud *src, mafTimeStamp srctime, mafTimeStamp trgtime, const mafMatrix& mpar1, const mafMatrix& mpar2, mafMatrix& result)
{
  vtkPoints    *pointsCur = vtkPoints::New();
  vtkPoints    *pointsNxt = vtkPoints::New();
  ExtractMatchingPoints(src, srctime, trgtime, mpar1, mpar2, pointsCur, pointsNxt);
  RegisterPoints(pointsCur, pointsNxt, result.GetVTKMatrix());
  vtkDEL(pointsCur);
  vtkDEL(pointsNxt);
}

void GetLocalTransform(mafVME *vme, mafTimeStamp ts1, mafTimeStamp ts2, mafMatrix& transf)
{
  mafMatrix     mt;
  mafMatrix     mt1;
  mafMatrix     tmp;

  transf.Identity();
  if(vme == NULL)
    return;


  mafVMELandmarkCloud *lm = mafVMELandmarkCloud::SafeDownCast(vme);
  if(lm != NULL && GetAFRefSys(vme) == NULL)
  {
    mafMatrix mpar1, mpar2;
    mpar1.Identity();
    mpar2.Identity();
    if(lm->GetParent() != NULL)
    {
      GetGlobalMatrix(lm->GetParent(), ts1, mpar1);
      GetGlobalMatrix(lm->GetParent(), ts2, mpar2);
    }
    findTransform(lm, ts1, ts2, mpar1, mpar2, transf);
    return;
  }

  GetLocalMatrix(vme, ts1, mt);
  GetLocalMatrix(vme, ts2, mt1);
  mt.Invert();
  mafMatrix::Multiply4x4(mt1, mt, transf);
}

bool MultiplyMatVec(Mat_I_DP& m, Vec_I_DP& vi, Vec_O_DP& vo)
{
  assert(m.ncols() == vi.size());
  assert(m.nrows() == vo.size());
  if((m.ncols() != vi.size()) || (m.nrows() != vo.size()))
    return false;
  for(int i = 0; i < vo.size(); i++)
  {
    vo[i] = 0.0;
    for(int j = 0; j < vi.size(); j++)
    {
      vo[i] += m[i][j] * vi[j];
    }
  }
  return true;
}

bool MultiplyMatrix(Mat_I_DP& i1, Mat_I_DP& i2, Mat_O_DP& o)
{
  assert(i1.ncols() == i2.nrows());
  assert(i1.nrows() == o.nrows());
  assert(i2.ncols() == o.ncols());
  if((i1.ncols() != i2.nrows()) || (i1.nrows() != o.nrows()) || (i2.ncols() != o.ncols()))
    return false;
  for(int i = 0; i < o.nrows(); i++)
  {
    for(int j = 0; j < o.ncols(); j++)
    {
      o[i][j] = 0.0;
      for(int k = 0; k < i1.ncols(); k++)
      {
        o[i][j] += i1[i][k] * i2[k][j];
      }
    }
  }
  return true;
}

bool AddMatrix(Mat_I_DP& i1, Mat_I_DP& i2, Mat_O_DP& o)
{
  assert(i1.nrows() == i2.nrows());
  assert(i1.ncols() == i2.ncols());
  assert(i2.nrows() == o.nrows());
  assert(i2.ncols() == o.ncols());
  if((i1.nrows() != i2.nrows()) || (i1.ncols() != i2.ncols()) || (i2.nrows() != o.nrows()) || (i2.ncols() != o.ncols()))
    return false;
  for(int i = 0; i < o.nrows(); i++)
  {
    for(int j = 0; j < o.ncols(); j++)
    {
      o[i][j] = i1[i][j] + i2[i][j];
    }
  }
  return true;
}

bool SubMatrix(Mat_I_DP& i1, Mat_I_DP& i2, Mat_O_DP& o)
{
  assert(i1.nrows() == i2.nrows());
  assert(i1.ncols() == i2.ncols());
  assert(i2.nrows() == o.nrows());
  assert(i2.ncols() == o.ncols());
  if((i1.nrows() != i2.nrows()) || (i1.ncols() != i2.ncols()) || (i2.nrows() != o.nrows()) || (i2.ncols() != o.ncols()))
    return false;
  for(int i = 0; i < o.nrows(); i++)
  {
    for(int j = 0; j < o.ncols(); j++)
    {
      o[i][j] = i1[i][j] - i2[i][j];
    }
  }
  return true;
}

bool IdentityMatrix(Mat_IO_DP& mi)
{
  assert(mi.nrows() == mi.ncols());
  if(mi.nrows() != mi.ncols())
    return false;
  for(int i = 0; i < mi.nrows(); i++)
  {
    for(int j = 0; j < mi.ncols(); j++)
    {
      if(i == j)
        mi[i][j] = 1.0;
      else
        mi[i][j] = 0.0;
    }
  }
  return true;
}


bool TransposeMatrix(Mat_I_DP& mi, Mat_O_DP& o)
{
  assert(mi.ncols() == o.nrows());
  assert(mi.nrows() == o.ncols());
  if((mi.ncols() != o.nrows()) || (mi.nrows() != o.ncols()))
    return false;
  for(int i = 0; i < o.nrows(); i++)
  {
    for(int j = 0; j < o.ncols(); j++)
    {
      o[i][j] = mi[j][i];
    }
  }
  return true;
}

bool VecMatrix(Vec_I_DP& vi, Mat_O_DP& o)
{
  assert(vi.size() == o.nrows());
  assert(o.ncols() == 1);
  if((vi.size() != o.nrows()) || (o.ncols() != 1))
    return false;
  for(int i = 0; i < o.nrows(); i++)
  {
    o[i][0] = vi[i];
  }
  return true;
}


bool MaxEigen(int N, int NDIM, Mat_I_DP& A, double EPS, Vec_O_DP& E_vec, double& E_lam, int& Ier, int& K_iter)
{
  Ier = 1001;
  assert(E_vec.size() == 3);
  if(E_vec.size() != 3)
    return false;
  for(int i = 0; i < 3; i++)
    E_vec[i] = sqrt(1.0 * N);
  K_iter = 0;
  double R, S = 1.0 + EPS;
  while(S > EPS && K_iter <= Ier)
  {
    K_iter = K_iter + 1;
    Vec_DP Wrk(E_vec);
    MultiplyMatVec(A, Wrk, E_vec);
    R = sqrt(E_vec[0] * E_vec[0] + E_vec[1] * E_vec[1] + E_vec[2] * E_vec[2]);
    if(R <= EPS)
    {
      Ier = 1; 
      return true;
    }
    R = 1.0 / R;
    for(int i = 0; i < 3; i++)
      E_vec[i] *= R;
    for(int i = 0; i < 3; i++)
      Wrk[i] -= E_vec[i];
    S = sqrt(Wrk[0] * Wrk[0] + Wrk[1] * Wrk[1] + Wrk[2] * Wrk[2]);
  }
  if(S <= EPS)
    Ier = 0;
  E_lam = R;

  return true;
}
}
//----------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEHelAxis)
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void mafVMEHelAxis::GetShowTransform(mafTimeStamp ts, const V3d<double>& helicalAxis, const V3d<double>& point, mafMatrix& globalMatrix)
//----------------------------------------------------------------------------
{
  mafMatrix mShowHelical;

  V3d<double> right;
  V3d<double> up = point;
  V3d<double> at = helicalAxis;
  at /= sqrt(at.length2());
  if(up.length2() == 0.0)
  {
    V3d<double> ort(0.0, 0.0, 0.0);
    int         minind  = 0;
    if(fabs(at[1]) < fabs(at[minind]))
      minind = 1;
    if(fabs(at[2]) < fabs(at[minind]))
      minind = 2;
    ort[minind] = 1.0;
    up = at ^ ort;
  }
  up /= sqrt(up.length2());
  right = up ^ at;
  right /= sqrt(right.length2());
  up = at ^ right;
  up /= sqrt(up.length2());

  mShowHelical.SetElement(0, 0, right.x);
  mShowHelical.SetElement(1, 0, right.y);
  mShowHelical.SetElement(2, 0, right.z);
  mShowHelical.SetElement(3, 0, 0.0);
  mShowHelical.SetElement(0, 1, up.x);
  mShowHelical.SetElement(1, 1, up.y);
  mShowHelical.SetElement(2, 1, up.z);
  mShowHelical.SetElement(3, 1, 0.0);
  mShowHelical.SetElement(0, 2, at.x);
  mShowHelical.SetElement(1, 2, at.y);
  mShowHelical.SetElement(2, 2, at.z);
  mShowHelical.SetElement(3, 2, 0.0);
  mShowHelical.SetElement(0, 3, point.x);
  mShowHelical.SetElement(1, 3, point.y);
  mShowHelical.SetElement(2, 3, point.z);
  mShowHelical.SetElement(3, 3, 1.0);

  mafMatrix tmp;

  tmp.Identity();
  if(GetParent() != NULL && GetParent()->GetParent() != NULL)
    GetGlobalMatrix(GetParent()->GetParent(), ts, tmp);

  mafMatrix::Multiply4x4(tmp, mShowHelical, globalMatrix);
  globalMatrix.SetTimeStamp(ts);
}

//----------------------------------------------------------------------------
void mafVMEHelAxis::GetMomentAxis(mafTimeStamp tsTime, V3d<double>& helicalAxis, V3d<double>& point, double& angle, double& translationAmount, mafTimeStamp tsTimeTo)
//----------------------------------------------------------------------------
{
  mafMatrix                 mt;
  mafMatrix                 mtNxt;
  std::vector<mafTimeStamp> timeStamps;

  mafTimeStamp tscur, tsnxt;
  bool         inited = false;

  helicalAxis       = V3d<double>(1.0, 0.0, 0.0);
  point             = V3d<double>(0.0, 0.0, 0.0);
  angle             = 0;
  translationAmount = 0;

  if(GetParent() == NULL)
    return;

  size_t       cur, nxt;
  GetParent()->GetAbsTimeStamps(timeStamps);
  if(timeStamps.size() == 0)
    return;

  cur   = 0;
  while((cur + 1 < timeStamps.size()) && (timeStamps[cur + 1] <= tsTime))
    cur++;
  tscur = timeStamps[cur];

  nxt = cur + 1;
  while(nxt < timeStamps.size() && (timeStamps[nxt] < tsTimeTo))
    nxt++;
  tscur = timeStamps[cur];

  for(/*nxt = cur + 1*/; nxt < timeStamps.size(); nxt++)
  {
    tsnxt = timeStamps[nxt];
    GetLocalTransform(GetParent(), tscur, tsnxt, mt);

    angle       = 0.0;
    helicalAxis = V3d<double>(1.0, 0.0, 0.0);
    double cq = 0.5 * (mt.GetElement(0, 0) + mt.GetElement(1, 1) + mt.GetElement(2, 2) - 1.0);
    if(fabs(cq) < 1.0)
      angle = fabs(acos(cq) * mafMatrix3x3::RadiansToDegrees());

    if(angle >= m_MinAngle)
    {
      MatrixToHelicalAxis(mt, helicalAxis.components, point.components, angle, translationAmount);
    }
    if(angle < 0)
    {
      helicalAxis       = -helicalAxis;
      angle             = -angle;
      translationAmount = -translationAmount;
    }
    if(angle >= m_MinAngle)
      break;
  }

  return;
  if(nxt >= timeStamps.size())
    return;

  V3d<double> haxisNxt;
  V3d<double> pointNxt;
  double      angleNxt;
  double      transNxt;

  Mat_DP Qt_t(0.0, 3, 3);
  Vec_DP Qv_t(0.0, 3);
  unsigned tstart = 0;
  unsigned tend   = 0;

  Vec_DP rc(0.0, 3);
  Mat_DP Qmatr(0.0, 3, 3);
  Vec_O_INT vidx(3);
  DP dp;

  {
    Mat_DP R(0.0, 3, 3);
    Mat_DP E(0.0, 3, 3);
    Mat_DP Q(0.0, 3, 3);
    Mat_DP Q1(0.0, 3, 3);
    IdentityMatrix(E);
    for(int i = 0; i < 3; i++)
    {
      for(int j = 0; j < 3; j++)
      {
        R[i][j] = mt.GetElement(i, j);
      }
    }
    SubMatrix(R, E, Q);
    TransposeMatrix(Q, Q1);

    Mat_DP Qt(0.0, 3, 3);
    Mat_DP Qtt(Qt_t);
    Vec_DP Qv(0.0, 3);
    Vec_DP v(0.0, 3);
    v[0] = mt.GetElement(0, 3);
    v[1] = mt.GetElement(1, 3);
    v[2] = mt.GetElement(2, 3);
    MultiplyMatrix(Q, Q1, Qt);
    MultiplyMatVec(Q1, v, Qv);
    AddMatrix(Qtt, Qt, Qt_t);
    Qv_t[0] -= Qv[0];
    Qv_t[1] -= Qv[1];
    Qv_t[2] -= Qv[2];
  }


  bool adjFound = false;

  cur = nxt;
  tscur = timeStamps[cur];
  for(nxt = cur + 1; nxt < timeStamps.size(); nxt++)
  {
    tsnxt = timeStamps[nxt];
    GetLocalTransform(GetParent(), tscur, tsnxt, mtNxt);

    angleNxt = 0.0;
    haxisNxt = V3d<double>(1.0, 0.0, 0.0);
    double cq = 0.5 * (mt.GetElement(0, 0) + mt.GetElement(1, 1) + mt.GetElement(2, 2) - 1.0);
    if(fabs(cq) < 1.0)
      angleNxt = fabs(acos(cq) * mafMatrix3x3::RadiansToDegrees());

    if(angleNxt >= m_MinAngle)
    {
      MatrixToHelicalAxis(mtNxt, haxisNxt.components, pointNxt.components, angleNxt, transNxt);
    }
    if(angleNxt < 0)
    {
      haxisNxt = -haxisNxt;
      angleNxt = -angleNxt;
      transNxt = -transNxt;
    }
    if(angleNxt >= m_MinAngle)
    {
      adjFound = true;
      break;
    }
  }

  for(nxt = cur; (!adjFound) && nxt > 0; nxt--)
  {
    tsnxt = timeStamps[nxt - 1];
    GetLocalTransform(GetParent(), tsnxt, tscur, mtNxt);

    angleNxt = 0.0;
    haxisNxt = V3d<double>(1.0, 0.0, 0.0);
    double cq = 0.5 * (mt.GetElement(0, 0) + mt.GetElement(1, 1) + mt.GetElement(2, 2) - 1.0);
    if(fabs(cq) < 1.0)
      angleNxt = fabs(acos(cq) * mafMatrix3x3::RadiansToDegrees());

    if(angleNxt >= m_MinAngle)
    {
      MatrixToHelicalAxis(mtNxt, haxisNxt.components, pointNxt.components, angleNxt, transNxt);
    }
    if(angleNxt < 0)
    {
      haxisNxt = -haxisNxt;
      angleNxt = -angleNxt;
      transNxt = -transNxt;
    }
    if(angleNxt >= m_MinAngle)
    {
      adjFound = true;
      break;
    }
  }

  if(!adjFound)
    return;

  {
    Mat_DP R(0.0, 3, 3);
    Mat_DP E(0.0, 3, 3);
    Mat_DP Q(0.0, 3, 3);
    Mat_DP Q1(0.0, 3, 3);
    IdentityMatrix(E);
    for(int i = 0; i < 3; i++)
    {
      for(int j = 0; j < 3; j++)
      {
        R[i][j] = mtNxt.GetElement(i, j);
      }
    }
    SubMatrix(R, E, Q);
    TransposeMatrix(Q, Q1);

    Mat_DP Qt(0.0, 3, 3);
    Mat_DP Qtt(Qt_t);
    Vec_DP Qv(0.0, 3);
    Vec_DP v(0.0, 3);
    v[0] = mtNxt.GetElement(0, 3);
    v[1] = mtNxt.GetElement(1, 3);
    v[2] = mtNxt.GetElement(2, 3);
    MultiplyMatrix(Q, Q1, Qt);
    MultiplyMatVec(Q1, v, Qv);
    AddMatrix(Qtt, Qt, Qt_t);
    Qv_t[0] -= Qv[0];
    Qv_t[1] -= Qv[1];
    Qv_t[2] -= Qv[2];

    rc    = Qv_t;
    Qmatr = Qt_t;
    NR::ludcmp(Qmatr, vidx, dp);
    NR::lubksb(Qmatr, vidx, rc);
    point = V3d<double>(rc[0], rc[1], rc[2]);
  }
}

void mafVMEHelAxis::GetMeanAxis(mafTimeStamp tsMinTime, mafTimeStamp tsMaxTime, V3d<double>& Direction, V3d<double>& StartPoint, double& resang)
{
  std::vector<mafTimeStamp> stamps;
  unsigned                  taken = 0;

  Direction  = V3d<double>(1.0, 0, 0);
  StartPoint = V3d<double>(0, 0, 0);
  resang     = 0.0;
  if(GetParent() == NULL)
    return;


  mafVME *vme = GetParent();
  vme->GetAbsTimeStamps(stamps);

  Mat_DP Qt_t(0.0, 3, 3);
  Mat_DP R_MHA(0.0, 3, 3);
  Vec_DP Qv_t(0.0, 3);
  unsigned tstart = 0;
  unsigned tend   = 0;

  Vec_DP rc(0.0, 3);
  Mat_DP Qmatr(0.0, 3, 3);
  Vec_O_INT vidx(3);
  DP dp;

  unsigned start_index;
  unsigned end_index;
  for(start_index = 0; start_index < stamps.size(); start_index++)
  {
    if(stamps[start_index] >= tsMinTime)
      break;
  }
  for(end_index = stamps.size(); end_index > 0; end_index--)
  {
    if(stamps[end_index - 1] <= tsMaxTime)
      break;
  }
  tstart = start_index;
  for(unsigned t = start_index + 1; t < end_index; t++)
  {
    mafMatrix mt;
    tend = t;
    GetLocalTransform(vme, stamps[tstart], stamps[tend], mt);
    V3d<double> helicalAxis; 
    V3d<double> point; 
    double angle; 
    double translationAmount;

    helicalAxis = V3d<double>(1.0, 0.0, 0.0);
    angle       = 0.0;
    double cq = 0.5 * (mt.GetElement(0, 0) + mt.GetElement(1, 1) + mt.GetElement(2, 2) - 1.0);
    if(fabs(cq) < 1.0)
      angle = fabs(acos(cq) * mafMatrix3x3::RadiansToDegrees());


    if(angle >= m_MinAngle)
      MatrixToHelicalAxis(mt, helicalAxis.components, point.components, angle, translationAmount);
    else
      continue;
    if(angle < 0)
    {
      helicalAxis       = -helicalAxis;
      angle             = -angle;
      translationAmount = -translationAmount;
    }

    tstart = tend;
    taken++;

    Mat_DP R(0.0, 3, 3);
    Mat_DP E(0.0, 3, 3);
    Mat_DP Q(0.0, 3, 3);
    Mat_DP Q1(0.0, 3, 3);
    IdentityMatrix(E);
    for(int i = 0; i < 3; i++)
    {
      for(int j = 0; j < 3; j++)
      {
        R[i][j] = mt.GetElement(i, j);
      }
    }
    SubMatrix(R, E, Q);
    TransposeMatrix(Q, Q1);

    Mat_DP Qt(0.0, 3, 3);
    Mat_DP Qtt(Qt_t);
    Vec_DP Qv(0.0, 3);
    Vec_DP v(0.0, 3);
    Vec_DP u(0.0, 3);
    Mat_DP um(0.0, 3, 1);
    Mat_DP umt(0.0, 1, 3);
    Mat_DP uut(0.0, 3, 3);
    Mat_DP RMH(0.0, 3, 3);
    Mat_DP RMHA(R_MHA);
    v[0] = mt.GetElement(0, 3);
    v[1] = mt.GetElement(1, 3);
    v[2] = mt.GetElement(2, 3);
    MultiplyMatrix(Q, Q1, Qt);
    MultiplyMatVec(Q1, v, Qv);
    AddMatrix(Qtt, Qt, Qt_t);
    Qv_t[0] -= Qv[0];
    Qv_t[1] -= Qv[1];
    Qv_t[2] -= Qv[2];

    u[0] = helicalAxis[0];
    u[1] = helicalAxis[1];
    u[2] = helicalAxis[2];
    VecMatrix(u, um);
    TransposeMatrix(um, umt);
    MultiplyMatrix(um, umt, uut);
    SubMatrix(E, uut, RMH);
    AddMatrix(RMHA, RMH, R_MHA);
  }
  if(taken >= 2)
  {
    rc    = Qv_t;
    Qmatr = Qt_t;
    NR::ludcmp(Qmatr, vidx, dp);
    NR::lubksb(Qmatr, vidx, rc);

    Vec_DP e1(0.0, 3);
    Vec_DP e2(0.0, 3);
    Vec_DP e3(0.0, 3);
    e1[0] = 1.0;
    e2[1] = 1.0;
    e3[2] = 1.0;

    Mat_DP RMHAMatr(R_MHA);

    NR::ludcmp(RMHAMatr, vidx, dp);
    NR::lubksb(RMHAMatr, vidx, e1);
    NR::lubksb(RMHAMatr, vidx, e2);
    NR::lubksb(RMHAMatr, vidx, e3);
    for(int i = 0; i < 3; i++)
    {
      RMHAMatr[i][0] = e1[i];
      RMHAMatr[i][1] = e2[i];
      RMHAMatr[i][2] = e3[i];
    }

    Vec_DP E_vec(0.0, 3);
    double E_lam;
    int Ier, K_iter;
    MaxEigen(3, 3, RMHAMatr, 1e-7, E_vec, E_lam, Ier, K_iter);
    Direction  = V3d<double>(E_vec[0], E_vec[1], E_vec[2]);
    Direction /= sqrt(Direction.length2());
    StartPoint = V3d<double>(rc[0], rc[1], rc[2]);
  }

  if(taken == 0)
    resang = 0.0;
  else
    resang = 1.0;
}


void mafVMEHelAxis::InternalUpdate()
{
  mafMatrix mtr;
  mafTimeStamp tsTime = GetTimeStamp();
  if(m_Mode == 0)
  {
    GetMomentAxis(tsTime, m_Direction, m_StartPoint, m_Angle, m_Translation);
  }
  else if(m_Mode == 1)
  {
    if(m_MeanChanges)
    {
      m_MeanChanges = 0;
      GetMeanAxis(m_MinTime, m_MaxTime, m_Direction, m_StartPoint, m_Angle);
      m_Translation = 0.0;
    }
  }
  else /*if(m_Mode == 2)*/
  {
    GetMomentAxis(m_RefTime, m_Direction, m_StartPoint, m_Angle, m_Translation, tsTime);
  }


  //adjust orientation of axis according to proximal segment orientation
  mafMatrix proxMatr;
  proxMatr.Identity();
  if(GetParent() != NULL && GetParent()->GetParent() != NULL)
  {
    GetGlobalMatrix(GetParent()->GetParent(), tsTime, proxMatr);
  }



  if(AlignAxis(m_Direction, proxMatr, m_AligningMode))
  {
    m_Direction   = -m_Direction;
    m_Angle       = - m_Angle;
    m_Translation = - m_Translation;
  }

  if(m_Gui)
  {
    m_StrDir[0] = wxString::Format("x: %f",m_Direction[0]);
    m_StrDir[1] = wxString::Format("y: %f",m_Direction[1]);
    m_StrDir[2] = wxString::Format("z: %f",m_Direction[2]);
    m_StrPnt[0] = wxString::Format("x: %f",m_StartPoint[0]);
    m_StrPnt[1] = wxString::Format("y: %f",m_StartPoint[1]);
    m_StrPnt[2] = wxString::Format("z: %f",m_StartPoint[2]);
    m_StrAng    = (m_Mode == 1) ? "" : wxString::Format("x: %f",m_Angle);
    m_StrTrl    = (m_Mode == 1) ? "" : wxString::Format("x: %f",m_Translation);
    m_Gui->Update();
  }

  GetShowTransform(tsTime, m_Direction, m_StartPoint, mtr);
  if(m_Angle == 0.0)
    m_AngleFactor = 0.0;
  else
    m_AngleFactor = 1.0;
  //m_AngleFactor = angle;
  UpdateLengthFactor();
  SetAbsMatrix(mtr, tsTime);
}



bool mafVMEHelAxis::AlignAxis(const V3d<double>& direction, const mafMatrix& prox, int mode)
{
  const int align_choices[6][3] = {{0, 1, 2}, {0, 2, 1}, {1, 2, 0}, {1, 0, 2}, {2, 0, 1}, {2, 1, 0}};
  int index[3];
  double delta = 0.00001;
  bool invert = false;
  V3d<double> prxdirs[3];
  V3d<double> ha;
  double projs[3];

  if(mode == 0)
    return false;

  for(unsigned i = 0; i < 3; i++)
    index[i] = align_choices[mode - 1][i];
  ha = m_Direction;
  ha /= sqrt(ha.length2());
  for(unsigned i = 0; i < 3; i ++)
  {
    for(unsigned j = 0; j < 3; j++)
    {
      prxdirs[i][j] = prox.GetElement(j, i);
    }
    projs[i] = prxdirs[i] * ha;
  }

  if(projs[index[0]] < -delta)
    invert = true;
  else if(projs[index[0]] < delta)
  {
    if(projs[index[1]] < -delta)
      invert = true;
    else if(projs[index[1]] < delta)
    {
      if(projs[index[2]] < -delta)
        invert = true;
    }
  }
  return invert;
}

//----------------------------------------------------------------------------
void mafVMEHelAxis::SetMatrix(const mafMatrix &mat)
//----------------------------------------------------------------------------
{
  m_Transform->SetMatrix(mat);
  Modified();
}

//-------------------------------------------------------------------------
void mafVMEHelAxis::UpdateCS()
//-------------------------------------------------------------------------
{
  GetOutput()->Update();
  mafEvent *e  = new mafEvent(this,CAMERA_UPDATE);
  ForwardUpEvent(e);
  delete e;
}

//-------------------------------------------------------------------------
double mafVMEHelAxis::GetLengthFactor()
//-------------------------------------------------------------------------
{
  return m_LengthFactor;
}
//-------------------------------------------------------------------------
double mafVMEHelAxis::GetRadiusFactor()
//-------------------------------------------------------------------------
{
  return m_RadiusFactor;
}
//-------------------------------------------------------------------------
void mafVMEHelAxis::SetLengthFactor(double scale)
//-------------------------------------------------------------------------
{
  m_LengthFactor = scale;
  if (m_Gui)
  {
    m_Gui->Update();
  }
  UpdateLengthFactor();
}
//-------------------------------------------------------------------------
void mafVMEHelAxis::SetRadiusFactor(double scale)
//-------------------------------------------------------------------------
{
  m_RadiusFactor = scale;
  if (m_Gui)
  {
    m_Gui->Update();
  }
  UpdateLengthFactor();
}
//-------------------------------------------------------------------------
void mafVMEHelAxis::UpdateLengthFactor()
//-------------------------------------------------------------------------
{
  m_ScaleAxisTransform->Identity();
  m_ScaleAxisTransform->Scale(m_AngleFactor * m_RadiusFactor,m_AngleFactor * m_RadiusFactor,m_AngleFactor * m_LengthFactor);
  m_ScaleAxisTransform->Update();
  m_ScaleAxis->Update();
  Modified();
}
//-------------------------------------------------------------------------
char **mafVMEHelAxis::GetIcon()
//-------------------------------------------------------------------------
{
#include "mafVMESurface.xpm"
  return mafVMESurface_xpm;
}


//-------------------------------------------------------------------------
mafVMEHelAxis::mafVMEHelAxis() : mafVME()
//-------------------------------------------------------------------------
{
  mafNEW(m_Transform);
  mafVMEOutputSurface *output=mafVMEOutputSurface::New(); // an output with no data
  output->SetTransform(m_Transform); // force my transform in the output
  SetOutput(output);

  // attach a datapipe which creates a bridge between VTK and MAF
  mafDataPipeCustom *dpipe = mafDataPipeCustom::New();
  SetDataPipe(dpipe);

  DependsOnLinkedNodeOn();

  m_LengthFactor = 1.0;
  m_RadiusFactor = 1.0;
  m_AngleFactor  = 1.0;
  m_MinAngle     = 5.0;
  m_MinTime      = 0.0;
  m_MaxTime      = 0.0;
  m_RefTime      = 0.0;
  m_Mode         = 0;
  m_AligningMode = 0;

  m_Angle        = 0.0;
  m_Translation  = 0.0;
  m_StartPoint   = V3d<double>(0.0, 0.0, 0.0);
  m_Direction    = V3d<double>(1.0, 0.0, 0.0);

  m_StrDir[0]    = "1.0";
  m_StrDir[1]    = m_StrDir[2] = "0.0";
  m_StrPnt[0]    = m_StrPnt[1] = m_StrPnt[2] = "0.0";
  m_StrAng       = "0.0";
  m_StrTrl       = "0.0";


  m_MeanChanges  = 1;

  vtkUnsignedCharArray *data;
  float scalar_red[3]   = {255,0,0};
  float scalar_green[3] = {0,255,0};
  float scalar_blu[3]   = {0,0,255};

  m_ZArrow = vtkArrowSource::New();
  m_ZArrow->SetShaftRadius(m_ZArrow->GetTipRadius() / 5);
  m_ZArrow->SetTipResolution(40);
  m_ZArrow->SetTipRadius(m_ZArrow->GetTipRadius() / 2);
  m_ZArrow->Update();

  m_CenterSphere = vtkSphereSource::New();
  m_CenterSphere->SetRadius(m_ZArrow->GetShaftRadius() * 2);
  m_CenterSphere->SetCenter(0, 0, 0);
  m_CenterSphere->SetPhiResolution(20);
  m_CenterSphere->SetThetaResolution(20);
  m_CenterSphere->Update();
  //m_PolyData->DeepCopy(surf->GetOutput());
  //m_PolyData->Update();


  m_ZAxisTransform = vtkTransform::New();
  m_ZAxisTransform->PostMultiply();
  m_ZAxisTransform->RotateY(-90);
  m_ZAxisTransform->Translate(0, 0, -0.5);
  m_ZAxisTransform->Update();

  m_CenterTransform = vtkTransform::New();
  m_CenterTransform->PostMultiply();
  m_CenterTransform->Update();


  m_ZAxis  = vtkTransformPolyDataFilter::New();
  m_ZAxis->SetInput(m_ZArrow->GetOutput());
  m_ZAxis->SetTransform(m_ZAxisTransform);
  m_ZAxis->Update();

  m_Center  = vtkTransformPolyDataFilter::New();
  m_Center->SetInput(m_CenterSphere->GetOutput());
  m_Center->SetTransform(m_CenterTransform);
  m_Center->Update();

  int points = m_ZArrow->GetOutput()->GetNumberOfPoints() + m_CenterSphere->GetOutput()->GetNumberOfPoints();

  data = vtkUnsignedCharArray::New();
  data->SetName("AXES");
  data->SetNumberOfComponents(3);
  data->SetNumberOfTuples(points * 3);
  int i;
  for (i = 0; i < points; i++)
    data->SetTuple(i, scalar_red);

  // this filter do not copy the scalars also if all input 
  m_Axes = vtkAppendPolyData::New();    
  m_Axes->AddInput(m_ZAxis->GetOutput());
  m_Axes->AddInput(m_Center->GetOutput());

  m_Axes->Update();

  m_ScaleAxisTransform = vtkTransform::New();
  m_ScaleAxisTransform->Scale(m_RadiusFactor,m_RadiusFactor,m_LengthFactor);
  m_ScaleAxisTransform->Update();

  vtkMAFSmartPointer<vtkPolyData> axes_surface;
  axes_surface = m_Axes->GetOutput();
  axes_surface->SetSource(NULL);
  axes_surface->GetPointData()->SetScalars(data);
  vtkDEL(data);

  m_ScaleAxis  = vtkTransformPolyDataFilter::New();
  m_ScaleAxis->SetInput(axes_surface.GetPointer());
  m_ScaleAxis->SetTransform(m_ScaleAxisTransform);
  m_ScaleAxis->Update();

  //SetData(m_ScaleAxis->GetOutput(), -1);

  dpipe->SetInput(m_ScaleAxis->GetOutput());
}

//-------------------------------------------------------------------------
int mafVMEHelAxis::DeepCopy(mafNode *a)
//-------------------------------------------------------------------------
{ 
  if (Superclass::DeepCopy(a)==MAF_OK)
  {
    mafVMEHelAxis *vme_hel_axis = mafVMEHelAxis::SafeDownCast(a);
    m_MinAngle = vme_hel_axis->m_MinAngle;
    m_MinTime  = vme_hel_axis->m_MinTime;
    m_MaxTime  = vme_hel_axis->m_MaxTime;
    m_Mode     = vme_hel_axis->m_Mode;
    return MAF_OK;
  }  
  return MAF_ERROR;
}

//-------------------------------------------------------------------------
int mafVMEHelAxis::InternalInitialize()
//-------------------------------------------------------------------------
{
  if (Superclass::InternalInitialize()==MAF_OK)
  {
    // force material allocation
    GetMaterial();
    return MAF_OK;
  }
  return MAF_ERROR;
}
//-------------------------------------------------------------------------
mmaMaterial *mafVMEHelAxis::GetMaterial()
//-------------------------------------------------------------------------
{
  mmaMaterial *material = (mmaMaterial *)GetAttribute("MaterialAttributes");
  if (material == NULL)
  {
    material = mmaMaterial::New();
    SetAttribute("MaterialAttributes", material);
    if (m_Output)
    {
      ((mafVMEOutputSurface *)m_Output)->SetMaterial(material);
    }
  }
  return material;
}

//-------------------------------------------------------------------------
mafVMEHelAxis::~mafVMEHelAxis()
//-------------------------------------------------------------------------
{
  mafDEL(m_Transform);
  SetOutput(NULL);

  vtkDEL(m_CenterSphere);
  vtkDEL(m_CenterTransform);
  vtkDEL(m_Center);

  vtkDEL(m_ZArrow);
  vtkDEL(m_ZAxisTransform);
  m_ZAxis->SetTransform(NULL);
  vtkDEL(m_ZAxis);

  vtkDEL(m_Axes);
  vtkDEL(m_ScaleAxisTransform);
  m_ScaleAxis->SetTransform(NULL);
  vtkDEL(m_ScaleAxis);
}

//----------------------------------------------------------------------------
void mafVMEHelAxis::InternalPreUpdate()
//----------------------------------------------------------------------------
{

}

//-----------------------------------------------------------------------
int mafVMEHelAxis::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{  
  if (Superclass::InternalStore(parent)==MAF_OK)
  {
    parent->StoreMatrix("Transform",&m_Transform->GetMatrix());
    //code for backward compatibility
    parent->StoreDouble("ScaleFactor", m_LengthFactor);
    parent->StoreDouble("RadiusFactor", m_RadiusFactor);
    parent->StoreDouble("LengthFactor", m_LengthFactor);
    parent->StoreDouble("MinAngle", m_MinAngle);
    parent->StoreDouble("MinTime", m_MinTime);
    parent->StoreDouble("MaxTime", m_MaxTime);
    parent->StoreInteger("Mode", m_Mode);
    parent->StoreInteger("AligningMode", m_AligningMode);
    return MAF_OK;
  }
  return MAF_ERROR;
}

//-----------------------------------------------------------------------
int mafVMEHelAxis::InternalRestore(mafStorageElement *node)
//-----------------------------------------------------------------------
{
  if (Superclass::InternalRestore(node)==MAF_OK)
  {
    mafMatrix matrix;
    if (node->RestoreMatrix("Transform",&matrix)==MAF_OK)
    {
      m_Transform->SetMatrix(matrix);
      //code for backward compatibility
      node->RestoreDouble("ScaleFactor", m_LengthFactor);
      m_RadiusFactor = m_LengthFactor;
      node->RestoreDouble("RadiusFactor", m_RadiusFactor);
      node->RestoreDouble("LengthFactor", m_LengthFactor);
      node->RestoreDouble("MinAngle", m_MinAngle);
      node->RestoreDouble("MinTime", m_MinTime);
      node->RestoreDouble("MaxTime", m_MaxTime);
      node->RestoreInteger("Mode", m_Mode);
      node->RestoreInteger("AligningMode", m_AligningMode);
      m_MeanChanges = 1;
      SetRadiusFactor(m_RadiusFactor);
      SetLengthFactor(m_LengthFactor);
      return MAF_OK;
    }
  }
  return MAF_ERROR;
}


//----------------------------------------------------------------------------
void mafVMEHelAxis::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  switch (maf_event->GetId())
  {
  case ID_MIN_ANGLE:
  case ID_MIN_TIME:
  case ID_MAX_TIME:
    {
      m_MeanChanges = 1;
      Modified();
      mafEvent cam_event(this,CAMERA_UPDATE);
      this->ForwardUpEvent(cam_event);
      break;
    }
  case ID_MODE:
    {
      m_MeanChanges = 1;
      m_Gui->Enable(ID_MIN_TIME, m_Mode == 1);
      m_Gui->Enable(ID_MAX_TIME, m_Mode == 1);
      m_Gui->Enable(ID_REF_TIME, m_Mode == 2);
      Modified();
      mafEvent cam_event(this,CAMERA_UPDATE);
      this->ForwardUpEvent(cam_event);
      break;
    }
  case ID_LENGTH_FACTOR:
    {
      SetLengthFactor(m_LengthFactor);
      mafEvent cam_event(this,CAMERA_UPDATE);
      this->ForwardUpEvent(cam_event);
      break;
    }
  case ID_RADIUS_FACTOR:
    {
      SetRadiusFactor(m_RadiusFactor);
      mafEvent cam_event(this,CAMERA_UPDATE);
      this->ForwardUpEvent(cam_event);
      break;
    }
  case ID_ALIGNING:
    {
      Modified();
      mafEvent cam_event(this,CAMERA_UPDATE);
      this->ForwardUpEvent(cam_event);
      break;
    }
  default:
    {
      Superclass::OnEvent(maf_event);
      break; 
    }
  }
}

//----------------------------------------------------------------------------
mafGUI *mafVMEHelAxis::CreateGui()
//----------------------------------------------------------------------------
{
  const mafString mode_choices[] = {_("Instant"), _("Mean"), _("Relative")};
  const mafString align_choices[] = {_("None"), _("XYZ"), _("XZY"), _("YZX"), _("YXZ"), _("ZXY"), _("ZYX")};
  m_Gui = Superclass::CreateGui();
  m_Gui->Show(false);
  m_Gui->Double(ID_RADIUS_FACTOR,_("radius scale"),&m_RadiusFactor);
  m_Gui->Double(ID_LENGTH_FACTOR,_("length scale"),&m_LengthFactor);
  m_Gui->Double(ID_MIN_ANGLE, _("Min angle"), &m_MinAngle, 0.0);
  m_Gui->Combo(ID_MODE, _("Mode"), &m_Mode, 3, mode_choices, _("Select mode"));
  m_Gui->Combo(ID_ALIGNING, _("Align"), &m_AligningMode, 7, align_choices, _("Select aligning"));

  m_Gui->Double(ID_MIN_TIME, _("Min time"), &m_MinTime, 0.0);
  m_Gui->Double(ID_MAX_TIME, _("Max time"), &m_MaxTime);
  m_Gui->Double(ID_REF_TIME, _("Ref time"), &m_RefTime);
  m_Gui->Divider();
  m_Gui->Button(ID_PRINT, "print", "debug info" );
  m_Gui->Enable(ID_MIN_TIME, m_Mode == 1);
  m_Gui->Enable(ID_MAX_TIME, m_Mode == 1);
  m_Gui->Enable(ID_REF_TIME, m_Mode == 2);
  m_Gui->Divider();
  m_Gui->Label("Direction:");
  m_StrDir[0] = wxString::Format("x: %f",m_Direction[0]);
  m_StrDir[1] = wxString::Format("y: %f",m_Direction[1]);
  m_StrDir[2] = wxString::Format("z: %f",m_Direction[2]);
  m_Gui->Label("", &m_StrDir[0]);
  m_Gui->Label("", &m_StrDir[1]);
  m_Gui->Label("", &m_StrDir[2]);
  m_Gui->Label("Rotation Center:");
  m_StrPnt[0] = wxString::Format("x: %f",m_StartPoint[0]);
  m_StrPnt[1] = wxString::Format("y: %f",m_StartPoint[1]);
  m_StrPnt[2] = wxString::Format("z: %f",m_StartPoint[2]);
  m_Gui->Label("", &m_StrPnt[0]);
  m_Gui->Label("", &m_StrPnt[1]);
  m_Gui->Label("", &m_StrPnt[2]);
  m_Gui->Label("Angle:");
  m_StrAng    = (m_Mode == 1) ? "" : wxString::Format("%f",m_Angle);
  m_Gui->Label("", &m_StrAng);
  m_Gui->Label("Translation:");
  m_StrTrl    = (m_Mode == 1) ? "" : wxString::Format("%f",m_Translation);
  m_Gui->Label("", &m_StrTrl);
  m_Gui->Update();
  return m_Gui;
}

//----------------------------------------------------------------------------
void mafVMEHelAxis::Print(std::ostream& os, const int tabs)// const
//-----------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  mafIndent indent(tabs);
  os<<indent<<"RadiusScale: "<<indent<<m_RadiusFactor;
  os<<indent<<"LengthScale: "<<indent<<m_LengthFactor;
}
