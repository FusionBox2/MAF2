/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpFlipNormals.h,v $
Language:  C++
Date:      $Date: 2010-02-22 10:09:26 $
Version:   $Revision: 1.4.2.2 $
Authors:   Matteo Giacomoni - Daniele Giunchi
==========================================================================
Copyright (c) 2002/2007
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)

MafMedical Library use license agreement

The software named MafMedical Library and any accompanying documentation, 
manuals or data (hereafter collectively "SOFTWARE") is property of the SCS s.r.l.
This is an open-source copyright as follows:
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution.
* Modified source versions must be plainly marked as such, and must not be misrepresented 
as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

MafMedical is partially based on OpenMAF.
=========================================================================*/

#ifndef __medOpFlipNormals_H__
#define __medOpFlipNormals_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"
#include "vtkIdList.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

class mafRWI;
class mafNode;
class mafGUIDialog;
class mafInteractorSelectCell;

class vtkActor;
class vtkPolyData;
class vtkMAFCellsFilter;
class vtkCellCenters;
class vtkArrowSource;
class vtkPolyDataMapper;
class vtkGlyph3D;

/**
class name : medOpFlipNormals

Perform cells selection and flip of the normals.
CTRL modifier must be used in order to select cells in render window while
performing mouse dragging.
This operation assumes that input polygonal data is made of triangles only.
*/
class medOpFlipNormals: public mafOp
{
public:

  /** constructor. */
	medOpFlipNormals(wxString label = "Flip Normals");

  /** destructor. */
	~medOpFlipNormals(); 

  /** RTTI macro */
	mafTypeMacro(medOpFlipNormals, mafOp);

	/*virtual*/ mafOp* Copy();

  /** Precess events coming from other objects */
	/*virtual*/ void OnEvent(mafEventBase *maf_event);

	/** Return true for the acceptable vme type. */
	/*virtual*/ bool Accept(mafNode* vme);

	/** Builds operation's interface by calling CreateOpDialog() method. */
	/*virtual*/ void OpRun();

	/** Execute the operation. */
	/*virtual*/ void OpDo();

	/** Makes the undo for the operation. */
	/*virtual*/ void OpUndo();

	/** set the seed ie the cell originating the selection area */
	void SetSeed(vtkIdType cellSeed);


  /** get the seed ie the cell originating the selection area */
  vtkIdType GetSeed(){return m_CellSeed;};

	/** Set the selection region radius */
	double GetDiameter() const { return m_Diameter; };

  /** Get the selection region radius */
	void SetDiameter(double val) {m_Diameter = val;};

	/** Mark cells */
	void MarkCells();

	/** performs flip normals */
	void FlipNormals();

protected:

	double GetCellsDistance(vtkIdType c1,vtkIdType c2);

	void FindTriangleCellCenter(vtkIdType id, double center[3]);

	// used to support algorithm execution
	vtkIdList						*m_NeighborCellPointIds;
	int									*m_VisitedCells;
	vtkIdType						*m_VisitedPoints;
	int									 m_RegionNumber;
	vtkIdType						 m_PointNumber;    
	vtkIdType						 m_CellSeed;
	vtkIdList						*m_Wave;
	vtkIdList						*m_Wave2;
	double							 m_Diameter;

	vtkGlyph3D					*m_NormalGlyph;
	vtkPolyDataMapper		*m_NormalMapper;
	vtkActor						*m_NormalActor;
	vtkCellCenters			*m_CenterPointsFilter;
	vtkArrowSource			*m_NormalArrow;
	vtkPolyData					*m_Centers;

	double							 m_MinBrushSize;
	double							 m_MaxBrushMSize;
	int                  m_UnselectCells;

	mafGUIDialog						*m_Dialog;
	mafRWI							*m_Rwi;
	vtkPolyDataMapper		*m_PolydataMapper;
	vtkActor						*m_PolydataActor;
	mafInteractorSelectCell				*m_SelectCellInteractor;
	vtkMAFCellsFilter			*m_CellFilter;
	vtkPolyData					*m_Mesh;

	vtkPolyData					*m_ResultPolydata;
	vtkPolyData					*m_OriginalPolydata;

	/** 
	Builds operation's interface and visualization pipeline. */
	void CreateOpDialog();

	/** 
	Remove operation's interface. */
	void DeleteOpDialog();

	void CreateNormalsPipe();
	void CreateSurfacePipeline();
	void TraverseMeshAndMark(double radius);
	void MarkCellsInRadius(double radius);
	void InitializeMesh();
	void ModifyAllNormal();

  friend class medOpFlipNormalsTest;

};
#endif
