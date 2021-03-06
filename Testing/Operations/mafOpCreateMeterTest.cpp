/*=========================================================================

 Program: MAF2
 Module: mafOpCreateMeterTest
 Authors: Matteo Giacomoni
 
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

#include <cppunit/config/SourcePrefix.h>
#include "mafOpCreateMeterTest.h"

#include "mafOpCreateMeter.h"
#include "mafVMEMeter.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
class DummyVme : public mafVME
//----------------------------------------------------------------------------
{
public:
  DummyVme(){};
  ~DummyVme(){};

  mafTypeMacro(DummyVme,mafVME);

  /*virtual*/ void SetMatrix(const mafMatrix &mat){};
  /*virtual*/ void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes){};
protected:
private:
};

mafCxxTypeMacro(DummyVme);

//----------------------------------------------------------------------------
class DummyObserver : public mafObserver
//----------------------------------------------------------------------------
{
public:

  DummyObserver() {};
  ~DummyObserver() {};

  /*virtual*/ void OnEvent(mafEventBase *maf_event);

protected:
};

//----------------------------------------------------------------------------
void DummyObserver::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    if(e->GetId() == VME_REMOVE)
    {
      e->GetVme()->ReparentTo(NULL);
    }
  }
}
//----------------------------------------------------------------------------
void mafOpCreateMeterTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafOpCreateMeterTest::setUp()
//----------------------------------------------------------------------------
{
  m_CreateMeter = new mafOpCreateMeter();
  result = false;
}
//----------------------------------------------------------------------------
void mafOpCreateMeterTest::tearDown()
//----------------------------------------------------------------------------
{
  cppDEL(m_CreateMeter);
}
//----------------------------------------------------------------------------
void mafOpCreateMeterTest::TestAccept()
//----------------------------------------------------------------------------
{
  DummyVme *dummy = new DummyVme();

  result = m_CreateMeter->Accept(dummy);
  TEST_RESULT;

  delete dummy;
}
//----------------------------------------------------------------------------
void mafOpCreateMeterTest::TestOpRun()
//----------------------------------------------------------------------------
{
  m_CreateMeter->OpRun();
  result = mafVMEMeter::SafeDownCast(m_CreateMeter->GetOutput())!=NULL;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafOpCreateMeterTest::TestOpDo()
//----------------------------------------------------------------------------
{
  DummyObserver *observer = new DummyObserver();
  DummyVme *vme = new DummyVme();
  m_CreateMeter->SetListener(observer);
  m_CreateMeter->SetInput(vme);
  m_CreateMeter->OpRun();
  m_CreateMeter->OpDo();
  result = mafVMEMeter::SafeDownCast(vme->GetChild(0))!=NULL;
  TEST_RESULT;
  m_CreateMeter->OpUndo();
  result = vme->GetNumberOfChildren()==0;
  TEST_RESULT;
  delete vme;
  delete observer;
}
