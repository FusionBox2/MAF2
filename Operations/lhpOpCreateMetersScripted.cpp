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
		std::ifstream ifs(dictFName.GetCStr());
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
lhpOpCreateMetersScripted::lhpOpCreateMetersScripted(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
}
//----------------------------------------------------------------------------
lhpOpCreateMetersScripted::~lhpOpCreateMetersScripted()
//----------------------------------------------------------------------------
{
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
  auto group = mafVMEGroup::NewSPtr();
  auto inputLMC = mafVMELandmarkCloud::SafeDownCast(GetInput());
  group->SetName(inputLMC->GetName() + _R("_scripted_meters"));
  SetOutput(group);

  mafString filename = mafGetOpenFile(mafGetApplicationDirectory(), _R("dic files (*.dic)|*.dic"), _R("Choose dictionary"));
  if (filename.empty())
  {
	  {mafEvent evUnq(this, OP_RUN_CANCEL); InvokeEvent(evUnq);}
	  return;
  }
  std::vector<std::vector<std::string> > dictionary = ReadDictionary(filename);
  for (const auto& entry : dictionary)
  {
	  auto meter = mafVMEMeter::NewSPtr();
	  mafString name = _R("meter");
	  for (const auto& el : entry)
	  {
		  name += _R("_");
		  name += _R(el.c_str());
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

	  int idx1 = inputLMC->FindLandmarkIndex(_R(entry[0].c_str()));
	  int idx2 = inputLMC->FindLandmarkIndex(_R(entry[1].c_str()));
	  int idx3 = entry.size() > 3 ? inputLMC->FindLandmarkIndex(_R(entry[2].c_str())) : -1;
	  int idx4 = entry.size() > 4 ? inputLMC->FindLandmarkIndex(_R(entry[3].c_str())) : -1;
	  if (idx1 >= 0)
		  meter->SetLink(_R("StartVME"), inputLMC.get(), idx1);
	  if (idx2 >= 0)
		  meter->SetLink(_R("EndVME1"), inputLMC.get(), idx2);
	  if (idx3 >= 0)
		  meter->SetLink(_R("EndVME2"), inputLMC.get(), idx3);
	  if (idx4 >= 0)
	  {
		  
		  meter->SetLink(_R("StartVME2"), inputLMC.get(), idx4);
		  
	  }
	  mafNode::ReparentTo(meter, group.get());
  }
  {mafEvent evUnq(this,OP_RUN_OK); InvokeEvent(evUnq);}
}
