/*=========================================================================

 Program: MAF2
 Module: mafGUIMaterialButton
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafGUIMaterialButton_H__
#define __mafGUIMaterialButton_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafBaseEventHandler.h"
#include "mafEvent.h"
#include "mafEventSender.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafVME;
class mafGUIPicButton;
class mmaMaterial;
class mafGUI;

//----------------------------------------------------------------------------
// mafGUIMaterialButton :
//----------------------------------------------------------------------------
/**
This component allow to manage surface materials.
*/
class MAF_EXPORT mafGUIMaterialButton : public mafBaseEventHandler, public mafEventSender
{
public:
	mafGUIMaterialButton(mafVME *vme, mafBaseEventHandler *listener = NULL);
	~mafGUIMaterialButton(); 
	void OnEvent(mafEventBase *maf_event);

	/** Return mafGUIMaterialButton User Interface */
	mafGUI *GetGui() {return m_Gui;};

  /** update material icon. */
	void UpdateMaterialIcon();

  /** Enable-Disable material button. */
  void Enable(bool enable);

  mmaMaterial *GetMaterial() {return m_Material;};

	void SetVME(mafVME *vme);

protected:
	/** Create the GUI for the materials */
	void CreateGui();

  mafGUIPicButton *m_MaterialButton;  // material pic button
  wxStaticText *m_MaterialLabel;   // material name label
  mmaMaterial  *m_Material;

	mafGUI			*m_Gui;
  mafVME      *m_Vme;
};
#endif
