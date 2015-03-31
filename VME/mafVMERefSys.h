/*=========================================================================

 Program: MAF2
 Module: mafVMERefSys
 Authors: Marco Petrone, Paolo Quadrani, Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafVMERefSys_h
#define __mafVMERefSys_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVMERefSysAbstract.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------

/* mafVMERefSys
 A procedural reference system */
class MAF_EXPORT mafVMERefSys : public mafVMERefSysAbstract
{
public:
  mafTypeMacro(mafVMERefSys,mafVMERefSysAbstract);

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);

  enum REFSYS_WIDGET_ID
  {
    ID_REF_SYS_ORIGIN = Superclass::ID_LAST,
    ID_POINT1,
    ID_POINT2,
    ID_RADIO,
    ID_FIXED,
    ID_LAST
  };

  /** Precess events coming from other objects */ 
  virtual void OnEvent(mafEventBase *maf_event);

  /** Copy the contents of another VME-RefSys into this one. */
  virtual int DeepCopy(mafNode *a);

  /** Compare with another VME-RefSys. */
  virtual bool Equals(mafVME *vme);

  mafVME *GetOriginVME();
  mafVME *GetPoint1VME();
  mafVME *GetPoint2VME();

protected:
  mafVMERefSys();
  virtual ~mafVMERefSys();

  virtual void CalculateMatrix(mafMatrix& mat, mafTimeStamp ts = -1);
  virtual int InternalStore(mafStorageElement *parent);
  virtual int InternalRestore(mafStorageElement *node);

  /** Internally used to create a new instance of the GUI.*/
  virtual mafGUI *CreateGui();

  mafString m_Point1VmeName;
  mafString m_Point2VmeName;
  mafString m_OriginVmeName;
  int       m_Radio;

private:
  mafVMERefSys(const mafVMERefSys&); // Not implemented
  void operator=(const mafVMERefSys&); // Not implemented
  // TODO: REFACTOR THIS 
  // Move these methods in a suitable logging helper class
  void LogVector3( double *vector , const char *logMessage /*= NULL*/ );
  void LogPoint3( double *point, const char *logMessage );
  void LogVTKMatrix4x4( vtkMatrix4x4 *mat, const char *logMessage );
  void LogMAFMatrix4x4( mafMatrix *mat, const char *logMessage );
};
#endif
