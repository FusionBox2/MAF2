/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: vtkMAFMuscleDecomposition.h,v $ 
  Language: C++ 
  Date: $Date: 2009-05-14 14:07:11 $ 
  Version: $Revision: 1.1.2.1 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
  vtkMAFMuscleDecomposition decomposes the muscle volume represented by
  the input surface into fibers represented by a set of polylines 
  (in vtkPolyData). Various fiber geometries are supported (e.g.,
  parallel, pennate, curven, fanned, etc.). The decomposition was
  written according to following papers:

  SILVIA S. BLEMKER and SCOTT L. DELP: Three-Dimensional Representation of
  Complex Muscle Architectures and Geometries. Annals of Biomedical Engineering, 
  Vol. 33, No. 5, May 2005, pp. 661–673
  http://www.mae.virginia.edu/muscle/pdf/Blemker2005b.pdf

  Silvia S. Blemker, Scott L. Delp: Rectus femoris and vastus intermedius fiber excursions predicted
  by three-dimensional muscle models. Journal of Biomechanics 39 (2006): 1383–1391
  http://www.mae.virginia.edu/muscle/pdf/Blemker2006.pdf
*/
#ifndef vtkMAFMuscleDecomposition_h__
#define vtkMAFMuscleDecomposition_h__

#pragma once

#pragma warning(push)
#pragma warning(disable:4996)
#include "vtkMAFConfigure.h"
#include "vtkPolyDataAlgorithm.h"
#pragma warning(pop)

#include "vtkMAFMuscleFibers.h"

class vtkPoints;
class vtkCellLocator;

class VTK_vtkMAF_EXPORT vtkMAFMuscleDecomposition : public vtkPolyDataAlgorithm
{
public:
  static vtkMAFMuscleDecomposition *New();

  vtkTypeMacro(vtkMAFMuscleDecomposition, vtkPolyDataAlgorithm);  

protected:
  vtkMAFMuscleDecomposition();           
  virtual ~vtkMAFMuscleDecomposition();

protected:
  //structures used by routines
  typedef double VCoord[3];
  typedef struct LOCAL_FRAME
  {    
    VCoord O;       //<origin
    VCoord uvw[3];  //<axis (may not be of unit size)    
  } LOCAL_FRAME;  

protected:
  vtkMAFMuscleFibers* FibersTemplate;    //<instance of fiber type to be used
  vtkPoints* OriginArea;                 //<origin area points for the input muscle
  vtkPoints* InsertionArea;              //<insertion area points for the input muscle

  int NumberOfFibres;   //<number of fibres to be generated; default = 50
  int Resolution;       //<number of segments per fibre; default = 9
  int SmoothFibers;		  //<non-zero, if fibres are smoothed (default)
  int SmoothSteps;      //<number of smoothing iterations
  double SmoothFactor;  //<smoothing weight (lower values mean more smoothed, higher less), default is 4
  int DebugMode;	//<masked debug mode - see below

public:
  typedef enum DebugModeFlags
  {
    dbgNone = 0,                  //no extra things
    dbgVisualizeFitting = 1,      //visualizes in an external renderer the cube fitting process
    dbgVisualizeFittingResult = 2,//visualizes in an external renderer the result of cube fitting process
    dbgDoNotProjectFibres = 4,    //does not project fibres 
  };
  

public:
  /** Gets the number of muscle fibres to be generated */
  vtkGetMacro(NumberOfFibres, int);

  /** Sets the number of muscle fibres to be generated */
  vtkSetMacro(NumberOfFibres, int);

  /** Gets the number of vertices per one fiber */
  vtkGetMacro(Resolution, int);

  /** Sets the number of vertices per one fiber */
  vtkSetMacro(Resolution, int);

  /** Sets new template for muscle fibers */
  virtual void SetFibersTemplate(vtkMAFMuscleFibers* pTemplate);

  /** Gets the currently associated template for muscle fibers */
  vtkGetMacro(FibersTemplate, vtkMAFMuscleFibers*);

  /** Sets new origin area points for the input muscle */
  virtual void SetOriginArea(vtkPoints* pPoints);

  /** Gets the currently associated origin area points for the input muscle */
  vtkGetMacro(OriginArea, vtkPoints*);

  /** Sets new insertion area points for the input muscle */
  virtual void SetInsertionArea(vtkPoints* pPoints);

  /** Gets the currently associated insertion area points for the input muscle */
  vtkGetMacro(InsertionArea, vtkPoints*);

  /** Gets non-zero, if  the generated fibres should be smoothed */
  vtkGetMacro(SmoothFibers, int);

  /** Defines whether the generated fibres should be smoothed (non-zero); by default they are smoothed */
  vtkSetMacro(SmoothFibers, int);

  /** Defines whether the generated fibres should be smoothed (non-zero) */
  vtkBooleanMacro(SmoothFibers, int);

  /** Gets the number of smoothing iteration steps (default is 5) */
  vtkGetMacro(SmoothSteps, int);

  /** Sets the number of smoothing iteration steps (default is 5)*/
  vtkSetMacro(SmoothSteps, int);

  /** Gets the smoothing weight; lower values mean more smoothed fibers, default is 4 */
  vtkGetMacro(SmoothFactor, double);

  /** Sets the smoothing weight; lower values mean more smoothed fibers, default is 4 */
  vtkSetMacro(SmoothFactor, double);


  /** Gets debug mode (see Dbg enums)*/
  vtkGetMacro(DebugMode, int);

  /** Sets debug mode (see Dbg enums) */
  vtkSetMacro(DebugMode, int);
protected:
  /** 
  By default, UpdateInformation calls this method to copy information
  unmodified from the input to the output.*/
  /*virtual*/void ExecuteInformation();

  /**
  This method is the one that should be used by subclasses, right now the 
  default implementation is to call the backwards compatibility method */
  /*virtual*/void ExecuteData(vtkDataObject *output);


  /** Samples E2 space <0..1>x<0..1> storing samples into points buffer.
  N samples are created. Points buffer must be capable enough to hold
  2*N doubles (format is x1,y1,x2,y2...).
  The routine is based on the code by Frances Y. Kuo <f.kuo@unsw.edu.au>*/
  void CreateSobolPoints(int N, double* points);

  /**
  Computes the minimal oriented box that fits the input data so that all
  points are inside of this box (or on its boundary) and the total squared
  distance of template origin points from the input mesh origin points and
  the total squared distance of template insertion points from the input
  mesh origin points are minimized */
  void ComputeFittingOB(vtkPoints* points, LOCAL_FRAME& out_lf);

  /** Computes the principal axis for the given point set. 
  N.B. the direction is normalized*/
  void ComputeAxisLine(vtkPoints* points, double* origin, double* direction); 

  /**
  Computes 4*nFrames vectors by rotating u around r vector. All vectors are
  normalized and stored in the order A, B, C, D where B is the vector
  opposite to A, C is vector perpendicular to A and r and D is vector
  opposite to C. N.B. vectors u and r must be normalized and perpendicular!
  The buffer pVectors must be capable enough to hold all vectors.*/
  void ComputeDirectionVectors(double* u, double* r, int nFrames, VCoord* pVectors);

  /** Adjusts the length of direction vectors (in pVects) to fit the given point set. 
  For each direction vector, the algorighm find a plane defined by the center 
  (it should be the centroid of points) and a normal of non-unit size that is 
  collinear with the input direction vector. This normal is chosen so that the 
  no point lies in the positive halfspace of the plane (i.e. in the direction 
  of normal from the plane). The computed normals are returned in pVects.
  N.B. the input vectors must be of unit size! */
  void FitDirectionVectorsToData(vtkPoints* points, double* center, 
    int nVects, VCoord* pVects);

  /**
  Computes local frame systems for various cubes defined by their center
  and two direction vectors in w and 4 direction vectors in u and v axis.
  Direction vectors in u and v are given in uv_dirs and have the structure
  compatible with the output of ComputeDirectionVectors method. 
  The computed LFs are stored in pLFS buffer. The buffer must be capable to
  hold 8*nCubes (= nFrames in ComputeDirectionVectors) entries. */
  void ComputeLFS(double* center, VCoord* w_dir, 
    int nCubes, VCoord* uv_dirs, LOCAL_FRAME* pLFS);

  /**
  Finds the best local frame system from those passed in pLFS that best
  maps template origin and insertion points to target origin and insertion
  points. N.B. any point set can be NULL, if it is not needed. Special
  case is when both target sets or template sets are NULL, then the
  routine returns the first LF. */
  int FindBestMatch(vtkPoints* template_O, vtkPoints* template_I,
    int nLFS, LOCAL_FRAME* pLFS, vtkPoints* target_O, vtkPoints* target_I);

  /** Returns the coordinates of surface point that is the closest to the given plane. */
  void FindClosestPoint(vtkPolyData* input, const double* origin, 
    const double* normal, double* x);

  /** Sorts the given points according to their iCoord coordinate.
  The resulting order is returned and the user is responsible for its
  deallocation when it is no longer needed. pPoints array is not touched. */
  int* SortPoints(const VCoord* pPoints, int nPoints, int iCoord);

  /**
  Gets edges from the contour and sorts them to form continuous path. The
  format of the returned array (the caller is responsible for its
  deallocation) is s1,s2,s2,s3,s3,s4, ... sn,s1 - for instance:
  0,2,2,3,3,4,4,6,0 */
  int* GetSortedEdges(vtkPolyData* contour);

  /** Adds new points into pContourPoints [in/out] so they form a polygon of at least 4 vertices. 
  Returns the new number of points in the list. 
  N.B, pContourPoints must be capable to hold at least 4 vertices.*/
  int FixPolygon(VCoord* pContourPoints, int nPoints);

  /** Divides the rectangle defined by one point and two vectors into
  nPoints edges such that the total square error between lengths of
  contour and rectangle edges is minimized. The routine stores beginning
  points of these rectangle edges into pOutRectPoints. */
  void DivideRectangle(double* origin, double* u, double* v, int nPoints,
    VCoord* pContourPoints, VCoord* pOutRectPoints);

  /**
  Computes new locations of given points (pPoints)lying inside the
  template polygon (pPolyTemplate) within the other polygon
  (pPolyTarget). Both polygons have the same number of edges. New
  coordinates are stored in pPoints buffer.*/
  void MapPoints(VCoord* pPoints, int nPoints, 
    VCoord* pPolyTemplate, VCoord* pPolyTarget, int nPolyPoints);

  /** Smooth the fiber defined by the given points. */
  void SmoothFiber(VCoord* pPoints, int nPoints);

  /** Creates an external rendering window and displays the fitted cube */
  void DebugVisualizeFitting(int nIndex, int nCount, LOCAL_FRAME& lfs, 
    vtkPoints* template_O, vtkPoints* template_I, 
    vtkPoints* target_O, vtkPoints* target_I, double dblScore, 
    bool bBestOne = false);

private:
  vtkMAFMuscleDecomposition(const vtkMAFMuscleDecomposition&);  // Not implemented.
  void operator = (const vtkMAFMuscleDecomposition&);  // Not implemented.  
};

#endif // vtkMAFMuscleDecomposition_h__