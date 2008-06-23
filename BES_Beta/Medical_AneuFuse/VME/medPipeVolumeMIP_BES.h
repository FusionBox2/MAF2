/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medPipeVolumeMIP_BES.h,v $
  Language:  C++
  Date:      $Date: 2008-06-23 16:38:40 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medPipeVolumeMIP_BES_H__
#define __medPipeVolumeMIP_BES_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafPipe.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class mafLODActor;
class vtkOutlineCornerFilter;
class vtkPolyDataMapper;
class vtkOutlineSource;
class vtkImageCast;
class vtkPiecewiseFunction;
class vtkVolumeProperty;
class vtkVolumeRayCastMIPFunction;
class vtkMAFVolumeRayCastMapper;
class vtkActor;
class vtkLODProp3D;
class vtkLookupTable;
class vtkVolume;
class vtkImageResample;
class vtkLookupTable;
class vtkImageCast;
class vtkColorTransferFunction;
//----------------------------------------------------------------------------
// medPipeVolumeMIP_BES :
//----------------------------------------------------------------------------
class medPipeVolumeMIP_BES : public mafPipe
{
public:
  mafTypeMacro(medPipeVolumeMIP_BES,mafPipe);

           medPipeVolumeMIP_BES();
  virtual ~medPipeVolumeMIP_BES();

  /** process events coming from gui */
  virtual void OnEvent(mafEventBase *maf_event);

  /** IDs for the GUI */
  enum PIPE_VOLUME_MIP_WIDGET_ID
  {
    ID_RESAMPLE_FACTOR = Superclass::ID_LAST,
    ID_LUT_CHOOSER,
    ID_LAST
  };

  virtual void Create(mafSceneNode *n);
  virtual void Select(bool select); 

  void SetResampleFactor(double value);
  double GetResampleFactor(){return m_ResampleFactor;};

protected:
  /** 
  Given a color LUT, generate color transfer function and opacity transfer function*/
  void UpdateMIPFromLUT();

  virtual mmgGui  *CreateGui();

  double					  m_ResampleFactor;
  vtkImageCast                *m_Caster;
  vtkImageResample			  *m_ResampleFilter;	
  vtkPiecewiseFunction        *m_OpacityTransferFunction;
  vtkColorTransferFunction    *m_ColorTransferFunction;
  vtkVolumeProperty           *m_VolumeProperty;
  vtkVolumeRayCastMIPFunction *m_MIPFunction;
  vtkMAFVolumeRayCastMapper      *m_VolumeMapper;
  vtkVolume					  *m_Volume;
  vtkLookupTable      *m_ColorLUT;

	/** For vtkRectilinearGrid */
	vtkOutlineSource       *m_Box;
	vtkPolyDataMapper	     *m_Mapper;
	mafLODActor            *m_Actor;
	vtkOutlineCornerFilter *m_OutlineBox;

};
#endif // __medPipeVolumeMIP_BES_H__
