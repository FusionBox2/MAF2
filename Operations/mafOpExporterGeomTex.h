/*=========================================================================

Program: MAF2
Module: mafOpExporterGeomTex

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpExporterGeomTex_H__
#define __mafOpExporterGeomTex_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafNode;

//----------------------------------------------------------------------------
// mafOpExporterGeomTex :
//----------------------------------------------------------------------------
/** */
class MAF_EXPORT mafOpExporterGeomTex : public mafOp
{
public:
	mafOpExporterGeomTex(const mafString& label = _R("GeomTexExporter"));
	~mafOpExporterGeomTex();

	mafTypeMacro(mafOpExporterGeomTex, mafOp);

	mafOp* Copy();
	void OnEvent(mafEventBase *maf_event);

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode *node);

	/** Builds operation's interface. */
	void OpRun();

	/** Set the filename for the .stl to export */
	void SetFileName(const mafString& file_name) { m_File = file_name; };

	void ApplyABSMatrixOn() { m_ABSMatrixFlag = 1; };
	void ApplyABSMatrixOff() { m_ABSMatrixFlag = 0; };
	void SetApplyABSMatrix(int apply_matrix) { m_ABSMatrixFlag = apply_matrix; };

	void ExportAsBynaryOn() { m_Binary = 1; };
	void ExportAsBynaryOff() { m_Binary = 0; };
	void SetExportAsBynary(int binary_file) { m_Binary = binary_file; };

	/** Export the surface. */
	void ExportSurface();
	/** Export the texture. */
	void ExportTexture();

protected:
	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	void OpStop(int result);

	mafString  m_File;
	mafString  m_FileDir;
	int				 m_Binary;
	int				 m_ABSMatrixFlag;
};
#endif
