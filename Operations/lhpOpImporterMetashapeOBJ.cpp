#include "lhpOpImporterMetashapeOBJ.h"
#include "wx/busyinfo.h"

#include "mafDecl.h"
#include "mafTagItem.h"
#include "mafTagArray.h"
#include "mafVME.h"
#include "mafVMESurface.h"
#include "vtkSmartPointer.h"
#include "mafFilesDirs.h"

#include "vtkMetashapeOBJReader.h"
#include "vtkPolyData.h"

#include <fstream>

lhpOpImporterMetashapeOBJ::lhpOpImporterMetashapeOBJ(const mafString& label) : Superclass(label)
{
	m_OpType = OPTYPE_IMPORTER;
	m_Canundo = true;
	m_FileDir = _R("");//mafGetApplicationDirectory().c_str();
}

lhpOpImporterMetashapeOBJ::~lhpOpImporterMetashapeOBJ() = default;

bool lhpOpImporterMetashapeOBJ::Accept(mafNode* node)
{
	return true;
}

mafOp* lhpOpImporterMetashapeOBJ::Copy()
{
	auto cp = new lhpOpImporterMetashapeOBJ(GetLabel());
	cp->m_Files = m_Files;
	return cp;
}

void lhpOpImporterMetashapeOBJ::OpRun()
{
	if (!m_TestMode && m_Files.empty())
	{
		m_Files = mafGetOpenMultiFiles(m_FileDir, _R("Wavefront (*.obj)|*.obj"));
	}

	int result = OP_RUN_CANCEL;

	if (!m_Files.empty())
	{
		result = OP_RUN_OK;
		ImportOBJ();
	}

	{ mafEvent evUnq(this, result); InvokeEvent(evUnq); }
}

void lhpOpImporterMetashapeOBJ::OpDo()
{
	for (auto& obj : m_ImportedOBJs)
	{
		if (obj)
		{
			mafNode::ReparentTo(obj, GetInput().get());
		}
	}
	{ mafEvent evUnq(this, CAMERA_UPDATE); InvokeEvent(evUnq); }
}

void lhpOpImporterMetashapeOBJ::OpUndo()
{
	for (auto& obj : m_ImportedOBJs)
	{
		if (obj)
		{
			mafNode::ReparentTo(obj, nullptr);
		}
	}
	{ mafEvent evUnq(this, CAMERA_UPDATE); InvokeEvent(evUnq); }
}

void lhpOpImporterMetashapeOBJ::ImportOBJ()
{
	if (!m_TestMode)
	{
		wxBusyInfo wait("Loading file: ...");
	}

	m_ImportedOBJs.clear();

	for (const auto& fn : m_Files)
	{
		vtkNew<vtkMetashapeOBJReader> reader;
		{ mafEvent evUnq(this, BIND_TO_PROGRESSBAR); evUnq.SetVtkObj(reader); InvokeEvent(evUnq); }
		reader->SetFileName(fn.GetCStr());
		reader->Update();

		mafString path, name, ext;
		mafSplitPath(fn, &path, &name, &ext);

		auto importedOBJ = mafVMESurface::NewSPtr();
		importedOBJ->SetName(name);
		importedOBJ->SetDataByDetaching(reader->GetOutput(), 0);

		mafTagItem tag_Nature;
		tag_Nature.SetName(_R("VME_NATURE"));
		tag_Nature.SetValue(_R("NATURAL"));
		importedOBJ->GetTagArray()->SetTag(tag_Nature);

		m_ImportedOBJs.push_back(importedOBJ);
	}
}

void lhpOpImporterMetashapeOBJ::SetFileName(const mafString& file_name)
{
	m_Files.assign(1, file_name);
}

const std::vector<std::shared_ptr<mafVMESurface> >& lhpOpImporterMetashapeOBJ::GetImportedOBJ() const
{
	return m_ImportedOBJs;
}
