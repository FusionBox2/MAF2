/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: V3d.h,v $
  Language:  C++
  Date:      $Date: 2007-10-12 10:27:24 $
  Version:   $Revision: 1.1 $
  Authors:   Fedor Moiseev
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#ifndef V3D_HEADER
#define V3D_HEADER

template <class Type>
class V3d
{
public:
  //V3d(0){x = Type(0); y = Type(0); z = Type(0);}
  explicit V3d(Type _x = Type(0), Type _y = Type(0), Type _z = Type(0)):x(_x), y(_y), z(_z){}
  V3d(Type _val[]):x(_val[0]), y(_val[1]), z(_val[2]){}
  V3d(const V3d& pnt):x(pnt.x), y(pnt.y), z(pnt.z){}
  V3d &operator=(const V3d& pnt){x = pnt.x; y = pnt.y; z = pnt.z; return *this;}
  V3d &operator+=(const V3d& pnt){x += pnt.x; y += pnt.y; z += pnt.z; return *this;}
  V3d &operator-=(const V3d& pnt){x -= pnt.x; y -= pnt.y; z -= pnt.z; return *this;}
  V3d &operator*=(const Type& val){x *= val; y *= val; z *= val; return *this;}
  V3d &operator/=(const Type& val){x /= val; y /= val; z /= val; return *this;}
  V3d operator+()const{V3d tmp(*this); return tmp;}
  V3d operator+(const V3d& pnt)const{V3d tmp(*this); tmp+=pnt; return tmp;}
  V3d operator-()const{V3d tmp(-x, -y, -z);return tmp;}
  V3d operator-(const V3d& pnt)const{V3d tmp(*this); tmp-=pnt; return tmp;}
  V3d operator*(const Type& val)const{V3d tmp(*this); tmp*=val; return tmp;}
  V3d operator/(const Type& val)const{V3d tmp(*this); tmp/=val; return tmp;}
  Type  operator*(const V3d& pnt)const{return x * pnt.x + y * pnt.y + z * pnt.z;}
  V3d operator^(const V3d& pnt)const{V3d tmp; tmp.x += y * pnt.z - z * pnt.y; tmp.y += z * pnt.x - x * pnt.z; tmp.z += x * pnt.y - y * pnt.x; return tmp;}
  Type  length2()const{return (*this) * (*this);}
  Type  operator|(const V3d& pnt){V3d tmp(*this); tmp-= pnt; return tmp.length2();}
  Type  &operator()(int i){return val[i - 1];}
  Type  &operator[](int i){return val[i];}
  const Type &operator()(int i)const{return val[i - 1];}
  const Type &operator[](int i)const{return val[i];}
  union
  {
    struct
    {
      Type x, y, z;
    };
    Type val[3];
  };
};

template <class Type>
V3d<Type> operator*(const Type& val, const V3d<Type>& pnt)
{
  V3d<Type> res(pnt);
  res *= val;
  return res;
}


#endif