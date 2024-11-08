#include "mafEvent.h"

#include "mafView.h"
#include "mafOp.h"
#include "mafNode.h"
#include "ftk/Base/String.h"
#include "mafMatrix.h"

mafEvent::mafEvent() : mafEventBase(nullptr, 9999) {}
mafEvent::mafEvent(void* sender, int id)
	: mafEventBase(sender, id)
	, m_Arg(0)
{
}

mafEvent* mafEvent::Copy() 
{
  mafEvent* e = new mafEvent;
  e->SetSender(m_Sender);
  e->SetId(m_Id);
  e->SetBool(m_Bool);
  e->SetArg(m_Arg);
  e->m_Double = m_Double;
  e->m_MAFString = m_MAFString;
  e->m_View  = m_View;
  e->m_Vme = m_Vme;
  e->m_Op = m_Op;
  e->m_MafObject = m_MafObject;
  e->m_x = m_x;
  e->m_y = m_y;
  e->m_width = m_width;
  e->m_height = m_height;
  e->m_WidgetData = m_WidgetData;
#ifdef MAF_USE_WX
  e->m_Win = m_Win;
  e->m_UpdateUIEvent = m_UpdateUIEvent;
  e->m_WxObj = m_WxObj;
#endif
#ifdef MAF_USE_VTK
  e->m_VtkProp = m_VtkProp;
  e->m_Matrix = m_Matrix;
  e->m_Matrix2 = m_Matrix2;
  e->m_VtkObj = m_VtkObj;
#endif
  return e;
}

void mafEvent::DeepCopy(const mafEventBase *maf_event)
{
  //m_Sender  = ((mafEvent *)maf_event)->GetSender();
  m_Id      = ((mafEvent *)maf_event)->GetId();
  m_Arg     = ((mafEvent *)maf_event)->GetArg();
  m_Bool    = ((mafEvent *)maf_event)->GetBool();
  m_Double  = ((mafEvent *)maf_event)->GetDouble();
  m_MAFString  = ((mafEvent *)maf_event)->GetString();
  m_Vme     = ((mafEvent *)maf_event)->GetVme();
  m_View    = ((mafEvent *)maf_event)->GetView();
  m_Op      = ((mafEvent *)maf_event)->GetOp();
  m_Matrix  = ((mafEvent *)maf_event)->GetMatrix();
  m_Matrix2 = ((mafEvent *)maf_event)->GetMatrix2();
  m_MafObject = ((mafEvent *)maf_event)->GetMafObject();
  m_x         = ((mafEvent *)maf_event)->GetX();
  m_y         = ((mafEvent *)maf_event)->GetY();
  m_width     = ((mafEvent *)maf_event)->GetWidth();
  m_height    = ((mafEvent *)maf_event)->GetHeight();
  ((mafEvent *)maf_event)->GetWidgetData(m_WidgetData);
#ifdef MAF_USE_WX
  m_WxObj   = ((mafEvent *)maf_event)->GetWxObj();
  m_Win     = ((mafEvent *)maf_event)->GetWin();
#endif
#ifdef MAF_USE_VTK
  m_VtkObj  = ((mafEvent *)maf_event)->GetVtkObj();
#endif
}

void mafEvent::GetWidgetData(WidgetDataType &widget_data)
{
  widget_data = m_WidgetData;
}

void mafEvent::SetWidgetData(WidgetDataType &widget_data)
{
  m_WidgetData = widget_data;
}

void mafEvent::SetVmeVector(std::vector<mafNode*> vmeVector)
{
  m_VmeVector.swap(vmeVector);
}

std::vector<mafNode*> mafEvent::GetVmeVector()
{
  return m_VmeVector;
}

mafString* mafEvent::GetString()
{
  return m_MAFString;
}

void mafEvent::SetString( mafString *s )
{
  m_MAFString = s;
}
