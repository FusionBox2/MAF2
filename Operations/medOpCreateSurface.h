#pragma once
//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------

#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMESurface;

//----------------------------------------------------------------------------
// medOpCreateSurface :
//----------------------------------------------------------------------------
/**Convert a Parametric Surface into a Normal Surface*/
class MED_OPERATION_EXPORT medOpCreateSurface: public mafOp
{
public:

	medOpCreateSurface(const mafString& label = _R("Create Surface"));
	~medOpCreateSurface() override; 

	mafTypeMacroN(medOpCreateSurface);

	mafOp* Copy() override;

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode* vme) override;

	/** Builds operation's interface by calling CreateOpDialog() method. */
	void OpRun() override;

	/** Execute the operation. */
	void OpDo() override;

protected:

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	void OpStop(int result) override;
};
