#pragma once

#include "ftkConfigure.h"

#include "ftk/Base/Object.h"
#include "ftk/Base/MTime.h"
#include "ftk/Base/String.h"
#include "ftk/IO/To.h"

typedef double (*mafMatrixElements)[4];

#ifdef MAF_USE_VTK
class vtkMatrix4x4;
#endif

BEGIN_FTK_NAMESPACE

class FTK_BASE_EXPORT mafMatrix final
{
public:
  mafBaseTypeMacro(mafMatrix);

  mafMatrix();

  ~mafMatrix();

  mafMatrix(const mafMatrix &mat);

  mafMatrix(mafMatrix&& mat) noexcept;

  mafMatrix &operator=(const mafMatrix &mat);

  mafMatrix& operator=(mafMatrix&& mat) noexcept;

  bool operator==(const mafMatrix& mat) const;

	bool operator!=(const mafMatrix& mat) const;

  bool Equals(const mafMatrix& mat) const;

	void DeepCopy(const mafMatrix& mat) {*this = mat;}

  std::shared_ptr<mafMatrix> MakeClone() const;

#ifdef MAF_USE_VTK

  /** this constructor references the given matrix instead of copying it */
  mafMatrix(vtkMatrix4x4 *mat);

  /** set internal VTK matrix reference to the given pointer */
  void SetVTKMatrix(vtkMatrix4x4 *mat,mafTimeStamp t=0);

  /** copy matrix from the given VTK matrix */
  void DeepCopy(vtkMatrix4x4 *mat);

  /** return the pointer to the internal VTK matrix */
  vtkMatrix4x4 *GetVTKMatrix() const {return m_VTKMatrix;}

  /** 
    this simply compares the 4x4 matrix with the VTK one,
    but does not compare the time stamp. */
  bool operator==(vtkMatrix4x4 *mat) const;  
#endif

  /** return pointer to elements matrix: returned type is a double [4][4] object */
  mafMatrixElements GetElements() const;

  /** 
    Sets the element i,j in the matrix. Remember to call explicitly
    Modified when using this function. */
  void SetElement(const int i, const int j, double value) {GetElements()[i][j]=value;Modified();}

  /** Returns the element i,j from the matrix. */
  double GetElement(const int i, const int j) const {return GetElements()[i][j];}

  /** return modification time for this object */
  MTimeType GetMTime() const;

  /** update modification time stamp for this object */
  void Modified();
  
  /** Set the TimeStamp for this matrix */
  void SetTimeStamp(mafTimeStamp t) {if (!mafEquals(m_TimeStamp,t)){m_TimeStamp=t; Modified();}};
  mafTimeStamp GetTimeStamp() const {return m_TimeStamp;};

  /** Get the given matrix versor. Static version. */
  static void GetVersor(const int axis, const mafMatrix &matrix, double versor[3]);
  /** Get the given matrix versor*/
  void GetVersor(const int axis, double versor[3]) const {GetVersor(axis,(*this),versor);}

  /** Copy the 3x3 rotation matrix from another 4x4 matrix */
  void CopyRotation(const mafMatrix &source) {CopyRotation(source,*this);}
  /** Copy the 3x3 rotation matrix from a 4x4 matrix to another. Static version.  */
  static void CopyRotation(const mafMatrix &source, mafMatrix &target);

  /** Set all of the elements to zero. Static version. */
  void Zero() { mafMatrix::Zero(*GetElements()); Modified(); }
  /** Set all of the elements to zero. */  
  static void Zero(double elements[16]);

  /** Set equal to Identity matrix */
  void Identity() { mafMatrix::Identity(*GetElements()); Modified();}
  static void Identity(double elements[16]);

  /**
    Matrix Inversion (adapted from Richard Carling in "Graphics Gems," 
    Academic Press, 1990). static version.*/
  static void Invert(const mafMatrix &in, mafMatrix &out) {mafMatrix::Invert(*in.GetElements(),*out.GetElements()); out.Modified();}
  /**
    Matrix Inversion (adapted from Richard Carling in "Graphics Gems," 
    Academic Press, 1990). */
  void Invert() { mafMatrix::Invert(*this,*this); }
  /**
    Matrix Inversion, (adapted from Richard Carling in "Graphics Gems," 
    Academic Press, 1990). static version.*/
  static void Invert(const double inElements[16], double outElements[16]);
  
  /** Matrix determinant */
  static double Determinant(const double Elements[16]);
  double Determinant() const {return Determinant(*GetElements());};

  /** Matrix adjoint */
  static void Adjoint(const double inElements[16], double outElements[16]);
  void Adjoint(const mafMatrix &inMat, mafMatrix &outMat) {Adjoint(*(inMat.GetElements()),*(outMat.GetElements()));outMat.Modified();};
  void Adjoint() {Adjoint(*GetElements(),*GetElements());Modified();}

  /** Transpose the matrix and put it into out. static version.*/
  static void Transpose(const mafMatrix &in, mafMatrix &out) 
    {mafMatrix::Transpose(*in.GetElements(),*out.GetElements()); out.Modified(); }
  /** Transpose the matrix and put it into out. */
  void Transpose() { mafMatrix::Transpose(*this,*this); }
  /** Transpose the matrix and put it into out. static version.*/
  static void Transpose(const double inElements[16], double outElements[16]);

  /**
    Multiply a homogeneous coordinate by this matrix, i.e. out = A*in.
    The in[4] and out[4] can be the same array. */
  void MultiplyPoint(const double in[4], double out[4]) const
    {mafMatrix::MultiplyPoint(*GetElements(),in,out); }
  /**
    Multiply a homogeneous coordinate by this matrix, i.e. out = A*in.
    The in[4] and out[4] can be the same array. */
  static void MultiplyPoint(const double Elements[16], 
                            const double in[4], double out[4]);

  /** Multiplies matrices a and b and stores the result in c.*/
  static void Multiply4x4(const mafMatrix &a, const mafMatrix &b, mafMatrix &c) {
    mafMatrix::Multiply4x4(*a.GetElements(),*b.GetElements(),*c.GetElements());c.Modified();};
  /** Multiplies matrices a and b and stores the result in c. this works with arrays. */
  static void Multiply4x4(const double a[16], const double b[16], 
                          double c[16]);

  /** bracket operator to access & write single elements */
  double *operator[](const unsigned int i) {return &(GetElements()[i][0]);Modified();}

  /** bracket operator to access single elements */
  const double *operator[](unsigned int i) const { return &(GetElements()[i][0]); }  

  //double *operator[][](const unsigned int i,const unsigned int j) {return &(GetElements()[i][j]);}
  
  /**
    Multiply a homogeneous coordinate by this matrix, i.e. out = in*A.
    The in[4] and out[4] can be the same array. */  
  static void PointMultiply(const double Elements[16], 
                            const float in[4], float out[4]);
  /**
    Multiply a homogeneous coordinate by this matrix, i.e. out = in*A.
    The in[4] and out[4] can be the same array. */
  static void PointMultiply(const double Elements[16], 
                            const double in[4], double out[4]);
  void Print(std::ostream& os, const int indent = 0) const;

protected:
  mafTimeStamp m_TimeStamp;

#ifdef MAF_USE_VTK
  vtkMatrix4x4 *m_VTKMatrix; ///< Use a VTK matrix to store the 4x4 elements
#else
  double m_Elements[4][4]; ///< has its own internal representation
  mafMTime m_MTime;
#endif
  
};
template<class Value>
mafMatrix Parse(const Value& value, parser::To<mafMatrix>)
{
  mafMatrix matrix;
  matrix.Zero();
  mafParseVector(value.template As<mafString>(), *matrix.GetElements(), 16);
  matrix.SetTimeStamp(value(_R("TimeStamp")).template As<double>());
  return matrix;
}

template<class Value>
void Serialize(Value& value, const mafMatrix& matrix)
{
  // Write all the 16 elements into as a single 16-tupla
  mafString elements;
  for (int i = 0; i < 4; i++)
  {
    for (int j = 0; j < 4; j++)
    {
      elements += mafToString(matrix.GetElements()[i][j]) + _R(" ");
    }
    elements += _R("\n"); // cr for read-ability
  }

  value.SetValue(elements);

  // add also the timestamp as an attribute
  value(_R("TimeStamp")).SetValue(mafToString(matrix.GetTimeStamp()));
}

END_FTK_NAMESPACE
