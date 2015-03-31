#ifndef _BBASIS_H_
#define _BBASIS_H_

#include <vector>
#include "polynom.h"


template <class T>
class BBasis
{
public:
  BBasis(unsigned order, unsigned knots)
  {
    SetOrder(order);
    SetKnots(knots);
  }
  BBasis(unsigned order, const std::vector<T>& kvalues)
  {
    SetOrder(order);
    SetKnots(kvalues);
  }
  BBasis(const BBasis& bas)
  {
    m_order  = bas.m_order;
    m_values = bas.m_values;
  }
  BBasis& operator=(const BBasis& bas)
  {
    if(&bas == this)
      return *this;
    m_order  = bas.m_order;
    m_values = bas.m_values;
    return *this;
  }
  T GetValue(unsigned index, const T& param)
  {
    return GetValue(index, m_order, param);
  }
  void GetValues(const std::vector<T>& params, unsigned index, std::vector<T>& output)
  {
    output.resize(params.size());
    for(unsigned i = 0; i < params.size(); i++)
      output[i] = GetValue(index, params[i]);
  }
  Polynom<T> GetPolynom(unsigned findex, unsigned pindex)
  {
    return GetPolynom(findex, m_order, pindex);
  }
  T GetDerivative(unsigned index, unsigned derivorder, const T& param)
  {
    return GetDerivative(index, m_order, derivorder, param);
  }
  void GetDerivatives(const std::vector<T>& params, unsigned index, unsigned derivorder, std::vector<T>& output)
  {
    output.resize(params.size());
    for(unsigned i = 0; i < params.size(); i++)
      output[i] = GetDerivative(index, derivorder, params[i]);
  }
  T GetMaxPoint(unsigned index)
  /*{
    return (m_values[index] + m_values[index + m_order]) / T(2);
  }*/
  {
    unsigned n = 100;
    T a, b, s1, s2, u1, u2, fu1, fu2;
    a   = m_values[index];
    b   = m_values[index + m_order];
    s1  = (T(3) - sqrt(T(5))) / T(2);
    s2  = (sqrt(T(5)) - T(1)) / T(2);
    u1  = a + s1 * (b - a);
    u2  = a + s2 * (b - a);
    fu1 = GetValue(index, u1);
    fu2 = GetValue(index, u2);
    do
    {
      if (fu1 >= fu2)
      {
        b   = u2;
        u2  = u1;
        fu2 = fu1;
        u1  = a + s1 * (b - a);
        fu1 = GetValue(index, u1);
      }
      else
      {
        a   = u1;
        u1  = u2;
        fu1 = fu2;
        u2  = a + s2 * (b - a);
        fu2 = GetValue(index, u2);
      }
    }
    while(fabs(a - b) > T(0.000001));
    return (a + b) / T(2);
  }
  unsigned GetOrder(){return m_order;}
  void     SetOrder(unsigned order){m_order = order;}
  void     SetKnots(unsigned knots){m_values.resize(knots);for(unsigned i = 0; i < knots; i++)m_values[i] = i;}
  void     SetKnots(const std::vector<T>& kvalues){m_values = kvalues;}
  void     AddKnot(){if(m_values.size() == 0){m_values.push_back(T(0));return;}m_values.push_back(m_values[m_values.size() - 1] + T(1));}
  void     AddKnot(const T& kval){m_values.push_back(kval);}
  unsigned GetNumKnots(){return (unsigned)m_values.size();}
  unsigned GetNumFunctions(){return ((GetNumKnots() < GetOrder()) ? 0 : GetNumKnots() - GetOrder());}
  T        GetMinParam(){return m_values[m_order - 1];}
  T        GetMaxParam(){return m_values[GetNumFunctions()];}
  std::vector<T>& GetKnots(){return m_values;}
  std::vector<T> GetValidKnots(){std::vector<T> output;output.clear();for(unsigned i = m_order - 1; i <= GetNumFunctions(); i++)output.push_back(m_values[i]);return output;}
protected:

  void shft2(T& a, T& b, T c){a = b;b = c;}
  void shft3(T& a, T& b, T& c, T d){a = b;b = c;c = d;}

  T GetValue(unsigned index, unsigned order, const T& param)
  {
    if(order == 0)
      return T(0);
    if(order == 1)
    {
      if(param < m_values[index])
        return T(0);
      if(param == m_values[index + 1])
      {
        if(m_values[index + 1] == m_values[m_values.size() - 1] && m_values[index + 1] != m_values[index])
          return T(1);
      }
      if(param >= m_values[index + 1])
        return T(0);
      return T(1);
    }
    T val1 = GetValue(index, order - 1, param);
    T val2 = GetValue(index + 1, order - 1, param);
    if(m_values[index + order - 1] != m_values[index])
      val1 *= ((param - m_values[index]) / (m_values[index + order - 1] - m_values[index]));
    if(m_values[index + order] != m_values[index + 1])
      val2 *= ((m_values[index + order] - param) / (m_values[index + order] - m_values[index + 1]));
    return val1 + val2;
  }
  T GetDerivative(unsigned index, unsigned order, unsigned derivorder, const T& param)
  {
    if(derivorder == 0)
      return GetValue(index, order, param);
    if(order == 1)
      return T(0);
    T val1 = GetDerivative(index, order - 1, derivorder - 1, param);
    if(m_values[index + order - 1] != m_values[index])
      val1 /= (m_values[index + order - 1] - m_values[index]);
    T val2 = GetDerivative(index + 1, order - 1, derivorder - 1, param);
    if(m_values[index + order] != m_values[index + 1])
      val2 /= (m_values[index + order] - m_values[index + 1]);
    return (order - 1) * (val1 - val2);
  }
  Polynom<T> GetPolynom(unsigned findex, unsigned order, unsigned pindex)
  {
    if(order == 0)
    {
      return Polynom<T>();
    }
    if(order == 1)
    {
      std::vector<T> coefs;
      coefs.resize(1);
      coefs[0] = T(1);
      if(m_values[pindex] < m_values[findex])
      {
        coefs[0] = T(0);
        return Polynom<T>(coefs);
      }
      if(m_values[pindex] == m_values[findex + 1])
      {
        if(m_values[findex + 1] == m_values[m_values.size() - 1] && m_values[findex + 1] != m_values[findex])
        {
          coefs[0] = T(1);
          return Polynom<T>(coefs);
        }
      }
      if(m_values[pindex] >= m_values[findex + 1])
        coefs[0] = T(0);
      return Polynom<T>(coefs);
    }
    Polynom<T> poly1 = GetPolynom(findex,     order - 1, pindex);
    Polynom<T> poly2 = GetPolynom(findex + 1, order - 1, pindex);
    if(!poly1.zero())
    {
      std::vector<T> coefs;
      coefs.resize(2);
      coefs[0] =  -m_values[findex] / (m_values[findex + order - 1] - m_values[findex]);
      coefs[1] = T(1) / (m_values[findex + order - 1] - m_values[findex]);
      poly1 *= Polynom<T>(coefs);
    }
    if(!poly2.zero())
    {
      std::vector<T> coefs;
      coefs.resize(2);
      coefs[0] = m_values[findex + order] / (m_values[findex + order] - m_values[findex + 1]);
      coefs[1] = T(-1) / (m_values[findex + order] - m_values[findex + 1]);
      poly2 *= Polynom<T>(coefs);
    }
    poly1 += poly2;
    return poly1;
  }
private:
  std::vector<T> m_values;
  unsigned       m_order;
};


#endif
