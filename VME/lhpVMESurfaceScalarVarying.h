/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpVMESurfaceScalarVarying.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 12:15:29 $
  Version:   $Revision: 1.6 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __lhpVMESurfaceScalarVarying_h
#define __lhpVMESurfaceScalarVarying_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVME.h"
#include "mafEvent.h"
#include "mafVMEOutputSurface.h"
#include "medVMEAnalog.h"

#include <vector>
#include <algorithm>

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafGUICheckListBox;
class mafTransform;
class mmaMaterial;
class vtkPolyData;
class mafInteractorPicker;
class vtkDoubleArray;
class vtkPoints;
class vtkIdList;
class vtkPointLocator;

/** This type of VME allow to choose a surface and one or more time varying mafVMEScalars. 
The scalar value of the medVMEAnalog is used to color the surface in a specific location given by the user.
@sa lhpVisualPipeSurfaceScalar*/
class MAF_EXPORT lhpVMESurfaceScalarVarying : public mafVME
{
public:
  mafTypeMacro(lhpVMESurfaceScalarVarying, mafVME);

  enum PROBER_WIDGET_ID
  {
    ID_SURFACE_LINK = Superclass::ID_LAST,
    ID_SCALAR_LINK,
    ID_LIST_SCALARS_AVAILABLES,
    ID_EDIT_SCALAR_POSITION,
    ID_RADIUS,
    ID_LAST
  };

  /** Callback used to choose a VME with output a surface.*/
  static bool OutputSurfaceAccept(mafNode *node) {return(node != NULL && ((mafVME *)node)->GetOutput()->IsMAFType(mafVMEOutputSurface));};

  /** Callback used to add time varying scalar VME to color surface.*/
  static bool VmeScalarAccept(mafNode *node) {return(node != NULL && node->IsMAFType(medVMEAnalog));};

  /** Precess events coming from other objects */
  void OnEvent(mafEventBase *maf_event) override;

  /** Update the scalar values on the polydata.*/
  void SetTimeStamp(mafTimeStamp t) override;

  /** Return the suggested pipe-typename for the visualization of this vme */
  mafString GetVisualPipe() override {return mafString(_R("lhpVisualPipeSurfaceScalar"));};

  /** Return pointer to material attribute. */
  std::shared_ptr<mmaMaterial> GetMaterial();

  /** Copy the contents of another lhpVMESurfaceScalarVarying into this one. */
  int DeepCopy(mafNode *a) override;

  /** Compare with another lhpVMESurfaceScalarVarying. */
  bool Equals(mafVME *vme) override;

  /** Get the link to the scalar signals.*/
  mafNode *GetScalarLink();

  /** Set the link to the scalar signals.*/
  void SetScalarLink(mafNode *scalar);

  /** Get the link to the surface.*/
  mafNode *GetSurfaceLink();

  /** Set the link to the surface.*/
  void SetSurfaceLink(mafNode *surface);

  /** Set the pose matrix for the Prober. */
  void SetMatrix(const mafMatrix &mat) override;

  /** Clear the parameter 'kframes' because lhpVMESurfaceScalarVarying has no timestamp. */
  void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes) override;

  /** Return true if the data associated with the VME is present and updated at the current time.*/
  bool IsDataAvailable() override;

  /** return icon */
  static const char** GetIcon();

  /** Return the number of medVMEAnalog used to color the polydata.*/
  int GetNumberOfScalarData() {return m_ScalarRegionMap.size();};

  /** Return the n-th row of the medVMEAnalog's inner vnl matrix corresponding to the index idx.*/
  int GetScalarVMEIndex(int idx);

  /** Return the scalar indexes of the surface associated to the medVMEAnalog.*/
  vtkIdList *GetSurfaceScalarIndexes(int idx);

  /** Set the Scalar index of the mafVMEAnalog and assign it the scalar index associated to the linked surface.*/
  void SetScalarIDs(int analog_scalar_index, vtkIdList *surface_scalar_idx);

  /** print a dump of this object */
  void Print(std::ostream& os, const int tabs=0) override;

protected:
  lhpVMESurfaceScalarVarying();
  ~lhpVMESurfaceScalarVarying() override; 

  /** Internally used to create a new instance of the GUI.*/
  mafGUI *CreateGui() override;

  /** used to initialize and create the material attribute if not yet present */
  int InternalInitialize() override;

  void InternalStore(mafStorageElementBuilder& parent) override;
  void InternalRestore(const mafStorageElement& node) override;

  /** called to prepare the update of the output */
  void InternalPreUpdate() override;

  /** update the output data structure */
  void InternalUpdate() override;

  /** Used to fill the listbox with the name of the scalars*/
  void FillScalarsName(bool new_scalars = true);

  /** Update surface scalars and data-pipe according to the linked surface.*/
  void UpdateSurface();

  /** Update the time index according to the current time*/
  void UpdateTimeIndex(mafTimeStamp t);

  /** Initialize scalar values and connect them to the polydata.*/
  void InitScalars();

  /** Find a circular region around the center vertex Id and return vertex Ids falling in it.*/
  void MarkRegion(int vertex_id, int radius, vtkIdList &Ids);

  /** Find the nearest timestamp of the mafVMEAnalog extracting the information from the matrix.*/
  std::vector<mafTimeStamp>::iterator FindNearestItem(mafTimeStamp t);

  mafString m_SurfaceName; ///< Name of the linked surface
  mafString m_ScalarName; ///< Name of the linked scalar
  std::shared_ptr<mafTransform> m_Transform;
  vtkPolyData *m_PolyData; ///< Surface data
  
  double m_ScalarRange[2];
  double m_ScalarMin; ///< Minimum scalar value that initialize the scalars.
  int m_CurrentTimeIndex; ///< Index of the matrix corresponding to the current time
  int m_EditMode; ///< Flag to attach and detach th picker from the surface.
  int m_ActiveScalarVMEIndex; ///< Store the information regarding the last medVMEAnalog's scalar index added
  int m_Radius; ///< Set the radius to consider for coloring neighbors of selected triangles.
  mafGUICheckListBox *m_ScalarsAvailableList; ///< Listbox representing the list of medVMEAnalog's scalars
  typedef std::map<int, vtkIdList *> SurfaceScalarRegionMap;
  SurfaceScalarRegionMap m_ScalarRegionMap;
  std::vector<mafTimeStamp> m_ScalarTimeStamps; ///< Vector of timestamps presents in mafVMEAnalog
  
  vtkDoubleArray *m_SurfaceScalars; ///< Data Array representing the scalar data of the polydata.
  vtkPoints *m_PickedPoint; ///< Represent the last picked point on the linked surface

  mafInteractorPicker *m_PickScalar; ///< Picker interactor used to position the time varying scalar value on the surface
  mafInteractor *m_OldBehavior; ///< pointer to the old behavior of the surface.

  bool m_SuggestUser; ///< This flag is to pop up a dialog (only the first time) to explain the work flow to add a new scalar to a particular position af the linked surface

  vtkPointLocator *m_Locator;
private:
  lhpVMESurfaceScalarVarying(const lhpVMESurfaceScalarVarying&); // Not implemented
  void operator=(const lhpVMESurfaceScalarVarying&); // Not implemented
};
#endif
