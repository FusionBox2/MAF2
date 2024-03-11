#ifndef _POLYNOM_H_
#define _POLYNOM_H_

#include <vector>

template <class T>
class Polynom
{
public:
  Polynom(){m_polynom.resize(1);m_polynom[0] = T(0);}
  Polynom(const std::vector<T>& coefs){if(coefs.size() > 0)m_polynom = coefs;else{m_polynom.resize(1);m_polynom[0] = T(0);}}
  unsigned Deg(){return (m_polynom.size() > 0) ? m_polynom.size() - 1 : 0;}
  bool zero()
  {
    if(m_polynom.size() == 0)
      return true;
    if(m_polynom.size() == 1 && m_polynom[0] == T(0))
      return true;
    return false;
  }
  Polynom &operator+=(const Polynom& poly)
  {
    setMinSize(poly.m_polynom.size());
    for(unsigned i = 0; i < poly.m_polynom.size(); i++)
      m_polynom[i] += poly.m_polynom[i];
    return *this;
  }
  Polynom &operator-=(const Polynom& poly)
  {
    setMinSize(poly.m_polynom.size());
    for(unsigned i = 0; i < poly.m_polynom.size(); i++)
      m_polynom[i] -= poly.m_polynom[i];
    return *this;
  }
  Polynom &operator*=(const Polynom& poly)
  {
    std::vector<T> curstate(m_polynom);
    setMinSize(poly.m_polynom.size() + m_polynom.size() - 1);
    for(unsigned i = 0; i < m_polynom.size(); i++)
      m_polynom[i] = T(0);
    for(unsigned i = 0; i < curstate.size(); i++)
    {
      for(unsigned j = 0; j < poly.m_polynom.size(); j++)
      {
        m_polynom[i + j] += curstate[i] * poly.m_polynom[j];
      }
    }
    return *this;
  }
  Polynom &operator*=(const T& val)
  {
    for(unsigned i = 0; i < m_polynom.size(); i++)
      m_polynom[i] *= val;
    return *this;
  }
  Polynom &operator/=(const T& val)
  {
    for(unsigned i = 0; i < m_polynom.size(); i++)
      m_polynom[i] /= val;
    return *this;
  }
  void differentiate()
  {
    if(m_polynom.size() < 2)
    {
      m_polynom[0] = T(0);
      return;
    }
    for(unsigned i = 0; i < m_polynom.size() - 1; i++)
      m_polynom[i] = m_polynom[i + 1] * (i + 1);
    m_polynom[m_polynom.size() - 1] = T(0);
    m_polynom.resize(m_polynom.size() - 1);
  }
  void integrate()
  {
    m_polynom.resize(m_polynom.size() + 1);
    if(m_polynom.size() == 1)
    {
      m_polynom[0] = T(0);
      return;
    }
    for(unsigned i = m_polynom.size(); i > 1; i--)
    {
      m_polynom[i - 1] = m_polynom[i - 2] / T(i - 1);
    }
    m_polynom[0] = T(0);
  }
  T evaluate(const T& val)
  {
    T result = T(0);
    for(unsigned i = m_polynom.size(); i > 0; i--)
    {
      result *= val;
      result += m_polynom[i - 1];
    }
    return result;
  }
private:
  void setMinSize(unsigned newSize)
  {
    if(newSize > m_polynom.size())
    {
      unsigned diff = newSize - m_polynom.size();
      m_polynom.resize(newSize);
      for(; diff > 0; diff--)
      {
        m_polynom[m_polynom.size() - diff] = T(0);
      }
    }
  }
  std::vector<T> m_polynom;
};


template <class T>
Polynom<T> operator+(const Polynom<T>& poly1, const Polynom<T>& poly2)
{
  Polynom<T> res(poly1);
  res += poly2;
  return res;
}
template <class T>
Polynom<T> operator-(const Polynom<T>& poly1, const Polynom<T>& poly2)
{
  Polynom<T> res(poly1);
  res -= poly2;
  return res;
}
template <class T>
Polynom<T> operator*(const Polynom<T>& poly1, const Polynom<T>& poly2)
{
  Polynom<T> res(poly1);
  res *= poly2;
  return res;
}


#endif