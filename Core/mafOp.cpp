#include "mafOp.h"
#include "mafDecl.h"
#include "ftk/Base/Object.h"
#include "mafGUI.h"
#include "mafGUIHolder.h"
#include "mafNode.h"

//----------------------------------------------------------------------------
mafOp::mafOp(const mafString &label) : m_Label(label)
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp::mafOp() : m_Label(_R("default op name"))
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp::~mafOp() = default;
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void mafOp::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	InvokeEvent(*maf_event);
}
//----------------------------------------------------------------------------
bool mafOp::Accept(mafNode* vme)
//----------------------------------------------------------------------------
{
  return false;
}
//----------------------------------------------------------------------------
void mafOp::OpRun()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafOp::OpDo()
//----------------------------------------------------------------------------
{
  if (m_Output)
  {
    m_Input->AddChild(m_Output);
    //{mafEvent evUnq(this, VME_ADD, m_Output); InvokeEvent(evUnq);}
    {mafEvent evUnq(this,CAMERA_UPDATE); InvokeEvent(evUnq);}
  }
}
//----------------------------------------------------------------------------
void mafOp::OpUndo()
//----------------------------------------------------------------------------
{
  if (m_Output)
  {
    {mafEvent evUnq(this, VME_REMOVE); evUnq.SetVme(m_Output.get()); InvokeEvent(evUnq);}
    {mafEvent evUnq(this,CAMERA_UPDATE); InvokeEvent(evUnq);}
  }
}

void mafOp::SetInput(std::shared_ptr<mafNode> vme)
{
  m_Input = vme;
}

std::shared_ptr<mafNode> mafOp::GetInput() const
{
  return m_Input;
}

/** Return the mafNode result of the operation.*/
std::shared_ptr<mafNode> mafOp::GetOutput() const
{
  return m_Output;
}

void mafOp::SetOutput(std::shared_ptr<mafNode> output)
{
  m_Output = output;
}


//----------------------------------------------------------------------------
mafOp* mafOp::Copy()
//----------------------------------------------------------------------------
{
   return NULL;
}
//----------------------------------------------------------------------------
bool mafOp::CanUndo()
//----------------------------------------------------------------------------
{
  return m_Canundo;
}
//----------------------------------------------------------------------------
bool mafOp::IsCompatible(long state)
//----------------------------------------------------------------------------
{
  return (m_Compatibility & state) != 0;
}

//----------------------------------------------------------------------------
int mafOp::GetType()
//----------------------------------------------------------------------------
{
  return m_OpType;
}
//----------------------------------------------------------------------------
void mafOp::ShowGui()
//----------------------------------------------------------------------------
{
  if(!m_Gui)
    return;
  m_Gui->Collaborate(m_CollaborateStatus);
  m_Guih = new mafGUIHolder(mafGetFrame(),-1);
  m_Guih->Put(m_Gui);
	
  mafString menu_codes=mafStripMenuCodes(m_Label);
  mafString title = _R(" ") + menu_codes + _R(" parameters:");
  m_Guih->SetTitle(title);
  { mafEvent evUnq(this, OP_SHOW_GUI); evUnq.SetWin(m_Guih); InvokeEvent(evUnq); }
}
//----------------------------------------------------------------------------
void mafOp::HideGui()
//----------------------------------------------------------------------------
{
  if(!m_Gui)
    return;
  {mafEvent evUnq(this,OP_HIDE_GUI); evUnq.SetWin(m_Guih); InvokeEvent(evUnq);}
  delete m_Guih;
  m_Guih = nullptr;
  m_Gui = nullptr;
}
//----------------------------------------------------------------------------
bool mafOp::OkEnabled()
//----------------------------------------------------------------------------
{
  if(!m_Gui) return false;
  wxWindow* win = m_Gui->FindWindow(wxOK);
  if (!win) return false;
  return win->IsEnabled();
}
//----------------------------------------------------------------------------
void mafOp::ForceStopWithOk()
//----------------------------------------------------------------------------
{
  mafEvent e(this, OP_RUN_OK);
  OnEvent(&e);
}
//----------------------------------------------------------------------------
void mafOp::ForceStopWithCancel()
//----------------------------------------------------------------------------
{
  mafEvent e(this, OP_RUN_CANCEL);
  OnEvent(&e);
}
//----------------------------------------------------------------------------
void mafOp::OpStop(int result)
//----------------------------------------------------------------------------
{
  HideGui();
  {mafEvent evUnq(this,result); InvokeEvent(evUnq);}        
}
//----------------------------------------------------------------------------
void mafOp::Collaborate(bool status)
//----------------------------------------------------------------------------
{
  m_CollaborateStatus = status; 
}
