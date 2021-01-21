/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medVMEMuscleWrapper.h,v $
  Language:  C++
  Date:      $Date: 2009-05-14 14:07:35 $
  Version:   $Revision: 1.1.2.1 $
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
#include "mafTransform.h"
#include <wx/listctrl.h>

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
    ID_INPUTMODE = Superclass::ID_LAST,
    ID_USE_REFSYS,
    
    ID_SELECT_RP,
    ID_SELECT_CP,
    ID_SELECT_RP_REFSYS_LINK,
    ID_SELECT_CP_REFSYS_LINK,
    ID_ADDWRAPPER,
    ID_LIST_WRAPPERS,
    ID_REMOVEWRAPPER, 

    ID_RESTPOSE_MUSCLE_LINK,
    ID_RESTPOSE_WRAPPER_LINK,
    ID_CURRENTPOSE_WRAPPER_LINK,
    ID_RESTPOSE_REFSYS_LINK,    
    ID_CURRENTPOSE_REFSYS_LINK,    
    ID_FIBERS_ORIGIN_LINK,
    ID_FIBERS_INSERTION_LINK,    

    ID_GENERATE_FIBERS,
    ID_FIBERS_TEMPLATE,     //template type
    ID_FIBERS_RESOLUTION,   //resolution
    ID_FIBERS_NUMFIB,       //number of fibers  
    ID_FIBERS_THICKNESS,    //how thick a fiber should be

    ID_FIBERS_SMOOTH,	//smooth computed fibres
    ID_FIBERS_SMOOTH_STEPS,   //number of smooth steps
    ID_FIBERS_SMOOTH_WEIGHT,   //smoothing weight
    ID_FIBERS_DEBUG_SHOWTEMPLATE,	//show template but not projection
    ID_FIBERS_DEBUG_SHOWFITTING,	//show fitting process
    ID_FIBERS_DEBUG_SHOWFITTINGRES,	//show fitting process

    ID_LAST,
  };

  enum MUSCLEWRAPPER_LINK_IDS
  {
    LNK_RESTPOSE_MUSCLE = 0,    
    LNK_RESTPOSE_WRAPPERx,    
    LNK_DYNPOSE_WRAPPERx,    
    LNK_RESTPOSE_REFSYSx,    
    LNK_DYNPOSE_REFSYSx,
    LNK_FIBERS_ORIGIN,
    LNK_FIBERS_INSERTION,    
    
    LNK_LAST,
  };

  //this must match combobox values sets in CreateOpDialog
  enum FIBER_TEMPLATES
  {
    FT_PARALLEL = 0,
    FT_PENNATE,
    FT_CURVED,
    FT_FANNED,
    FT_RECTUS,
  };

  const static char* MUSCLEWRAPPER_LINK_NAMES[];    
protected:
  typedef struct WRAPPER_ITEM 
  {    
    mafVME* pVmeRP_CP[2];           //<VME with poly-line representing action lines of the muscle in the rest/current pose
    mafVME* pVmeRefSys_RP_CP[2];    //<VME with reference system
    unsigned long VMECheckSums[2];  //<checksums of these VMEs to prevent recalculation of everything
    
    vtkPolyData* pCurves[2];        //<the refined curves (valid as long as VMEChecksums of pVmeRP_CP are correct)
    double RefSysOrigin[2][3];      //<and origins of their associated referenced systems (ignored, if pVmeRefSys_RP_CP is NULL)
    bool RefSysOriginValid[2];      //<false, if the coordinates are not available

    WRAPPER_ITEM* pNext;
    WRAPPER_ITEM* pLast;
  };
  
  mafTransform *m_Transform;      ///< pose matrix for the output (taken from input muscle)

  mafVME* m_RefSysVme[2];         //<VMEs with the reference system
  mafString m_RefSysVmeName[2];   //<and their name to be shown in GUI  

  mafVME* m_MuscleVme;            //<the VME with muscle geometry in the rest pose  
  mafString m_MuscleVmeName;      //<and its name to be shown in GUI  

  mafVME* m_WrappersVme[2];        //<wrapper VMEs for AddWrapper command
  mafString m_WrappersVmeName[2];  //<and their names

  mafVME* m_OIVME[2];             //<VMEs with landmark(s) denoting origin and insertion areas
  mafString m_OIVMEName[2];       //<and their names
  int m_nWrappers;                //<number of wrappers in m_pWrappers
  WRAPPER_ITEM* m_pWrappers;      //<list of wrappers
  bool m_bLinksRestored;          //<true, if links has been restored and the changes can be saved
  
    
  vtkPolyData* m_PolyData;        //<output polydata
  bool m_bNeedUpdate;             //<true, if the deformation must be reexecuted
  bool m_bDoNotUpdate;            //<true, if the InternalUpdate routine should terminate immediately
  
  int m_InputMode;    //<0 = simple, 1 = advanced
  int m_VisMode;      //<non-zero, if the output are fibers instead of deformed mesh
  int m_UseRefSys;    //<non-zero, if the reference systems should be used whenever applicable
  int m_FbTemplate;   //<fiber template geometry
  int m_FbNumFib;     //<number of fibers
  int m_FbResolution; //<resolution
  double m_FbThickness; //<thickness of fibre
  int m_FbSmooth;		  //<non-zero, if fibres should be smoothed
  int m_FbSmoothSteps;  //<number of smoothing steps (higher means more smoothed)
  double m_FbSmoothWeight; //<smoothing weight (lower means more smoothed)
  int m_FbDebugShowTemplate;	//<non-zero, if template fibres should be displayed, but not mapped ones
  int m_FbDebugShowFitting; //<non-zero, if fitting process should be visualized (debug)
  int m_FbDebugShowFittingRes;

#pragma region GUI
  wxStaticText* m_SmLabel1;
  wxTextCtrl* m_SmStepsCtrl;
  wxStaticText* m_SmLabel2;
  wxTextCtrl* m_SmWeightCtrl;  
  wxStaticText* m_LabelRP;
  wxTextCtrl* m_RPNameCtrl;
  wxButton* m_BttnSelRP;    
  wxButton* m_BttnAddWrapper;
  wxListCtrl* m_WrappersCtrl;  
  wxButton* m_BttnRemoveWrapper;
  wxStaticText* m_LabelRP_RS;
  wxTextCtrl* m_RPRefSysVmeCtrl;
  wxButton* m_BttnSelectRPRefSys;
#pragma endregion GUI
public:
  static bool VMEAcceptMuscle(mafNode *node);
  static bool VMEAcceptWrapper(mafNode *node);  
  static bool VMEAcceptOIAreas(mafNode *node);
  static bool VMEAcceptRefSys(mafNode *node);
  
  /** Precess events coming from other objects */ 
  virtual void OnEvent(mafEventBase *maf_event);

  /** Copy the contents of another VME-Meter into this one. */
  virtual int DeepCopy(mafNode *a);

  /** Compare with another VME-Meter. */
  virtual bool Equals(mafVME *vme);

  /** Return the suggested pipe-typename for the visualization of this vme */
  virtual mafString GetVisualPipe() {
    return mafString(_R("mafPipeSurface"));
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

  /** Gets the nIndex action line VME in its rest pose */
  mafVME* GetWrapperVME_RP(int nIndex);
  
  /** Gets the nIndex action line VME in its current pose */
  mafVME* GetWrapperVME(int nIndex);
  
  /** Gets the reference system VME for the nIndex action line in its rest pose. */
  mafVME* GetWrapperRefSysVME_RP(int nIndex);

  /** Gets the reference system VME for the nIndex action line in its current pose. */
  mafVME* GetWrapperRefSysVME(int nIndex);

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

  /** Internally used to create a new instance of the GUI.*/
  virtual mafGUI *CreateGui();
      
  /** Shows dialog (with the message in title) where the user selects vme.
  The VME that can be selected are defined by accept_callback.
  If no VME is selected, the routine returns false, otherwise it returns
  reference to the VME, its name and updates GUI */  
  bool SelectVme(mafString title, long accept_callback,
    mafVME*& pOutVME, mafString& szOutVmeName);

  /** Updates the visibility (etc) of GUI controls */
  void UpdateControls();

  /** Creates a new polydata without duplicate vertices and edges that might be in the input. */
  vtkPolyData* FixPolyline(vtkPolyData* input);
  
  /** Gets the origin of the specified vme.
  Returns false, if the vme is invalid. */
  bool GetRefSysVMEOrigin(mafVME* vme, double* origin);

  /** Deforms the muscle according to existing wrappers.
  It also invokes the generation of fibers, if required. */  
  void DeformMuscle(vtkPolyData* pMuscle);

  /** Generates fibers for the given muscle */
  void GenerateFibers(vtkPolyData* pMuscle);

  /** Creates points form landmark cloud vme, landmark, etc.
  N.B. the caller is responsible for deleting the returned object. */
  vtkPoints* CreatePointsFromVME(mafVME* vme);

  /** Compute checksum for VTK polydata. */
  unsigned long ComputeCheckSum(vtkPolyData* pPoly);

  /** Adds a new wrapper into the list of wrappers and GUI list.
  pxP_RS denotes reference systems used for corresponding wrappers (optional). */
  void AddWrapper(mafVME* pRP, mafVME* pCP, mafVME* pRP_RS = NULL, mafVME* pCP_RS = NULL);

  /** //Adds a new wrapper into the list of wrappers and GUI list */
  void AddWrapper(WRAPPER_ITEM* pItem);

  /** Remove wrapper from the GUI and releases its memory */
  void RemoveWrapper(int nIndex);

  /** Removes all wrappers from the list (NOT FROM GUI!) */
  void DeleteAllWrappers();

  /** Set a new link for the given vme. 
  The link have name with prefix from the link table at index nLinkNameId (see LNK_ enums)
  and suffix nPosId. If nPosId is -1, no suffix is specified  */
  void StoreMeterLink(mafVME* vme, int nLinkNameId, int nPosId = -1);

  /** Restore vme with the specified link. 
  Returns NULL, if there is no VME. IMPORTANT: removes vme from the Links list */
  mafVME* RestoreMeterLink(int nLinkNameId, int nPosId = -1);

  /** Stores all meter links into Links list.
  N.B. This method is supposed to be call from InternalStore*/
  void StoreMeterLinks();

  /** Restore all meter links from Links list. 
  N.B. this method is supposed to be called only after all VMEs were restored,
  i.e, it cannot be called from InternalRestore */
  void RestoreMeterLinks();

  /** Sets the new time for the given vme and ALL vmes linked to it */
  void SetVmeTimeStamp(mafVME* vme, double t);

  /** Transform the given inPoints having inTransform matrix into
  outPoints that have outTransform matrix (i.e., transforms coordinates
  from one reference system into another one. */
  void TransformPoints(vtkPoints* inPoints, vtkPoints* outPoints, 
    const mafMatrix* inTransform, const mafMatrix* outTransform);

  /** Transform the coordinates of the inoutPoints that are given in the 
  reference system described by inTransform into the coordinates in the
  output reference system (i.e., output of this VME) */
  inline void TransformPoints(vtkPoints* inoutPoints, const mafMatrix* inTransform){
    TransformPoints(inoutPoints, inoutPoints, inTransform,
      &GetOutput()->GetAbsTransform()->GetMatrix());
  }
private:
  medVMEMuscleWrapper(const medVMEMuscleWrapper&); // Not implemented
  void operator=(const medVMEMuscleWrapper&); // Not implemented
};
#endif
