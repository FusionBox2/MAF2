#ifndef _ARRAYS_H_
#define _ARRAYS_H_

#include <vector>
#include <assert.h>
#include "nr.h"

template <class T>
class ArrayND
{
  virtual size_t   GetNumElems() const = 0;
  virtual ArrayND* CreateDimCopy() const = 0;
  virtual T& operator()(int i) = 0;
  virtual const T& operator()(int i) const = 0;
};

template <class T>
class Array1D : public ArrayND<T>, private std::vector<T>
{
public:
  Array1D():std::vector<T>(){}
  Array1D(size_t _XSize):std::vector<T>(_XSize){}
  Array1D(const Array1D& _array):std::vector<T>(_array){}

  ArrayND<T>* CreateDimCopy() const {return new Array1D<T>(GetDimension());}

  T& operator()(int i)
  {
    return std::vector<T>::at(i);
  }
  const T& operator()(int i)const
  {
    return std::vector<T>::at(i);
  }
  void resize(size_t _XSize)
  {
    std::vector<T>::resize(_XSize);
  }
  void   GetDimension(size_t& dimX)const
  {
    dimX = std::vector<T>::size();
  }
  size_t GetDimension()const
  {
    return std::vector<T>::size();
  }
  void   SetDimension(size_t sz)
  {
    resize(sz);
  }
  size_t GetNumElems()const
  {
    return std::vector<T>::size();
  }
  Array1D& operator=(const Array1D& _array)
  {
    if(&_array == this) 
      return *this; 
    std::vector<T>::operator=(_array);
    return *this;
  }
};

template <class T>
class Array2D : public ArrayND<T>, private std::vector<Array1D<T> >
{
public:
  struct Dimension
  {
    Dimension(size_t _x, size_t _y):x(_x), y(_y){}
    size_t x, y;
  };

  operator NRVec<double>()
  {
    NRVec<double> vec(GetNumElems() * T::DIMENSION);
    for(unsigned i = 0; i < GetNumElems(); i++)
    {
      for(unsigned j = 0; j < T::DIMENSION; j++)
        vec[T::DIMENSION * i + j] = operator()(i)[j];
    }
    return vec;
  }

  Array2D(const NRVec<double>& vec, const Dimension& dim):std::vector<Array1D<T> >(dim.x)
  {
    for(size_t i = 0; i < dim.x; i++)
		std::vector<Array1D<T> >::at(i).resize(dim.y);
    assert(vec.size() == GetNumElems() * T::DIMENSION);
    for(unsigned i = 0; i < GetNumElems(); i++)
    {
      for(unsigned j = 0; j < T::DIMENSION; j++)
        operator()(i)[j] = vec[T::DIMENSION * i + j];
    }
  }
  Array2D(const Dimension& dim):std::vector<Array1D<T> >(dim.x)
  {
    for(size_t i = 0; i < dim.x; i++)
		std::vector<Array1D<T> >::at(i).resize(dim.y);
  }

  Array2D():std::vector<Array1D<T> >(){}

  Array2D(size_t _XSize, size_t _YSize):std::vector<Array1D<T> >(_XSize)
  {
    for(size_t i = 0; i < _XSize; i++)
		std::vector<Array1D<T> >::at(i).resize(_YSize);
  }
  Array2D(const Array2D& _array):std::vector<Array1D<T> >(_array){}

  ArrayND<T>* CreateDimCopy() const {return new Array2D<T>(GetDimension());}

  T& operator()(int i)
  {
    assert(GetNumElems() > 0);
    return std::vector<Array1D<T> >::at(i / std::vector<Array1D<T> >::at(0).GetNumElems())(i % std::vector<Array1D<T> >::at(0).GetNumElems());
  }
  const T& operator()(int i)const
  {
    assert(GetNumElems() > 0);
    return std::vector<Array1D<T> >::at(i / std::vector<Array1D<T> >::at(0).GetNumElems())(i % std::vector<Array1D<T> >::at(0).GetNumElems());
  }
  T& operator()(int i, int j)
  {
    assert(GetNumElems() > 0);
    return std::vector<Array1D<T> >::at(i)(j);
  }
  const T& operator()(int i, int j)const
  {
    assert(GetNumElems() > 0);
    return std::vector<Array1D<T> >::at(i)(j);
  }
  void resize(size_t _XSize, size_t _YSize)
  {
    std::vector<Array1D<T> >::resize(_XSize);
    for(size_t i = 0; i < _XSize; i++)
		std::vector<Array1D<T> >::at(i).resize(_YSize);
  }
  void GetDimension(size_t& dimX, size_t& dimY)const
  {
    dimX = std::vector<Array1D<T> >::size();
    if(dimX == 0)
    {
      dimY = 0;
      return;
    }
    dimY = std::vector<Array1D<T> >::at(0).GetDimension();
  }
  Dimension GetDimension()const
  {
    if(std::vector<Array1D<T> >::size() == 0)
      return Dimension(0, 0);
    return Dimension(std::vector<Array1D<T> >::size(), std::vector<Array1D<T> >::at(0).GetDimension());
  }
  void SetDimension(size_t _XSize, size_t _YSize)
  {
    resize(_XSize, _YSize);
  }
  void SetDimension(const Dimension& sz)
  {
    resize(sz.x, sz.y);
  }
  size_t GetNumElems()const
  {
    if(std::vector<Array1D<T> >::size() == 0)
      return 0;
    return std::vector<Array1D<T> >::size() * std::vector<Array1D<T> >::at(0).GetNumElems();
  }
  Array2D& operator=(const Array2D& _array)
  {
    if(&_array == this) 
      return *this; 
    std::vector<Array1D<T> >::operator=(_array);
    return *this;
  }
};

template <class T>
class Array3D : public ArrayND<T>, private std::vector<Array2D<T> >
{
public:
  struct Dimension
  {
    Dimension(size_t _x, size_t _y, size_t _z):x(_x), y(_y), z(_z){}
    typedef typename Array2D<T>::Dimension Dim;
    Dimension(size_t _x, Dim& _dim):x(_x), y(_dim.x), z(_dim.y){}
    size_t x, y, z;
  };

  Array3D():std::vector<Array2D<T> >(){}
  Array3D(size_t _XSize, size_t _YSize, size_t _ZSize):std::vector<Array2D<T> >(_XSize)
  {
    for(size_t i = 0; i < _XSize; i++)
		std::vector<Array2D<T> >::at(i).resize(_YSize, _ZSize);
  }
  Array3D(const Dimension& dim):std::vector<Array2D<T> >(dim.x)
  {
    for(size_t i = 0; i < dim.x; i++)
		std::vector<Array2D<T> >::at(i).resize(dim.y, dim.z);
  }

  Array3D(const Array3D& _array):std::vector<Array2D<T> >(_array){}
  ArrayND<T>* CreateDimCopy() const {return nullptr;}
  T& operator()(int i)
  {
    assert(GetNumElems() > 0);
    return std::vector<Array2D<T> >::at(i / std::vector<Array2D<T> >::at(0).GetNumElems())(i % std::vector<Array2D<T> >::at(0).GetNumElems());
  }
  const T& operator()(int i)const
  {
    assert(GetNumElems() > 0);
    return std::vector<Array2D<T> >::at(i / std::vector<Array2D<T> >::at(0).GetNumElems())(i % std::vector<Array2D<T> >::at(0).GetNumElems());
  }
  T& operator()(int i, int j, int k)
  {
    assert(GetNumElems() > 0);
    return std::vector<Array2D<T> >::at(i)(j, k);
  }
  const T& operator()(int i, int j, int k)const
  {
    assert(GetNumElems() > 0);
    return std::vector<Array2D<T> >::at(i)(j, k);
  }
  void resize(size_t _XSize, size_t _YSize, size_t _ZSize)
  {
    std::vector<Array2D<T> >::resize(_XSize);
    for(size_t i = 0; i < _XSize; i++)
		std::vector<Array2D<T> >::at(i).resize(_YSize, _ZSize);
  }
  void GetDimension(size_t& dimX, size_t& dimY, size_t& dimZ)const
  {
    dimX = std::vector<Array2D<T> >::size();
    if(dimX == 0)
    {
      dimY = 0;
      dimZ = 0;
      return;
    }
	  std::vector<Array2D<T> >::at(0).GetDimension(dimY, dimZ);
  }
  Dimension GetDimension()const
  {
    //if(std::vector<Array2D<T> >::size() == 0)
      return Dimension(0, 0, 0);
    //return Dimension(std::vector<Array2D<T> >::size(), std::vector<Array2D<T> >::at(0).GetDimension());
  }
  void SetDimension(size_t _XSize, size_t _YSize, size_t _ZSize)
  {
    resize(_XSize, _YSize, _ZSize);
  }
  void SetDimension(const Dimension& sz)
  {
    resize(sz.x, sz.y, sz.y);
  }
  size_t GetNumElems()const
  {
    if(std::vector<Array2D<T> >::size() == 0)
      return 0;
    return std::vector<Array2D<T> >::size() * std::vector<Array2D<T> >::at(0).GetNumElems();
  }
  Array3D& operator=(const Array3D& _array)
  {
    if(&_array == this) 
      return *this; 
    std::vector<Array2D<T> >::operator=(_array);
    return *this;
  }
};

#endif
