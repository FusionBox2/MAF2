#ifndef __lhpOpTextureOrientation_H__
#define __lhpOpTextureOrientation_H__

#include "mafOp.h"
#include "mafEventBase.h"
#include "mafNode.h"
#include "mafGUIDialog.h"
#include "mafGUIFloatSlider.h"
#include "mafRWI.h"

#include "vtkPolyData.h"

#include "lhpHistogramEqualizationFilter.h"
#include "lhpTextureOrientationFilter.h"
#include "lhpTextureOrientationSlicePipe.h"
#include "lhpTextureOrientationVectorGlyphPipe.h"
#include "lhpTextureOrientationCallbacks.h"



//------------------------------------------------------------------------------
// Operation which finds the texture direction
// It converts an image VME to a surface VME.
// The surface is a set of polydata points with the direction tensor as attributes.
//------------------------------------------------------------------------------
class lhpOpTextureOrientation : public mafOp
{
public:
  lhpOpTextureOrientation(wxString label = "Texture Orientation");
  ~lhpOpTextureOrientation(); 

  mafTypeMacro(lhpOpTextureOrientation, mafOp);

  mafOp* Copy();

  void OnEvent(mafEventBase *maf_event);

  /** Return true for the acceptable vme type. */
  bool Accept(mafNode* vme);

  /** Builds operation's interface by calling CreateOpDialog() method. */
  void OpRun();

  /** Execute the operation. */
  void OpDo();

  /** Makes the undo for the operation. */
  void OpUndo();

protected:
  //----------------------------------------------------------------------------
  // methods for operation's workflow
  //----------------------------------------------------------------------------

  /** Builds operation's interface and visualization pipeline. */
  void CreateOpDialog();

  /** Remove operation's interface. */
  void DeleteOpDialog();

  /** Create the vtk pipe */
  void CreateVisualPipe() ;

  /** Create the output vme and reparent */
  void CreateOutputVME() ;

  /** Update the camera */
  void UpdateCamera() ;

  /** Get renderer */
  vtkRenderer* GetRenderer() ;



  //----------------------------------------------------------------------------
  // methods for controlling the slice
  //----------------------------------------------------------------------------

  /** Initialize the parameters of the slice (origin and view index)
  This only sets the parameters - it does not set or change the visual pipes ! */
  void InitSliceParams(int viewIndex, double *bounds) ;

  /** Move the global slice */
  //void lhpOpMultiscaleExplore::UpdateSlicePosition() ;
  void UpdateSlicePosition() ;

  /** Update the view direction.
  This also recalculates the slice position */
  void UpdateViewAxis(double *bounds) ;

  /** Set the slider range to fit the given bounds */
  void SetSliderRange(double *bounds) ;



  //----------------------------------------------------------------------------
  // member variables
  //----------------------------------------------------------------------------
  mafGUIDialog		*m_Dialog;        // dialog and interactor
  mafRWI          *m_Rwi;

  double m_SliceOrigin[3] ;           // position of slice
  int m_ViewIndex ;                   // view direction validator (don't assume that 0,1,2 = x,y,z !!)
  int m_ViewIndex_old ;
  double m_SliderOrigin ;             // position slider validator
  double m_SliderOrigin_old ;
  double m_texWinSize ;
  double m_texWinStepSize ;
  int m_outputFormat ;
  wxString m_outputFilename ;         // filename for output

  // These widgets are destroyed with the dialog
  // don't try to delete in deconstructor
  mafGUIFloatSlider *m_PosSlider ;
  mafGUIButton *m_printButton ;
  wxGauge *m_progressGauge ;

  lhpTextureOrientationProgressCallback *m_progressCallback ;

  vtkRenderer     *m_externalRenderer ;

  lhpTextureOrientationSlicePipe *m_slicePipe ;
  lhpTextureOrientationVectorGlyphPipe *m_vectorGlyphPipe ;
  vtkPolyData *m_polydata ;
  mafVMESurface *m_polydataVME ;
};


#endif