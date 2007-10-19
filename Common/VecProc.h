/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: VecProc.h,v $
  Language:  C++
  Date:      $Date: 2007-10-19 10:04:33 $
  Version:   $Revision: 1.2 $
  Authors:   Fedor Moiseev
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#ifndef __VECPROC__H__
#define __VECPROC__H__

#include <strstream>
#include <vector>
#include <map>
#include <string>
#include <math.h>
#include "V3d.h"



template <class Type>
class Param
{
public:
  enum VarType
  {
    VECTOR = 0,
    SCALAR
  };
  Param(VarType type, bool dn = false, bool up = false):m_type(type), m_dn(dn), m_up(up){}
  VarType          GetType(){return m_type;}
  Type&            GetScalar(){return m_scalar;}
  const Type&      GetScalar()const{return m_scalar;}
  bool             IsUpLimited()const{return m_up;}
  bool             IsDnLimited()const{return m_dn;}
  bool&            UpLimited(){return m_up;}
  bool&            DnLimited(){return m_dn;}
  Type&            GetUpLimit(){return m_upLimit;}
  const Type&      GetUpLimit()const{return m_upLimit;}
  Type&            GetDnLimit(){return m_dnLimit;}
  const Type&      GetDnLimit()const{return m_dnLimit;}
  V3d<Type>&       GetVector(){return m_vector;}
  const V3d<Type>& GetVector()const{return m_vector;}
private:
  Type          m_scalar;
  Type          m_upLimit;
  Type          m_dnLimit;
  bool          m_up;
  bool          m_dn;
  V3d<Type>     m_vector;
  const VarType m_type;
};

template <class Type>
struct oneParam
{
  bool        vector;
  bool        input;
  bool        constant;
  bool        optional;
  Param<Type> **param;
};
#define FIELDS_BEGIN(Type)                          \
virtual std::vector<oneParam<Type> > getFields()  \
{                                                   \
std::vector<oneParam<Type> > fields;              oneParam<typename Type> op[] = {
#define FIELDS_END()                               \
};for(unsigned i = 0; i < sizeof(op) / sizeof(op[0]); i++) fields.push_back(op[i]);\
return fields;                             \
}
#define DEFINE_FIELD(name,vector,input) {vector, input, false, false, &m_##name},
#define DEFINE_VFIELDI(name)   {true,  true,  false, false, &m_##name},
#define DEFINE_VFIELDO(name)   {true,  false, false, false, &m_##name},
#define DEFINE_SFIELDI(name)   {false, true,  false, false, &m_##name},
#define DEFINE_SFIELDI(name)   {false, true,  false, false, &m_##name},
#define DEFINE_SFIELDO(name)   {false, false, false, false, &m_##name},
#define DEFINE_SFIELDIC(name)  {false, true,  true,  false, &m_##name},
#define DEFINE_SFIELDICO(name) {false, true,  true,  true,  &m_##name},

template <class Type>
class Oper
{
public:
  Oper(){}
  virtual bool postRead() = 0;
  virtual void process() = 0;
  virtual std::vector<oneParam<Type> > getFields() = 0;
private:
};

template <class Type>
class AssignVector : public Oper<Type>
{
public:
  AssignVector(){m_out = NULL;m_in1 = NULL;}
  bool postRead(){return true;}
  void process(){m_out->GetVector() = m_in1->GetVector();}
  FIELDS_BEGIN(typename Type)
  DEFINE_FIELD(out, true, false)
  DEFINE_FIELD(in1, true, true)
  FIELDS_END()
private:
  Param<Type>       *m_out;
  Param<Type>       *m_in1;
};

template <class Type>
class AssignScalar : public Oper<Type>
{
public:
  AssignScalar(){m_out = NULL;m_in1 = NULL;}
  bool postRead(){return true;}
  void process(){m_out->GetScalar() = m_in1->GetScalar();}
  FIELDS_BEGIN(typename Type)
  DEFINE_FIELD(out, false, false)
  DEFINE_FIELD(in1, false, true)
  FIELDS_END()
private:
  Param<Type>       *m_out;
  Param<Type>       *m_in1;
};


template <class Type>
class CrossProduct : public Oper<Type>
{
public:
  CrossProduct(){m_out = NULL;m_in1 = NULL;m_in2 = NULL;}
  bool postRead(){return true;}
  void process(){m_out->GetVector() = m_in1->GetVector() ^ m_in2->GetVector();}
  FIELDS_BEGIN(typename Type)
  DEFINE_FIELD(out, true, false)
  DEFINE_FIELD(in1, true, true)
  DEFINE_FIELD(in2, true, true)
  FIELDS_END()
private:
  Param<Type>       *m_out;
  Param<Type>       *m_in1;
  Param<Type>       *m_in2;
};

template <class Type>
class DotProduct : public Oper<Type>
{
public:
  DotProduct(){m_out = NULL;m_in1 = NULL;m_in2 = NULL;}
  bool postRead(){return true;}
  void process(){m_out->GetScalar() = m_in1->GetVector() * m_in2->GetVector();}
  FIELDS_BEGIN(typename Type)
  DEFINE_FIELD(out, false, false)
  DEFINE_FIELD(in1, true, true)
  DEFINE_FIELD(in2, true, true)
  FIELDS_END()
private:
  Param<Type>       *m_out;
  Param<Type>       *m_in1;
  Param<Type>       *m_in2;
};
template <class Type>
class AddScalar : public Oper<Type>
{
public:
  AddScalar(){m_out = NULL;m_in1 = NULL;m_in2 = NULL;}
  bool postRead(){return true;}
  void process(){m_out->GetScalar() = m_in1->GetScalar() + m_in2->GetScalar();}
  FIELDS_BEGIN(typename Type)
  DEFINE_FIELD(out, false, false)
  DEFINE_FIELD(in1, false, true)
  DEFINE_FIELD(in2, false, true)
  FIELDS_END()
private:
  Param<Type>       *m_out;
  Param<Type>       *m_in1;
  Param<Type>       *m_in2;
};
template <class Type>
class SubScalar : public Oper<Type>
{
public:
  SubScalar(){m_out = NULL;m_in1 = NULL;m_in2 = NULL;}
  bool postRead(){return true;}
  void process(){m_out->GetScalar() = m_in1->GetScalar() - m_in2->GetScalar();}
  FIELDS_BEGIN(typename Type)
  DEFINE_FIELD(out, false, false)
  DEFINE_FIELD(in1, false, true)
  DEFINE_FIELD(in2, false, true)
  FIELDS_END()
private:
  Param<Type>       *m_out;
  Param<Type>       *m_in1;
  Param<Type>       *m_in2;
};
template <class Type>
class MulScalar : public Oper<Type>
{
public:
  MulScalar(){m_out = NULL;m_in1 = NULL;m_in2 = NULL;}
  bool postRead(){return true;}
  void process(){m_out->GetScalar() = m_in1->GetScalar() * m_in2->GetScalar();}
  FIELDS_BEGIN(typename Type)
  DEFINE_FIELD(out, false, false)
  DEFINE_FIELD(in1, false, true)
  DEFINE_FIELD(in2, false, true)
  FIELDS_END()
private:
  Param<Type>       *m_out;
  Param<Type>       *m_in1;
  Param<Type>       *m_in2;
};
template <class Type>
class DivScalar : public Oper<Type>
{
public:
  DivScalar(){m_out = NULL;m_in1 = NULL;m_in2 = NULL;}
  bool postRead(){return true;}
  void process(){m_out->GetScalar() = m_in1->GetScalar() / m_in2->GetScalar();}
  FIELDS_BEGIN(typename Type)
  DEFINE_FIELD(out, false, false)
  DEFINE_FIELD(in1, false, true)
  DEFINE_FIELD(in2, false, true)
  FIELDS_END()
private:
  Param<Type>       *m_out;
  Param<Type>       *m_in1;
  Param<Type>       *m_in2;
};
template <class Type>
class AddVector : public Oper<Type>
{
public:
  AddVector(){m_out = NULL;m_in1 = NULL;m_in2 = NULL;}
  bool postRead(){return true;}
  void process(){m_out->GetVector() = m_in1->GetVector() + m_in2->GetVector();}
  FIELDS_BEGIN(typename Type)
  DEFINE_FIELD(out, true, false)
  DEFINE_FIELD(in1, true, true)
  DEFINE_FIELD(in2, true, true)
  FIELDS_END()
private:
  Param<Type>       *m_out;
  Param<Type>       *m_in1;
  Param<Type>       *m_in2;
};
template <class Type>
class SubVector : public Oper<Type>
{
public:
  SubVector(){m_out = NULL;m_in1 = NULL;m_in2 = NULL;}
  bool postRead(){return true;}
  void process(){m_out->GetVector() = m_in1->GetVector() - m_in2->GetVector();}
  FIELDS_BEGIN(typename Type)
  DEFINE_FIELD(out, true, false)
  DEFINE_FIELD(in1, true, true)
  DEFINE_FIELD(in2, true, true)
  FIELDS_END()
private:
  Param<Type>       *m_out;
  Param<Type>       *m_in1;
  Param<Type>       *m_in2;
};

template <class Type>
class LineComb : public Oper<Type>
{
public:
  LineComb(){m_out = NULL;m_in1v = NULL;m_in1s = NULL;m_in2v = NULL;m_in2s = NULL;}
  bool postRead(){return true;}
  void process()
  {
    V3d<Type> p1 = m_in1s->GetScalar() * m_in1v->GetVector();
    V3d<Type> p2 = m_in2s->GetScalar() * m_in2v->GetVector();
    m_out->GetVector() = p1 + p2;
  }
  FIELDS_BEGIN(typename Type)
  DEFINE_FIELD(out, true, false)
  DEFINE_FIELD(in1s, false, true)
  DEFINE_FIELD(in1v, true, true)
  DEFINE_FIELD(in2s, false, true)
  DEFINE_FIELD(in2v, true, true)
  FIELDS_END()
private:
  Param<Type>       *m_out;
  Param<Type>       *m_in1v;
  Param<Type>       *m_in1s;
  Param<Type>       *m_in2v;
  Param<Type>       *m_in2s;
};

template <class Type>
class DefVecIn : public Oper<Type>
{
public:
  DefVecIn(){m_in = NULL;}
  bool postRead(){return true;}
  void process(){}
  FIELDS_BEGIN(typename Type)
  DEFINE_FIELD(in, true, true)
  FIELDS_END()
private:
  Param<Type>       *m_in;
};

template <class Type>
class DefVecOut : public Oper<Type>
{
public:
  DefVecOut(){m_out = NULL;}
  bool postRead(){return true;}
  void process(){}
  FIELDS_BEGIN(typename Type)
  DEFINE_FIELD(out, true, false)
  FIELDS_END()
private:
  Param<Type>       *m_out;
};

template <class Type>
class DefSclIn : public Oper<Type>
{
public:
  DefSclIn(){m_in = NULL;}
  bool postRead();
  void process(){}
  FIELDS_BEGIN(typename Type)
  DEFINE_SFIELDICO(dn)
  DEFINE_SFIELDI(in)
  DEFINE_SFIELDICO(up)
  FIELDS_END()
private:
  Param<Type>       *m_dn;
  Param<Type>       *m_in;
  Param<Type>       *m_up;
};
template <class Type>
bool DefSclIn<Type>::postRead()
{
  if(m_dn != NULL) 
  {
    m_in->DnLimited() = true;
    m_in->GetDnLimit() = m_dn->GetScalar();
  }
  if(m_up != NULL) 
  {
    m_in->UpLimited() = true;
    m_in->GetUpLimit() = m_up->GetScalar();
  }
  if(m_in->IsUpLimited() && m_in->IsDnLimited())
  {
    m_in->GetScalar() = (m_in->GetUpLimit() + m_in->GetDnLimit()) / 2;
    if(m_in->GetUpLimit() < m_in->GetDnLimit())
      std::swap(m_in->GetUpLimit(), m_in->GetDnLimit());
  }
  else
  {
    m_in->GetScalar() = 0;
    if(m_in->IsUpLimited() && (m_in->GetScalar() > m_in->GetUpLimit()))
      m_in->GetScalar() = m_in->GetUpLimit();
    if(m_in->IsDnLimited() && (m_in->GetScalar() < m_in->GetDnLimit()))
      m_in->GetScalar() = m_in->GetDnLimit();
  }
  return true;
}


template <class Type>
class DefSclOut : public Oper<Type>
{
public:
  DefSclOut(){m_out = NULL;}
  bool postRead(){return true;}
  void process(){}
  FIELDS_BEGIN(typename Type)
  DEFINE_FIELD(out, false, false)
  FIELDS_END()
private:
  Param<Type>       *m_out;
};


template <class Type>
class Normalize : public Oper<Type>
{
public:
  Normalize(){m_out = NULL;}
  bool postRead(){return true;}
  void process(){Type ln = m_out->GetVector().length2(); if(ln != Type(0)) m_out->GetVector() /= sqrt(ln);}
  FIELDS_BEGIN(typename Type)
  DEFINE_FIELD(out, true, false)
  FIELDS_END()
private:
  Param<Type>       *m_out;
};


template <class Type>
class VecManVM
{
public:
  bool        readFromFile(const char *filename);
  const std::vector<std::pair<std::string, Param<Type>*> > &getInputs(){return m_inputs;}
  bool        processString(const char *str);
  Param<Type> *getParam(const char *name);
  bool        execute()
  {  
    for(unsigned i = 0; i < m_operators.size(); i++) 
      m_operators[i]->process(); 
    return true;
  }
  ~VecManVM(){clean();}
private:
  void        clean();
  const char  *skipSpaces(const char *str);
  bool        parseCurrentLexem(const char *&str, char *lex);
  bool        isFloatCorrect(const char *string);
  Param<Type> *parseVector(const char *name, bool input);
  Param<Type> *parseScalar(const char *name, bool input, bool constant);
  std::vector<Oper<Type>*>                            m_operators;
  std::map<std::string, Param<Type>*>                 m_operands;
  std::vector<Param<Type>*>                           m_constants;
  std::vector<std::pair<std::string, Param<Type>*> >  m_inputs;
};


template <class Type>
Param<Type> *VecManVM<Type>::getParam(const char *name)
{  
  std::map<std::string, Param<Type>*>::iterator it = m_operands.find(name);
  if(it == m_operands.end())
    return NULL;
  return it->second;
}


template <class Type>
void VecManVM<Type>::clean()
{
  for(std::vector<Oper<Type>*>::iterator it = m_operators.begin(); it != m_operators.end(); ++it)
    delete *it;
  m_operators.clear();
  for(std::map<std::string, Param<Type>*>::iterator it = m_operands.begin(); it != m_operands.end(); ++it)
    delete it->second;
  m_operands.clear();
  for(std::vector<Param<Type>*>::iterator it = m_constants.begin(); it != m_constants.end(); ++it)
    delete *it;
  m_constants.clear();
}


template <class Type>
const char *VecManVM<Type>::skipSpaces(const char *str)
{
  if(str == NULL)
    return NULL;
  while(str[0] && isspace(str[0]))
    str++;
  return str;
}

template <class Type>
bool VecManVM<Type>::parseCurrentLexem(const char *&str, char *lex)
{
  str = skipSpaces(str);
  if(str == NULL || lex == NULL)
    return false;
  if(str[0] == '#')
  {
    *lex++ = '#';
  }
  else if(str[0] != '\"')
  {
    //skip first word
    while(str[0] && !isspace(str[0]))
      *lex++ = toupper(*str++);
  }
  else
  {
    str++;
    while(str[0] && str[0] != '\"')
      *lex++ = toupper(*str++);
  }
  lex[0] = '\0';
  return true;
}



template <class Type>
bool VecManVM<Type>::isFloatCorrect(const char *string)
{
  int nState,nI;

  nState=0;
  for(nI=0;;nI++)
  {
    switch(string[nI])
    {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      if(nState==0||nState==1)
      {
        nState=2;
      }
      else if(nState==3)
      {
        nState=4;
      }
      break;
    case '+':
    case '-':
      if(nState!=0)
      {
        return false;
      }
      nState=1;
      break;
    case '.':
      if(nState==2)
      {
        nState=3;
      }
      else
      {
        return false;
      }
      break;
    case 0:
      return(nState==2||nState==4);
    default:
      return false;
    }
  }
}

template <class Type>
Param<Type> *VecManVM<Type>::parseVector(const char *name, bool input)
{
  Param<Type> *o = NULL;
  std::map<std::string, Param<Type>*>::iterator it = m_operands.find(name);
  if(it == m_operands.end())
  {
    o = new Param<Type>(Param<Type>::VECTOR);
    m_operands[name] = o;
    if(input)
      m_inputs.push_back(std::make_pair(name, o));
  }
  else if(it->second->GetType() == Param<Type>::VECTOR)
  {
    o = it->second;
  }
  return o;
}



template <class Type>
Param<Type> *VecManVM<Type>::parseScalar(const char *name, bool input, bool constant)
{
  Param<Type> *o = NULL;
  if(isFloatCorrect(name))
  {
    if(input)
    {
      Type num;
      std::istrstream is(name);
      is >> num;
      o = new Param<Type>(Param<Type>::SCALAR);
      o->GetScalar() = num;
      m_constants.push_back(o);
    }
  }
  else if(!constant)
  {
    std::map<std::string, Param<Type>*>::iterator it = m_operands.find(name);
    if(it == m_operands.end())
    {
      o = new Param<Type>(Param<Type>::SCALAR);
      m_operands[name] = o;
      if(input)
        m_inputs.push_back(std::make_pair(name, o));
    }
    else if(it->second->GetType() == Param<Type>::SCALAR)
    {
      o = it->second;
    }
  }
  return o;
}


template <class Type>
bool VecManVM<Type>::processString(const char *pLine)
{
  char      activ[1000];

  if(pLine == NULL)
    return false;

  parseCurrentLexem(pLine, activ);
  if(activ[0] == '\0' || activ[0] == '#')
    return true;
  Oper<Type> *oper = NULL;
  if(strcmp(activ, "DEFVI") == 0)
  {
    oper = new DefVecIn<Type>;
  }
  else if(strcmp(activ, "DEFVO") == 0)
  {
    oper = new DefVecOut<Type>;
  }
  else if(strcmp(activ, "DEFSI") == 0)
  {
    oper = new DefSclIn<Type>;
  }
  else if(strcmp(activ, "DEFSO") == 0)
  {
    oper = new DefSclOut<Type>;
  }
  else if(strcmp(activ, "CROSS") == 0)
  {
    oper = new CrossProduct<Type>();
  }
  else if(strcmp(activ, "LNCMB") == 0)
  {
    oper = new LineComb<Type>();
  }
  else if(strcmp(activ, "SCALR") == 0)
  {
    oper = new DotProduct<Type>();
  }
  else if(strcmp(activ, "NRML") == 0)
  {
    oper = new Normalize<Type>();
  }
  else if(strcmp(activ, "ASSV") == 0)
  {
    oper = new AssignVector<Type>();
  }
  else if(strcmp(activ, "ASSS") == 0)
  {
    oper = new AssignScalar<Type>();
  }
  else if(strcmp(activ, "ADDS") == 0)
  {
    oper = new AddScalar<Type>();
  }
  else if(strcmp(activ, "SUBS") == 0)
  {
    oper = new SubScalar<Type>();
  }
  else if(strcmp(activ, "MULS") == 0)
  {
    oper = new MulScalar<Type>();
  }
  else if(strcmp(activ, "DIVS") == 0)
  {
    oper = new DivScalar<Type>();
  }
  if(oper == NULL)
  {
    clean();
    return false;
  }
  std::vector<oneParam<Type> > signature = oper->getFields();
  for(unsigned i = 0; i < signature.size(); i++)
  {
    parseCurrentLexem(pLine, activ);
    if(signature[i].vector)
      *signature[i].param = parseVector(activ, signature[i].input);
    else
      *signature[i].param = parseScalar(activ, signature[i].input, signature[i].constant);
    if(!signature[i].optional && signature[i].param == NULL)
    {
      clean();
      delete oper;
      return false;
    }
  }
  m_operators.push_back(oper);
  if(!oper->postRead())
  {
    clean();
    return false;
  }
  return true;
}


//----------------------------------------------------------------------------
template <class Type>
bool VecManVM<Type>::readFromFile(const char *filename)
//----------------------------------------------------------------------------
{
  FILE                            *fp;
  fp = fopen(filename, "rt");
  if(fp == NULL)
  {
    return false;
  }

  int const maxStrLen = 1000;
  char      sLine[maxStrLen];
  char      *pRet;

  while(true)
  {
    pRet = fgets(sLine, maxStrLen, fp);
    if(pRet == NULL)
      break;
    if(!processString(pRet))
      break;
  }

  fclose(fp);
  return true;
}

#endif