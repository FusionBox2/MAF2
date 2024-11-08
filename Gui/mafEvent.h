#pragma once

#include "ftkConfigure.h"

#include "mafDefines.h"   // mafDefines should alway be included as first
#include "mafEventBase.h" // base class for mafEvent
#include "mafBaseEventHandler.h"

//----------------------------------------------------------------------------
// Forward References:
//----------------------------------------------------------------------------
class mafView;
class mafOp;
class mafNode;
class mafMatrix;
class mafString;

#ifdef MAF_USE_VTK
  class vtkObject;
  class vtkProp;  
#endif

//----------------------------------------------------------------------------
// mafEvent :
//----------------------------------------------------------------------------
/** mafEvent - Class implementing MAF application events.
  This class implements a type of event object similar to original mafEvent, where
  all kind of possible information traveling around the MAF is defined. 
  @sa mafEventBase mafSubject mafObserver
*/
class MAF_EXPORT mafEvent : public mafEventBase
{
public:
  mafTypeMacroN(mafEvent);

  mafEvent();
  mafEvent(void* sender, int id);

  mafEvent* Copy();
  void DeepCopy(const mafEventBase *maf_event) override;

  intptr_t          GetArg()     {return m_Arg;};
  bool              GetBool()    {return m_Bool;};
  double            GetDouble()   {return m_Double;};
  mafString*        GetString();
  mafView*          GetView()    {return m_View;};
  mafNode*          GetVme()     {return m_Vme;};
  mafOp*            GetOp()      {return m_Op;};
  std::shared_ptr<mafMatrix>         GetMatrix()    {return m_Matrix;};
  std::shared_ptr<mafMatrix> GetMatrix2() {return m_Matrix2;};
  mafObject*        GetMafObject() {return m_MafObject;}

  int GetX() {return m_x;};
  int GetY() {return m_y;};
  int GetWidth() {return m_width;};
  int GetHeight() {return m_height;};

  /** set call data, data sent by sender (event's invoker) to all observers. 
  Be aware that the vmeVector argument will be empty after the Set */
  void SetVmeVector(std::vector<mafNode*> vmeVector);

  /** return call data, data sent by sender (event's invoker) to all observers */
  std::vector<mafNode*> GetVmeVector();

  void GetWidgetData(WidgetDataType &widget_data);

  void SetArg(intptr_t arg)         { m_Arg = arg;};
  void SetBool(bool b)          { m_Bool = b;};
  void SetDouble(double f)      { m_Double = f;};
  void SetString(mafString *s);
  void SetView(mafView* view)   { m_View = view;};
  void SetVme(mafNode* vme)     { m_Vme = vme;};
  void SetOp(mafOp* op)         { m_Op = op;};
  void SetMatrix(std::shared_ptr<mafMatrix> mat)       { m_Matrix = mat;};
  void SetMatrix2(std::shared_ptr<mafMatrix> mat2)   { m_Matrix2 =mat2;};
  void SetMafObject(mafObject* obj)    { m_MafObject = obj;}
  void SetWidgetData(WidgetDataType &widget_data);
  void SetX(int x) { m_x = x; };
  void SetY(int y) { m_y = y; };
  void SetWidth(int w) { m_width = w; };
  void SetHeight(int h) { m_height = h; };

protected:
  intptr_t         m_Arg = 0;
  bool             m_Bool = false;
  double           m_Double = 0.0;
  mafString       *m_MAFString = nullptr;

  mafNode         *m_Vme = nullptr;
  mafView         *m_View = nullptr;
  mafOp           *m_Op = nullptr;
  std::shared_ptr<mafMatrix> m_Matrix;
  std::shared_ptr<mafMatrix> m_Matrix2;
  mafObject       *m_MafObject = nullptr;
  std::vector<mafNode*> m_VmeVector;
  WidgetDataType   m_WidgetData;

  int m_x = 0;
  int m_y = 0;
  int m_width = 0;
  int m_height = 0;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#ifdef MAF_USE_WX
public:
  wxWindow*        GetWin()       {return m_Win;};
  wxUpdateUIEvent* GetUIEvent()   {return m_UpdateUIEvent;};
  wxObject*        GetWxObj()     {return m_WxObj;};

  void SetWin(wxWindow* win)            { m_Win = win;};
  void SetUIEvent(wxUpdateUIEvent *e)   { m_UpdateUIEvent =e;};
  void SetWxObj(wxObject *wxobj)        { m_WxObj = wxobj;};

protected:
  wxWindow        *m_Win = nullptr;
  wxUpdateUIEvent *m_UpdateUIEvent = nullptr; 
  wxObject        *m_WxObj = nullptr; 
#endif

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#ifdef MAF_USE_VTK
public:
  vtkProp*         GetProp()      {return m_VtkProp;};
  vtkObject*       GetVtkObj()    {return m_VtkObj;};

  void SetProp(vtkProp* prop)             { m_VtkProp = prop;};
  void SetVtkObj(vtkObject *vtkobj)       { m_VtkObj = vtkobj;};

protected:
  vtkProp         *m_VtkProp = nullptr;
  vtkObject       *m_VtkObj = nullptr; 
#endif  
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

};
