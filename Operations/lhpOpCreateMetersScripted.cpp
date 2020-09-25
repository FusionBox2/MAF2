/*=========================================================================

 Program: MAF2
 Module: lhpOpCreateMetersScripted
 Authors: Paolo Quadrani
 
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


#include "lhpOpCreateMetersScripted.h"
#include "mafDecl.h"
#include "mafEvent.h"

#include "mafVMERoot.h"
#include "mafVMEGroup.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMEMeter.h"

#include <fstream>
#include <sstream>
#include <string>

namespace
{
	std::vector<std::vector<std::string> > ReadDictionary(const mafString& dictFName)
	{
		std::vector<std::vector<std::string> > dictionary;
		std::ifstream ifs(dictFName);
		std::string str;
		while (getline(ifs, str))
		{
			std::istringstream iss(str);
			std::vector<std::string> entry;
			std::copy(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), std::back_inserter(entry));
			if (entry.size() >= 3)
				dictionary.push_back(std::move(entry));
		}
		return dictionary;
	}
}

//----------------------------------------------------------------------------
mafCxxTypeMacro(lhpOpCreateMetersScripted);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
lhpOpCreateMetersScripted::lhpOpCreateMetersScripted(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
  m_Group   = NULL;
}
//----------------------------------------------------------------------------
lhpOpCreateMetersScripted::~lhpOpCreateMetersScripted()
//----------------------------------------------------------------------------
{
  mafDEL(m_Group);
}
//----------------------------------------------------------------------------
mafOp* lhpOpCreateMetersScripted::Copy()   
//----------------------------------------------------------------------------
{
	return new lhpOpCreateMetersScripted(GetLabel());
}
//----------------------------------------------------------------------------
bool lhpOpCreateMetersScripted::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsMAFType(mafVMELandmarkCloud));
}
//----------------------------------------------------------------------------
void lhpOpCreateMetersScripted::OpRun()
//----------------------------------------------------------------------------
{
  mafNEW(m_Group);
  auto inputLMC = mafVMELandmarkCloud::SafeDownCast(m_Input);
  m_Group->SetName(inputLMC->GetName() + "_scripted_meters");
  m_Output = m_Group;

  mafString filename = mafGetOpenFile(mafGetApplicationDirectory(), "dic files (*.dic)|*.dic", "Choose dictionary");
  if (filename.IsEmpty())
  {
	  mafEventMacro(mafEvent(this, OP_RUN_CANCEL));
	  return;
  }
  std::vector<std::vector<std::string> > dictionary = ReadDictionary(filename);
  for (const auto& entry : dictionary)
  {
	  mafVMEMeter *meter;
	  mafNEW(meter);
	  mafString name = "meter";
	  for (const auto& el : entry)
	  {
		  name += "_";
		  name += el.c_str();
	  }
	  meter->SetName(name);
	  meter->SetMeterMeasureTypeToAbsolute();
	  auto mType = entry.back();
	  if (mType == "la")
		  meter->SetMeterModeToLineAngle();
	  else if (mType == "ld")
		  meter->SetMeterModeToLineDistance();
	  else //if (mType == "pd")
		  meter->SetMeterModeToPointDistance();


	  if (entry.size() > 4)
	  {
		  meter->SetLineAngle2(1);
	  }

	  int idx1 = inputLMC->FindLandmarkIndex(entry[0].c_str());
	  int idx2 = inputLMC->FindLandmarkIndex(entry[1].c_str());
	  int idx3 = entry.size() > 3 ? inputLMC->FindLandmarkIndex(entry[2].c_str()) : -1;
	  int idx4 = entry.size() > 4 ? inputLMC->FindLandmarkIndex(entry[3].c_str()) : -1;
	  if (idx1 >= 0)
		  meter->SetLink("StartVME", inputLMC, idx1);
	  if (idx2 >= 0)
		  meter->SetLink("EndVME1", inputLMC, idx2);
	  if (idx3 >= 0)
		  meter->SetLink("EndVME2", inputLMC, idx3);
	  if (idx4 >= 0)
	  {
		  
		  meter->SetLink("StartVME2", inputLMC, idx4);
		  
	  }
	  meter->ReparentTo(m_Group);
	  mafDEL(meter);
  }

  //inputLMC->GetLandmarkName()
	  //inputLMC->GetNumberOfLandmarks()
  mafEventMacro(mafEvent(this,OP_RUN_OK));
}
//----------------------------------------------------------------------------
void lhpOpCreateMetersScripted::OpDo()
//----------------------------------------------------------------------------
{
  m_Group->ReparentTo(m_Input->GetParent());
}
