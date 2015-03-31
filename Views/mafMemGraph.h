/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafMemGraph.h,v $
  Language:  C++
  Date:      $Date: 2007-08-22 14:01:40 $
  Version:   $Revision: 1.1 $
  Authors:   Vladik Aranov/Fedor Moiseev    
==========================================================================
  Copyright (c) 2002/2004
  ULB - Universite Libre de Bruxelles
=========================================================================*/

#ifndef __mafMemoryGraph_H__
#define __mafMemoryGraph_H__

#include <vector>
#include "vectors.h"
#include "splines.h"

//----------------------------------------------------------------------------
// forward references
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// type definitions
//----------------------------------------------------------------------------
/**
Main class for graph in-memory representation*/

template <class T> 
class mafMemoryGraphBase
{
public:
  /**
  public constructor */
  mafMemoryGraphBase(unsigned int size = 1000);
  //and  destructor
  virtual ~mafMemoryGraphBase();
  /**
  extract point based on logical index
  nPoint is logical index
  nCoord the actual coordinate index*/
  T operator()(unsigned int point,unsigned int coord, unsigned int deriv = 0) const{if(deriv > 2)deriv = 2;return GetSmoothedValue(point, coord, deriv);/* : /GetValue(point, coord, deriv);*/}

  /**
  Remove all elements from in-memory graph container*/
  void                Clean();
  
  /**
  Support warp break index: needed for visualization*/
  unsigned int        BreakBegin() const{return m_BreakBegin;}
  unsigned int        BreakEnd() const{return m_BreakEnd;}

  /**
  Add point as vector to graph storage */
  void   AddPoint(const std::vector<T> &point);
  /**
  Add only one coordinate: other will be undefined!  */
  void                SetAddCoord(unsigned int coord, T value);
  
  /**
  Get number of free memory in storage in elements */
  int                 GetFreeMemSpace() const;
  /**
  Get number of used memory in storage in elements */
  int                 GetUsedMemSpace() const;

  //
  inline unsigned int GetDim() const {return m_Dim;}
  inline unsigned int GetXDim()const {return m_XParams.Dim;}
  inline unsigned int GetYDim()const {return m_YParams.Dim;}

  // Following functions works with indexes. Each value can have unique int32 index 
  // that allow user to identify what kind of value stored in certain index
  inline unsigned int GetXIndex(unsigned int index) const{wxASSERT(index < m_XParams.Dim);return m_XParams.Indexes[index];}
  inline unsigned int GetYIndex(unsigned int index) const{wxASSERT(index < m_YParams.Dim);return m_YParams.Indexes[index];}

  inline unsigned int GetXDeriv(unsigned int index) const{wxASSERT(index < m_XParams.Dim);return m_XParams.Derives[index];}
  inline unsigned int GetYDeriv(unsigned int index) const{wxASSERT(index < m_YParams.Dim);return m_YParams.Derives[index];}

  inline void         SetXDeriv(unsigned int index,unsigned int XDerive){wxASSERT(index < m_XParams.Dim);m_XParams.Derives[index] = XDerive;}
  inline void         SetYDeriv(unsigned int index,unsigned int YDerive){wxASSERT(index < m_YParams.Dim);m_YParams.Derives[index] = YDerive;}
 
  inline int          GetID(unsigned int coord) const{wxASSERT(coord < m_Dim);return m_IDs[coord];}
  inline void         SetID(unsigned int coord, int ID){wxASSERT(coord < m_Dim && coord > 0);m_IDs[coord] = ID;}

  inline int          GetXID(unsigned int index) const{wxASSERT(index < m_XParams.Dim);return GetID(m_XParams.Indexes[index]);}
  inline int          GetYID(unsigned int index) const{wxASSERT(index < m_YParams.Dim);return GetID(m_YParams.Indexes[index]);}

  bool                AddYVar(int ID, unsigned int deriv = 0){return AddVar(ID, deriv, m_YParams);}
  bool                RemYVar(unsigned int coord){return RemVar(coord, m_YParams, m_XParams);}
  bool                SetYVar(unsigned int index, int ID, unsigned int deriv = 0){return SetVar(index, ID, deriv, m_YParams, m_XParams);}

  bool                AddXVar(int ID, unsigned int deriv = 0){return AddVar(ID, deriv, m_XParams);}
  bool                RemXVar(unsigned int coord){return RemVar(coord, m_XParams, m_YParams);}
  bool                SetXVar(unsigned int index, int ID, unsigned int deriv = 0){return SetVar(index, ID, deriv, m_XParams, m_YParams);}

  void                SetSmooth(bool smooth){m_Smooth = smooth;}
  bool                GetSmooth() const{return m_Smooth;}

  void                SetSmoothParam(const T& param){m_SmoothParam = param;  for(unsigned i = 0; i < m_Dim; i++){m_Smoothed[i] = false;}}
  T                   GetSmoothParam() const{return m_SmoothParam;}
protected:
  T                   GetValue(unsigned int point,unsigned int coord, unsigned int deriv = 0) const;
  T                   GetSmoothedValue(unsigned int point, unsigned int coord, unsigned int deriv = 0) const;
  bool                GetValueByParam(T& value, const T& param, unsigned int coord, unsigned int deriv = 0) const;
private:
  struct VarParams
  {
    std::vector<unsigned int> Indexes;
    std::vector<unsigned int> Derives;
    unsigned int              Dim;
  };

  void                SetDim(unsigned int dimension);

  bool                AddVar(int, unsigned int deriv, VarParams& primparam);
  bool                RemVar(unsigned int coord, VarParams& primparam, VarParams& secparam);
  bool                SetVar(unsigned int index, int ID, unsigned int deriv, VarParams& primparam, VarParams& secparam);

  void                RemovePoints(int points);
  void                RemovePoint();
  void                UpdateBreakPosition();
  void                RemovePrevPassPoints();
  unsigned int        GetActualIdx(unsigned int point) const;
  void                FillSmoothedValues(unsigned int coord) const;

  virtual const T&    GetGarbageValue() = 0;

  /// user data is stored here!!! 
  std::vector<std::vector<T> >        m_Points;
  ///last added point sometimes stored here. No user access. For SetAddCoord method.
  std::vector<T>                      m_Coords;
  /// marks what fields in last added coords are filled. For SetAddCoord method.
  std::vector<bool>                   m_Filled;
  /// number of already added fields. For SetAddCoord method.
  unsigned int                        m_FilledPoints;
  /// marks what fields in last added coords are filled. For SetAddCoord method.
  mutable std::vector<bool>           m_Smoothed;

  ///native index of array beginning
  unsigned int                        m_Begin;
  ///native index of array end
  unsigned int                        m_End;

  //normalized index for break
  unsigned int                        m_BreakBegin;
  //normalized index for break end
  unsigned int                        m_BreakEnd;

  ///maximum possible number of points
  unsigned int                        m_MaxSize;
  ///number of graphs drawn at once
  unsigned int                        m_Dim;

  ///Can be managed by user
  std::vector<int>                    m_IDs;
  VarParams                           m_XParams;
  VarParams                           m_YParams;

  T                                   m_MinParam, m_MaxParam;

  ///internal usage
  T                                   m_UnnoticedRange;
  //memory graph tunings
  T                                   m_SmoothParam;
  T                                   m_RangeGrowStep;
  T                                   m_PointNoticeCriterion;
  T                                   m_SpaceInRange;

  mutable std::vector<ForArray<T> >   m_X;
  mutable std::vector<ForArray<T> >   m_Y;
  mutable std::vector<ForArray<T> >   m_S;
  mutable std::vector<ForArray<T> >   m_Z;
  mutable std::vector<ForArray<T> >   m_A;
};

template <class T>
mafMemoryGraphBase<T>::mafMemoryGraphBase(unsigned int size)
{
  m_MaxSize        = size;

  m_Points.resize(size);

  m_XParams.Dim = 0;
  m_YParams.Dim = 0;

  SetDim(1);

  m_IDs[0]                = 0;
  m_RangeGrowStep         = 5.0;
  m_PointNoticeCriterion  = 0.0025;
  m_SpaceInRange          = 0.000001;
  m_SmoothParam           = 0.0;
}

//Get number of free entries
template <class T>
int  mafMemoryGraphBase<T>::GetFreeMemSpace() const
{
  return m_MaxSize - GetUsedMemSpace();
}

//Get number of allocated entries
template <class T>
int mafMemoryGraphBase<T>::GetUsedMemSpace() const
{
  return m_End - m_Begin;
}

template <class T>
unsigned int mafMemoryGraphBase<T>::GetActualIdx(unsigned int point) const
{
  return (m_Begin + point) % m_MaxSize;
}


template <class T>
void mafMemoryGraphBase<T>::FillSmoothedValues(unsigned int coord) const
{
  if(!m_Smoothed[coord])
  {
    m_Smoothed[coord] = true;

    unsigned int part_begin = m_BreakEnd;
    unsigned int part_end   = GetUsedMemSpace();
    for(int p = 0; p < 2; p++)
    {
      unsigned int sz = part_end - part_begin;
      //arrays to process separately all components of 3D vectors
      if(sz >= 2)
      {
        //initial sizes
        m_X[2 * coord + p].assign(sz, 0.0);
        m_Y[2 * coord + p].assign(sz, 0.0);
        m_S[2 * coord + p].assign(sz, 0.0);
        m_Z[2 * coord + p].assign(sz, 0.0);
        m_A[2 * coord + p].assign(sz, 0.0);

        V3d<T> tmp;

        //filling with values from input parameters
        for(unsigned int k = part_begin; k < part_end; k++)
        {
          m_X[2 * coord + p][k - part_begin] = m_Points[GetActualIdx(k)][0];//GetValue(k, 0, 0);
          m_Y[2 * coord + p][k - part_begin] = m_Points[GetActualIdx(k)][coord];//GetValue(k, coord, 0);
          m_S[2 * coord + p][k - part_begin] = m_SmoothParam;
        }
        //building smoothing 1D splines for each vector component
        Smspline<T>(sz, 2, 0, m_X[2 * coord + p], m_Y[2 * coord + p], m_S[2 * coord + p], 0, 0, m_A[2 * coord + p], m_Z[2 * coord + p], 0, tmp[0], tmp[1], tmp[2]);
        /*for(unsigned int k = part_begin; k < part_end; k++)
        {
          Smspline<T>(sz, 2, 1, x, y, s, 0, 0, a, z, x[k - part_begin], tmp[0], tmp[1], tmp[2]);
          m_PointsSmth[GetActualIdx(k)][coord][0] = tmp[0];
          m_PointsSmth[GetActualIdx(k)][coord][1] = tmp[1];
          m_PointsSmth[GetActualIdx(k)][coord][2] = tmp[2];
        }*/
      }
      part_begin = 0;
      part_end   = m_BreakBegin;
    }
  }
}


template <class T>
T mafMemoryGraphBase<T>::GetValue(unsigned int point, unsigned int coord, unsigned int deriv) const
{
  wxASSERT(deriv <= 2 && deriv >=0);
  if(deriv == 0)
    return m_Points[GetActualIdx(point)][coord];
  return GetSmoothedValue(point, coord, deriv);
}

template <class T>
T mafMemoryGraphBase<T>::GetSmoothedValue(unsigned int point, unsigned int coord, unsigned int deriv) const
{
  T res;
  bool ok;
  wxASSERT(deriv <= 2 && deriv >=0);
  FillSmoothedValues(coord);
  ok = GetValueByParam(res, m_Points[GetActualIdx(point)][0], coord, deriv);
  if(!ok)
  {
    if(deriv == 0)
      return m_Points[GetActualIdx(point)][coord];
  }
  //wxASSERT(ok);
  return res;
}

template <class T>
bool mafMemoryGraphBase<T>::GetValueByParam(T& value, const T& param, unsigned int coord, unsigned int deriv) const
{
  wxASSERT(deriv <= 2 && deriv >=0);
  FillSmoothedValues(coord);
  unsigned int p1_begin = m_BreakEnd;
  unsigned int p1_end   = GetUsedMemSpace();
  unsigned int p2_begin = 0;
  unsigned int p2_end   = m_BreakBegin;
  if(p1_end - p1_begin >= 2)
  {
    if(param >= m_Points[GetActualIdx(p1_begin)][0] && param <= m_Points[GetActualIdx(p1_end - 1)][0])
    {
      unsigned p = 0;
      V3d<T> tmp;
      Smspline<T>(p1_end - p1_begin, 2, 1, m_X[2 * coord + p], m_Y[2 * coord + p], m_S[2 * coord + p], 0, 0, m_A[2 * coord + p], m_Z[2 * coord + p], param, tmp[0], tmp[1], tmp[2]);
      value = tmp[deriv];
      return true;
    }
  }
  if(p2_end - p2_begin >= 2)
  {
    if(param >= m_Points[GetActualIdx(p2_begin)][0] && param <= m_Points[GetActualIdx(p2_end - 1)][0])
    {
      unsigned p = 1;
      V3d<T> tmp;
      Smspline<T>(p2_end - p2_begin, 2, 1, m_X[2 * coord + p], m_Y[2 * coord + p], m_S[2 * coord + p], 0, 0, m_A[2 * coord + p], m_Z[2 * coord + p], param, tmp[0], tmp[1], tmp[2]);
      value = tmp[deriv];
      return true;
    }
  }
  return false;
}

//Remove N obsolete points
template <class T>
void   mafMemoryGraphBase<T>::RemovePoints(int points)
{
  for(int i = 0; i < points; i++)
    RemovePoint();
} // end of mafMemoryGraphBase<T>::RemovePoints

//remove one point obsolete for some reasons
template <class T>
void   mafMemoryGraphBase<T>::RemovePoint()
{
  unsigned int i;

  //access storage
  if(GetUsedMemSpace() == 0)
  {
    wxASSERT(m_Begin == 0 && m_End == 0);
    //nothing to do here anymore
    return;
  }
  else if(GetUsedMemSpace() == 1)
  {
    //one element case
    //invalidate it
    for(i = 0;i < m_Dim; i++)
    {
      m_Points[m_Begin][i] = GetGarbageValue();
    }
    //remove it
    m_Begin = 0;
    m_End   = 0;
  }
  else
  {
    //have more than two elements here to spare
    //invalidate it
    for(i = 0;i < m_Dim; i++)
    {
      m_Points[m_Begin][i] = GetGarbageValue();
    }
    m_Begin++;
    //check warp
    if(m_Begin == m_MaxSize)
    {
      m_Begin = 0;
      m_End  -= m_MaxSize;
    }
    if(m_Begin + 1 == m_End)
    {
      //move it
      for(i = 0;i < m_Dim; i++)
      {
        m_Points[0][i] = m_Points[m_Begin][i];
      }
      //remove it
      m_Begin = 0;
      m_End   = 1;
    }
  }

  UpdateBreakPosition();
}
  
//Add new point into the class
template <class T>
void mafMemoryGraphBase<T>::AddPoint(const std::vector<T> &point)
{
  T            diff;
  T            noticeableVal;
  T            length;
  T            addGrow;

  for(unsigned i = 0; i < m_Dim; i++)
  {
    m_Smoothed[i] = false;
  }

  if(GetUsedMemSpace() == 0)
  {
    //list is empty: just add to end
    m_Points[m_End++] = point;
    m_MinParam = point[0] - m_SpaceInRange;
    m_MaxParam = point[0] + m_SpaceInRange;
  }
  else //add to non empty container
  {
    diff = point[0] - GetValue(GetUsedMemSpace() - 1, 0, 0);
    if(diff > 0.0) //do not apply to warped points
    {
      noticeableVal = m_PointNoticeCriterion * (m_MaxParam - m_MinParam);
      if((diff + m_UnnoticedRange) < noticeableVal)
      {
        //this point is not worth to be noticed ==> ignore, bat mark
        m_UnnoticedRange += diff;
        return;
      }
      else
      {
        //adjust marker
        m_UnnoticedRange = (diff + m_UnnoticedRange) - noticeableVal;
        //do not affect to much
        m_UnnoticedRange = ((m_UnnoticedRange > noticeableVal) ? noticeableVal : m_UnnoticedRange);
        m_UnnoticedRange = ((m_UnnoticedRange < 0.0) ? 0.0 : m_UnnoticedRange);
      }
    }
    //update points rect
    length                = m_MaxParam - m_MinParam;
    addGrow               = length * m_RangeGrowStep * 0.01;
    if(point[0] > m_MaxParam)
    {
      m_MaxParam   = point[0] + addGrow;
    }
    if(point[0] < m_MinParam)
    {
      m_MinParam = point[0] - addGrow;
    }

    //not empty
    //TBD: Check is this point already exists in container SLOW!
    //check end position
    m_Points[m_End % m_MaxSize] = point;
    m_End++;
    if(m_End  % m_MaxSize == m_Begin  % m_MaxSize)
      m_Begin++;
    if(m_Begin >= m_MaxSize)
    {
      m_Begin -= m_MaxSize;
      m_End   -= m_MaxSize;
    }
  }
  UpdateBreakPosition();
  RemovePrevPassPoints();
  //wxASSERT(!this->bSecondaryBreak);
}


template <class T>
void  mafMemoryGraphBase<T>::SetAddCoord(unsigned int coord, T val)
{
  unsigned int i;
  wxASSERT(coord < m_Dim);
  wxASSERT(!m_Filled[coord]);
  m_Coords[coord] = val;
  if(!m_Filled[coord])
  {
    m_Filled[coord] = true;
    m_FilledPoints++;
  }
  if(m_FilledPoints == m_Dim)
  {
    m_FilledPoints   = 0;
    for(i = 0;i < m_Dim;i++)
    {
      m_Filled[i]   = false;
      m_Smoothed[i] = false;
    }
    AddPoint(m_Coords);
  }
}


//Remove points from prev. pass
template <class T>
void  mafMemoryGraphBase<T>::RemovePrevPassPoints()
{
  int   points = GetUsedMemSpace();
  unsigned int stage, NumToRem;

  //do not work until having less than 3 points
  if(points <= 3) 
    return;

  stage    = 0;
  NumToRem = 0;
  for(int i = points; i >= 2; i--)
  {
    if(GetValue(i - 2, 0, 0) >= GetValue(i - 1, 0, 0))
    {
      stage++;
    }
    if(stage >= 2 || (GetValue(points - 1, 0, 0) >= GetValue(i - 2, 0, 0) && stage >= 1))
    {
      NumToRem++;
    }
  }
  RemovePoints(NumToRem);
  if(NumToRem > 0)
  {
    UpdateBreakPosition();
  }
}

//Support state for break index
template <class T>
void  mafMemoryGraphBase<T>::UpdateBreakPosition()
{
  int   NumUsed = GetUsedMemSpace();
  m_BreakBegin = m_BreakEnd = NumUsed;
  for(int i = 0;i + 1 < NumUsed; i++)
  {
    if(GetValue(i, 0, 0) >= GetValue(i + 1, 0, 0))
    {
      m_BreakBegin = m_BreakEnd = i + 1;
      break;
    }
  }
}

template <class T>
mafMemoryGraphBase<T>::~mafMemoryGraphBase()
{
}

template <class T>
void  mafMemoryGraphBase<T>::SetDim(unsigned int dimension)
{
  unsigned int i;

  wxASSERT(dimension>=1);
  m_Dim = dimension;

  for(i = 0;i < m_MaxSize;i++)
  {
    m_Points[i].resize(dimension);
  }

  m_X.resize(2 * dimension);
  m_Y.resize(2 * dimension);
  m_S.resize(2 * dimension);
  m_Z.resize(2 * dimension);
  m_A.resize(2 * dimension);


  m_Coords.resize(dimension);
  m_Filled.resize(dimension);
  m_Smoothed.resize(dimension);
  m_IDs.resize(dimension);
  m_XParams.Indexes.resize(3 * dimension);
  m_YParams.Indexes.resize(3 * dimension);
  m_XParams.Derives.resize(3 * dimension);
  m_YParams.Derives.resize(3 * dimension);
  Clean();
}

template <class T>
void mafMemoryGraphBase<T>::Clean()
{
  unsigned int i;

  m_Begin          = 0;
  m_BreakBegin     = 0;
  m_End            = 0;
  m_BreakEnd       = 0;

  m_FilledPoints = 0;
  for(i = 0;i < m_Dim;i++)
  {
    m_Filled[i] = false;
    m_Smoothed[i] = false;
  }
  //similar points skip system
  m_UnnoticedRange = 0.0f;
}

template <class T>
bool mafMemoryGraphBase<T>::AddVar(int ID, unsigned int deriv, typename mafMemoryGraphBase::VarParams& primparam)
{
  unsigned int index;
  unsigned int i;

  for(i = 0; i < primparam.Dim; i++)//if this variable is already in Y list we have nothing to do, report about error
  {
    if(ID == GetID(primparam.Indexes[i]) && deriv == primparam.Derives[i])
    {
      return false;
    }
  }

  index = m_Dim;
  for(i = 0; i < m_Dim; i++)//check if variable is already in list
  {
    if(ID == GetID(i))
    {
      index = i;
      break;
    }
  }

  if(index == m_Dim)//variable not found
  {
    Clean();//clean all stored data
    SetDim(m_Dim + 1);//increase only total dimension, Y dimension will be increased later 
    SetID(index, ID);
  }

  primparam.Dim++;
  primparam.Indexes[primparam.Dim - 1] = index;
  primparam.Derives[primparam.Dim - 1] = deriv;
  return true;
}

template <class T>
bool mafMemoryGraphBase<T>::RemVar(unsigned int coord, typename mafMemoryGraphBase::VarParams& primparam, typename mafMemoryGraphBase::VarParams& secparam)
{
  unsigned int index;
  unsigned int i, j;

  if(primparam.Dim == 0)
  {
    return false;
  }


  wxASSERT(coord < primparam.Dim);
  index = primparam.Indexes[coord];

  wxASSERT(index < m_Dim);
  if(index == 0)
  {
    //<clean only Y indexes>
    for(j = coord; j < primparam.Dim - 1; j++)
    {
      primparam.Indexes[j] = primparam.Indexes[j + 1];
      primparam.Derives[j] = primparam.Derives[j + 1];
    }
    primparam.Dim--;
    return true;
  }
  for(i = 0; i < secparam.Dim; i++)
  {
    if(index == secparam.Indexes[i])
    {
      //<clean only Y indexes>
      for(j = coord; j < primparam.Dim - 1; j++)
      {
        primparam.Indexes[j] = primparam.Indexes[j + 1];
        primparam.Derives[j] = primparam.Derives[j + 1];
      }
      primparam.Dim--;
      return true;
    }
  }
  for(i = 0; i < primparam.Dim; i++)
  {
    if(index == primparam.Indexes[i] && i != coord)
    {
      //<clean only Y indexes>
      for(j = coord; j < primparam.Dim - 1; j++)
      {
        primparam.Indexes[j] = primparam.Indexes[j + 1];
        primparam.Derives[j] = primparam.Derives[j + 1];
      }
      primparam.Dim--;
      return true;
    }
  }
  Clean();//clean all stored data

  //<clean both IDs and Y indexes>
  for(j = coord; j < primparam.Dim - 1; j++)
  {
    primparam.Indexes[j] = primparam.Indexes[j + 1];
    primparam.Derives[j] = primparam.Derives[j + 1];
  }

  for(i = index; i < m_Dim - 1; i++)//shift IDs
  {
    SetID(i, GetID(i + 1));
  }

  for(i = 0; i < secparam.Dim; i++)//correct X indexes
  {
    if(index < secparam.Indexes[i])
    {
      secparam.Indexes[i]--;
    }
  }

  for(i = 0; i < primparam.Dim; i++)//correct Y indexes
  {
    if(index < primparam.Indexes[i])
    {
      primparam.Indexes[i]--;
    }
  }
  primparam.Dim--;
  SetDim(m_Dim - 1);
  return true;
}

template <class T>
bool mafMemoryGraphBase<T>::SetVar(unsigned int index, int ID, unsigned int deriv, typename mafMemoryGraphBase::VarParams& primparam, typename mafMemoryGraphBase::VarParams& secparam)
{
  unsigned int NewIndex, OldIndex;
  unsigned int i;
  bool   bRemove, bAdd;
  if(ID == GetID(primparam.Indexes[index]))//if we don't want to change it return
  {
    primparam.Derives[index] = deriv;
    return true;
  }
  for(i = 0; i < primparam.Dim; i++)//if we try to set this ID for second time return with error
  {
    if(ID == GetID(primparam.Indexes[i]) && primparam.Derives[i] == deriv)
    {
      return false;
    }
  }

  OldIndex = primparam.Indexes[index];//Index of old coordinate
  if(OldIndex == 0)
  {
    bRemove = false;
  }
  else
  {
    bRemove   = true;
    for(i = 0; i < secparam.Dim; i++)
    {
      if(OldIndex == secparam.Indexes[i])
      {
        bRemove = false;
        break;
      }
    }
    for(i = 0; i < primparam.Dim; i++)
    {
      if(OldIndex == primparam.Indexes[i] && i != index)
      {
        bRemove = false;
        break;
      }
    }
  }
  NewIndex = m_Dim;
  for(i = 0; i < m_Dim; i++)//find index for new coordinate
  {
    if(ID == GetID(i))
    {
      NewIndex = i;
      break;
    }
  }
  bAdd = (NewIndex == m_Dim);
  if(bAdd && bRemove)//add and remove variable, so change only ID
  {
    Clean();
    SetID(OldIndex, ID);
    primparam.Derives[index] = deriv;
    return true;
  }
  if(!bAdd && !bRemove)//neither add nor remove, so change only index
  {
    primparam.Indexes[index] = NewIndex;
    primparam.Derives[index] = deriv;
    return true;
  }
  if(bAdd && !bRemove)//add but not remove
  {
    Clean();
    SetDim(m_Dim + 1);
    SetID(NewIndex, ID);
    primparam.Indexes[index] = NewIndex;
    primparam.Derives[index] = deriv;
    return true;
  }
  //else//remove but not add
  {
    Clean();
    primparam.Indexes[index] = NewIndex;
    primparam.Derives[index] = deriv;
    for(i = OldIndex; i < m_Dim - 1; i++)//shift IDs
    {
      SetID(i, GetID(i + 1));
    }

    for(i = 0; i < secparam.Dim; i++)//correct X indexes
    {
      if(OldIndex < secparam.Indexes[i])
      {
        secparam.Indexes[i]--;
      }
    }

    for(i = 0; i < primparam.Dim; i++)//correct Y indexes
    {
      if(OldIndex < primparam.Indexes[i])
      {
        primparam.Indexes[i]--;
      }
    }
    SetDim(m_Dim - 1);
  }
  return true;
}

/**
Memory Graph instantiation with double*/
class mafMemoryGraph: public mafMemoryGraphBase<double>
{
public:
  mafMemoryGraph(double garbage,unsigned int size = 1000): m_garbage(garbage),  mafMemoryGraphBase(size){}
  const double&   GetGarbageValue(){return m_garbage;}
private:
  double m_garbage;
};
#endif
