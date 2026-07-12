#include "ftk/VME/VMEFactory.h"
#include "ftk/VME/PipeFactoryVME.h"
#include "ftk/Core/AttributeFactory.h"
#include "ftk/VME/ItemFactory.h"

#include "Interaction/mafInteractionManager.h"

#include "AppFrame.h"

#include "mafOpCreateGroup.h"
#include "lhpOpImporterOBJ.h"
#include "lhpOpImporterMetashapeOBJ.h"



#include <memory>


#include "mafPics.h"
#include <wx/bitmap.h>
#include <wx/bmpbndl.h>
#include <wx/dcmemory.h>
#include <wx/renderer.h>
#include <wx/window.h>

















#include "mafOpSelect.h"
#include "mafOpDecomposeTimeVarVME.h"
#include "mafOpImporterMSF.h"
#include "mafOpImporterExternalFile.h"
#include "mafOpLabelExtractor.h"
#include "mafOpOpenExternalFile.h"
#include "mafOpExporterBmp.h"
#include "mafOpCreateGroup.h"
#include "mafOpCreateMeter.h"
#include "mafOpCreateMuscleWrapping2.h"
#include "mafOpCreateMeter2.h"
#include "mafOpCreateGravityLine.h"
#include "mafOpCreateCenterline.h"
#include "medOpCreateWrappedMeter.h"
#include "mafOpCreateSlicer.h"
#include "mafOpCreateRefSys.h"
#include "mafOpFilterSurface.h"
#include "mafOpEditMetadata.h"
#include "mafOp2DMeasure.h"
#include "mafOpReparentTo.h"
#include "medOpImporterDicomOffis.h"
#include "mafOpReparentTo.h"
#include "mafOpImporterImage.h"
#include "mafOpImporterSTL.h"
#include "mafOpExporterSTL.h"
#include "mafOpExporterVTK.h"
#include "mafOpExporterVRML.h"
#include "mafOpExporterGeomTex.h"
#include "mafOpImporterVTK.h"
#include "mafOpImporterMSF1x.h"
#include "mafOpImporterVRML.h"
#include "mafOpCreateVolume.h"
#include "mafOpCreatePlane.h"
#include "mafOpCreateOsteometricBoard.h"
#include "mafOpVOIDensityEditor.h"
#include "mafOpImporterBBF.h"
#include "mafOpCreateQuadricSurfaceFitting.h"
//#include "mafOpCreateMuscleWrapper.h"
//BES: 23.6.2008 - Large Volume - to be merged 
#include "mafOpImporterRAWVolume_BES.h"
#include "mafOpImporterRAWVolume.h"
#include "lhpOpKinectUtil.h"
#include "lhpOpKinectModel.h"
#include "lhpOpKinectAFs.h"
#include "lhpOpCreateMetersScripted.h"
#include "mafOpExporterRaw.h"
#include "medOpImporterRAWImages.h"
#include "mafOpExtractIsosurface.h"
#include "mafOpCrop.h"
#include "mafOpVOIDensity.h"
#include "medOpVolumeResample.h"
#include "mafOpAddLandmark.h"
#include "medOpRegisterClusters2.h"
#include "medOpRegisterClusters.h"

#include "lhpOpFuseLMScripted.h"
#include "lhpOpRegisterLMScripted.h"
#include "mafOpCreateSurfaceParametric.h"
#include "mafOpCreateEllipsoid.h"
#include "mafOpCreateHyperboloid.h"
#include "mafOpCreateHyperboloid2S.h"
#include "mafOpCreateCylinder.h"
#include "lhpOpBuildHierarchy.h"
#include "lhpOpTimeReduce.h"
#include "lhpOpTimeShift.h"
#include "mafOpExporterOBJ.h"
#include "lhpOpINPExporter.h"
#include "lhpOpMTRExporter.h"
#include "lhpOpINPImporter.h"
#include "lhpOpMTRImporter.h"
#include "lhpOpImporterOBJ.h"
#include "lhpOpImporterMetashapeOBJ.h"
#include "lhpOpImporterPLY.h"
#include "lhpOpLnSurf.h"
#include "lhpOpAFSys.h"
#include "lhpOpAverageLM.h"
#include "lhpOpHelAxis.h"
#include "lhpOpStickPalpation.h"
#include "medOpScaleDataset.h"
#include "medOpMove.h"
//#include "mafOpMAFTransform.h"
#include "medOpImporterGRFWS.h"
#include "medPipeGraph.h"
#include "mafVMERawMotionData.h" 
#include "medOpImporterMotionData.h"
#include "medOpExporterLandmark.h"
#include "medOpExporterMeters.h"
#include "medOpClassicICPRegistration.h"
#include "mafOpImporterMesh.h"
#include "mafOpImporterVMEDataSetAttributes.h"
#include "medOpImporterLandmark.h"
#include "medOpImporterLandmarkWS.h"
#include "lhpOpBonemat.h"
#include "medOpFreezeVME.h"
#include "medOpExporterWrappedMeter.h"
#include "medOpIterativeRegistration.h"
#include "medOpCreateLabeledVolume.h"
#include "medOpSurfaceMirror.h"
#include "medOpImporterAnalogWS.h"
#include "medOpMML.h"
#include "mafViewVTK.h"
#include "mafViewCompound.h"
#include "mafViewRXCT.h"
#include "mafViewRX.h"
#include "mafViewOrthoSlice.h"
#include "mafViewArbitrarySlice.h"
#include "mafViewGlobalSliceCompound.h"
#include "mafViewSlice.h"
#include "mafViewImageCompound.h"
#include "mafViewIntGraph.h"
#include "medViewSlicer.h"
#include "lhpOpMultiscaleExplore.h"
#include "lhpOpTextureOrientation.h"
#include "lhpOpComputeTensor.h"
#include "medOpCropDeformableROI.h"
#include "mafOpValidateTree.h"
#include "mafOpApplyTrajectory.h"

#include "mafOpCrop3DSurface.h"
#include "medOpComputeWrapping.h"
#include "medPipeComputeWrapping.h"
#include "medGUIDicomSettings.h"

//temporary for testing
#include "mafViewSingleSliceCompound.h"

#include "lhpOpFindCentroid.h"
#include "lhpOpCreateRefSysLM.h"

#ifdef MAF_USE_ITK
#include "lhpOpCreateSurfaceScalar.h"
#endif
#include "lhpVisualPipeSurfaceScalar.h"

//BES: 14.11.2008 - added muscle wrapping
#include "medOpCreateMuscleWrapper.h"
#include "medOpMeshDeformation.h"

#include <vtkTimerLog.h>

#include "lhpPipeIntGraph.h"
#include "lhpPipeIntGraphHAxis.h"
#include "lhpPipeIntGraphPolyline.h"
#include "lhpPipeIntGraphAnalog.h"
#include "lhpPipeLeverArm.h"
#include "lhpOpFingerStick.h"
#include "lhpOpMTRULBImporter.h"
#include "lhpOpSoftReg.h"
#include "lhpOpRegression.h"
#include "lhpOpCreateObject.h"
#include "lhpOpRegistration.h"
#include "lhpOpRegSurfWithCloud.h"
#include "lhpOpRepresentInAF.h"
#include "lhpOpImporterC3DBTK.h" 
#include "lhpOpImporterC3DFused.h" 
#include "lhpOpImporterPressionCenter.h" 
#include "lhpOpExporterC3DBTK.h" 
#include "lhpOpExporterCSVGraph.h" 
#include "lhpOpLMMirror.h"
#include "lhpOpLMProj.h"
#include "lhpOpSolidify.h"
#include "lhpOpJoinSurf.h"
#include "lhpOpMergeClouds.h"
#include "lhpOpICPRegFollow.h"
#include "lhpOpImporterRSScan.h"
#include "lhpOpCutSurface.h"

#include "lhpPipeInfo.h"
#include "lhpViewInfo.h"
#include "lhpVMELMCLines.h" 
#include "mafVMEPGDData.h" 
#include "mafVMEBSplineLine.h"
#include "mafVMEBSplineSurface.h"
#include "mafVMEBSplineVolume.h"
#include "lhpVMELeverArm.h"
#include "OperationCreateFactory.h"


// TODO: REFACTOR THIS 
// this component is used only to override the Accept,  
// and it`s the minimal amount of code in order to override the method
// it could go in a separate file with other redefined Accept`s

class lhpOpMove : public medOpMove
{
public:
	mafTypeMacro(lhpOpMove, medOpMove)

		lhpOpMove(const mafString& label = _R("Move\tCtrl+T")) :Superclass(label) {}
	//----------------------------------------------------------------------------
	mafOp* Copy() override
		//----------------------------------------------------------------------------
	{
		return new lhpOpMove(GetLabel());
	}

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode* vme) override
	{
		bool accepted = false;

		accepted = !vme->IsA("lhpVMESurfaceScalarVarying") \
			&& !vme->IsA("mafVMEMeter") \
			&& !vme->IsA("medVMEWrappedMeter") \
			&& !vme->IsA("medVMELabeledVolume") \
			&& !vme->IsA("mafVMEHelicalAxis");

		if (accepted == false)
		{
			return false;
		}
		else
		{
			return medOpMove::Accept(vme);
		}
	}
};

class lhpOpMoveSeq : public lhpOpMove
{
public:
	mafTypeMacro(lhpOpMoveSeq, lhpOpMove)
		lhpOpMoveSeq(const mafString& label = _R("Move Sequence")) :Superclass(label) { m_EnableScaling = 0; }
	mafOp* Copy() override
		//----------------------------------------------------------------------------
	{
		return new lhpOpMoveSeq(GetLabel());
	}
	void OpDo() override;
	void OpUndo() override;
private:
	void TransfMatr(mafMatrix& convMatrix, mafTimeStamp tsSkip = -1);
	mafMatrix m_ConvMatrix;
};



class lhpOpCreateLMCLines : public mafOp
{
public:
	mafTypeMacro(lhpOpCreateLMCLines, mafOp);
	lhpOpCreateLMCLines(const mafString& label = _R("CreateObject")) :Superclass(label)
	{
		m_OpType = OPTYPE_OP;
		m_Canundo = true;
	}
	~lhpOpCreateLMCLines() override
	{
	}

	mafOp* Copy() override { return new lhpOpCreateLMCLines(GetLabel()); }

	bool Accept(mafNode* node) override { return (node != NULL); }
	void OpRun() override
	{
		m_Created = lhpVMELMCLines::NewSPtr();
		m_Created->SetName(_R("Cloud lines"));
		SetOutput(m_Created);
		{ mafEvent evUnq(this, OP_RUN_OK); InvokeEvent(evUnq); }
	}
	void OpDo() override;
	void OpUndo() override;

protected:
	std::shared_ptr<lhpVMELMCLines> m_Created;
};

void lhpOpCreateLMCLines::OpDo()
{
	mafNode::ReparentTo(GetOutput(), GetInput().get());
	auto lmc = mafVMELandmarkCloud::SafeDownCast(GetInput());
	if (m_Created && lmc)
	{
		m_Created->SetCloud(lmc.get());
	}
}
void lhpOpCreateLMCLines::OpUndo()
{
	if (m_Created)
		m_Created->SetCloud(nullptr);
	mafNode::ReparentTo(GetOutput(), nullptr);
}
//----------------------------------------------------------------------------
void lhpOpMoveSeq::TransfMatr(mafMatrix& convMatrix, mafTimeStamp tsSkip)
//----------------------------------------------------------------------------
{
	mafMatrix newMatr;
	mafMatrix oldMatr;
	std::vector<mafTimeStamp> stamps;
	mafVME::StaticDownCast(GetInput())->GetTimeStamps(stamps);
	for (int i = 0; i < stamps.size(); i++)
	{
		if (stamps[i] == tsSkip)
			continue;
		// apply roto-translation to abs pose
		mafVME::StaticDownCast(GetInput())->GetOutput()->GetAbsMatrix(oldMatr, stamps[i]);
		mafMatrix::Multiply4x4(convMatrix, oldMatr, newMatr);
		mafVME::StaticDownCast(GetInput())->SetAbsMatrix(newMatr, stamps[i]);
	}
	mafVME::StaticDownCast(GetInput())->GetOutput()->Update();
	{ mafEvent evUnq(this, CAMERA_UPDATE); InvokeEvent(evUnq); }
}
void lhpOpMoveSeq::OpDo()
//----------------------------------------------------------------------------
{
	mafMatrix newMatr;
	mafMatrix oldMatr;
	mafMatrix convMatrix;
	newMatr = m_NewAbsMatrix;
	oldMatr = m_OldAbsMatrix;
	oldMatr.Invert();
	mafMatrix::Multiply4x4(newMatr, oldMatr, convMatrix);
	m_ConvMatrix = convMatrix;
	//((mafVME *)GetInput())->SetAbsMatrix(oldMatr);
	TransfMatr(convMatrix, mafVME::StaticDownCast(GetInput())->GetTimeStamp());
}
void lhpOpMoveSeq::OpUndo()
//----------------------------------------------------------------------------
{
	mafMatrix convMatrix;
	convMatrix = m_ConvMatrix;
	convMatrix.Invert();
	TransfMatr(convMatrix);
}

class lhpOpScaleDataset : public medOpScaleDataset
{
public:
	lhpOpScaleDataset(const mafString& label = _R("Scale Dataset")):medOpScaleDataset(label){}

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode* vme) override
	{
		bool accepted = false;

		accepted = !vme->IsA("lhpVMESurfaceScalarVarying") \
			&& !vme->IsA("mafVMEMeter") \
			&& !vme->IsA("medVMEWrappedMeter") \
			&& !vme->IsA("medVMELabeledVolume") \
			&& !vme->IsA("mafVMEHelicalAxis");

		if (accepted == false)
		{
			return false;
		}
		else
		{
			return medOpScaleDataset::Accept(vme);
		}
	}
};












class NativeCheckboxBundleImpl : public wxBitmapBundleImpl
	{
	public:
		NativeCheckboxBundleImpl(wxWindow* win, int stateFlags)
			: m_win(win), m_stateFlags(stateFlags)
		{
		}

		// Defines the base (100% scale) logical size of your checkbox icon
		wxSize GetDefaultSize() const override
		{
			return wxSize(16, 16);
		}

		wxSize GetPreferredBitmapSizeAtScale(double scale) const override
		{
			return GetDefaultSize() * scale;
		}

		// Called automatically by wxWidgets when the system requests a specific resolution
		wxBitmap GetBitmap(const wxSize& size) override
		{
			// 1. Create a bitmap matched perfectly to the requested physical DPI size
			wxBitmap bmp(size);
			wxMemoryDC dc(bmp);

			// 2. Clear background to match the parent control
			dc.SetBackground(wxBrush(m_win->GetBackgroundColour()));
			dc.Clear();

			// 3. Ask the native OS theme renderer to draw the checkbox at this exact size
			wxRect rect(0, 0, size.x, size.y);
			wxRendererNative::Get().DrawCheckBox(m_win, dc, rect, m_stateFlags);

			dc.SelectObject(wxNullBitmap);
			return bmp;
		}

	private:
		wxWindow* m_win;
		int m_stateFlags; // e.g., wxCONTROL_CHECKED or 0 for unchecked
	};

	// Convenience factory function to instantiate the bundle
	wxBitmapBundle CreateNativeCheckboxBundle(wxWindow* win, int stateFlags)
	{
		return wxBitmapBundle::FromImpl(new NativeCheckboxBundleImpl(win, stateFlags));
	}

	/*wxBitmapBundle create_native_checkbox_bundle(win, state_flags) :
		# state_flags example : wx.CONTROL_CHECKED or 0 (for unchecked)
		size = wx.Size(16, 16) # Base logical size

		# Create a bundle from an implementation that scales natively
		bmp = wx.Bitmap(size)
		dc = wx.MemoryDC(bmp)
		dc.SetBackground(wx.Brush(win.GetBackgroundColour()))
		dc.Clear()

		# Ask the native OS renderer to draw the checkbox
		wx.RendererNative.Get().DrawCheckBox(win, dc, wx.Rect(0, 0, 16, 16), state_flags)
		dc.SelectObject(wx.NullBitmap)

		# Convert to a bundle so wxTreeCtrl scales it automatically under high - DPI
		return wx.BitmapBundle(bmp)*/
	const std::vector<wxBitmapBundle>& initializeImageList(wxWindow* win)
	{
		static std::optional<std::vector<wxBitmapBundle>> images;
		if (images)
			return *images;
		std::vector<wxBitmapBundle> newImages;
		newImages.push_back(mafPictureFactory::GetPictureFactory()->GetBmp(_R("DISABLED")));
		newImages.push_back(mafPictureFactory::GetPictureFactory()->GetBmp(_R("CHECK_OFF")));
		//newImages.push_back(CreateNativeCheckboxBundle(win, wxCONTROL_CHECKED));// mafPictureFactory::GetPictureFactory()->GetBmp(_R("CHECK_OFF")));
		//newImages.push_back(CreateNativeCheckboxBundle(win , 0));// mafPictureFactory::GetPictureFactory()->GetBmp(_R("CHECK_OFF")));
		//newImages.push_back(CreateNativeCheckboxBundle(win, wxCONTROL_CHECKED));// mafPictureFactory::GetPictureFactory()->GetBmp(_R("CHECK_OFF")));
		newImages.push_back(mafPictureFactory::GetPictureFactory()->GetBmp(_R("CHECK_ON")));
		newImages.push_back(mafPictureFactory::GetPictureFactory()->GetBmp(_R("RADIO_OFF")));
		newImages.push_back(mafPictureFactory::GetPictureFactory()->GetBmp(_R("RADIO_ON")));
		/*const size_t num_of_status = 5;
		wxBitmap state_ico[num_of_status];
		state_ico[0] = mafPictureFactory::GetPictureFactory()->GetBmp(_R("DISABLED"));
		state_ico[1] = mafPictureFactory::GetPictureFactory()->GetBmp(_R("CHECK_OFF"));
		state_ico[2] = mafPictureFactory::GetPictureFactory()->GetBmp(_R("CHECK_ON"));
		state_ico[3] = mafPictureFactory::GetPictureFactory()->GetBmp(_R("RADIO_OFF"));
		state_ico[4] = mafPictureFactory::GetPictureFactory()->GetBmp(_R("RADIO_ON"));
		int sw = state_ico[0].GetWidth();
		int sh = state_ico[0].GetHeight();

		auto images = std::make_unique<wxImageList>(sw, sh, false, num_of_status);
		for (auto& icon : state_ico)
		{
			images->Add(icon);
		}
		return images;*/
		images = std::move(newImages);
		return *images;
	}

	wxAcceleratorEntry* SetAccelerator(const wxString& name, int id)
	{
		wxString accelerator = name;
		wxString flag;
		wxString extra_flag;
		wxString key_code;
		int flag_num = 0;
		wxStringTokenizer tokenizer(accelerator, "\t");
		int token = tokenizer.CountTokens();

		if (token > 1)
		{
			accelerator = tokenizer.GetNextToken();
			accelerator = tokenizer.GetNextToken();
			wxStringTokenizer tokenizer2(accelerator, "+");
			token = tokenizer2.CountTokens();
			if (token == 2)
			{
				flag = tokenizer2.GetNextToken();
				key_code = tokenizer2.GetNextToken();
			}
			else
			{
				flag = tokenizer2.GetNextToken();
				extra_flag = tokenizer2.GetNextToken();
				key_code = tokenizer2.GetNextToken();
			}
			if (flag == "Ctrl")
			{
				flag_num = wxACCEL_CTRL;
			}
			else if (flag == "Alt")
			{
				flag_num = wxACCEL_ALT;
			}
			else if (flag == "Shift")
			{
				flag_num = wxACCEL_SHIFT;
			}

			if (extra_flag == "Ctrl")
			{
				flag_num |= wxACCEL_CTRL;
			}
			else if (extra_flag == "Alt")
			{
				flag_num |= wxACCEL_ALT;
			}
			else if (extra_flag == "Shift")
			{
				flag_num |= wxACCEL_SHIFT;
			}
			
			return new wxAcceleratorEntry{ flag_num, static_cast<int>(key_code.c_str()[0]), id };
		}
		return nullptr;
	}

#ifdef jhjkhkjhkj
void mafLogicWithManagers::Plug(mafOp* op, const mafString& menuPath, bool canUndo, mafGUISettings* setting)
{
	if (m_logic->m_OpManager)
	{
		mafString fullLabel = op->GetLabel();
		op->SetLabel(mafStripMenuCodes(fullLabel));
		long id = m_logic->m_OpManager->OpAdd(op/*, canUndo/*, setting*/);
		wxMenu* path_menu = m_logic->m_OpMenu;
		if (op->GetType() == OPTYPE_IMPORTER)
			path_menu = m_logic->m_ImportMenu;
		else if (op->GetType() == OPTYPE_EXPORTER)
			path_menu = m_logic->m_ExportMenu;
		else if (op->GetType() == OPTYPE_EDIT)
			path_menu = m_logic->m_EditMenu;
		mafID command = GetNewMenuId();
		AddToMenu(fullLabel, command, path_menu, menuPath);
		m_logic->m_MenuElems.push_back(mafMenuElems(true, id, command));


		// currently mafInteraction is strictly dependent on VTK
#ifdef MAF_USE_VTK    
		if (m_logic->m_InteractionManager)
		{
			if (const char** actions = op->GetActions())
			{
				const char* action;
				for (int i = 0; action = actions[i]; i++)
				{
					m_logic->m_InteractionManager->AddAction(action);
				}
			}
		}
#endif
	}
}
#endif

template<class BaseFrame>
class ConfiguredAppFrame : public AppFrame<BaseFrame>
{
public:
	ConfiguredAppFrame();

	ConfiguredAppFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE, const wxString& name = wxASCII_STR(wxFrameNameStr));

	~ConfiguredAppFrame() override;

	bool Create(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE, const wxString& name = wxASCII_STR(wxFrameNameStr));

	//private:
protected:
	void ConfigureOperations() override;
};
template <class BaseFrame>
ConfiguredAppFrame<BaseFrame>::ConfiguredAppFrame() = default;

template <class BaseFrame>
ConfiguredAppFrame<BaseFrame>::ConfiguredAppFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
	Create(parent, id, title, pos, size, style, name);
}

template <class BaseFrame>
ConfiguredAppFrame<BaseFrame>::~ConfiguredAppFrame() = default;

template <class BaseFrame>
bool ConfiguredAppFrame<BaseFrame>::Create(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
	if (!AppFrame<BaseFrame>::Create(parent, id, title, pos, size, style, name))
	{
		return false;
	}
	return true;
}

template <class BaseFrame>
void ConfiguredAppFrame<BaseFrame>::ConfigureOperations()
{
	AppFrame<BaseFrame>::m_operationsRegistry = std::make_unique<OperationsRegistry>();
	auto addCreateFactoryOp = [this](const base::String& name, const base::String& typeName, const base::String& nodeName)
		{
			AppFrame<BaseFrame>::m_operationsRegistry->registerOperation(name, [n = name, t = typeName, nn = nodeName](DocumentContext& context) {return std::make_unique<OperationCreateFactory>(_R("Create"), t, context, nn); });
		};
	addCreateFactoryOp(_R("Create/New/Group(new)"), _R("mafVMEGroup"), _R("Group"));
	//addCreateFactoryOp(_R("Create/New/Surface Parametric"), _R("mafVMESurfaceParametric"), _R("Surface Parametric"));

	auto addCompatibleOp = [this]<typename T>(const base::String & name, const base::String & namePrefix = {})
	{
		base::String fullName = namePrefix.empty() ? name : namePrefix + _R("/") + name;
		AppFrame<BaseFrame>::m_operationsRegistry->registerOperation(fullName, [n = fullName](DocumentContext& context)
			{
				auto op = std::make_unique<T>(n);
				if (auto sel = context.getSelectionController().selected(); sel.size() == 1)
				{
					op->SetInput(sel[0]->SharedFromThis());
				}
				return op;
			});
	};

	//addCompatibleOp.template operator()<mafOpCreateGroup>(_R("Create/New/Group"));
	//addCompatibleOp.template operator()<lhpOpImporterOBJ> (_R("Geometries/Import OBJ"));
	//addCompatibleOp.template operator()<lhpOpImporterMetashapeOBJ>(_R("Geometries/Import Metashape OBJ"));
	bool fullVersion = true;


	//------------------------- Editors -------------------------
	addCompatibleOp.template operator()<mafOpDelete>(_L("Delete   \tCtrl+Shift+D"), _R(""));
	addCompatibleOp.template operator()<mafOpCut>(_L("Cut   \tCtrl+Shift+X"), _R(""));
	addCompatibleOp.template operator()<mafOpCopy>(_L("Copy  \tCtrl+Shift+C"), _R(""));
	addCompatibleOp.template operator()<mafOpPaste>(_L("Paste \tCtrl+Shift+V"), _R(""));
	//------------------------- Importers -------------------------
	addCompatibleOp.template operator()<mafOpImporterSTL>(_R("STL"), _R("Geometries"));
	addCompatibleOp.template operator()<lhpOpImporterOBJ>(_R("OBJ"), _R("Geometries"));
	addCompatibleOp.template operator()<lhpOpImporterMetashapeOBJ>(_R("Metashape OBJ"), _R("Geometries"));
	addCompatibleOp.template operator()<lhpOpImporterPLY>(_R("PLY"), _R("Geometries"));
	addCompatibleOp.template operator()<mafOpImporterMSF>(_R("MSF"), _R("Other"));
	addCompatibleOp.template operator()<medOpImporterLandmark>(_R("Landmark"), _R("Motion Analysis"));
#ifdef COMPLETE
	addCompatibleOp.template operator()<medOpImporterMotionData<mafVMERawMotionData>>(_R("Raw Motion Data"), _R("RAW Motion Data (*.MAN)|*.MAN"), _R("Dictionary (*.txt)|*.txt"), _R("Motion Analysis"));
	addCompatibleOp.template operator()<medOpImporterMotionData<mafVMEPGDData>>(_R("PGD Data"), _R("PGD Data (*.PGD)|*.PGD"), _R("Dictionary (*.txt)|*.txt"), _R("Motion Analysis"));
#endif
	addCompatibleOp.template operator()<mafOpImporterVRML>(_R("VRML"), _R("Geometries"));
	addCompatibleOp.template operator()<lhpOpINPImporter>(_R("INP/INP_AF"), _R("Geometries"));
	addCompatibleOp.template operator()<lhpOpMTRImporter>(_R("MTR"), _R("Geometries"));
	addCompatibleOp.template operator()<lhpOpMTRULBImporter>(_R("MTR (ULB)"), _R("Geometries"));
	addCompatibleOp.template operator()<lhpOpImporterC3DBTK>(_R("C3D BTK"), _R("Motion Analysis"));
	addCompatibleOp.template operator()<lhpOpImporterC3DFused>(_R("C3D Automated"), _R("Motion Analysis"));
	addCompatibleOp.template operator()<lhpOpImporterPressionCenter>(_R("plantar pression center"), _R("Gait Analysis"));
	if (fullVersion)
	{
#ifdef COMPLETE
		medGUIDicomSettings* dicomSettings = new medGUIDicomSettings(NULL, _R("DICOM"));
		medOpImporterDicomOffis* dimp = new medOpImporterDicomOffis>(_R("DICOM"));
		dimp->SetSetting(dicomSettings);
		m_Logic->Plug(dimp, _R("DICOM Suite"));
#endif
		//addCompatibleOp.template operator()<medOpImporterDicomOffis>(_R("DICOM"),_R("Images"));
		addCompatibleOp.template operator()<mafOpImporterVTK>(_R("VTK"), _R("Other"));
		addCompatibleOp.template operator()<mafOpImporterMSF1x>(_R("MAF 1.x"), _R("Other"));
		addCompatibleOp.template operator()<mafOpImporterBBF>(_R("BFF (VolumeLarge)"), _R("Other"));
		addCompatibleOp.template operator()<mafOpImporterRAWVolume_BES>(_R("Raw Volume"), _R("Images"));
		addCompatibleOp.template operator()<mafOpImporterRAWVolume>(_R("Raw Volume Legacy"), _R("Images"));
		addCompatibleOp.template operator()<medOpImporterRAWImages>(_R("Raw Images"), _R("Images"));
		//addCompatibleOp.template operator()<medOpImporterRAWImages>(_R("Raw Images Legacy"),_R("Images"));
		addCompatibleOp.template operator()<mafOpImporterImage>(_R("Images"), _R("Images"));
		addCompatibleOp.template operator()<medOpImporterLandmarkWS>(_R("ASCII trajectories (VWs)"), _R("Motion Analysis"));
		// addCompatibleOp.template operator()<lhpOpLandmarkImporter>(_R("Landmark"))); //Old Importer
		addCompatibleOp.template operator()<medOpImporterGRFWS>(_R("ASCII Force Plates (VWs)"), _R("Motion Analysis"));
		addCompatibleOp.template operator()<mafOpImporterMesh>(_R("Generic Mesh"), _R("Finite Element"));
		addCompatibleOp.template operator()<mafOpImporterExternalFile>(_R("External data"), _R("Other"));
		addCompatibleOp.template operator()<medOpImporterAnalogWS>(_R("ASCII Analog (VWs)"), _R("Motion Analysis"));
#ifdef COMPLETE
		addCompatibleOp.template operator()<lhpOpKinectUtil(false, _R("Kinect Full"), false, true, true), _R("Motion Analysis"));
		addCompatibleOp.template operator()<lhpOpKinectUtil(false, _R("Kinect Lower"), false, true, false), _R("Motion Analysis"));
		addCompatibleOp.template operator()<lhpOpKinectUtil(false, _R("Kinect Upper"), false, false, true), _R("Motion Analysis"));
		addCompatibleOp.template operator()<lhpOpKinectUtil(true, _R("Kinect App Full"), false, true, true), _R("Motion Analysis"));
		addCompatibleOp.template operator()<lhpOpKinectUtil(true, _R("Kinect App Lower"), false, true, false), _R("Motion Analysis"));
		addCompatibleOp.template operator()<lhpOpKinectUtil(true, _R("Kinect App Upper"), false, false, true), _R("Motion Analysis"));
		addCompatibleOp.template operator()<lhpOpImporterRSScan>(_R("RSScan"), _R("Finite Element"));
#endif
	}

	//-------------------------------------------------------------

	//------------------------- Exporters -------------------------
	addCompatibleOp.template operator()<mafOpExporterSTL>(_R("STL"), _R("Geometries"));
	addCompatibleOp.template operator()<lhpOpINPExporter>(_R("INP"), _R("Geometries"));
	addCompatibleOp.template operator()<mafOpExporterOBJ>(_R("OBJ"), _R("Geometries"));
	//addCompatibleOp.template operator()<mafOpExporterVRML>(_R("VRML"), _R("Geometries"));
	//addCompatibleOp.template operator()<mafOpExporterVRML>(_R("FBX"), _R("Geometries"));
	//addCompatibleOp.template operator()<mafOpExporterGeomTex>(_R("GEOMTEX"), _R("Geometries"));
	addCompatibleOp.template operator()<mafOpExporterVTK>(_R("VTK"), _R("Other"));
	addCompatibleOp.template operator()<mafOpExporterBmp>(_R("Bmp"), _R("Images"));
	addCompatibleOp.template operator()<lhpOpMTRExporter>(_R("MTR"), _R("Motion Analysis"));
	addCompatibleOp.template operator()<medOpExporterLandmark>(_R("Landmark"), _R("Motion Analysis"));
	addCompatibleOp.template operator()<lhpOpExporterC3DBTK>(_R("C3D BTK"), _R("Motion Analysis"));
	addCompatibleOp.template operator()<lhpOpExporterCSVGraph>(_R("CSV Graph"), _R("Motion Analysis"));
	if (fullVersion)
	{
		addCompatibleOp.template operator()<mafOpExporterRAW>(_R("Raw"), _R("Images"));
		addCompatibleOp.template operator()<medOpExporterWrappedMeter>(_R("Wrapped Meter"), _R("Other"));
		addCompatibleOp.template operator()<medOpExporterMeters>(_R("Meters"), _R("Other"));
	}
	//-------------------------------------------------------------

	//------------------------- Operations -------------------------
	addCompatibleOp.template operator()<mafOpValidateTree>(_R("Validate Tree"), _R(""));
	addCompatibleOp.template operator()<mafOpCreateGroup>(_R("Group"), _R("Create/New"));
	addCompatibleOp.template operator()<lhpOpCreateMetersScripted>(_R("Scripted meters"), _R("Create/New"));
	addCompatibleOp.template operator()<mafOpCreateSurfaceParametric>(_R("Parametric Surface"), _R("Create/New"));
	addCompatibleOp.template operator()<mafOpCreateEllipsoid>(_R("Ellipsoid"), _R("Create/New/QuadricSurface"));
	addCompatibleOp.template operator()<mafOpCreateHyperboloid>(_R("Hyperboloid1S"), _R("Create/New/QuadricSurface"));
	addCompatibleOp.template operator()<mafOpCreateHyperboloid2S>(_R("Hyperboloid2S"), _R("Create/New/QuadricSurface"));
	addCompatibleOp.template operator()<mafOpCreateCylinder>(_R("Cylinder"), _R("Create/New/QuadricSurface"));
	addCompatibleOp.template operator()<mafOpCreateQuadricSurfaceFitting>(_R("Quadric Surface Fitting"), _R("Create/Derive"));
	addCompatibleOp.template operator()<mafOpAddLandmark>(_R("Add Landmark \tCtrl+A"), _R("Create/New"));
	addCompatibleOp.template operator()<mafOpCreateMeter>(_R("Meter"), _R("Create/Derive"));
	addCompatibleOp.template operator()<mafOpCreateMeter2>(_R("Meter2"), _R("Create/Derive"));
	addCompatibleOp.template operator()<mafOpCreateCenterLine>(_R("Centerline"), _R("Create/Derive"));
	addCompatibleOp.template operator()<mafOpCreateGravityLine>(_R("Virtual Osteometricboard"), _R("Create/New/Osteometric Tools"));
	addCompatibleOp.template operator()<lhpOpFindCentroid>(_R("Geometry centroid"), _R("Create/Derive"));

	//  addCompatibleOp.template operator()<mafOpCreateMuscleWrapperAQ>(_R("Muscle Wrapper_AQ"), _R("Create/Derive"));
	addCompatibleOp.template operator()<mafOpCreateMuscleWrapping2>(_R("Muscle Wrapper_2"), _R("Create/Derive"));
	addCompatibleOp.template operator()<mafOpReparentTo>(_R("Reparent to...  \tCtrl+R"), _R("Modify/Fuse"));
#ifdef COMPLETE
	addCompatibleOp.template operator()<mafOpReparentTo>(_R("Local reparent to..."), false), _R("Modify/Fuse"));
#endif
	addCompatibleOp.template operator()<lhpOpMove>(_R("Move\tCtrl+T"), _R("Modify"));
	addCompatibleOp.template operator()<lhpOpAverageLM>(_R("Average landmark"), _R("Create/Derive"));
	addCompatibleOp.template operator()<lhpOpCreateLMCLines>(_R("Cloud lines"), _R("Create/Derive"));
	addCompatibleOp.template operator()<lhpOpJoinSurf>(_R("JoinSurface"), _R("Create/Derive"));
	addCompatibleOp.template operator()<lhpOpMergeClouds>(_R("Merge clouds"), _R("Create/Derive"));
	addCompatibleOp.template operator()<mafOpCreatePlane>(_R("Plane"), _R("Create/New/Osteometric Tools"));
	//addCompatibleOp.template operator()<mafOpCreateOsteometricBoard>(_R("OsteometricBoard"), _R("Create/New/Osteometric Tools"));
	if (fullVersion)
	{
		addCompatibleOp.template operator()<mafOpCreateVolume>(_R("Constant Volume"), _R("Create/New"));

#ifdef MAF_USE_ITK
		addCompatibleOp.template operator()<lhpOpCreateSurfaceScalar>(_R("Surface Scalar"), _R("Create/Derive"));
#endif
		addCompatibleOp.template operator()<lhpOpLnSurf>(_R("Lineset and surface"), _R("Create/Derive"));
		//addCompatibleOp.template operator()<mafOpCreateRefSys>(_R("Refsys"),_R("Create/New"));
		addCompatibleOp.template operator()<mafOpCreateSlicer>(_R("Slicer"), _R("Create/Derive"));
		addCompatibleOp.template operator()<medOpFreezeVME>(_R("Freeze VME"), _R("Create/Derive"));
		addCompatibleOp.template operator()<medOpRegisterClusters>(_R("Register Landmark Cloud"), _R("Modify/Fuse"));
		addCompatibleOp.template operator()<medOpRegisterClusters2>(_R("Motion Scale"), _R("Modify/Fuse"));
		addCompatibleOp.template operator()<lhpOpFuseLMScripted>(_R("Fuse LM based model"), _R("Modify/Fuse"));
		addCompatibleOp.template operator()<lhpOpRegisterLMScripted>(_R("Register Landmark Cloud Tree"), _R("Modify/Fuse"));
		//addCompatibleOp.template operator()<medOpCreateWrappedMeter>(_R("Wrapped Meter"),_R("Create/Derive"));
		addCompatibleOp.template operator()<medOpComputeWrapping>(_R("Wrapped Action Line"), _R("Create/Derive"));//15-1-2009
		addCompatibleOp.template operator()<medOpCreateMuscleWrapper>(_R("Muscle Wrapper"), _R("Create/Derive")); //BES: 14.11.2008
		// addCompatibleOp.template operator()<lhpOpEditMetadata>(_R("Metadata Editor"),_R("Modify"));
		addCompatibleOp.template operator()<mafOpFilterSurface>(_R("Filter Surface"), _R("Modify"));
		addCompatibleOp.template operator()<mafOpVOIDensityEditor>(_R("Volume Density"), _R("Modify"));
		addCompatibleOp.template operator()<medOpMeshDeformation>(_R("Deform Surface"), _R("Modify"));
		addCompatibleOp.template operator()<mafOpApplyTrajectory>(_R("Apply Trajectory"), _R("Modify"));

		addCompatibleOp.template operator()<mafOpExtractIsosurface>(_R("Extract Isosurface"), _R("Create/Derive"));
		addCompatibleOp.template operator()<medOpSurfaceMirror>(_R("Group/Surface Mirror"), _R("Modify"));
		addCompatibleOp.template operator()<mafOpCrop>(_R("Crop Volume"), _R("Modify"));
		addCompatibleOp.template operator()<mafOpCrop3DSurface>(_R("Crop 3D Surface"), _R("Modify"));
		addCompatibleOp.template operator()<medOpVolumeResample>(_R("Volume Resample"), _R("Modify"));
		addCompatibleOp.template operator()<mafOp2DMeasure>(_R("2D Measure"), _R("Measure"));
		addCompatibleOp.template operator()<mafOpVOIDensity>(_R("VOI Density"), _R("Measure"));
		addCompatibleOp.template operator()<lhpOpScaleDataset>(_R("Scale Dataset"), _R("Modify"));
		addCompatibleOp.template operator()<medOpCropDeformableROI>(_L("Masking"), _L("Modify"));
		addCompatibleOp.template operator()<mafOpImporterVMEDataSetAttributes>(_R("VME DataSet Attributes Adder"), _R("Modify"));
		addCompatibleOp.template operator()<medOpClassicICPRegistration>(_R("Register Surface"), _R("Modify/Fuse"));
		addCompatibleOp.template operator()<lhpOpAFSys>(_R("AFRefsys"), _R("Create/Derive"));
		addCompatibleOp.template operator()<lhpOpKinectAFs>(_R("Kinect Refsys"), _R("Create/Derive"));
		addCompatibleOp.template operator()<lhpOpStickPalpation>(_R("Wand palpated landmark"), _R("Create/Derive"));

		addCompatibleOp.template operator()<lhpOpHelAxis>(_R("Helical axis"), _R("Create/Derive"));
		addCompatibleOp.template operator()<lhpOpTimeReduce>(_R("Time reduce"), _R("Modify"));
		addCompatibleOp.template operator()<lhpOpTimeShift>(_R("Time shift"), _R("Modify"));
#ifdef COMPLETE
		addCompatibleOp.template operator()<lhpOpLMProj(true, _R("Landmark Cloud Projection"), _R("Create/Derive"));
#endif
		addCompatibleOp.template operator()<lhpOpSolidify>(_R("Solidify Landmark Cloud"), _R("Create/Derive"));
		addCompatibleOp.template operator()<lhpOpSoftReg>(_R("Soft tissue registration"), _R("Create/Derive"));
#ifdef COMPLETE
		addCompatibleOp.template operator()<lhpOpCreateObject<mafVMEBSplineLine>>(_R("BSplineLine"), _R("BSplineLine"), _R("Create/New"));
		addCompatibleOp.template operator()<lhpOpCreateObject<mafVMEBSplineSurface>>(_R("BSplineSurface"), _R("BSplineSurface"), _R("Create/New"));
		addCompatibleOp.template operator()<lhpOpCreateObject<mafVMEBSplineVolume>>(_R("BSplineVolume"), _R("BSplineVolume"), _R("Create/New"));
		addCompatibleOp.template operator()<lhpOpCreateObject<lhpVMELeverArm>>(_R("Lever Arm"), _R("Lever Arm"), _R("Create/Derive"));
#endif
		addCompatibleOp.template operator()<lhpOpRegSurfWithCloud>(_R("Register Surface with Landmark Cloud"), _R("Modify/Fuse"));
		addCompatibleOp.template operator()<lhpOpRepresentInAF>(_R("Represent in RefSys"), _R("Modify/Fuse"));
		addCompatibleOp.template operator()<lhpOpLMMirror>(_R("Landmark Cloud Mirror"), _R("Modify"));
		addCompatibleOp.template operator()<lhpOpMoveSeq>(_R("Move Sequence"), _R("Modify"));
		addCompatibleOp.template operator()<lhpOpICPRegFollow>(_R("Move Surface As Registered"), _R("Modify/Fuse"));
		addCompatibleOp.template operator()<lhpOpRegression>(_R("Regression"), _R("Create/Derive"));
		addCompatibleOp.template operator()<lhpOpFingerStick>(_R("Finger stick"), _R("Create/Derive"));
		addCompatibleOp.template operator()<lhpOpCutSurface>(_R("CutSurface"), _R("Create/Derive"));
		addCompatibleOp.template operator()<medOpComputeWrapping>(_R("Compute Wrapping"), _R("Create/Derive"));
		//addCompatibleOp.template operator()<lhpOpMeanHelAxis>(_R("Mean helical axis"),_R("Create/Derive"));
		addCompatibleOp.template operator()<lhpOpRegistration>(_R("DSRegistration"), _R("Modify"));
		addCompatibleOp.template operator()<lhpOpKinectModel>(_R("KinectModel"), _R("Modify"));
		addCompatibleOp.template operator()<lhpOpBuildHierarchy>(_R("Make hierarchical"), _R("Modify/Fuse"));
		addCompatibleOp.template operator()<lhpOpBonemat>(_R("Bonemat"), _R("Modify"));
		addCompatibleOp.template operator()<medOpIterativeRegistration>(_R("Iterative Registration"), _R("Modify/Fuse"));
		addCompatibleOp.template operator()<mafOpOpenExternalFile>(_R("Open with external program"), _R("Manage"));
		addCompatibleOp.template operator()<medOpCreateLabeledVolume>(_R("Labeled Volume"), _R("Create/Derive"));

		addCompatibleOp.template operator()<mafOpDecomposeTimeVarVME>(_R("Decompose Time"), _R("Create/Derive"));
		addCompatibleOp.template operator()<mafOpLabelExtractor>(_R("Extract Label"), _R("Create/Derive"));
		addCompatibleOp.template operator()<lhpOpMultiscaleExplore>(_R("Multiscale Viewer"), _R("Manage"));
		addCompatibleOp.template operator()<medOpMML>(_R("Register from template"), _R("Modify"));
		addCompatibleOp.template operator()<lhpOpComputeTensor>(_R("Compute Tensors"), _R("Modify"));

		addCompatibleOp.template operator()<lhpOpTextureOrientation>(_R("Texture Orientation"), _R("Create/Derive"));
		addCompatibleOp.template operator()<lhpOpCreateRefSysLM>(_R("Create RefSys Landmarks"), _R("Create/Derive"));

	}

}


class App : public wxApp
{
public:
	App();

	App(const App&) = delete;

	App& operator=(const App&) = delete;

	~App() override;

	bool OnInit() override;

	int OnExit() override;

	void OnAbout(wxCommandEvent& event);
};

wxDECLARE_APP(App);

App::App()
{
#ifdef WIN32
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF) | _CRTDBG_LEAK_CHECK_DF);
#endif
	SetVendorName("Company");
	SetAppName("App");
	SetAppDisplayName("App");
	Bind(wxEVT_MENU, &App::OnAbout, this, wxID_ABOUT);
}

App::~App() = default;

bool App::OnInit()
{
	if (!wxApp::OnInit())
	{
		return false;
	}

	model::data::AttributeFactory::Initialize();

	ItemFactory::Initialize();

	VMEFactory::Initialize();

	mafPipeFactoryVME::Initialize();

	auto frame = new ConfiguredAppFrame<gui::wx::PaneFrame<wxAuiMDIParentFrame>>
		(
			nullptr, wxID_ANY,
			GetAppDisplayName(),
			wxDefaultPosition,
			wxWindow::FromDIP(wxSize(1280, 720), nullptr)
		);
	frame->SetIcon(wxICON(APP));
	frame->Show();

	SetTopWindow(frame);//may be needed in case of splash
	
	return true;
}

int App::OnExit()
{
	return wxApp::OnExit();
}

void App::OnAbout(wxCommandEvent& WXUNUSED(event))
{
	wxAboutDialogInfo info;
	info.SetVersion(FTK_VERSION);
	wxAboutBox(info);
}

wxIMPLEMENT_APP(App);
