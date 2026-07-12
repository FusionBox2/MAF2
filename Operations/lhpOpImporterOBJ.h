#pragma once

#include "ftkConfigure.h"

#include "Base/mfString.h"

#include "mafOp.h"

#include <vector>

class mafVME;
class mafVMESurface;
class mafEvent;

class MAF_EXPORT lhpOpImporterOBJ : public mafOp
{
public:
	lhpOpImporterOBJ(const mafString& label = _R("OBJImporter"));
	~lhpOpImporterOBJ() override;

	mafTypeMacroN(lhpOpImporterOBJ);

	mafOp* Copy() override;

	bool Accept(mafNode* node) override;

	void SetFileName(const mafString& file_name);

	void OpRun() override;

	void OpUndo() override;

	void OpDo() override;

	void ImportOBJ();

	const std::vector<std::shared_ptr<mafVMESurface> >& GetImportedOBJ() const;

protected:

	std::vector<std::shared_ptr<mafVMESurface> > m_ImportedOBJs;
	std::vector<mafString> m_Files;
	mafString m_FileDir;
};
