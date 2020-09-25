/*=========================================================================

 Program: MAF2
 Module: mafVMEOutputMuscleWrapping
 Authors: Taha Jerbi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafVMEOutputMuscleWrapping_h
#define __mafVMEOutputMuscleWrapping_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVMEOutputPolyline.h"
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafGUI;

/** NULL output for VME node with a VTK image output data.
  mafVMEOutputMeter is the output produced by a node generating an output
  with a VTK dataset.
*/
class MAF_EXPORT mafVMEOutputMuscleWrapping : public mafVMEOutputPolyline
{
public:
	mafVMEOutputMuscleWrapping();
	virtual ~mafVMEOutputMuscleWrapping();

	mafTypeMacro(mafVMEOutputMuscleWrapping, mafVMEOutputPolyline);

  /** Update all the output data structures (data, bounds, matrix and abs matrix).*/
  virtual void Update();

	/* Get Distance String*/
	const char *GetDistance(){return m_Distance.GetCStr();}

	/* Get Angle String*/
	//const char *GetAngle(){return m_Angle.GetCStr();}

protected:
  mafGUI *CreateGui();

  mafString m_Distance;
 // mafString m_Angle;

private:
	mafVMEOutputMuscleWrapping(const mafVMEOutputMuscleWrapping&); // Not implemented
	void operator=(const mafVMEOutputMuscleWrapping&); // Not implemented
};

#endif
