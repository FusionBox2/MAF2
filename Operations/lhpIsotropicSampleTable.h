#ifndef __lhpIsotropicSampleTable_H__
#define __lhpIsotropicSampleTable_H__

#include <ostream>

//------------------------------------------------------------------------------
// Table of isotropic sample points in polar coords, and corresponding data.
// 
// Each sample is indexed by sample(i,j,k) where i indexes theta, j indexes phi and k indexes r.
// The polar coords are theta(i), phi(i,j) and r(k)
// The cartesian coords are X(i,j,k), Y(i,j,k) and Z(i,j,k)
//
// Note that phi is a function of i and j because the number of phi samples depends on theta.
// The r values are equally spaced, so the resolution is not homgeneous.
//------------------------------------------------------------------------------

class lhpIsotropicSampleTable
{
public:
  // Constructor
  // Best if ntheta is odd, nphi is a multiple of 4, and nphi = 2(ntheta-1), eg 11 and 20
  lhpIsotropicSampleTable(int numberOfTheta, int maxNumberOfPhi, int numberOfR, double rstepSize) ;

  // Deconstructor
  ~lhpIsotropicSampleTable() ;

  int GetNumberOfTheta() const {return m_numberOfTheta ;}      // Get number of theta samples
  int GetNumberOfPhi(int i) const {return m_numberOfPhi[i] ;}  // Get number of phi samples (depends on theta id)
  int GetNumberOfR() const {return m_numberOfR ;}              // Get number of r samples

  double GetTheta(int i) const {return m_theta[i] ;}
  double GetPhi(int i, int j) const {return m_phi[i][j] ;}
  double GetR(int k) const {return m_r[k] ;}

  double GetX(int i, int j, int k) const {int id = Get3DId(i,j,k);  return m_x[id] ;}    // Get x[i][j][k]
  double GetY(int i, int j, int k) const {int id = Get3DId(i,j,k);  return m_y[id] ;}    // Get y[i][j][k]
  double GetZ(int i, int j, int k) const {int id = Get3DId(i,j,k);  return m_z[id] ;}    // Get z[i][j][k]

  double GetSampleValue(int i, int j, int k) const {int id = Get3DId(i,j,k);  return m_sampleValue[id] ;}
  void SetSampleValue(int i, int j, int k, double val) const {int id = Get3DId(i,j,k);  m_sampleValue[id] = val ;}

  void PrintSelf(std::ostream& os, int indent) ;

private:
  int Get3DId(int i, int j, int k) const {return i*m_numberOfPhiMax*m_numberOfR + m_numberOfR*j + k ;}
  void SetX(int i, int j, int k, double x) {int id = Get3DId(i,j,k);m_x[id] = x ;}    // Set x[i][j][k]
  void SetY(int i, int j, int k, double y) {int id = Get3DId(i,j,k);m_x[id] = y ;}    // Set x[i][j][k]
  void SetZ(int i, int j, int k, double z) {int id = Get3DId(i,j,k);m_x[id] = z ;}    // Set x[i][j][k]
  void SetXYZ(int i, int j, int k, double x, double y, double z) {int id = Get3DId(i,j,k); m_x[id]=x; m_y[id]=y; m_z[id]=z;}    // Set x, y and z

  // table for sample points
  int m_numberOfTheta ;
  int m_numberOfPhiMax ;
  int m_numberOfR ;
  int *m_numberOfPhi ;
  double *m_theta ;
  double **m_phi ;
  double *m_r ;

  // table for cartesian world coords and results (these are 3d arrays in 1d form)
  double *m_x ;
  double *m_y ;
  double *m_z ;
  double *m_sampleValue ;



};


#endif