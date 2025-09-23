#include "mafVMEScalarMatrix.h"
#include "mafGUI.h"

#include "ftk/IO/StorageElement.h"
#include "mafVME.h"
#include "mafVMEOutputScalarMatrix.h"

#include "mafDataPipeInterpolatorScalarMatrix.h"
#include "mafDataVector.h"
#include "mafVMEItemScalarMatrix.h"

//-------------------------------------------------------------------------
mafVMEScalarMatrix::mafVMEScalarMatrix()
//-------------------------------------------------------------------------
{
	m_DataVector = std::unique_ptr<mafDataVector>();
	m_DataVector->SetItemTypeName(mafVMEItemScalarMatrix::GetStaticTypeName());
	m_DataVector->SetListener(this);
	SetDataPipe(mafDataPipeInterpolatorScalarMatrix::NewSPtr()); // interpolator data pipe

	m_ScalarArrayOrientationInMatrix = ROWS;
	m_Xtype = USE_TIME;
	m_Ytype = USE_PROGRESS_NUMBER;
	m_Ztype = USE_SCALAR;
	m_XID = 0;
	m_YID = 0;
	m_ZID = 0;
	m_ActiveScalar = -1;
}

mafVMEScalarMatrix::~mafVMEScalarMatrix() = default;

//-------------------------------------------------------------------------
int mafVMEScalarMatrix::DeepCopy(mafNode* a)
//-------------------------------------------------------------------------
{
	if (Superclass::DeepCopy(a) == MAF_OK)
	{
		auto scalar = mafVMEScalarMatrix::SafeDownCast(a);
		this->SetTypeForXCoordinates(scalar->GetTypeForXCoordinates());
		this->SetTypeForYCoordinates(scalar->GetTypeForYCoordinates());
		this->SetTypeForZCoordinates(scalar->GetTypeForZCoordinates());
		this->SetScalarArrayOrientation(scalar->GetScalarArrayOrientation());
		this->SetScalarIdForXCoordinate(scalar->GetScalarIdForXCoordinate());
		this->SetScalarIdForYCoordinate(scalar->GetScalarIdForYCoordinate());
		this->SetScalarIdForZCoordinate(scalar->GetScalarIdForZCoordinate());
		this->SetActiveScalarOnGeometry(scalar->GetActiveScalarOnGeometry());
		return MAF_OK;
	}
	return MAF_ERROR;
}
//-------------------------------------------------------------------------
bool mafVMEScalarMatrix::Equals(mafVME* vme)
//-------------------------------------------------------------------------
{
	bool ret = false;
	if (Superclass::Equals(vme))
	{
		mafVMEScalarMatrix* scalar = mafVMEScalarMatrix::SafeDownCast(vme);
		ret = this->m_Xtype == scalar->GetTypeForXCoordinates() && \
			this->m_Ytype == scalar->GetTypeForYCoordinates() && \
			this->m_Ztype == scalar->GetTypeForZCoordinates() && \
			this->m_XID == scalar->GetScalarIdForXCoordinate() && \
			this->m_YID == scalar->GetScalarIdForYCoordinate() && \
			this->m_ZID == scalar->GetScalarIdForZCoordinate() && \
			this->m_ActiveScalar == scalar->GetActiveScalarOnGeometry() && \
			this->m_ScalarArrayOrientationInMatrix == scalar->GetScalarArrayOrientation();
	}
	return ret;
}
//-------------------------------------------------------------------------
mafVMEOutput* mafVMEScalarMatrix::GetOutput()
//-------------------------------------------------------------------------
{
	// allocate the right type of output on demand
	if (!m_Output)
	{
		SetOutput(mafVMEOutputScalarMatrix::NewUPtr()); // create the output
	}
	return m_Output.get();
}

//-------------------------------------------------------------------------
int mafVMEScalarMatrix::SetData(vnl_matrix<double>& data, mafTimeStamp t)
//-------------------------------------------------------------------------
{
	auto item = mafVMEItemScalarMatrix::NewSPtr();
	item->SetData(data);
	item->SetTimeStamp(t);
	GetDataVector()->InsertItem(item);

	return MAF_OK;
}
//-------------------------------------------------------------------------
mafGUI* mafVMEScalarMatrix::CreateGui()
//-------------------------------------------------------------------------
{
	mafString type_array[3] = { _L("time"), _L("progress number"), _L("scalar ID") };

	auto gui = mafNode::CreateGui(); // Called to show info about vmes' type and name
	gui->SetListener(this);
	gui->Divider();
	gui->Label(_L("Parameters used for"));
	gui->Label(_L("VTK representation"));
	gui->Divider(2);
	gui->Label(_L("x coord"));
	gui->Combo(ID_TYPE_FOR_X, _L("type"), &m_Xtype, 3, type_array);
	gui->Integer(ID_SCALAR_FOR_X, _L("scalar id"), &m_XID, 0, MAXINT, _L("Scalar ID associated to the X coordinate \nof the VTK representation"));
	gui->Label(_L("y coord"));
	gui->Combo(ID_TYPE_FOR_Y, _L("type"), &m_Ytype, 3, type_array);
	gui->Integer(ID_SCALAR_FOR_Y, _L("scalar id"), &m_YID, 0, MAXINT, _L("Scalar ID associated to the Y coordinate \nof the VTK representation"));
	gui->Label(_L("z coord"));
	gui->Combo(ID_TYPE_FOR_Z, _L("type"), &m_Ztype, 3, type_array);
	gui->Integer(ID_SCALAR_FOR_Z, _L("scalar id"), &m_ZID, 0, MAXINT, _L("Scalar ID associated to the Z coordinate \nof the VTK representation"));
	gui->Divider();
	gui->Label(_L("scalar for geometry."));
	gui->Integer(ID_ACTIVE_SCALAR, _L("scalat id"), &m_ActiveScalar, -1, MAXINT, _L("Active scalar to use with geometry;\n -1 means no scalar active."));
	gui->Divider();

	EnableWidgets();
	return gui;
}
//-------------------------------------------------------------------------
void mafVMEScalarMatrix::EnableWidgets()
//-------------------------------------------------------------------------
{
	auto gui = AccessGUI();
	gui->Enable(ID_SCALAR_FOR_X, m_Xtype == USE_SCALAR);
	gui->Enable(ID_SCALAR_FOR_Y, m_Ytype == USE_SCALAR);
	gui->Enable(ID_SCALAR_FOR_Z, m_Ztype == USE_SCALAR);
}
//-------------------------------------------------------------------------
void mafVMEScalarMatrix::OnEvent(mafEventBase* maf_event)
//-------------------------------------------------------------------------
{
	// events to be sent up or down in the tree are simply forwarded
	if (mafEvent* e = mafEvent::SafeDownCast(maf_event))
	{
		switch (e->GetId())
		{
		case ID_SCALAR_FOR_X:
		case ID_SCALAR_FOR_Y:
		case ID_SCALAR_FOR_Z:
		case ID_ACTIVE_SCALAR:
			break;
		case ID_TYPE_FOR_X:
		case ID_TYPE_FOR_Y:
		case ID_TYPE_FOR_Z:
			EnableWidgets();
			break;
		default:
			mafNode::OnEvent(maf_event);
		}
	}
	else
	{
		Superclass::OnEvent(maf_event);
	}
}
//-----------------------------------------------------------------------
void mafVMEScalarMatrix::InternalStore(mafStorageElementBuilder& parent)
//-----------------------------------------------------------------------
{
	Superclass::InternalStore(parent);
	parent[_R("XID")].SetValue(m_XID);
	parent[_R("YID")].SetValue(m_YID);
	parent[_R("ZID")].SetValue(m_ZID);
	parent[_R("Xtype")].SetValue(m_Xtype);
	parent[_R("Ytype")].SetValue(m_Ytype);
	parent[_R("Ztype")].SetValue(m_Ztype);
	parent[_R("ActiveScalar")].SetValue(m_ActiveScalar);
	parent[_R("ScalarArrayOrientationInMatrix")].SetValue(m_ScalarArrayOrientationInMatrix);
}
//-----------------------------------------------------------------------
void mafVMEScalarMatrix::InternalRestore(const mafStorageElement& node)
//-----------------------------------------------------------------------
{
	Superclass::InternalRestore(node);
	m_XID = node[_R("XID")].As<int>();
	m_YID = node[_R("YID")].As<int>();
	m_ZID = node[_R("ZID")].As<int>();
	m_Xtype = node[_R("Xtype")].As<int>();
	m_Ytype = node[_R("Ytype")].As<int>();
	m_Ztype = node[_R("Ztype")].As<int>();
	m_ActiveScalar = node[_R("ActiveScalar")].As<int>();
	m_ScalarArrayOrientationInMatrix = node[_R("ScalarArrayOrientationInMatrix")].As<int>();
}
//-------------------------------------------------------------------------
void mafVMEScalarMatrix::SetScalarArrayOrientation(int o)
//-------------------------------------------------------------------------
{
	m_ScalarArrayOrientationInMatrix = o;
	m_ScalarArrayOrientationInMatrix = m_ScalarArrayOrientationInMatrix < ROWS ? ROWS : m_ScalarArrayOrientationInMatrix;
	m_ScalarArrayOrientationInMatrix = m_ScalarArrayOrientationInMatrix > COLUMNS ? COLUMNS : m_ScalarArrayOrientationInMatrix;
#ifdef MAF_USE_VTK
	GetScalarOutput()->UpdateVTKRepresentation();
#endif
	Modified();
}
//-------------------------------------------------------------------------
void mafVMEScalarMatrix::SetActiveScalarOnGeometry(int scalar)
//-------------------------------------------------------------------------
{
	if (scalar == m_ActiveScalar)
	{
		return;
	}
	m_ActiveScalar = scalar < -1 ? -1 : scalar;
	Modified();
}
//-------------------------------------------------------------------------
int mafVMEScalarMatrix::GetActiveScalarOnGeometry()
//-------------------------------------------------------------------------
{
	return m_ActiveScalar;
}
//-------------------------------------------------------------------------
void mafVMEScalarMatrix::SetScalarIdForXCoordinate(int id)
//-------------------------------------------------------------------------
{
	if (m_XID == id)
	{
		return;
	}
	m_XID = id;
#ifdef MAF_USE_VTK
	GetScalarOutput()->UpdateVTKRepresentation();
#endif
	Modified();
}
//-------------------------------------------------------------------------
int mafVMEScalarMatrix::GetScalarIdForXCoordinate()
//-------------------------------------------------------------------------
{
	mafVMEOutputScalarMatrix* output = GetScalarOutput();
	vnl_matrix<double> data = output->GetScalarData();
	if (!data.empty())
	{
		if (m_ScalarArrayOrientationInMatrix == COLUMNS)
		{
			m_XID = m_XID > data.columns() ? data.columns() : m_XID;
		}
		else
		{
			m_XID = m_XID > data.rows() ? data.rows() : m_XID;
		}
	}
	UpdateGUI();
	return m_XID;
}
//-------------------------------------------------------------------------
void mafVMEScalarMatrix::SetTypeForXCoordinates(int t)
//-------------------------------------------------------------------------
{
	if (m_Xtype == t)
	{
		return;
	}
	m_Xtype = t;
	m_Xtype = m_Xtype < USE_TIME ? USE_TIME : m_Xtype;
	m_Xtype = m_Xtype > USE_SCALAR ? USE_SCALAR : m_Xtype;
#ifdef MAF_USE_VTK
	GetScalarOutput()->UpdateVTKRepresentation();
#endif
	Modified();
}
//-------------------------------------------------------------------------
void mafVMEScalarMatrix::SetScalarIdForYCoordinate(int id)
//-------------------------------------------------------------------------
{
	if (m_YID == id)
	{
		return;
	}
	m_YID = id;
#ifdef MAF_USE_VTK
	GetScalarOutput()->UpdateVTKRepresentation();
#endif
	Modified();
}
//-------------------------------------------------------------------------
int mafVMEScalarMatrix::GetScalarIdForYCoordinate()
//-------------------------------------------------------------------------
{
	mafVMEOutputScalarMatrix* output = GetScalarOutput();
	vnl_matrix<double> data = output->GetScalarData();
	if (!data.empty())
	{
		if (m_ScalarArrayOrientationInMatrix == COLUMNS)
		{
			m_YID = m_YID > data.columns() ? data.columns() : m_YID;
		}
		else
		{
			m_YID = m_YID > data.rows() ? data.rows() : m_YID;
		}
	}
	UpdateGUI();
	return m_YID;
}
//-------------------------------------------------------------------------
void mafVMEScalarMatrix::SetTypeForYCoordinates(int t)
//-------------------------------------------------------------------------
{
	if (m_Ytype == t)
	{
		return;
	}
	m_Ytype = t;
	m_Ytype = m_Ytype < USE_TIME ? USE_TIME : m_Ytype;
	m_Ytype = m_Ytype > USE_SCALAR ? USE_SCALAR : m_Ytype;
#ifdef MAF_USE_VTK
	GetScalarOutput()->UpdateVTKRepresentation();
#endif
	Modified();
}
//-------------------------------------------------------------------------
void mafVMEScalarMatrix::SetScalarIdForZCoordinate(int id)
//-------------------------------------------------------------------------
{
	if (m_ZID == id)
	{
		return;
	}
	m_ZID = id;
#ifdef MAF_USE_VTK
	GetScalarOutput()->UpdateVTKRepresentation();
#endif
	Modified();
}
//-------------------------------------------------------------------------
int mafVMEScalarMatrix::GetScalarIdForZCoordinate()
//-------------------------------------------------------------------------
{
	mafVMEOutputScalarMatrix* output = GetScalarOutput();
	vnl_matrix<double> data = output->GetScalarData();
	if (!data.empty())
	{
		if (m_ScalarArrayOrientationInMatrix == COLUMNS)
		{
			m_ZID = m_ZID > data.columns() ? data.columns() : m_ZID;
		}
		else
		{
			m_ZID = m_ZID > data.rows() ? data.rows() : m_ZID;
		}
	}
	UpdateGUI();
	return m_ZID;
}
//-------------------------------------------------------------------------
void mafVMEScalarMatrix::SetTypeForZCoordinates(int t)
//-------------------------------------------------------------------------
{
	if (m_Ztype == t)
	{
		return;
	}
	m_Ztype = t;
	m_Ztype = m_Ztype < USE_TIME ? USE_TIME : m_Ztype;
	m_Ztype = m_Ztype > USE_SCALAR ? USE_SCALAR : m_Ztype;
#ifdef MAF_USE_VTK
	GetScalarOutput()->UpdateVTKRepresentation();
#endif
	Modified();
}
//-----------------------------------------------------------------------
void mafVMEScalarMatrix::InternalPreUpdate()
//-----------------------------------------------------------------------
{
#ifdef MAF_USE_VTK
	GetScalarOutput()->UpdateVTKRepresentation();
#endif
}
//-----------------------------------------------------------------------
void mafVMEScalarMatrix::SetTimeStamp(mafTimeStamp t)
//-----------------------------------------------------------------------
{
	t = t < 0 ? 0 : t;
	bool update_vtk_data = t != m_CurrentTime;
	if (update_vtk_data)
	{
		Superclass::SetTimeStamp(t);

#ifdef MAF_USE_VTK
		GetScalarOutput()->UpdateVTKRepresentation();
#endif
	}
}
//-----------------------------------------------------------------------
void mafVMEScalarMatrix::Print(std::ostream& os, const int tabs)
//-----------------------------------------------------------------------
{
	Superclass::Print(os, tabs);
	mafIndent indent(tabs);

	os << indent << _L("Scalar Array orientation in matrix: ");
	os << (m_ScalarArrayOrientationInMatrix == 0) ? _L("Rows") : _L("Columns");
	os << _L("\n");

	mafString type_scalar[3] = { _L("Time"), _L("Progress numbers"), _L("Scalars") };

	os << indent << _L("Scalar type for X coordinate: ");
	os << type_scalar[m_Xtype].GetCStr();
	os << _L("\n");

	if (m_Xtype == USE_SCALAR)
	{
		os << indent << _L("Scalar ID for X coordinate: ");
		os << m_XID;
		os << _L("\n");
	}

	os << indent << _L("Scalar type for Y coordinate: ");
	os << type_scalar[m_Ytype].GetCStr();
	os << _L("\n");

	if (m_Ytype == USE_SCALAR)
	{
		os << indent << _L("Scalar ID for Y coordinate: ");
		os << m_YID;
		os << _L("\n");
	}

	os << indent << _L("Scalar type for Z coordinate: ");
	os << type_scalar[m_Ztype].GetCStr();
	os << _L("\n");

	if (m_Ztype == USE_SCALAR)
	{
		os << indent << _L("Scalar ID for Z coordinate: ");
		os << m_ZID;
		os << _L("\n");
	}
}
//-------------------------------------------------------------------------
const char** mafVMEScalarMatrix::GetIcon()
//-------------------------------------------------------------------------
{
#include "mafVMEScalar.xpm"
	return mafVMEScalar_xpm;
}
