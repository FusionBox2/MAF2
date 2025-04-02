#include "medOpExporterAnalogWS.h"

#include <wx/busyinfo.h>
#include "mafGUI.h"

#include "mafTagArray.h"
#include "mafVMEOutputScalarMatrix.h"

#include <fstream>
#include <iostream>

#include <vnl/vnl_matrix.h>

using namespace std;

#define TAG_FORMAT "ANALOG"
#define FREQ 1.00


//----------------------------------------------------------------------------
medOpExporterAnalogWS::medOpExporterAnalogWS(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_EXPORTER;
	m_Canundo	= true;
	m_File		= _R("");

  m_Analog = NULL;
}
//----------------------------------------------------------------------------
medOpExporterAnalogWS::~medOpExporterAnalogWS()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
bool medOpExporterAnalogWS::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsA("medVMEAnalog"));
}
//----------------------------------------------------------------------------
mafOp* medOpExporterAnalogWS::Copy()   
//----------------------------------------------------------------------------
{
	medOpExporterAnalogWS *cp = new medOpExporterAnalogWS(GetLabel());
	cp->m_File = m_File;
	return cp;
}
//----------------------------------------------------------------------------
void medOpExporterAnalogWS::OpRun()   
//----------------------------------------------------------------------------
{
	mafString proposed = mafGetApplicationDirectory();
  proposed += _R("/Data/External/");
	proposed += GetInput()->GetName();
	proposed += _R(".csv");
	
  mafString wildc = _R("ASCII CSV file (*.csv)|*.csv");
	mafString f = mafGetSaveFile(proposed,wildc); 

	int result = OP_RUN_CANCEL;
	if(!f.empty())
	{
		m_File = f;
		Write();
		result = OP_RUN_OK;
	}
	{mafEvent evUnq(this,result); InvokeEvent(evUnq);}
}

//----------------------------------------------------------------------------
void medOpExporterAnalogWS::Write()   
//----------------------------------------------------------------------------
{
  if (!m_TestMode)
  {
    wxSetCursor(wxCursor(wxCURSOR_WAIT));
	  {mafEvent evUnq(this,PROGRESSBAR_SHOW); InvokeEvent(evUnq);}
  }
  
  m_Analog = medVMEAnalog::SafeDownCast(GetInput()).get();
  mafTagItem *tag_sig = m_Analog->GetTagArray()->GetTag(_R("SIGNALS_NAME"));
  int n_sig = (tag_sig) ? tag_sig->GetNumberOfComponents() : 0;

  mafString empty;

  std::ofstream f_Out(m_File.GetCStr());
  if (!f_Out.bad())
  {
    // Add ANALOG tag
    f_Out << TAG_FORMAT << "\n";

    // Add the first row containing the frequency
    f_Out << FREQ << ",\n";

    //Add the third line containing the signal names
    f_Out << "FRAME,";
    if(n_sig > 0)
      f_Out << tag_sig->GetValue(0).GetCStr();
    for (int i=1;i<n_sig;i++)
    {
      if (empty != tag_sig->GetValue(i))
      {
        f_Out << "," << tag_sig->GetValue(i).GetCStr();
      }
    }
    f_Out << "\n";
    
    //Add a blank line 
    f_Out << "\n";

    //Add times and values; time is always the first row
    vnl_matrix<double> emgMatrix = m_Analog->GetScalarOutput()->GetScalarData();
    for (int i=0;i<emgMatrix.columns();i++)
    {
      // Add time
      double time = emgMatrix.get(0,i);
      f_Out << time << ",";
      for (int j=1;j<emgMatrix.rows()-1;j++)
      {
        // Add all values but last one
        f_Out << emgMatrix.get(j,i) << ",";
      }
      if (emgMatrix.rows()>0)
      {
        // Add last one
        if (i==emgMatrix.columns()-1)
        {
          f_Out << emgMatrix.get(emgMatrix.rows()-1,i);
        }
        else
        {
          f_Out << emgMatrix.get(emgMatrix.rows()-1,i) << "\n";
        }
      }
      if (!m_TestMode)
      {
        {mafEvent evUnq(this,PROGRESSBAR_SET_VALUE); evUnq.SetArg(((double) i)/((double) emgMatrix.columns())*100.); InvokeEvent(evUnq);}
      }
    }
    
    f_Out.close();
  }  

  if (!m_TestMode)
  {
    {mafEvent evUnq(this,PROGRESSBAR_HIDE); InvokeEvent(evUnq);}
    wxSetCursor(wxCursor(wxCURSOR_DEFAULT));
  }
}