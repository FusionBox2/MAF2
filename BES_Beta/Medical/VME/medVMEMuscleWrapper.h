/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medVMEMuscleWrapper.h,v $
  Language:  C++
  Date:      $Date: 2008-12-08 13:07:52 $
  Version:   $Revision: 1.1.2.2 $
  Authors:   Josef Kohout
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __medVMEMuscleWrapper_h
#define __medVMEMuscleWrapper_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVME.h"
#include "mafVMEOutputPolyline.h"
#include "mafVMEVolumeGray.h"
#include "mafEvent.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkPoints;
class vtkPolyData;
class vtkMAFPolyDataDeformation;

/** medVMEMuscleWrapper - */
class MAF_EXPORT medVMEMuscleWrapper : public mafVME
{
public:      
  mafTypeMacro(medVMEMuscleWrapper,mafVME);

  enum MUSCLEWRAPPER_WIDGET_ID
  {
    ID_RESTPOSE_MUSCLE_LINK = Superclass::ID_LAST,
    ID_RESTPOSE_WRAPPER1_LINK,
    ID_RESTPOSE_WRAPPER2_LINK,
    ID_DYNAMIC_WRAPPER1_LINK,
    ID_DYNAMIC_WRAPPER2_LINK,
    ID_FIBERS_ORIGIN_LINK,
    ID_FIBERS_INSERTION_LINK,
    ID_DEFORMATIONMODE,

    ID_GENERATE_FIBERS,
    ID_FIBERS_TEMPLATE,     //template type
    ID_FIBERS_RESOLUTION,   //resolution
    ID_FIBERS_NUMFIB,       //number of fibers    

	ID_FIBERS_SMOOTH,	//smooth computed fibres
	ID_FIBERS_DEBUG_SHOWTEMPLATE,	//show template but not projection

    ID_LAST,
  };

  enum MUSCLEWRAPPER_LINK_IDS
  {
    LNK_RESTPOSE_MUSCLE = 0,
    LNK_RESTPOSE_WRAPPER1,
    LNK_RESTPOSE_WRAPPER2,
    LNK_DYNPOSE_WRAPPER1,
    LNK_DYNPOSE_WRAPPER2,
    LNK_FIBERS_ORIGIN,
    LNK_FIBERS_INSERTION,
    
    LNK_LAST,
  };

  //this must match combobox values sets in CreateOpDialog
  enum DEFORMATION_MODES
  {
    DEM_BLANCO,             //Implementation of Blanco's method
    DEM_SEPPLANES,          //Has precise separating planes between edges
    DEM_WARPING,            //Simple warping
  };

  enum FIBER_TEMPLATES
  {
    FT_PARALLEL = 0,
    FT_PENNATE,
    FT_CURVED,
    FT_FANNED,
    FT_RECTUS,
  };

  const static char* MUSCLEWRAPPER_LINK_NAMES[];  
  const static int MAX_WRAPPERS = 2;

protected:
  mafString m_RestPoseMuscleVmeName;                    //<name of VME with muscle geometry in the rest pose
  mafString m_RestPoseWrapperVmeNames[MAX_WRAPPERS];    //<name of VME with poly-line representing action lines of the muscle in the rest pose
  mafString m_WrapperVmeNames[MAX_WRAPPERS];            //<name of VME with poly-line representing action lines of the muscle in the current pose
  mafString m_OIVmeNames[2];                            //<name of VMEs with landmarks representing origin and insertion areas

  vtkPolyData* m_PolyData;            //<output polydata
  bool m_bNeedUpdate;                 //<true, if the deformation must be reexecuted
  bool m_bDoNotUpdate;                //<true, if the InternalUpdate routine should terminate immediately

  int m_DeformerType;                     //<deformation mode to be used (DEM_WARPING by the default)
  mafVME* m_CurVMEs[3 + 2*MAX_WRAPPERS];  //<VMEs currently associated with the wrapper
  vtkPolyData* m_OldCurves[2*MAX_WRAPPERS];

  int m_VisMode;    //<non-zero, if the output are fibers instead of deformed mesh
  int m_FbTemplate;   //<fiber template geometry
  int m_FbNumFib;     //<number of fibers
  int m_FbResolution; //<resolution
  int m_FbSmooth;		//<non-zero, if fibres should be smoothed
  int m_FbDebugShowTemplate;	//<non-zero, if template fibres should be displayed, but not mapped ones
public:
  static bool VMEAcceptMuscle(mafNode *node);
  static bool VMEAcceptWrapper(mafNode *node);  
  static bool VMEAcceptOIAreas(mafNode *node);
  
  /** Precess events coming from other objects */ 
  virtual void OnEvent(mafEventBase *maf_event);

  /** Copy the contents of another VME-Meter into this one. */
  virtual int DeepCopy(mafNode *a);

  /** Compare with another VME-Meter. */
  virtual bool Equals(mafVME *vme);

  /** Return the suggested pipe-typename for the visualization of this vme */
  virtual mafString GetVisualPipe() {
    return mafString("mafPipeSurface");
  };
  
  /**
  Set the Pose matrix of the VME. This function modifies the MatrixVector. You can
  set or get the Pose for a specified time. When setting, if the time does not exist
  the MatrixVector creates a new KeyMatrix on the fly. When getting, the matrix vector
  interpolates on the fly according to the matrix interpolator.*/
  virtual void SetMatrix(const mafMatrix &mat);

  /** Return the list of timestamps for this VME. Timestamps list is 
    obtained merging timestamps for matrices and VME items*/
  virtual void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes);

  /** return always false since (currently) the slicer is not an animated VME (position 
      is the same for all timestamps). */
  virtual bool IsAnimated();  
  
  /** Gets muscle VME in its rest pose */
  mafVME* GetMuscleVME_RP();

  /** Gets the first action line VME in its rest pose */
  mafVME* GetWrapper1VME_RP();
  
  /** Gets the second action line VME in its rest pose */
  mafVME* GetWrapper2VME_RP();  

  /** Gets the first action line VME in its current pose */
  mafVME* GetWrapper1VME();

  /** Gets the second action line VME in its current pose */
  mafVME* GetWrapper2VME();    

  /** Gets the origin area VME in its current pose */
  mafVME* GetFibersOriginVME();

  /** Gets the insertion area VME in its current pose */
  mafVME* GetFibersInsertionVME();

  /** Return pointer to material attribute. */
  mmaMaterial *GetMaterial();
   
protected:
  medVMEMuscleWrapper();
  virtual ~medVMEMuscleWrapper();
 
  virtual int InternalStore(mafStorageElement *parent);
  virtual int InternalRestore(mafStorageElement *node);
  
  /** this creates the Material attribute at the right time... */
  virtual int InternalInitialize();

  /** update the output data structure */
  virtual void InternalUpdate();

  /** Update the names for the meter's links. */
  void UpdateLinks();

  /** Internally used to create a new instance of the GUI.*/
  virtual mafGUI *CreateGui();
    
  /** Handles the click on one of select VME buttons. */
  virtual void OnSelectLink(mafEvent* ev);

  /** Creates a new polydata without duplicate vertices and edges that might be in the input. */
  vtkPolyData* FixPolyline(vtkPolyData* input);

  /** Returns true, if curves are identical (although they are different objects) */
  bool CheckCurves(vtkPolyData** pOldCurves, vtkPolyData** pNewCurves);

  /** Template to deform the muscle */
  template< class T >
  void DeformMuscle(vtkPolyData* pMuscle, vtkPolyData** pCurves);

  /** Generates fibers for the given muscle */
  void GenerateFibers(vtkPolyData* pMuscle);

  /** Creates points form landmark cloud vme, landmark, etc.
  N.B. the caller is responsible for deleting the returned object. */
  vtkPoints* CreatePointsFromVME(mafVME* vme);
private:
  medVMEMuscleWrapper(const medVMEMuscleWrapper&); // Not implemented
  void operator=(const medVMEMuscleWrapper&); // Not implemented
};
#endif
