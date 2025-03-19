#include "mafEvent.h"

#include "mafView.h"
#include "mafOp.h"
#include "mafNode.h"
#include "ftk/Base/String.h"
#include "mafMatrix.h"

#ifdef MAF_USE_VTK
  #include "vtkObject.h"
  #include "vtkProp.h"
#endif

//----------------------------------------------------------------------------
// mafEvent CTOR's
//----------------------------------------------------------------------------
mafEvent::mafEvent() = default;
mafEvent::mafEvent(void *sender, int id) : mafEventBase(sender, id) { Init();}
		
//----------------------------------------------------------------------------
void mafEvent::Log() 
//----------------------------------------------------------------------------
{
  /*mafString s = _R("[EV]");
               //s += _R(" sender= ") + mafToString((intptr_t)m_Sender);
               s += _R(" ID= ") + mafIdString(m_Id);
  if(m_Arg)    s += _R(" arg= ") + mafToString(m_Arg);
  if(m_Bool)   s += _R(" bool= ") + mafToString((int)m_Bool);
  if(m_Double)  s += _R(" double= ") + mafToString(m_Double);
  if(m_MAFString) s += _R(" string= ") + *m_MAFString;
  if(m_View)   s += _R(" view= ") + mafToString((intptr_t)m_View) + _R(" : ") + m_View->GetLabel();
  if(m_x)      s += _R(" x= ") + mafToString(m_x);
  if(m_y)      s += _R(" y= ") + mafToString(m_y);
  if(m_width)  s += _R(" width= ") + mafToString(m_width);
  if(m_height) s += _R(" height= ") + mafToString(m_height);
  if(m_Vme)    s += _R(" vme= ") + mafToString((intptr_t)m_Vme) + _R(" : ") + m_Vme->GetName();
  if(m_Op)     s += _R(" op= ") + mafToString((intptr_t)m_Op) + _R(" : ") + m_Op->GetLabel();
#ifdef MAF_USE_WX
  if(m_Win)    s += _R(" win= ") + mafToString((intptr_t)m_Win);
  if(m_UpdateUIEvent)   s += _R(" ui_evt= ") + mafToString((intptr_t)m_UpdateUIEvent);
  if(m_WxObj)  s += _R(" wxobj= ") + mafToString((intptr_t)m_WxObj);
#endif
#ifdef MAF_USE_VTK
  if(m_VtkProp)   s += _R(" prop= ") + mafToString((intptr_t)m_VtkProp);
  if(m_Matrix) s += _R(" matrix= ") + mafToString((intptr_t)m_Matrix.get());
  if(m_Matrix2)s += _R(" matrix= ") + mafToString((intptr_t)m_Matrix2.get());
  if(m_VtkObj) s += _R(" vtkobj= ") + mafToString((intptr_t)m_VtkObj) + _R(" : ") + _R(m_VtkObj->GetClassName());
#endif
  if(m_MafObject) s += _R(" mafobj= ") + mafToString((intptr_t)m_MafObject) + _R(" : ") + _R(m_MafObject->GetTypeName());

  mafLogMessage(_M(s));*/
}
//----------------------------------------------------------------------------
mafEvent* mafEvent::Clone() const 
//----------------------------------------------------------------------------
{
  mafEvent *e	= new mafEvent(m_Sender,m_Id);
  e->DeepCopy(this);
  return e;
}
//----------------------------------------------------------------------------
void mafEvent::DeepCopy(const mafEventBase *maf_base_event)
//----------------------------------------------------------------------------
{
  Superclass::DeepCopy(maf_base_event);
  auto maf_event = static_cast<const mafEvent*>(maf_base_event);
  m_Arg        = maf_event->GetArg();
  m_Bool       = maf_event->GetBool();
  m_Double     = maf_event->GetDouble();
  m_MAFString  = maf_event->GetString();
  m_Vme        = maf_event->GetVme();
  m_View       = maf_event->GetView();
  m_Op         = maf_event->GetOp();
  m_Matrix     = maf_event->GetMatrix();
  m_Matrix2    = maf_event->GetMatrix2();
  m_MafObject  = maf_event->GetMafObject();
  m_VmeVector  = maf_event->GetVmeVector();
  m_x          = maf_event->GetX();
  m_y          = maf_event->GetY();
  m_width      = maf_event->GetWidth();
  m_height     = maf_event->GetHeight();
  maf_event->GetWidgetData(m_WidgetData);
#ifdef MAF_USE_WX
  m_WxObj      = maf_event->GetWxObj();
  m_Win        = maf_event->GetWin();
  m_UpdateUIEvent = maf_event->GetUIEvent();
#endif
#ifdef MAF_USE_VTK
  m_VtkProp     = maf_event->GetProp();
  m_VtkObj = maf_event->GetVtkObj();
#endif
}
//----------------------------------------------------------------------------
void mafEvent::Init() 
//----------------------------------------------------------------------------
{
  /*bool *verbose = GetLogVerbose();
  if (id != UPDATE_UI && (*verbose))
  {
    mafString sender_type = _R("Sent Event. Sender: ");
    try
    {
      mafObject *obj = (mafObject *)sender;
      sender_type += _R(typeid(*obj).name());
    }
    catch (...)
    {
      sender_type = _R("not mafObject (");
      sender_type += mafToString((intptr_t)sender);
      sender_type += _R(")");
    }
    mafString id_name = mafIdString(id);
    mafString msg = sender_type + _R("  ID: ") + id_name;
    mafLogMessage(_M(msg));
  }*/
}
//----------------------------------------------------------------------------
void mafEvent::Initialized() 
//----------------------------------------------------------------------------
{
  if(m_LogMode) Log();
}
//----------------------------------------------------------------------------
/** turn on/off Auto-Logging of every Event */
void mafEvent::SetLogMode(int logmode)
//----------------------------------------------------------------------------
{
	m_LogMode = logmode;
}
//----------------------------------------------------------------------------
void mafEvent::GetWidgetData(WidgetDataType &widget_data) const
//----------------------------------------------------------------------------
{
  widget_data = m_WidgetData;
}
//----------------------------------------------------------------------------
void mafEvent::SetWidgetData(WidgetDataType &widget_data)
//----------------------------------------------------------------------------
{
  m_WidgetData = widget_data;
}
//------------------------------------------------------------------------------
void mafEvent::SetVmeVector(std::vector<mafNode*> vmeVector)
//------------------------------------------------------------------------------
{
  m_VmeVector.swap(vmeVector);
}

//------------------------------------------------------------------------------
std::vector<mafNode*> mafEvent::GetVmeVector() const
//------------------------------------------------------------------------------
{
  return m_VmeVector;
}
//------------------------------------------------------------------------------
mafString* mafEvent::GetString() const
//------------------------------------------------------------------------------
{
  return m_MAFString;
}
//------------------------------------------------------------------------------
void mafEvent::SetString( mafString *s )
//------------------------------------------------------------------------------
{
  m_MAFString = s;
}

//------------------------------------------------------------------------------
void mafEvent::SetX(int x)
//------------------------------------------------------------------------------
{
  m_x = x;
}
//------------------------------------------------------------------------------
void mafEvent::SetY(int y)
//------------------------------------------------------------------------------
{
  m_y = y;
}
//------------------------------------------------------------------------------
void mafEvent::SetWidth(int width)
//------------------------------------------------------------------------------
{
  m_width = width;
}
//------------------------------------------------------------------------------
void mafEvent::SetHeight(int height)
//------------------------------------------------------------------------------
{
  m_height = height;
}

//----------------------------------------------------------------------------
int mafEvent::m_LogMode = 0;
//----------------------------------------------------------------------------
