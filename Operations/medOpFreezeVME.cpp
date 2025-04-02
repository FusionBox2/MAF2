/*=========================================================================

Program: MAF2Medical
Module: medOpFreezeVME
Authors: Daniele Giunchi , Stefano Perticoni

Copyright (c) B3C
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medOpFreezeVME.h"
#include "wx/busyinfo.h"

#include "mafDecl.h"
#include "mafVME.h"

#include "mafVMEGenericAbstract.h"
#include "mafVMERoot.h"
#include "mafVMERefSys.h"
#include "mafVMEVolume.h"

#include "mafVMESurface.h"
#include "mafVMEPolyline.h"
#include "mafVMEPolylineSpline.h"
#include "mafVMESurfaceParametric.h"
#include "mafVMEMeter.h"
#include "mafVMESlicer.h"
#include "mafVMEProber.h"
#include "medVMEWrappedMeter.h"
#include "medVMELabeledVolume.h"

#include "vtkRectilinearGrid.h"
#include "mmaMaterial.h"
#include "mmaVolumeMaterial.h"
#include "mafGUI.h"
#include "mafAbsMatrixPipe.h"

#include "vtkPolyData.h"
#include "vtkImageData.h"
#include "medVMEComputeWrapping.H"



//----------------------------------------------------------------------------
mafCxxTypeMacro(medOpFreezeVME);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medOpFreezeVME::medOpFreezeVME(const mafString& label) : Superclass(label)
	//----------------------------------------------------------------------------
{
	m_OpType  = OPTYPE_OP;
	m_Canundo = true;
	m_InputPreserving = true;

}
//----------------------------------------------------------------------------
medOpFreezeVME::~medOpFreezeVME( ) 
	//----------------------------------------------------------------------------
{

}
//----------------------------------------------------------------------------
bool medOpFreezeVME::Accept(mafNode *node)
	//----------------------------------------------------------------------------
{
	return (node->IsMAFType(mafVME) && 
		!node->IsMAFType(mafVMEGenericAbstract) && 
		!node->IsMAFType(mafVMERoot) &&
		//!node->IsMAFType(mafVMERefSysAbstract) &&
		!node->IsMAFType(mafVMEVolume) ); //return if is a procedural vme
}
//----------------------------------------------------------------------------
mafOp* medOpFreezeVME::Copy()   
	//----------------------------------------------------------------------------
{
	medOpFreezeVME *cp = new medOpFreezeVME(GetLabel());
	return cp;
}
//----------------------------------------------------------------------------
void medOpFreezeVME::OpRun()   
	//----------------------------------------------------------------------------
{
	if(!m_TestMode)
	{
		m_Gui = new mafGUI(this);
		ShowGui();
	}

	//control the output and create the right vme
	auto vme = mafVME::SafeDownCast(GetInput());
	vme->Update();
	mafVMEOutput *output = vme->GetOutput();
	output->Update();

	if(vtkImageData *imageData = vtkImageData::SafeDownCast(output->GetVTKData()))
	{
		if(auto labeledVolume = medVMELabeledVolume::SafeDownCast(vme))
		{
			auto material = labeledVolume->GetMaterial();

			auto newVolume = mafVMEVolumeGray::NewSPtr();
			newVolume->SetName(labeledVolume->GetName());
			newVolume->SetData(imageData,labeledVolume->GetTimeStamp());
			newVolume->Update();

			if(material)
			{
				newVolume->GetMaterial()->DeepCopy(material.get());
				newVolume->GetMaterial()->UpdateProp();
			}

			newVolume->SetMatrix(*labeledVolume->GetOutput()->GetMatrix());
			SetOutput(newVolume);
			if (GetOutput())
			{
				mafNode::ReparentTo(GetOutput(), GetInput()->GetParent());
				if(!m_TestMode)
					OpStop(OP_RUN_OK);
			}
		}
	}
	else
		if(vtkRectilinearGrid *rectilinearGrid = vtkRectilinearGrid::SafeDownCast(output->GetVTKData()))
		{
			if(auto labeledVolume = medVMELabeledVolume::SafeDownCast(vme))
			{
				auto material = labeledVolume->GetMaterial();

				auto newVolume = mafVMEVolumeGray::NewSPtr();
				newVolume->SetName(labeledVolume->GetName());
				newVolume->SetData(rectilinearGrid,labeledVolume->GetTimeStamp());
				newVolume->Update();

				if(material)
				{
					newVolume->GetMaterial()->DeepCopy(material.get());
					newVolume->GetMaterial()->UpdateProp();
				}

				newVolume->SetMatrix(*labeledVolume->GetOutput()->GetMatrix());
				SetOutput(newVolume);
				if (GetOutput())
				{
					mafNode::ReparentTo(GetOutput(), GetInput()->GetParent());
					if(!m_TestMode)
						OpStop(OP_RUN_OK);
				}
			}
		}
		else
			if(vtkPolyData *polyData = vtkPolyData::SafeDownCast(output->GetVTKData()))
			{
				if(auto vmeSpline = mafVMEPolylineSpline::SafeDownCast(vme))
				{
					auto material = vmeSpline->GetMaterial();

					auto newPolyline = mafVMEPolyline::NewSPtr();
					newPolyline->SetName(vmeSpline->GetName());
					newPolyline->SetData(polyData,vmeSpline->GetTimeStamp());
					newPolyline->Update();

					if(material)
					{
						newPolyline->GetMaterial()->DeepCopy(material.get());
						newPolyline->GetMaterial()->UpdateProp();
					}
					newPolyline->SetMatrix(*vmeSpline->GetOutput()->GetMatrix());
					SetOutput(newPolyline);
					if (GetOutput())
					{
						mafNode::ReparentTo(GetOutput(), GetInput()->GetParent());
						if(!m_TestMode)
							OpStop(OP_RUN_OK);
					}
				}
				else if(auto vmeSurface = mafVMESurfaceParametric::SafeDownCast(vme))
				{
					auto material = vmeSurface->GetMaterial();

					auto newSurface = mafVMESurface::NewSPtr();
					newSurface->SetName(vmeSurface->GetName());
					newSurface->SetData(polyData,vmeSurface->GetTimeStamp());
					newSurface->Update();

					if(material)
					{
						newSurface->GetMaterial()->DeepCopy(material.get());
						newSurface->GetMaterial()->UpdateProp();
					}

					newSurface->SetMatrix(*vmeSurface->GetOutput()->GetMatrix());
					SetOutput(newSurface);
					if (GetOutput())
					{
						mafNode::ReparentTo(GetOutput(), GetInput()->GetParent());
						if(!m_TestMode)
							OpStop(OP_RUN_OK);
					}
				}
				else if(auto meter = mafVMEMeter::SafeDownCast(vme))
				{
					auto material = meter->GetMaterial();

					auto  newPolyline = mafVMEPolyline::NewSPtr();
					newPolyline->SetName(meter->GetName());
					newPolyline->SetData(polyData,meter->GetTimeStamp());
					newPolyline->Update();

					if(material)
					{
						newPolyline->GetMaterial()->DeepCopy(material.get());
						newPolyline->GetMaterial()->UpdateProp();
					}

					newPolyline->SetMatrix(*meter->GetOutput()->GetMatrix());
					SetOutput(newPolyline);
					if (GetOutput())
					{
						mafNode::ReparentTo(GetOutput(), GetInput()->GetParent());
						if(!m_TestMode)
							OpStop(OP_RUN_OK);
					}
				}
				else if(auto refsys = mafVMERefSys::SafeDownCast(vme))
				{
					auto material = refsys->GetMaterial();

					auto surface = mafVMESurface::NewSPtr();
					surface->SetName(refsys->GetName());
					surface->SetData(polyData,refsys->GetTimeStamp());
					surface->Update();

					if(material)
					{
						surface->GetMaterial()->DeepCopy(material.get());
						surface->GetMaterial()->UpdateProp();
					}

					surface->SetMatrix(*refsys->GetOutput()->GetMatrix());
					SetOutput(surface);
					if (GetOutput())
					{
						mafNode::ReparentTo(GetOutput(), GetInput()->GetParent());
						if(!m_TestMode)
							OpStop(OP_RUN_OK);
					}
				}
				else if(auto slicer = mafVMESlicer::SafeDownCast(vme))
				{
					auto material = slicer->GetMaterial();

					auto  newSurface = mafVMESurface::NewSPtr();
					newSurface->SetName(slicer->GetName());
					newSurface->SetData(polyData,slicer->GetTimeStamp());
					vtkNew<vtkImageData> text;
					text->DeepCopy(slicer->GetSurfaceOutput()->GetTexture());
					newSurface->GetSurfaceOutput()->SetTexture(text);
					newSurface->Update();

					if(material)
					{
						newSurface->GetMaterial()->DeepCopy(material.get());
						newSurface->GetMaterial()->SetMaterialTextureData(newSurface->GetSurfaceOutput()->GetTexture());
						newSurface->GetMaterial()->UpdateProp();
					}

					newSurface->SetMatrix(*slicer->GetOutput()->GetMatrix());
					SetOutput(newSurface);
					if (GetOutput())
					{
						mafNode::ReparentTo(GetOutput(), GetInput()->GetParent());
						if(!m_TestMode)
							OpStop(OP_RUN_OK);
					}

				}
				else if(auto prober = mafVMEProber::SafeDownCast(vme))
				{
					auto material = prober->GetMaterial();

					auto newSurface = mafVMESurface::NewSPtr();
					newSurface->SetName(prober->GetName());
					newSurface->SetData(polyData,prober->GetTimeStamp());
					newSurface->Update();

					if(material)
					{
						newSurface->GetMaterial()->DeepCopy(material.get());
						newSurface->GetMaterial()->UpdateProp();
					}

					newSurface->SetMatrix(*prober->GetOutput()->GetMatrix());
					SetOutput(newSurface);
					if (GetOutput())
					{
						mafNode::ReparentTo(GetOutput(), GetInput()->GetParent());
						if(!m_TestMode)
							OpStop(OP_RUN_OK);
					}
				}
				else if(auto wrappedMeter = medVMEWrappedMeter::SafeDownCast(vme))
				{
					auto material = wrappedMeter->GetMaterial();

					auto newPolyline = mafVMEPolyline::NewSPtr();
					newPolyline->SetName(wrappedMeter->GetName());
					newPolyline->SetData(polyData,wrappedMeter->GetTimeStamp());
					newPolyline->Update();

					if(material)
					{
						newPolyline->GetMaterial()->DeepCopy(material.get());
						newPolyline->GetMaterial()->UpdateProp();
					}

					newPolyline->SetMatrix(*wrappedMeter->GetOutput()->GetMatrix());
					SetOutput(newPolyline);
					if (GetOutput())
					{
						mafNode::ReparentTo(GetOutput(), GetInput()->GetParent());
						if(!m_TestMode)
							OpStop(OP_RUN_OK);
					}
				}
				else if(auto wrappedMeter = medVMEComputeWrapping::SafeDownCast(vme))
				{
					auto material = wrappedMeter->GetMaterial();

					auto newPolyline = mafVMEPolyline::NewSPtr();
					newPolyline->SetName(wrappedMeter->GetName());
					newPolyline->SetData(polyData,wrappedMeter->GetTimeStamp());
					newPolyline->Update();

					if(material)
					{
						newPolyline->GetMaterial()->DeepCopy(material.get());
						newPolyline->GetMaterial()->UpdateProp();
					}

					newPolyline->SetMatrix(*wrappedMeter->GetOutput()->GetMatrix());
					SetOutput(newPolyline);
					if (GetOutput())
					{
						mafNode::ReparentTo(GetOutput(), GetInput()->GetParent());
						if(!m_TestMode)
							OpStop(OP_RUN_OK);
					}
				}
				else
					OpStop(OP_RUN_CANCEL);

			}
			else
				OpStop(OP_RUN_CANCEL);


}
//----------------------------------------------------------------------------
void medOpFreezeVME::OpDo()   
	//----------------------------------------------------------------------------
{
	{mafEvent evUnq(this,CAMERA_UPDATE); InvokeEvent(evUnq);}
}


