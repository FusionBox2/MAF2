#pragma once

#include "ftkConfigure.h"

#include "mafDefines.h"   // mafDefines should alway be included as first
#include "mafDecl.h"
#include "mafEventBase.h" // base class for mafEvent
#include "mafBaseEventHandler.h"

class mafView;
class mafOp;
class mafNode;
class mafMatrix;
class mafString;
class mafAgent;

#ifdef MAF_USE_VTK
  class vtkObject;
  class vtkProp;  
#endif

class MAF_EXPORT mafEvent : public mafEventBase
{
public:
  mafTypeMacroN(mafEvent);

  mafEvent();                                                         
  mafEvent(void *sender, int id);

  void DeepCopy(const mafEventBase *maf_event) override;

  intptr_t                   GetArg() const {return m_Arg;};
  bool                       GetBool() const {return m_Bool;};
  double                     GetDouble() const {return m_Double;};
  mafString*                 GetString() const;
  mafView*                   GetView() const {return m_View;};
  mafNode*                   GetVme() const {return m_Vme;};
  mafOp*                     GetOp() const {return m_Op;};
  std::shared_ptr<mafMatrix> GetMatrix() const {return m_Matrix;};
  std::shared_ptr<mafMatrix> GetMatrix2() const {return m_Matrix2;};
  mafObject*                 GetMafObject() const {return m_MafObject;}
  mafAgent*  GetAgent() const { return m_Agent; }

  int GetX() const {return m_x;};
  int GetY() const {return m_y;};
  int GetWidth() const {return m_width;};
  int GetHeight() const {return m_height;};

  void SetX(int x);
  void SetY(int y);
  void SetWidth(int width);
  void SetHeight(int height);
  /** set call data, data sent by sender (event's invoker) to all observers. 
  Be aware that the vmeVector argument will be empty after the Set */
  void SetVmeVector(std::vector<mafNode*> vmeVector);

  /** return call data, data sent by sender (event's invoker) to all observers */
  std::vector<mafNode*> GetVmeVector() const;

  void GetWidgetData(WidgetDataType &widget_data) const;

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
  void SetAgent(mafAgent* agent) { m_Agent = agent; }
  void SetWidgetData(WidgetDataType &widget_data);

protected:
  intptr_t         m_Arg;
  bool             m_Bool = false;
  double           m_Double = 0.0;
  mafString       *m_MAFString = nullptr;

  mafNode         *m_Vme = nullptr;
  mafView         *m_View = nullptr;
  mafOp						*m_Op = nullptr;
  std::shared_ptr<mafMatrix> m_Matrix;
  std::shared_ptr<mafMatrix> m_Matrix2;
  mafObject       *m_MafObject = nullptr;
  std::vector<mafNode*> m_VmeVector;
  WidgetDataType   m_WidgetData;
  mafAgent* m_Agent;

  int m_x = 0;
  int m_y = 0;
  int m_width = 0;
  int m_height = 0;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#ifdef MAF_USE_WX
public:
  mafEvent(void *sender, int id, wxWindow        *win,    intptr_t arg=0) = delete;
  mafEvent(void *sender, int id, wxUpdateUIEvent *e,      intptr_t arg=0) = delete;
  mafEvent(void *sender, int id, wxObject        *wxobj,  intptr_t arg=0) = delete;

  wxWindow*        GetWin() const {return m_Win;};
  wxUpdateUIEvent* GetUIEvent() const {return m_UpdateUIEvent;};
  wxObject*        GetWxObj() const {return m_WxObj;};

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
  mafEvent(void *sender, int id, vtkProp         *prop,   mafNode *vme=NULL) = delete;
  mafEvent(void *sender, int id, vtkObject       *vtkobj, intptr_t arg=0) = delete;
  mafEvent(void *sender, int id, vtkObject       *vtkobj, mafString *s) = delete;

  vtkProp*         GetProp() const {return m_VtkProp;};
  vtkObject*       GetVtkObj() const {return m_VtkObj;};

  void SetProp(vtkProp* prop)             { m_VtkProp = prop;};
  void SetVtkObj(vtkObject *vtkobj)       { m_VtkObj = vtkobj;};

protected:
  vtkProp         *m_VtkProp = nullptr;
  vtkObject       *m_VtkObj = nullptr; 
#endif  

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

public:
  mafEvent* Clone() const;

  void Log();
  static void     SetLogMode(int logmode);
  static int      m_LogMode;
  
protected:

  void Init();
  void Initialized();
};
