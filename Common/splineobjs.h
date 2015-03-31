#ifndef _SPLINEOBKS_H_
#define _SPLINEOBKS_H_

#include <vector>
#include "bbasis.h"
#include "arrays.h"
#include "vectors.h"
#include "nrtypes_nr.h"
#include "nr.h"


#define CREATEFASTTESSEL()do{\
m_fastTessel.resize(m_tesselation.size());\
for(unsigned i = 0; i < m_fastTessel.size(); i++)\
{\
  m_fastTessel[i].clear();\
  for(unsigned j = 0; j < m_tesselation[i].GetNumElems(); j++)\
  {\
    if(m_tesselation[i](j) != T(0))\
    {\
      m_fastTessel[i].push_back(tesdata(m_tesselation[i](j), j));\
    }\
  }\
}}while(0)

#define FASTTESSELATION() do{\
  result.resize(m_fastTessel.size());\
for(unsigned i = 0; i < m_fastTessel.size(); i++)\
{\
  for(unsigned cm = 0; cm < VecType::DIMENSION; cm++)\
    result[i][cm] = T(0);\
  for(unsigned j = 0; j < m_fastTessel[i].size(); j++)\
  {\
    result[i] += m_fastTessel[i][j].coef * control(m_fastTessel[i][j].i);\
  }\
}return true;} while(0)


#define TESSELATION()     do{result.resize(m_tesselation.size());\
for(unsigned i = 0; i < m_tesselation.size(); i++)\
{\
  if(control.GetNumElems() != m_tesselation[i].GetNumElems())\
    return false;\
  if(control.GetNumElems() > 0)\
    result[i] = m_tesselation[i](0) * control(0);\
  for(unsigned j = 1; j < control.GetNumElems(); j++)\
  {\
    result[i] += m_tesselation[i](j) * control(j);\
  }\
}\
return true;}while(0)

//#define CONTROLPOINTS     cpoints = spatial;

#define CPOINTSMAXINFL()    do{cpoints.SetDimension(spatial.GetDimension());\
for(int i = 0; i < VecType::DIMENSION; i++)\
{\
  Vec_IO_DP dt(spatial.GetNumElems());\
  for(unsigned j = 0; j < spatial.GetNumElems(); j++)\
    dt[j] = spatial(j)[i];\
  NR::lubksb(m_matrix, m_vidx, dt);\
  for(unsigned j = 0; j < spatial.GetNumElems(); j++)\
    cpoints(j)[i] = dt[j];\
}}while(0)

#define CPOINTSINTERPOLATE()    do{cpoints.SetDimension(spatial.GetDimension() - 2);\
  for(int i = 0; i < VecType::DIMENSION; i++)\
{\
  Vec_IO_DP dt(spatial.GetNumElems());\
  dt[0] = 0;\
  for(unsigned j = 0; j < spatial.GetNumElems(); j++)\
  dt[j + 1] = spatial(j)[i];\
  dt[spatial.GetNumElems() + 1] = 0;\
  NR::lubksb(m_matrixInterp, m_vidxInterp, dt);\
  for(unsigned j = 0; j < spatial.GetNumElems(); j++)\
  cpoints(j)[i] = dt[j];\
}}while(0)

#define SPATIALPOINTS()     do{spatial.SetDimension(cpoints.GetDimension());\
GenerateTesselationMaxima(tesselationMaxima);\
for(unsigned i = 0; i < tesselationMaxima.size(); i++)\
{\
  if(cpoints.GetNumElems() != tesselationMaxima[i].GetNumElems())\
    return;\
  if(cpoints.GetNumElems() > 0)\
    spatial(i) = tesselationMaxima[i](0) * cpoints(0);\
  for(unsigned j = 1; j < cpoints.GetNumElems(); j++)\
  {\
    spatial(i) += tesselationMaxima[i](j) * cpoints(j);\
  }\
}}while(0)


#define CONTROLSPATIALMATRIX()     do{GenerateTesselationMaxima(tesselationMaxima);\
Mat_O_DP  matr(tesselationMaxima.size(), tesselationMaxima.size());\
Vec_O_INT vidx(tesselationMaxima.size());\
for(unsigned i = 0; i < tesselationMaxima.size(); i++)\
{\
  assert(tesselationMaxima.size() == tesselationMaxima[i].GetNumElems());\
  for(unsigned j = 0; j < tesselationMaxima[i].GetNumElems(); j++)\
  {\
    matr[i][j] = tesselationMaxima[i](j);\
  }\
}\
DP dp;\
NR::ludcmp(matr, vidx, dp);\
matrix = matr;\
  vindex = vidx;}while(0)

template <class T>
void fillUniform(int numElems, const T& minParam, const T& maxParam, std::vector<T>& output)
{
  output.clear();
  if(numElems < 1) 
    return; 
  output.push_back(minParam);
  if(numElems >= 2)
  {
    T diff = maxParam - minParam;
    for(int i = 1; i < numElems - 1; i++)
    {
      output.push_back(minParam + (i * diff) / (numElems - 1));
    }
    output.push_back(maxParam);
  }
}

template <class T>
std::vector<V2d<T> > operator^(const std::vector<T>& first, const std::vector<T>& second)
{
  std::vector<V2d<T> > result;
  result.resize(first.size() * second.size());
  for(unsigned i = 0; i < first.size(); i++)
  {
    for(unsigned j = 0; j < second.size(); j++)
    {
      result[i * second.size() + j] = V2d<T>(first[i], second[j]);
    }
  }
  return result;
}
template <class T>
std::vector<V3d<T> > operator^(const std::vector<V2d<T> >& first, const std::vector<T>& second)
{
  std::vector<V3d<T> > result;
  result.resize(first.size() * second.size());
  for(unsigned i = 0; i < first.size(); i++)
  {
    for(unsigned j = 0; j < second.size(); j++)
    {
      result[i * second.size() + j] = V3d<T>(first[i].x, first[i].y, second[j]);
    }
  }
  return result;
}


template <class T>
class BCurve
{
public:
  BCurve(int order, int knots):m_basis(order, knots),m_mode(SPT_MODE_DIRECT),m_submode(SPT_MODE_2NDDER)
  {
    GetCSMatrix(m_matrix, m_vidx);
    if(m_basis.GetOrder() == 4)
      GetINTMatrix(m_matrixInterp, m_vidxInterp);
  }
  BCurve(const BBasis<T>& basis):m_basis(basis),m_mode(SPT_MODE_DIRECT),m_submode(SPT_MODE_2NDDER)
  {
    GetCSMatrix(m_matrix, m_vidx);
    if(m_basis.GetOrder() == 4)
      GetINTMatrix(m_matrixInterp, m_vidxInterp);
  }
  void SetTessel(const std::vector<T>& grid)
  {
    GenerateValuesFromGrid(grid, m_tesselation);
    CreateFastTessel();
  }
  void SetUniformTessel(int numElems)
  {
    std::vector<T> grid;
    fillUniform(numElems, m_basis.GetMinParam(), m_basis.GetMaxParam(), grid);
    GenerateValuesFromGrid(grid, m_tesselation);
    CreateFastTessel();
  }
  template <class VecType>
  bool UpdateCurve(const Array1D<VecType>& control, std::vector<VecType>& result)
  {
    //TESSELATION();
    FASTTESSELATION();
  }
  enum SpatMode
  {
    SPT_MODE_DIRECT,
    SPT_MODE_MAXINF,
    SPT_MODE_INTERP
  };
  enum SpatSubMode
  {
    SPT_MODE_1STDER,
    SPT_MODE_2NDDER,
    SPT_MODE_PERIOD
  };
  void SetSpatMode(int mode){m_mode = mode;}
  int  GetSpatMode(){return m_mode;}
  void SetSpatSubMode(int mode){m_submode = mode;if(m_basis.GetOrder() == 4)GetINTMatrix(m_matrixInterp, m_vidxInterp);}
  int  GetSpatSubMode(){return m_submode;}
  template <class VecType>
  void GetCPoints(Array1D<VecType>& spatial, Array1D<VecType>& cpoints)
  {
    if(m_mode == SPT_MODE_DIRECT)
    {
      cpoints = spatial;
      return;
    }
    if(m_mode == SPT_MODE_MAXINF)
    {
      CPOINTSMAXINFL();
      return;
    }

    cpoints.SetDimension(m_basis.GetNumFunctions());
    for(int i = 0; i < VecType::DIMENSION; i++)
    {
      Vec_IO_DP dt(cpoints.GetNumElems());
      dt[0] = 0;
      for(unsigned j = 0; j < cpoints.GetNumElems() - 2; j++)
        dt[j + 1] = spatial(j)[i];
      dt[cpoints.GetNumElems() - 1] = 0;
      NR::lubksb(m_matrixInterp, m_vidxInterp, dt);
      for(unsigned j = 0; j < cpoints.GetNumElems(); j++)
        cpoints(j)[i] = dt[j];
    }
  }
  template <class VecType>
  void GetSPoints(Array1D<VecType>& cpoints, Array1D<VecType>& spatial)
  {
    std::vector<Array1D<T> >  tesselationMaxima;
    SPATIALPOINTS;
  }
protected:
private:
  void CreateFastTessel()
  {
    CREATEFASTTESSEL();
  }
  void GetCSMatrix(Mat_O_DP &matrix, Vec_O_INT& vindex)
  {
    std::vector<Array1D<T> >  tesselationMaxima;
    CONTROLSPATIALMATRIX();
  }
  void GetINTMatrix(Mat_O_DP &matrix, Vec_O_INT& vindex)
  {
    std::vector<Array1D<T> >  tesselationInterp;
    Array1D<T> derivs1;
    Array1D<T> derivs2;

    GenerateValuesFromGrid(m_basis.GetValidKnots(), tesselationInterp);
    Mat_O_DP  matr(m_basis.GetNumFunctions() /*- (order - 4)*/, m_basis.GetNumFunctions());
    Vec_O_INT vidx(m_basis.GetNumFunctions());
    for(unsigned i = 0; i < tesselationInterp.size(); i++)
    {
      assert(m_basis.GetNumFunctions() == tesselationInterp[i].GetNumElems());
      for(unsigned j = 0; j < tesselationInterp[i].GetNumElems(); j++)
      {
        matr[i + 1][j] = tesselationInterp[i](j);
      }
    }
    if(m_submode != SPT_MODE_PERIOD)
    {
      unsigned derorder = (m_submode == SPT_MODE_1STDER) ? 1 : 2;
      FillDerivArray(m_basis.GetMinParam(), derorder, derivs1);
    }
    else
    {
      FillDerivArray(m_basis.GetMinParam(), 1, derivs1);
      FillDerivArray(m_basis.GetMaxParam(), 1, derivs2);
      for(unsigned j = 0; j < derivs1.GetNumElems(); j++)
        derivs1(j) -= derivs2(j);
    }
    for(unsigned j = 0; j < derivs1.GetNumElems(); j++)
    {
      matr[0][j] = derivs1(j);
    }
    if(m_submode != SPT_MODE_PERIOD)
    {
      unsigned derorder = (m_submode == SPT_MODE_1STDER) ? 1 : 2;
      FillDerivArray(m_basis.GetMaxParam(), derorder, derivs1);
    }
    else if(m_submode == SPT_MODE_PERIOD)
    {
      FillDerivArray(m_basis.GetMinParam(), 2, derivs1);
      FillDerivArray(m_basis.GetMaxParam(), 2, derivs2);
      for(unsigned j = 0; j < derivs1.GetNumElems(); j++)
        derivs1(j) -= derivs2(j);
    }
    for(unsigned j = 0; j < derivs1.GetNumElems(); j++)
    {
      matr[m_basis.GetNumFunctions() - 1][j] = derivs1(j);
    }
    DP dp;
    NR::ludcmp(matr, vidx, dp);
    matrix = matr;
    vindex = vidx;
  }
  void FillValuesArray(const T& param, Array1D<T>& output)
  {
    output.resize(m_basis.GetNumFunctions());
    for(unsigned j = 0; j < m_basis.GetNumFunctions(); j++)
      output(j) = m_basis.GetValue(j, param);
  }
  void FillDerivArray(const T& param, unsigned order, Array1D<T>& output)
  {
    output.resize(m_basis.GetNumFunctions());
    for(unsigned j = 0; j < m_basis.GetNumFunctions(); j++)
    {
      output(j) = m_basis.GetDerivative(j, order, param);
    }
  }
  void GenerateValuesFromGrid(const std::vector<T>& grid, std::vector<Array1D<T> >&  tesselation)
  {
    tesselation.resize(grid.size());
    for(unsigned i = 0; i < grid.size(); i++)
    {
      FillValuesArray(grid[i], tesselation[i]);
    }
  }
  void GenerateTesselationMaxima(std::vector<Array1D<T> >&  tesselationMaxima)
  {
    std::vector<T> grid;
    grid.resize(m_basis.GetNumFunctions());
    for(unsigned i = 0; i < m_basis.GetNumFunctions(); i++)
      grid[i] = m_basis.GetMaxPoint(i);
    GenerateValuesFromGrid(grid, tesselationMaxima);
  }
  Mat_DP                    m_matrix;
  Vec_INT                   m_vidx;
  Mat_DP                    m_matrixInterp;
  Vec_INT                   m_vidxInterp;
  std::vector<Array1D<T> >  m_tesselation;
  BBasis<T>                 m_basis;
  int                       m_mode;
  int                       m_submode;
  struct tesdata
  {
    tesdata(const T& _coef, int _i):coef(_coef), i(_i){}
    T coef;
    int i;
  };
  std::vector<std::vector<tesdata> > m_fastTessel;

};

template <class T>
class BSurface
{
public:
  BSurface(int orderU, int knotsU, int orderV, int knotsV):m_basisU(orderU, knotsU), m_basisV(orderV, knotsV),m_mode(SPT_MODE_DIRECT),m_submodeU(SPT_MODE_2NDDER),m_submodeV(SPT_MODE_2NDDER)
  {
    GetCSMatrix(m_matrix, m_vidx);
    if(m_basisU.GetOrder() == 4 && m_basisV.GetOrder() == 4)
      GetINTMatrix(m_matrixInterp, m_vidxInterp);
    /*PrepareSquareData();*/
  }
  BSurface(const BBasis<T>& basisU, const BBasis<T>& basisV):m_basisU(basisU), m_basisV(basisV),m_mode(SPT_MODE_DIRECT),m_submodeU(SPT_MODE_2NDDER),m_submodeV(SPT_MODE_2NDDER)
  {
    GetCSMatrix(m_matrix, m_vidx);
    if(m_basisU.GetOrder() == 4 && m_basisV.GetOrder() == 4)
      GetINTMatrix(m_matrixInterp, m_vidxInterp);
    /*PrepareSquareData();*/
  }
  void SetTessel(const std::vector<V2d<T> >& grid)
  {
    GenerateValuesFromGrid(grid, m_tesselation);
    CreateFastTessel();
  }
  void SetUniformTessel(int numElemsU, int numElemsV)
  {
    std::vector<T> gridU;
    std::vector<T> gridV;
    fillUniform(numElemsU, m_basisU.GetMinParam(), m_basisU.GetMaxParam(), gridU);
    fillUniform(numElemsV, m_basisV.GetMinParam(), m_basisV.GetMaxParam(), gridV);

    GenerateValuesFromGrid(gridU ^ gridV, m_tesselation);
    CreateFastTessel();
  }
  template <class VecType>
  bool UpdateSurface(const Array2D<VecType>& control, std::vector<VecType>& result)
  {
    //TESSELATION();
    FASTTESSELATION();
  }
  T Square(const Array2D<V2d<T> >& control)
  {
    T result = T(0);
    for(unsigned i = 0; i < m_squareData.size(); i++)
      result += control(m_squareData[i].i, m_squareData[i].j).x * control(m_squareData[i].k, m_squareData[i].l).y * m_squareData[i].coef;
    return result;
  }
  DP Square(const Vec_I_DP& control, const Array2D<V2d<double> >::Dimension& dim)
  {
    DP result = 0.0;
    for(unsigned i = 0; i < m_squareData.size(); i++)
      result += control[2 * (m_squareData[i].i * dim.x + m_squareData[i].j) + 0] * 
                control[2 * (m_squareData[i].k * dim.x + m_squareData[i].l) + 1] * 
                m_squareData[i].coef;
    return result;
  }
  Array2D<V2d<T> > SquareGrad(const Array2D<V2d<T> >& control)
  {
    Array2D<V2d<T> > grad;
    grad.SetDimension(control.GetDimension());
    for(unsigned i = 0; i < grad.GetNumElems(); i++)
    {
      for(unsigned j = 0; j < V2d<T>::DIMENSION; j++)
        grad(i)[j] = T(0);
    }
    for(unsigned i = 0; i < m_squareData.size(); i++)
    {
      grad(m_squareData[i].i, m_squareData[i].j).x += control(m_squareData[i].k, m_squareData[i].l).y * m_squareData[i].coef;
      grad(m_squareData[i].k, m_squareData[i].l).y += control(m_squareData[i].i, m_squareData[i].j).x * m_squareData[i].coef;
    }
    return grad;
  }
  void SquareGrad(const Vec_I_DP& control, const Array2D<V2d<double> >::Dimension& dim, Vec_O_DP& output)
  {
    //Array2D<V2d<T> > grad;
    //grad.SetDimension(control.GetDimension());
    for(unsigned i = 0; i < control.size(); i++)
    {
      output[i] = 0.0;
    }
    for(unsigned i = 0; i < m_squareData.size(); i++)
    {
      output[2 * (m_squareData[i].i * dim.x + m_squareData[i].j) + 0] += control[2 * (m_squareData[i].k * dim.x + m_squareData[i].l) + 1] * m_squareData[i].coef;
      output[2 * (m_squareData[i].k * dim.x + m_squareData[i].l) + 1] += control[2 * (m_squareData[i].i * dim.x + m_squareData[i].j) + 0] * m_squareData[i].coef;
    }
  }
  enum SpatMode
  {
    SPT_MODE_DIRECT,
    SPT_MODE_MAXINF,
    SPT_MODE_INTERP
  };
  enum SpatSubMode
  {
    SPT_MODE_1STDER,
    SPT_MODE_2NDDER,
    SPT_MODE_PERIOD
  };
  void SetSpatMode(int mode){m_mode = mode;}
  int  GetSpatMode(){return m_mode;}
  void SetSpatSubModeU(int mode){m_submodeU = mode;if(m_basisU.GetOrder() == 4 && m_basisV.GetOrder() == 4)GetINTMatrix(m_matrixInterp, m_vidxInterp);}
  int  GetSpatSubModeU(){return m_submodeU;}
  void SetSpatSubModeV(int mode){m_submodeV = mode;if(m_basisU.GetOrder() == 4 && m_basisV.GetOrder() == 4)GetINTMatrix(m_matrixInterp, m_vidxInterp);}
  int  GetSpatSubModeV(){return m_submodeV;}

  template <class VecType>
  void GetCPoints(Array2D<VecType>& spatial, Array2D<VecType>& cpoints)
  {
    if(m_mode == SPT_MODE_DIRECT)
    {
      cpoints = spatial;
      return;
    }
    if(m_mode == SPT_MODE_MAXINF)
    {
      CPOINTSMAXINFL();
      return;
    }

    cpoints.SetDimension(m_basisU.GetNumFunctions(), m_basisV.GetNumFunctions());
    for(int i = 0; i < VecType::DIMENSION; i++)
    {
      Vec_IO_DP dt(cpoints.GetNumElems());
      for(unsigned j = 0; j < cpoints.GetNumElems(); j++)
      {
        dt[j] = (j < spatial.GetNumElems()) ? spatial(j)[i] : 0.0;
      }
      NR::lubksb(m_matrixInterp, m_vidxInterp, dt);
      for(unsigned j = 0; j < cpoints.GetNumElems(); j++)
        cpoints(j)[i] = dt[j];
    }
  }
  template <class VecType>
  void GetSPoints(Array2D<VecType>& cpoints, Array2D<VecType>& spatial)
  {
    std::vector<Array2D<T> >  tesselationMaxima;
    SPATIALPOINTS();
  }
  BBasis<T>& GetUBasis(){return m_basisU;}
  BBasis<T>& GetVBasis(){return m_basisV;}
protected:
private:
  void CreateFastTessel()
  {
    CREATEFASTTESSEL();
  }
  void PrepareSquareData()
  {
    BBasis<T>& u = m_basisU;
    BBasis<T>& v = m_basisV;
    std::vector<T>& uknots = u.GetKnots();
    std::vector<T>& vknots = v.GetKnots();

    for(unsigned i = 0; i < u.GetNumFunctions(); i++)
    {
      for(unsigned j = 0; j < v.GetNumFunctions(); j++)
      {
        for(unsigned k = 0; k < u.GetNumFunctions(); k++)
        {
          for(unsigned l = 0; l < v.GetNumFunctions(); l++)
          {
            if(i > u.GetOrder() - 1 || k > u.GetOrder() - 1)
              if(i < u.GetNumFunctions() - u.GetOrder() || k < u.GetNumFunctions() - u.GetOrder())
                if(j > v.GetOrder() - 1 || l > v.GetOrder() - 1)
                  if(j < v.GetNumFunctions() - v.GetOrder() || l < v.GetNumFunctions() - v.GetOrder())
                    continue;
            T muik = T(0);
            T muki = T(0);
            T nujl = T(0);
            T nulj = T(0);
            for(unsigned t = u.GetOrder() - 1; t < u.GetNumFunctions(); t++)
            {
              Polynom<T> pi = u.GetPolynom(i, t);
              Polynom<T> pk = u.GetPolynom(k, t);
              Polynom<T> dpi = pi;
              Polynom<T> dpk = pk;
              dpi.differentiate();
              dpk.differentiate();
              dpi *= pk;
              dpk *= pi;
              dpi.integrate();
              dpk.integrate();
              muik += dpi.evaluate(uknots[t + 1]) - dpi.evaluate(uknots[t]);
              muki += dpk.evaluate(uknots[t + 1]) - dpk.evaluate(uknots[t]);
            }
            for(unsigned t = v.GetOrder() - 1; t < v.GetNumFunctions(); t++)
            {
              Polynom<T> pj = v.GetPolynom(j, t);
              Polynom<T> pl = v.GetPolynom(l, t);
              Polynom<T> dpj = pj;
              Polynom<T> dpl = pl;
              dpj.differentiate();
              dpl.differentiate();
              dpj *= pl;
              dpl *= pj;
              dpj.integrate();
              dpl.integrate();
              nujl += dpj.evaluate(vknots[t + 1]) - dpj.evaluate(vknots[t]);
              nulj += dpl.evaluate(vknots[t + 1]) - dpl.evaluate(vknots[t]);
            }
            T det = muik * nulj - muki * nujl;
            m_squareData.push_back(sqdata(det, i, j, k, l));
          }
        }
      }
    }
  }
  void GetCSMatrix(Mat_O_DP &matrix, Vec_O_INT& vindex)
  {
    std::vector<Array2D<T> >  tesselationMaxima;
    CONTROLSPATIALMATRIX();
  }

  void GetINTMatrix(Mat_O_DP &matrix, Vec_O_INT& vindex)
  {
    std::vector<Array2D<T> >  tesselationInterp;
    Array2D<T> derivs1;
    Array2D<T> derivs2;
    std::vector<T> validKnotsU = m_basisU.GetValidKnots();
    std::vector<T> validKnotsV = m_basisV.GetValidKnots();

    GenerateValuesFromGrid(validKnotsU ^ validKnotsV, tesselationInterp);
    Mat_O_DP  matr(m_basisU.GetNumFunctions() * m_basisV.GetNumFunctions(), m_basisU.GetNumFunctions() * m_basisV.GetNumFunctions());
    Vec_O_INT vidx(m_basisU.GetNumFunctions() * m_basisV.GetNumFunctions());
    for(unsigned i = 0; i < tesselationInterp.size(); i++)
    {
      assert(m_basisU.GetNumFunctions() * m_basisV.GetNumFunctions() == tesselationInterp[i].GetNumElems());
      for(unsigned j = 0; j < tesselationInterp[i].GetNumElems(); j++)
      {
        matr[i][j] = tesselationInterp[i](j);
      }
    }
    int stringIndex = tesselationInterp.size();
    for(unsigned i = 0; i < validKnotsV.size(); i++)
    {
      if(m_submodeU != SPT_MODE_PERIOD)
      {
        unsigned derorderU = (m_submodeU == SPT_MODE_1STDER) ? 1 : 2;
        FillDerivArray(V2d<T>(m_basisU.GetMinParam(), validKnotsV[i]), derorderU, 0, derivs1);
      }
      else
      {
        FillDerivArray(V2d<T>(m_basisU.GetMinParam(), validKnotsV[i]), 1, 0, derivs1);
        FillDerivArray(V2d<T>(m_basisU.GetMaxParam(), validKnotsV[i]), 1, 0, derivs2);
        for(unsigned j = 0; j < derivs1.GetNumElems(); j++)
          derivs1(j) -= derivs2(j);
      }
      for(unsigned j = 0; j < derivs1.GetNumElems(); j++)
      {
        matr[stringIndex][j] = derivs1(j);
      }
      stringIndex++;
      if(m_submodeU != SPT_MODE_PERIOD)
      {
        unsigned derorderU = (m_submodeU == SPT_MODE_1STDER) ? 1 : 2;
        FillDerivArray(V2d<T>(m_basisU.GetMaxParam(), validKnotsV[i]), derorderU, 0, derivs1);
      }
      else if(m_submodeU == SPT_MODE_PERIOD)
      {
        FillDerivArray(V2d<T>(m_basisU.GetMinParam(), validKnotsV[i]), 2, 0, derivs1);
        FillDerivArray(V2d<T>(m_basisU.GetMaxParam(), validKnotsV[i]), 2, 0, derivs2);
        for(unsigned j = 0; j < derivs1.GetNumElems(); j++)
          derivs1(j) -= derivs2(j);
      }
      for(unsigned j = 0; j < derivs1.GetNumElems(); j++)
      {
        matr[stringIndex][j] = derivs1(j);
      }
      stringIndex++;
    }
    for(unsigned i = 0; i < validKnotsU.size(); i++)
    {
      if(m_submodeV != SPT_MODE_PERIOD)
      {
        unsigned derorderV = (m_submodeV == SPT_MODE_1STDER) ? 1 : 2;
        FillDerivArray(V2d<T>(validKnotsU[i], m_basisV.GetMinParam()), 0, derorderV, derivs1);
      }
      else
      {
        FillDerivArray(V2d<T>(validKnotsU[i], m_basisV.GetMinParam()), 0, 1, derivs1);
        FillDerivArray(V2d<T>(validKnotsU[i], m_basisV.GetMaxParam()), 0, 1, derivs2);
        for(unsigned j = 0; j < derivs1.GetNumElems(); j++)
          derivs1(j) -= derivs2(j);
      }
      for(unsigned j = 0; j < derivs1.GetNumElems(); j++)
      {
        matr[stringIndex][j] = derivs1(j);
      }
      stringIndex++;
      if(m_submodeV != SPT_MODE_PERIOD)
      {
        unsigned derorderV = (m_submodeV == SPT_MODE_1STDER) ? 1 : 2;
        FillDerivArray(V2d<T>(validKnotsU[i], m_basisV.GetMaxParam()), 0, derorderV, derivs1);
      }
      else if(m_submodeU == SPT_MODE_PERIOD)
      {
        FillDerivArray(V2d<T>(validKnotsU[i], m_basisV.GetMinParam()), 0, 2, derivs1);
        FillDerivArray(V2d<T>(validKnotsU[i], m_basisV.GetMaxParam()), 0, 2, derivs2);
        for(unsigned j = 0; j < derivs1.GetNumElems(); j++)
          derivs1(j) -= derivs2(j);
      }
      for(unsigned j = 0; j < derivs1.GetNumElems(); j++)
      {
        matr[stringIndex][j] = derivs1(j);
      }
      stringIndex++;
    }
    if(m_submodeU != SPT_MODE_PERIOD && m_submodeV != SPT_MODE_PERIOD)
    {
      unsigned derorderU = (m_submodeU == SPT_MODE_1STDER) ? 1 : 2;
      unsigned derorderV = (m_submodeV == SPT_MODE_1STDER) ? 1 : 2;
      FillDerivArray(V2d<T>(m_basisU.GetMinParam(), m_basisV.GetMinParam()), derorderU, derorderV, derivs1);
      for(unsigned j = 0; j < derivs1.GetNumElems(); j++)
      {
        matr[stringIndex][j] = derivs1(j);
      }
      stringIndex++;
      FillDerivArray(V2d<T>(m_basisU.GetMinParam(), m_basisV.GetMaxParam()), derorderU, derorderV, derivs1);
      for(unsigned j = 0; j < derivs1.GetNumElems(); j++)
      {
        matr[stringIndex][j] = derivs1(j);
      }
      stringIndex++;
      FillDerivArray(V2d<T>(m_basisU.GetMaxParam(), m_basisV.GetMinParam()), derorderU, derorderV, derivs1);
      for(unsigned j = 0; j < derivs1.GetNumElems(); j++)
      {
        matr[stringIndex][j] = derivs1(j);
      }
      stringIndex++;
      FillDerivArray(V2d<T>(m_basisU.GetMaxParam(), m_basisV.GetMaxParam()), derorderU, derorderV, derivs1);
      for(unsigned j = 0; j < derivs1.GetNumElems(); j++)
      {
        matr[stringIndex][j] = derivs1(j);
      }
      stringIndex++;
    }
    else if(m_submodeU != SPT_MODE_PERIOD)
    {
      unsigned derorderU = (m_submodeU == SPT_MODE_1STDER) ? 1 : 2;

      FillDerivArray(V2d<T>(m_basisU.GetMinParam(), m_basisV.GetMinParam()), derorderU, 1, derivs1);
      FillDerivArray(V2d<T>(m_basisU.GetMinParam(), m_basisV.GetMaxParam()), derorderU, 1, derivs2);
      for(unsigned j = 0; j < derivs1.GetNumElems(); j++)
        derivs1(j) -= derivs2(j);
      for(unsigned j = 0; j < derivs1.GetNumElems(); j++)
      {
        matr[stringIndex][j] = derivs1(j);
      }
      stringIndex++;

      FillDerivArray(V2d<T>(m_basisU.GetMinParam(), m_basisV.GetMinParam()), derorderU, 2, derivs1);
      FillDerivArray(V2d<T>(m_basisU.GetMinParam(), m_basisV.GetMaxParam()), derorderU, 2, derivs2);
      for(unsigned j = 0; j < derivs1.GetNumElems(); j++)
        derivs1(j) -= derivs2(j);
      for(unsigned j = 0; j < derivs1.GetNumElems(); j++)
      {
        matr[stringIndex][j] = derivs1(j);
      }
      stringIndex++;

      FillDerivArray(V2d<T>(m_basisU.GetMaxParam(), m_basisV.GetMinParam()), derorderU, 1, derivs1);
      FillDerivArray(V2d<T>(m_basisU.GetMaxParam(), m_basisV.GetMaxParam()), derorderU, 1, derivs2);
      for(unsigned j = 0; j < derivs1.GetNumElems(); j++)
        derivs1(j) -= derivs2(j);
      for(unsigned j = 0; j < derivs1.GetNumElems(); j++)
      {
        matr[stringIndex][j] = derivs1(j);
      }
      stringIndex++;

      FillDerivArray(V2d<T>(m_basisU.GetMaxParam(), m_basisV.GetMinParam()), derorderU, 2, derivs1);
      FillDerivArray(V2d<T>(m_basisU.GetMaxParam(), m_basisV.GetMaxParam()), derorderU, 2, derivs2);
      for(unsigned j = 0; j < derivs1.GetNumElems(); j++)
        derivs1(j) -= derivs2(j);
      for(unsigned j = 0; j < derivs1.GetNumElems(); j++)
      {
        matr[stringIndex][j] = derivs1(j);
      }
      stringIndex++;
    }
    else if(m_submodeV != SPT_MODE_PERIOD)
    {
      unsigned derorderV = (m_submodeV == SPT_MODE_1STDER) ? 1 : 2;

      FillDerivArray(V2d<T>(m_basisU.GetMinParam(), m_basisV.GetMinParam()), 1, derorderV, derivs1);
      FillDerivArray(V2d<T>(m_basisU.GetMaxParam(), m_basisV.GetMinParam()), 1, derorderV, derivs2);
      for(unsigned j = 0; j < derivs1.GetNumElems(); j++)
        derivs1(j) -= derivs2(j);
      for(unsigned j = 0; j < derivs1.GetNumElems(); j++)
      {
        matr[stringIndex][j] = derivs1(j);
      }
      stringIndex++;

      FillDerivArray(V2d<T>(m_basisU.GetMinParam(), m_basisV.GetMinParam()), 2, derorderV, derivs1);
      FillDerivArray(V2d<T>(m_basisU.GetMaxParam(), m_basisV.GetMinParam()), 2, derorderV, derivs2);
      for(unsigned j = 0; j < derivs1.GetNumElems(); j++)
        derivs1(j) -= derivs2(j);
      for(unsigned j = 0; j < derivs1.GetNumElems(); j++)
      {
        matr[stringIndex][j] = derivs1(j);
      }
      stringIndex++;

      FillDerivArray(V2d<T>(m_basisU.GetMinParam(), m_basisV.GetMaxParam()), 1, derorderV, derivs1);
      FillDerivArray(V2d<T>(m_basisU.GetMaxParam(), m_basisV.GetMaxParam()), 1, derorderV, derivs2);
      for(unsigned j = 0; j < derivs1.GetNumElems(); j++)
        derivs1(j) -= derivs2(j);
      for(unsigned j = 0; j < derivs1.GetNumElems(); j++)
      {
        matr[stringIndex][j] = derivs1(j);
      }
      stringIndex++;

      FillDerivArray(V2d<T>(m_basisU.GetMinParam(), m_basisV.GetMaxParam()), 2, derorderV, derivs1);
      FillDerivArray(V2d<T>(m_basisU.GetMaxParam(), m_basisV.GetMaxParam()), 2, derorderV, derivs2);
      for(unsigned j = 0; j < derivs1.GetNumElems(); j++)
        derivs1(j) -= derivs2(j);
      for(unsigned j = 0; j < derivs1.GetNumElems(); j++)
      {
        matr[stringIndex][j] = derivs1(j);
      }
      stringIndex++;
    }
    else
    {
      FillDerivArray(V2d<T>(m_basisU.GetMinParam(), m_basisV.GetMinParam()), 1, 1, derivs1);
      FillDerivArray(V2d<T>(m_basisU.GetMaxParam(), m_basisV.GetMinParam()), 1, 1, derivs2);
      for(unsigned j = 0; j < derivs1.GetNumElems(); j++)
        derivs1(j) -= derivs2(j);
      for(unsigned j = 0; j < derivs1.GetNumElems(); j++)
      {
        matr[stringIndex][j] = derivs1(j);
      }
      stringIndex++;

      FillDerivArray(V2d<T>(m_basisU.GetMinParam(), m_basisV.GetMinParam()), 2, 2, derivs1);
      FillDerivArray(V2d<T>(m_basisU.GetMaxParam(), m_basisV.GetMinParam()), 2, 2, derivs2);
      for(unsigned j = 0; j < derivs1.GetNumElems(); j++)
        derivs1(j) -= derivs2(j);
      for(unsigned j = 0; j < derivs1.GetNumElems(); j++)
      {
        matr[stringIndex][j] = derivs1(j);
      }
      stringIndex++;

      FillDerivArray(V2d<T>(m_basisU.GetMinParam(), m_basisV.GetMaxParam()), 1, 1, derivs1);
      FillDerivArray(V2d<T>(m_basisU.GetMaxParam(), m_basisV.GetMaxParam()), 1, 1, derivs2);
      for(unsigned j = 0; j < derivs1.GetNumElems(); j++)
        derivs1(j) -= derivs2(j);
      for(unsigned j = 0; j < derivs1.GetNumElems(); j++)
      {
        matr[stringIndex][j] = derivs1(j);
      }
      stringIndex++;

      FillDerivArray(V2d<T>(m_basisU.GetMinParam(), m_basisV.GetMaxParam()), 2, 2, derivs1);
      FillDerivArray(V2d<T>(m_basisU.GetMaxParam(), m_basisV.GetMaxParam()), 2, 2, derivs2);
      for(unsigned j = 0; j < derivs1.GetNumElems(); j++)
        derivs1(j) -= derivs2(j);
      for(unsigned j = 0; j < derivs1.GetNumElems(); j++)
      {
        matr[stringIndex][j] = derivs1(j);
      }
      stringIndex++;
    }
    DP dp;
    NR::ludcmp(matr, vidx, dp);
    matrix = matr;
    vindex = vidx;
  }
  void FillValuesArray(const V2d<T>& param, Array2D<T>& output)
  {
    output.resize(m_basisU.GetNumFunctions(), m_basisV.GetNumFunctions());
    for(unsigned j = 0; j < m_basisU.GetNumFunctions(); j++)
    {
      for(unsigned k = 0; k < m_basisV.GetNumFunctions(); k++)
      {
        output(j, k)  = m_basisU.GetValue(j, param.x) * m_basisV.GetValue(k, param.y);
      }
    }
  }
  void FillDerivArray(const V2d<T>& param, unsigned orderU, unsigned orderV, Array2D<T>& output)
  {
    output.resize(m_basisU.GetNumFunctions(), m_basisV.GetNumFunctions());
    for(unsigned j = 0; j < m_basisU.GetNumFunctions(); j++)
    {
      for(unsigned k = 0; k < m_basisV.GetNumFunctions(); k++)
      {
        output(j, k)  = m_basisU.GetDerivative(j, orderU, param.x) * m_basisV.GetDerivative(k, orderV, param.y);
      }
    }
  }

  void GenerateValuesFromGrid(const std::vector<V2d<T> >& grid, std::vector<Array2D<T> >&  tesselation)
  {
    tesselation.resize(grid.size());
    for(unsigned i = 0; i < grid.size(); i++)
    {
      FillValuesArray(grid[i], tesselation[i]);
    }
  }  
  
  void GenerateTesselationMaxima(std::vector<Array2D<T> >&  tesselationMaxima)
  {
    std::vector<T> gridU;
    std::vector<T> gridV;
    gridU.resize(m_basisU.GetNumFunctions());
    gridV.resize(m_basisV.GetNumFunctions());
    for(unsigned i = 0; i < m_basisU.GetNumFunctions(); i++)
      gridU[i] = m_basisU.GetMaxPoint(i);
    for(unsigned i = 0; i < m_basisV.GetNumFunctions(); i++)
      gridV[i] = m_basisV.GetMaxPoint(i);
    GenerateValuesFromGrid(gridU ^ gridV, tesselationMaxima);
  }

  Mat_DP                    m_matrix;
  Vec_INT                   m_vidx;
  Mat_DP                    m_matrixInterp;
  Vec_INT                   m_vidxInterp;
  std::vector<Array2D<T> >  m_tesselation;
  BBasis<T>                 m_basisU;
  BBasis<T>                 m_basisV;
  int                       m_mode;
  int                       m_submodeU;
  int                       m_submodeV;
  struct sqdata
  {
    sqdata(const T& _coef, int _i, int _j, int _k, int _l):coef(_coef), i(_i), j(_j), k(_k), l(_l){}
    T coef;
    int i, j, k, l;
  };
  struct tesdata
  {
    tesdata(const T& _coef, int _i):coef(_coef), i(_i){}
    T coef;
    int i;
  };
  std::vector<std::vector<tesdata> >  m_fastTessel;
  std::vector<sqdata>                 m_squareData;
};




template <class T>
class BVolume
{
public:
  BVolume(int orderU, int knotsU, int orderV, int knotsV, int orderW, int knotsW):m_basisU(orderU, knotsU), m_basisV(orderV, knotsV), m_basisW(orderW, knotsW),m_mode(SPT_MODE_DIRECT)
  {
    GetCSMatrix(m_matrix, m_vidx);
    /*PrepareVolumeData();*/
  }
  BVolume(const BBasis<T>& basisU, const BBasis<T>& basisV, const BBasis<T>& basisW):m_basisU(basisU), m_basisV(basisV), m_basisW(basisW),m_mode(SPT_MODE_DIRECT)
  {
    GetCSMatrix(m_matrix, m_vidx);
    /*PrepareVolumeData();*/
  }
  void SetTessel(const std::vector<V3d<T> >& grid)
  {
    GenerateValuesFromGrid(grid, m_tesselation);
    CreateFastTessel();
  }

  void SetUniformTessel(int numElemsU, int numElemsV, int numElemsW)
  {
    std::vector<T> gridU;
    std::vector<T> gridV;
    std::vector<T> gridW;
    fillUniform(numElemsU, m_basisU.GetMinParam(), m_basisU.GetMaxParam(), gridU);
    fillUniform(numElemsV, m_basisV.GetMinParam(), m_basisV.GetMaxParam(), gridV);
    fillUniform(numElemsW, m_basisW.GetMinParam(), m_basisW.GetMaxParam(), gridW);

    GenerateValuesFromGrid((gridU ^ gridV) ^ gridW, m_tesselation);
    CreateFastTessel();
  }

  template <class VecType>
  bool UpdateVolume(const Array3D<VecType>& control, std::vector<VecType>& result)
  {
    //TESSELATION();
    FASTTESSELATION();
  }
  T Volume(const Array3D<V3d<T> >& control)
  {
    T result = T(0);
    for(unsigned i = 0; i < m_volumeData.size(); i++)
      result += control(m_volumeData[i].i, m_volumeData[i].j, m_volumeData[i].k).x * control(m_volumeData[i].l, m_volumeData[i].m, m_volumeData[i].n).y * control(m_volumeData[i].o, m_volumeData[i].p, m_volumeData[i].q).z * m_volumeData[i].coef;
    return result;
  }
  DP Volume(const Vec_I_DP& control, const Array3D<V3d<double> >::Dimension& dim)
  {
    DP result = 0.0;
    for(unsigned i = 0; i < m_volumeData.size(); i++)
      result += control[3 * ((m_volumeData[i].i * dim.z + m_volumeData[i].j) * dim.y + m_volumeData[i].k) + 0] * 
                control[3 * ((m_volumeData[i].l * dim.z + m_volumeData[i].m) * dim.y + m_volumeData[i].n) + 1] * 
                control[3 * ((m_volumeData[i].o * dim.z + m_volumeData[i].p) * dim.y + m_volumeData[i].q) + 2] * m_volumeData[i].coef;
    return result;
  }
  Array3D<V3d<T> > VolumeGrad(const Array3D<V3d<T> >& control)
  {
    Array3D<V3d<T> > grad;
    grad.SetDimension(control.GetDimension());
    for(unsigned i = 0; i < grad.GetNumElems(); i++)
    {
      for(unsigned j = 0; j < V3d<T>::DIMENSION; j++)
        grad(i)[j] = T(0);
    }
    for(unsigned i = 0; i < m_volumeData.size(); i++)
    {
      grad(m_volumeData[i].i, m_volumeData[i].j, m_volumeData[i].k).x += 
      control(m_volumeData[i].l, m_volumeData[i].m, m_volumeData[i].n).y * 
      control(m_volumeData[i].o, m_volumeData[i].p, m_volumeData[i].q).z * 
      m_volumeData[i].coef;

      grad(m_volumeData[i].l, m_volumeData[i].m, m_volumeData[i].n).y += 
      control(m_volumeData[i].i, m_volumeData[i].j, m_volumeData[i].k).x * 
      control(m_volumeData[i].o, m_volumeData[i].p, m_volumeData[i].q).z * 
      m_volumeData[i].coef;

      grad(m_volumeData[i].o, m_volumeData[i].p, m_volumeData[i].q).z += 
      control(m_volumeData[i].i, m_volumeData[i].j, m_volumeData[i].k).x * 
      control(m_volumeData[i].l, m_volumeData[i].m, m_volumeData[i].n).y * 
      m_volumeData[i].coef;
    }
    return grad;
  }
  void VolumeGrad(const Vec_I_DP& control, const Array3D<V3d<double> >::Dimension& dim, Vec_O_DP& output)
  {
    for(unsigned i = 0; i < control.size(); i++)
    {
      output[i] = 0.0;
    }
    for(unsigned i = 0; i < m_volumeData.size(); i++)
    {
      output[3 * ((m_volumeData[i].i * dim.z + m_volumeData[i].j) * dim.y + m_volumeData[i].k) + 0] += 
      control[3 * ((m_volumeData[i].l * dim.z + m_volumeData[i].m) * dim.y + m_volumeData[i].n) + 1] * 
      control[3 * ((m_volumeData[i].o * dim.z + m_volumeData[i].p) * dim.y + m_volumeData[i].q) + 2] * 
      m_volumeData[i].coef;

      output[3 * ((m_volumeData[i].l * dim.z + m_volumeData[i].m) * dim.y + m_volumeData[i].n) + 1] += 
      control[3 * ((m_volumeData[i].i * dim.z + m_volumeData[i].j) * dim.y + m_volumeData[i].k) + 0] * 
      control[3 * ((m_volumeData[i].o * dim.z + m_volumeData[i].p) * dim.y + m_volumeData[i].q) + 2] * 
      m_volumeData[i].coef;

      output[3 * ((m_volumeData[i].o * dim.z + m_volumeData[i].p) * dim.y + m_volumeData[i].q) + 2] += 
      control[3 * ((m_volumeData[i].i * dim.z + m_volumeData[i].j) * dim.y + m_volumeData[i].k) + 0] * 
      control[3 * ((m_volumeData[i].l * dim.z + m_volumeData[i].m) * dim.y + m_volumeData[i].n) + 1] * 
      m_volumeData[i].coef;
    }
  }

  enum SpatMode
  {
    SPT_MODE_DIRECT,
    SPT_MODE_MAXINF,
    //SPT_MODE_INTERP
  };
  void SetSpatMode(int mode){m_mode = mode;}
  int  GetSpatMode(){return m_mode;}
  template <class VecType>
  void GetCPoints(Array3D<VecType>& spatial, Array3D<VecType>& cpoints)
  {
    //CONTROLPOINTS();
    if(m_mode == SPT_MODE_DIRECT)
    {
      cpoints = spatial;
      return;
    }
    //if(m_mode == SPT_MODE_MAXINF)
    //{
      CPOINTSMAXINFL();
      return;
    //}
  }
  BBasis<T>& GetUBasis(){return m_basisU;}
  BBasis<T>& GetVBasis(){return m_basisV;}
  BBasis<T>& GetWBasis(){return m_basisW;}
protected:
private:
  void CreateFastTessel()
  {
    CREATEFASTTESSEL();
  }
  void PrepareVolumeData()
  {
    BBasis<T>& u = m_basisU;
    BBasis<T>& v = m_basisV;
    BBasis<T>& w = m_basisW;
    std::vector<T>& uknots = u.GetKnots();
    std::vector<T>& vknots = v.GetKnots();
    std::vector<T>& wknots = w.GetKnots();

    for(unsigned i = 0; i < u.GetNumFunctions(); i++)
    {
      for(unsigned j = 0; j < v.GetNumFunctions(); j++)
      {
        for(unsigned k = 0; k < w.GetNumFunctions(); k++)
        {
          for(unsigned l = 0; l < u.GetNumFunctions(); l++)
          {
            for(unsigned m = 0; m < v.GetNumFunctions(); m++)
            {
              for(unsigned n = 0; n < w.GetNumFunctions(); n++)
              {
                for(unsigned o = 0; o < u.GetNumFunctions(); o++)
                {
                  for(unsigned p = 0; p < v.GetNumFunctions(); p++)
                  {
                    for(unsigned q = 0; q < w.GetNumFunctions(); q++)
                    {
                      if(i > u.GetOrder() - 1 || l > u.GetOrder() - 1  || o > u.GetOrder() - 1)
                        if(i < u.GetNumFunctions() - u.GetOrder() || l < u.GetNumFunctions() - u.GetOrder() || o < u.GetNumFunctions() - u.GetOrder())
                          if(j > v.GetOrder() - 1 || m > v.GetOrder() - 1  || p > v.GetOrder() - 1)
                            if(j < v.GetNumFunctions() - v.GetOrder() || m < v.GetNumFunctions() - v.GetOrder() || p < v.GetNumFunctions() - v.GetOrder())
                              if(k > w.GetOrder() - 1 || n > w.GetOrder() - 1  || q > w.GetOrder() - 1)
                                if(k < w.GetNumFunctions() - w.GetOrder() || n < w.GetNumFunctions() - w.GetOrder() || q < w.GetNumFunctions() - w.GetOrder())
                                  continue;
                      T muilo = T(0);
                      T mulio = T(0);
                      T muoil = T(0);
                      T nujmp = T(0);
                      T numjp = T(0);
                      T nupjm = T(0);
                      T omknq = T(0);
                      T omnkq = T(0);
                      T omqkn = T(0);
                      for(unsigned t = u.GetOrder() - 1; t < u.GetNumFunctions(); t++)
                      {
                        Polynom<T> pi = u.GetPolynom(i, t);
                        Polynom<T> pl = u.GetPolynom(l, t);
                        Polynom<T> po = u.GetPolynom(o, t);

                        Polynom<T> pil = pi;
                        Polynom<T> plo = pl;
                        Polynom<T> pio = po;

                        pil *= pl;
                        plo *= po;
                        pio *= pi;

                        Polynom<T> dpi = pi;
                        Polynom<T> dpl = pl;
                        Polynom<T> dpo = po;

                        dpi.differentiate();
                        dpl.differentiate();
                        dpo.differentiate();

                        dpi *= plo;
                        dpl *= pio;
                        dpo *= pil;

                        dpi.integrate();
                        dpl.integrate();
                        dpo.integrate();
                        muilo += dpi.evaluate(uknots[t + 1]) - dpi.evaluate(uknots[t]);
                        mulio += dpl.evaluate(uknots[t + 1]) - dpl.evaluate(uknots[t]);
                        muoil += dpo.evaluate(uknots[t + 1]) - dpo.evaluate(uknots[t]);
                      }
                      for(unsigned t = v.GetOrder() - 1; t < v.GetNumFunctions(); t++)
                      {
                        Polynom<T> pj = v.GetPolynom(j, t);
                        Polynom<T> pm = v.GetPolynom(m, t);
                        Polynom<T> pp = v.GetPolynom(p, t);

                        Polynom<T> pjm = pj;
                        Polynom<T> pmp = pm;
                        Polynom<T> pjp = pp;

                        pjm *= pm;
                        pmp *= pp;
                        pjp *= pj;

                        Polynom<T> dpj = pj;
                        Polynom<T> dpm = pm;
                        Polynom<T> dpp = pp;

                        dpj.differentiate();
                        dpm.differentiate();
                        dpp.differentiate();

                        dpj *= pmp;
                        dpm *= pjp;
                        dpp *= pjm;

                        dpj.integrate();
                        dpm.integrate();
                        dpp.integrate();
                        nujmp += dpj.evaluate(vknots[t + 1]) - dpj.evaluate(vknots[t]);
                        numjp += dpm.evaluate(vknots[t + 1]) - dpm.evaluate(vknots[t]);
                        nupjm += dpp.evaluate(vknots[t + 1]) - dpp.evaluate(vknots[t]);
                      }
                      for(unsigned t = w.GetOrder() - 1; t < w.GetNumFunctions(); t++)
                      {
                        Polynom<T> pk = w.GetPolynom(k, t);
                        Polynom<T> pn = w.GetPolynom(n, t);
                        Polynom<T> pq = w.GetPolynom(q, t);

                        Polynom<T> pkn = pk;
                        Polynom<T> pnq = pn;
                        Polynom<T> pkq = pq;

                        pkn *= pn;
                        pnq *= pq;
                        pkq *= pk;

                        Polynom<T> dpk = pk;
                        Polynom<T> dpn = pn;
                        Polynom<T> dpq = pq;

                        dpk.differentiate();
                        dpn.differentiate();
                        dpq.differentiate();

                        dpk *= pnq;
                        dpn *= pkq;
                        dpq *= pkn;

                        dpk.integrate();
                        dpn.integrate();
                        dpq.integrate();
                        omknq += dpk.evaluate(wknots[t + 1]) - dpk.evaluate(wknots[t]);
                        omnkq += dpn.evaluate(wknots[t + 1]) - dpn.evaluate(wknots[t]);
                        omqkn += dpq.evaluate(wknots[t + 1]) - dpq.evaluate(wknots[t]);
                      }
                      T det = muilo * numjp * omqkn + mulio * nupjm * omknq + muoil * nujmp * omnkq - 
                              muoil * numjp * omknq - mulio * nujmp * omqkn - muilo * nupjm * omnkq;
                      m_volumeData.push_back(vldata(det, i, j, k, l, m, n, o, p, q));
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  void GetCSMatrix(Mat_O_DP &matrix, Vec_O_INT& vindex)
  {
    std::vector<Array3D<T> >  tesselationMaxima;
    CONTROLSPATIALMATRIX();
  }

  void FillValuesArray(const V3d<T>& param, Array3D<T>& output)
  {
    output.resize(m_basisU.GetNumFunctions(), m_basisV.GetNumFunctions(), m_basisW.GetNumFunctions());
    for(unsigned j = 0; j < m_basisU.GetNumFunctions(); j++)
    {
      for(unsigned k = 0; k < m_basisV.GetNumFunctions(); k++)
      {
        for(unsigned l = 0; l < m_basisW.GetNumFunctions(); l++)
        {
          output(j, k, l)  = m_basisU.GetValue(j, param.x) * m_basisV.GetValue(k, param.y) * m_basisW.GetValue(l, param.z);
        }
      }
    }
  }

  void FillDerivArray(const V3d<T>& param, unsigned orderU, unsigned orderV, unsigned orderW, Array3D<T>& output)
  {
    output.resize(m_basisU.GetNumFunctions(), m_basisV.GetNumFunctions(), m_basisW.GetNumFunctions());
    for(unsigned j = 0; j < m_basisU.GetNumFunctions(); j++)
    {
      for(unsigned k = 0; k < m_basisV.GetNumFunctions(); k++)
      {
        for(unsigned l = 0; l < m_basisW.GetNumFunctions(); l++)
        {
          output(j, k, l)  = m_basisU.GetDerivative(j, orderU, param.x) * m_basisV.GetDerivative(k, orderV, param.y) * m_basisW.GetDerivative(l, orderW, param.z);
        }
      }
    }
  }

  void FillDerivArray(const V2d<T>& param, unsigned orderU, unsigned orderV, Array2D<T>& output)
  {
    output.resize(m_basisU.GetNumFunctions(), m_basisV.GetNumFunctions());
    for(unsigned j = 0; j < m_basisU.GetNumFunctions(); j++)
    {
      for(unsigned k = 0; k < m_basisV.GetNumFunctions(); k++)
      {
        output(j, k)  = m_basisU.GetDerivative(j, orderU, param.x) * m_basisV.GetDerivative(k, orderV, param.y);
      }
    }
  }

  void GenerateValuesFromGrid(const std::vector<V3d<T> >& grid, std::vector<Array3D<T> >& tesselation)
  {
    tesselation.resize(grid.size());
    for(unsigned i = 0; i < grid.size(); i++)
    {
      FillValuesArray(grid[i], tesselation[i]);
    }
  }  

  void GenerateTesselationMaxima(std::vector<Array3D<T> >&  tesselationMaxima)
  {
    std::vector<T> gridU;
    std::vector<T> gridV;
    std::vector<T> gridW;
    for(unsigned i = 0; i < m_basisU.GetNumFunctions(); i++)
      gridU.push_back(m_basisU.GetMaxPoint(i));
    for(unsigned i = 0; i < m_basisV.GetNumFunctions(); i++)
      gridV.push_back(m_basisV.GetMaxPoint(i));
    for(unsigned i = 0; i < m_basisW.GetNumFunctions(); i++)
      gridW.push_back(m_basisW.GetMaxPoint(i));
    GenerateValuesFromGrid((gridU ^ gridV) ^gridW, tesselationMaxima);
  }

  Mat_DP                    m_matrix;
  Vec_INT                   m_vidx;
  std::vector<Array3D<T> >  m_tesselation;
  BBasis<T>                 m_basisU;
  BBasis<T>                 m_basisV;
  BBasis<T>                 m_basisW;
  int                       m_mode;
  struct vldata
  {
    vldata(const T& _coef, int _i, int _j, int _k, int _l, int _m, int _n, int _o, int _p, int _q):coef(_coef), i(_i), j(_j), k(_k), l(_l), m(_m), n(_n), o(_o), p(_p), q(_q){}
    T coef;
    int i, j, k, l, m, n, o, p, q;
  };
  struct tesdata
  {
    tesdata(const T& _coef, int _i):coef(_coef), i(_i){}
    T coef;
    int i;
  };
  std::vector<std::vector<tesdata> > m_fastTessel;
  std::vector<vldata>                m_volumeData;
};


#endif
