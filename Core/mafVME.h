/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVME.h,v $
  Language:  C++
  Date:      $Date: 2009-12-17 11:44:31 $
  Version:   $Revision: 1.37.2.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafVME_h
#define __mafVME_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafNode.h"
#include "mafVMEOutput.h"
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafMatrix;
class mafTransform;
class mafInteractor;

class mafDataPipe;
class mafMatrixPipe;
class mafAbsMatrixPipe;

/** mafVME - a tree node implementation with an output with a pose matrix and a VTK dataset.
  mafVME is a node for sciViz purposes. It features a procedural core generating an output 
  data structure storing a pose matrix and a VTK dataset. The mafVME generates also an absolute
  pose matrix, a matrix representing the pose of the VME in the world reference system.
  @sa mafNode mafVMEIterator

  @todo
  - implement the GetMTime() function: it's used by pipeline to trigger the update
  - Change the SetParent to update the AbsMatrixPipe connections
  - fix the VME_TIME_SET issuing and propagation
  - implement Update() function
  */
class MAF_EXPORT mafVME : public mafNode
{
public:
  mafAbstractTypeMacro(mafVME,mafNode);

  enum VME_VISUAL_MODE
  {
    DEFAULT_VISUAL_MODE = 0,
    NO_DATA_VISUAL_MODE
  };

  //typedef std::vector<mafTimeStamp> std::vector<mafTimeStamp>;

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);// const;

  /** 
    return the parent VME Node. Notice that a VME can only reparented 
    under another VME, not to other kind of nodes! */
  mafVME *GetParent() const;

  /**
    This function set the parent for this Node. It has been redefined to update 
    AbsMatrixPipe input frame. */
  virtual int SetParent(mafNode *parent);

  /**
    Copy the contents of another VME into this one. Notice that subtrees
    are not copied, i.e. copy is not recursive!
    Concrete class should reimplement this function to verify admitted
    conversion. */
  virtual int DeepCopy(mafNode *a);

  /** 
    perform a copy by simply referencing the copied VME's data array. 
    Beware: This can allow to save memory when doing special tasks, but
    can be very dangerous making one of the VME inconsistent. Some VMEs
    do not support such a function! */  
  virtual int ShallowCopy(mafVME *a);

  /** Compare two VMEs. The specific comparison must be implemented by subclasses. */
  virtual bool Equals(mafVME *vme);

  /**
   Set the time for this VME (not for the whole tree). Normally time 
   of the tree is set by sending an event with id VME_TIME_SET */
  virtual void SetTimeStamp(mafTimeStamp t);
  virtual mafTimeStamp GetTimeStamp();
  
  /**
    Set/Get CurrentTime for this VME and all subtree. Normally time 
    of the tree is set by sending an event with id VME_TIME_SET*/
  void SetTreeTime(mafTimeStamp t);

  /**
    Set the Pose matrix of the VME. This function modifies pose matrix of the VME.
    The pose is modified for the given timestamp, if VME supports 4D pose (e.g. 
    MatrixVector of VME-Generic) the output matrix is interpolated among set
    key matrices.*/
  virtual void SetMatrix(const mafMatrix &mat)=0;

  /** set the pose matrix for a specific time stamp (ignore the matrix internal timestamp!) */
  void SetPose(const mafMatrix &mat,mafTimeStamp t);

  /** 
    Set the pose for this VME This function modifies pose matrix of the VME.
    The pose is modified for the given timestamp, if VME supports 4D pose (e.g. 
    MatrixVector of VME-Generic) the output matrix is interpolated among set
    key matrices.*/
  void SetPose(double x,double y,double z,double rx,double ry,double rz, mafTimeStamp t);

  /** 
    Set the pose for this VME This function modifies pose matrix of the VME.
    The pose is modified for the given timestamp, if VME supports 4D pose (e.g. 
    MatrixVector of VME-Generic) the output matrix is interpolated among set
    key matrices.*/
  void SetPose(double xyz[3],double rxyz[3], mafTimeStamp t);

  /** apply a matrix to the VME pose matrix */
  void ApplyMatrix(const mafMatrix &matrix,int premultiply,mafTimeStamp t=-1);
  
  /** Set the global pose of this VME for the given time "t". This function usually modifies the MatrixVector. */
  void SetAbsPose(double x,double y,double z,double rx,double ry,double rz, mafTimeStamp t=-1);
  /** Set the global pose of this VME for the given time "t". This function usually modifies the MatrixVector. */
  void SetAbsPose(double xyz[3],double rxyz[3], mafTimeStamp t=-1);
  /** Set the global pose of this VME for the given time "t". This function usually modifies the MatrixVector. */
  void SetAbsMatrix(const mafMatrix &matrix,mafTimeStamp t);
  /** Set the global pose of this VME for the current time. This function usually modifies the MatrixVector. */
  void SetAbsMatrix(const mafMatrix &matrix);

  /** apply a matrix to the VME abs pose matrix */
  void ApplyAbsMatrix(const mafMatrix &matrix,int premultiply,mafTimeStamp t=-1);
 
  /**
    return true if the VME can be reparented under the specified node */
  virtual bool CanReparentTo(mafNode *parent);
  
  // to be revised
	/** Set auxiliary reference system and its name*/
	//int SetAuxiliaryRefSys(mflTransform *AuxRefSys, const char *RefSysName, int type = MFL_LOCAL_FRAME_TAG);
  //int SetAuxiliaryRefSys(vtkMatrix4x4 *AuxRefSys, const char *RefSysName, int type = MFL_LOCAL_FRAME_TAG);
  
  // to be revises
	/** Get auxiliary reference system from its name*/
  //int GetAuxiliaryRefSys(mflTransform *AuxRefSys, const char *RefSysName, int type = MFL_LOCAL_FRAME_TAG);
  //int GetAuxiliaryRefSys(vtkMatrix4x4 *AuxRefSys, const char *RefSysName, int type = MFL_LOCAL_FRAME_TAG);
  
  /** return the matrix pipe object, i.e. the source of the output matrix. */
  mafMatrixPipe *GetMatrixPipe() {return m_MatrixPipe;}
  
  /** return the matrix pipe used for computing the AbsMatrix.*/
  mafAbsMatrixPipe *GetAbsMatrixPipe() {return m_AbsMatrixPipe;}
  
  /** return the data pipe object, i.e. the source of the output dataset. */
  mafDataPipe *GetDataPipe() {return m_DataPipe;}

  /**
    this function makes the current data pointer to point the right output
    data, usually the DataPipe output data but subclasses can redefine this
    function. By default if no data pipe is defined, current data is set to
    NULL.*/
  virtual void Update();
  
  /**
    Return the list of timestamps for this VME. Timestamps list is 
    obtained merging timestamps for matrices and VME items*/
  virtual void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes)=0;

  /** Set the time bounds for the time varying VME (base VME is not time varying).*/
  virtual void GetLocalTimeBounds(mafTimeStamp tbounds[2]) {tbounds[0] = m_CurrentTime; tbounds[1] = m_CurrentTime;};
  
	/**
    Return the list of timestamps considering all parents timestamps. Timestamps list is
    obtained merging timestamps for matrices and VME items*/
	virtual void GetAbsTimeStamps(std::vector<mafTimeStamp> &kframes);
  
	/**
    Return the list of timestamps for the full sub tree. Timestamps list is
    obtained merging timestamps for matrices and VME items*/
  virtual void GetTimeStamps(std::vector<mafTimeStamp> &kframes);  
  
  /** Return the number of time stamps in the whole tree*/
  int GetNumberOfTimeStamps();
  
  /** Return the number of time stamps local to the vme*/
  int GetNumberOfLocalTimeStamps();
  
  /** Return true if the number of local time stamps is > 1*/
  virtual bool IsAnimated();

  /** Return true if the data associated with the VME is present and updated at the current time.*/
  virtual bool IsDataAvailable();
  
  /** Set the crypting status for the vme. */
  void SetCrypting(int crypting);
  
  /** Get the crypting status of the vme. */
  int GetCrypting();

  /** return a pointer to the output data structure */
  virtual mafVMEOutput *GetOutput() {return m_Output;}

  /** process events coming from other components */
  virtual void OnEvent(mafEventBase *maf_event);

  /** Return the suggested pipe-typename for the visualization of this vme */
  virtual mafString GetVisualPipe() {return mafString("");};

  /** Set the mode with which render the VME: DEFAULT_VISUAL_MODE or NO_DATA_VISUAL_MODE.
  Setting the visual mode to default will produce also the reset of the m_EditingVisualPipe
  to the empty string. This because the editing visual pipe is set dynamically by the class that
  use this feature.*/
  void SetVisualMode(int mode);

  /** Get the mode with which render the VME: DEFAULT_VISUAL_MODE or NO_DATA_VISUAL_MODE*/
  int GetVisualMode() {return m_VisualMode;};

  /** 
    return the interactor assigned as a behavior to this VME. This is the 
    interactor to which events coming from input devices are sent when 
    user pick this VME. Responsible of this routing is the PER (@sa mafInteractorPER) */
  mafInteractor *GetBehavior() {return m_Behavior;}

  /** set the interactor representing the behavior of this VME. @sa GetBehavior() */
  void SetBehavior(mafInteractor *bh) {m_Behavior=bh;};
  
protected:
  mafVME(); // to be allocated with New()
  virtual ~mafVME(); // to be deleted with Delete()

  virtual int InternalStore(mafStorageElement *parent);
  virtual int InternalRestore(mafStorageElement *node);

  /** used to initialize the AbsMatrixPipe */
  virtual int InternalInitialize();

  /** called to prepare the update of output */
  virtual void InternalPreUpdate() {}

  /** update the output data structure */
  virtual void InternalUpdate() {}

  /** 
    Set the output and connect it to the VME. This is automatically called
    by GetOutput() of specific VME's */
  void SetOutput(mafVMEOutput *output);
  
  /** Set/Get the data pipe object, i.e. the source of the output dataset. */
  virtual int SetDataPipe(mafDataPipe *dpipe);

  /** Set the matrix pipe object, i.e. the source of the output matrix. */
  int SetMatrixPipe(mafMatrixPipe *pipe);

  /** Set the abs matrix pipe object, i.e. the source of the output abs matrix. */
  void SetAbsMatrixPipe(mafAbsMatrixPipe *pipe);

  /** Create GUI for the VME */
  virtual mafGUI  *CreateGui();

  mafAutoPointer<mafDataPipe>       m_DataPipe;
  mafAutoPointer<mafMatrixPipe>     m_MatrixPipe;
  mafAutoPointer<mafAbsMatrixPipe>  m_AbsMatrixPipe;

  mafVMEOutput*   m_Output;       ///< the data structure storing the output of this VME
  mafTimeStamp    m_CurrentTime;  ///< the time parameter for generation of the output
  int             m_Crypting;     ///< enable flag for this VME
  
  mafInteractor*  m_Behavior;     ///< the interactor representing the behavior of this VME

  int             m_VisualMode; ///< Store the visual mode to allow the visual pipe to choose the right visual pipe to render the VME
  
private:
  mafVME(const mafVME&); // Not implemented
  void operator=(const mafVME&); // Not implemented
  virtual bool Equals(mafNode *node); // not accessible from other classes
};

#endif
