/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewSliceLHPBuilder.h,v $
  Language:  C++
  Date:      $Date: 2006-06-27 14:24:00 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mafViewSliceLHPBuilder_H__
#define __mafViewSliceLHPBuilder_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafViewVTK.h"
#include "mafViewSlice.h"
#include <vector>

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;
class mafSceneNode;
class vtkActor2D;
class mafViewSlice;
class vtkTextMapper;

//----------------------------------------------------------------------------
// mafViewSlice :
//----------------------------------------------------------------------------
/** 
mafViewSlice is a View that visualize volume as slices and 
initialize the visual pipe according to the camera position that is passed through constructor
\sa mafViewVTK
*/
class mafViewSliceLHPBuilder: public mafViewSlice
{
public:
  mafViewSliceLHPBuilder(wxString label = "Slice", int camera_position = CAMERA_CT, bool show_axes = false, bool show_grid = false, bool show_ruler = false, int stereo = 0);
  virtual ~mafViewSliceLHPBuilder(); 

  mafTypeMacro(mafViewSliceLHPBuilder, mafViewSlice);

  virtual void Create();

  virtual void VmeCreatePipe(mafNode *vme);

  virtual void SetSlice(double origin[3]);

  virtual mafView*  Copy(mafObserver *Listener);

  /**Refresh text in CT View**/
  void UpdateText(int ID = 1);

  void UpdateListSurface(mafNode *node);

  /**Set color of the text in CT View**/
  void SetTextColor(double color[3]);
  //double* GetSliceCenter(); 

protected:

	vtkActor2D    *m_TextActor;
	vtkTextMapper	*m_TextMapper;
	wxString m_Text;
	double	 m_TextColor[3];
	//mafSceneNode* m_CurrentSurface;
	std::vector<mafSceneNode*> m_CurrentSurface;
};
#endif
