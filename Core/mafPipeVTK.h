#pragma once

#include "mafDecl.h" // for MINID
#include "mafPipe.h"
#include "vtkMAFAssembly.h"
#include "mafVME.h"
#include "vtkRenderer.h"

class MAF_EXPORT mafPipeVTK : public mafPipe
{
public:
	mafTypeMacro(mafPipeVTK, mafPipe)

	mafPipeVTK();

	~mafPipeVTK() override;

	void Create(mafNode* node, vtkRenderer** ren, vtkMAFAssembly** assembly);

	void Create(mafNode* node, mafView* view) override;

protected:
	virtual vtkMAFAssembly* GetAssemblyFront() { return m_AssemblyFront; }

	virtual vtkMAFAssembly* GetAssemblyBack() { return m_AssemblyBack; }

	mafVME* m_Vme = nullptr;//it is for compatibility purposes, will be removed later;
	bool m_Selected = false; ///< Flag used to say if the rendered VME is selected.
	vtkMAFAssembly* m_AssemblyFront = nullptr; ///< Assembly used to contain the actor in the front layer
	vtkMAFAssembly* m_AssemblyBack = nullptr;  ///< Assembly used to contain the actor in the background layer
	vtkMAFAssembly* m_AlwaysVisibleAssembly = nullptr;
	vtkRenderer* m_RenFront = nullptr; ///< Renderer associated to the front layer
	vtkRenderer* m_RenBack = nullptr;  ///< Renderer associated to the background layer
	vtkRenderer* m_AlwaysVisibleRenderer = nullptr; /// < Renderer used to superimpose utility stuff to main render window
};
