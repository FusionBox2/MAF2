/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: lhpOpMultiscaleExplore.cpp,v $
Language:  C++
Date:      $Date: 2007-11-30 12:01:57 $
Version:   $Revision: 1.3 $
Authors:   Nigel McFarlane
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "wx/busyinfo.h"

#include "mafDecl.h"
#include "mmgGui.h"
#include "mmgDialog.h"
#include "mafRWIBase.h"
#include "mafRWI.h"
#include "mmdMouse.h"
#include "mmgButton.h"
#include "mmgValidator.h"
#include "mafEventBase.h"
#include "mafEvent.h"
#include "mafEventInteraction.h"
#include "mafVME.h"
#include "mafVMESurface.h"
#include "mafVMEVolumeGray.h"
#include "mafVMEOutput.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkSphereSource.h"
#include "vtkProperty.h"
#include "vtkInteractorStyleTrackballCamera.h"

#include "lhpMultiscaleUtility.h"

#include "lhpMultiscaleActorCoordsUtility.h"
#include "lhpMultiscaleActor.h"
#include "lhpMultiscaleCallbacks.h"
#include "lhpMultiscaleCameraUtility.h"
#include "lhpMultiscaleVectorMath.h"
#include "lhpMultiscaleVisualPipes.h"
#include "lhpOpMultiscaleExplore.h"

#include <fstream>
#include <iostream>


//----------------------------------------------------------------------------
mafCxxTypeMacro(lhpOpMultiscaleExplore);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Constructor
lhpOpMultiscaleExplore::lhpOpMultiscaleExplore(wxString label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_OP;
  m_Canundo = true;
  m_Dialog = NULL;
  m_Rwi = NULL;

  m_MultiscaleUtility = new lhpMultiscaleUtility ;
  m_nextTokenColor = 0 ;
}


//----------------------------------------------------------------------------
// Destructor
lhpOpMultiscaleExplore::~lhpOpMultiscaleExplore()
//----------------------------------------------------------------------------
{
  int i ;

  delete m_MultiscaleUtility ;

  // delete pipes (each pipe must delete its own vtk objects, including the actor and mapper)
  for (i = 0 ;  i < (int)m_surfacePipes.size() ;  i++)
    delete m_surfacePipes.at(i) ;
  for (i = 0 ;  i < (int)m_tokenPipes.size() ;  i++)
    delete m_tokenPipes.at(i) ;
    
}


//----------------------------------------------------------------------------
// Copy
mafOp* lhpOpMultiscaleExplore::Copy()
//----------------------------------------------------------------------------
{
  /** return a copy of itself, needs to put it into the undo stack */
  return new lhpOpMultiscaleExplore(m_Label);
}


//----------------------------------------------------------------------------
// Accept surface or volume data
bool lhpOpMultiscaleExplore::Accept(mafNode* vme)
//----------------------------------------------------------------------------
{
  return (vme != NULL && (vme->IsMAFType(mafVMESurface))) ;
}

//----------------------------------------------------------------------------
// Static copy of accept function
bool lhpOpMultiscaleExplore::AcceptStatic(mafNode* vme)
//----------------------------------------------------------------------------
{
  return (vme != NULL && (vme->IsMAFType(mafVMESurface))) ;
}


//----------------------------------------------------------------------------
// Create gui etc before event loop takes over
void lhpOpMultiscaleExplore::OpRun()
//----------------------------------------------------------------------------
{
  int result = OP_RUN_CANCEL;

  if (m_TestMode == false)
  {
    CreateOpDialog();

    int ret_dlg = m_Dialog->ShowModal();
    if( ret_dlg == wxID_OK )
    {
      result = OP_RUN_OK;
    }
    else 
    {
      result = OP_RUN_CANCEL;
    }

    DeleteOpDialog();

    mafEventMacro(mafEvent(this,result));
  }
}


//----------------------------------------------------------------------------
// Finish executing the op after the gui has finished
void lhpOpMultiscaleExplore::OpDo()
//----------------------------------------------------------------------------
{
  mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}


//----------------------------------------------------------------------------
// Undo the op
void lhpOpMultiscaleExplore::OpUndo()
//----------------------------------------------------------------------------
{
  mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}


//----------------------------------------------------------------------------
// widget ID's
//----------------------------------------------------------------------------
enum EXTRACT_ISOSURFACE_ID
{
  ID_ADDVME = MINID,
  ID_SCALEVALUETXT,
  ID_SCALEUNITSTXT,
  ID_ZOOMOUT,
  ID_CAMERARESET,
  ID_GOBACK,
  ID_OK,
  ID_CANCEL,
  ID_DEBUG,
  ID_START_RENDER,  // vtk start render event
  ID_MOUSE_CLICK    // vtk mouse click event
};


//----------------------------------------------------------------------------
void lhpOpMultiscaleExplore::CreateOpDialog()
//----------------------------------------------------------------------------
{
  wxBusyCursor wait;

  //===== setup interface ====
  m_Dialog = new mmgDialog("Multiscale Explorer", mafCLOSEWINDOW | mafRESIZABLE);

  m_Rwi = new mafRWI(m_Dialog,ONE_LAYER,false);
  m_Rwi->SetListener(this);
  m_Rwi->CameraSet(CAMERA_PERSPECTIVE);

  m_Rwi->m_RenderWindow->SetDesiredUpdateRate(0.0001f);
  m_Rwi->SetSize(0,0,600,600);
  m_Rwi->Show(true);
  m_Rwi->m_RwiBase->SetMouse(m_Mouse);


  // Add selected vme to the list and create visual pipe for it.
  // Additional vme's must be added later from the dialog, because this is an op - not a view.
  mafVME* vme = mafVME::SafeDownCast(m_Input) ;
  AddVmeToScene(vme) ;

  vtkPolyData *polydata = vtkPolyData::SafeDownCast(((mafVME *)m_Input)->GetOutput()->GetVTKData());

  double bounds[6] = {0,0,0,0,0,0};
  polydata->GetBounds(bounds);

  m_Rwi->m_RenFront->ResetCamera(polydata->GetBounds());
  m_Rwi->m_RenFront->ResetCameraClippingRange(bounds);

  wxPoint p = wxDefaultPosition;

  wxStaticText *help  = new wxStaticText(m_Dialog,-1, "Multiscale View");

  wxStaticText *scaleStaticTxt = new wxStaticText(m_Dialog, -1, "Current scale:") ;
  wxTextCtrl *scaleValueTxt = new wxTextCtrl(m_Dialog, ID_SCALEVALUETXT, wxEmptyString, p, wxSize(80,20), wxTE_READONLY | wxTE_RIGHT) ;
  wxTextCtrl *scaleUnitsTxt = new wxTextCtrl(m_Dialog, ID_SCALEUNITSTXT, wxEmptyString, p, wxSize(80,20), wxTE_READONLY | wxTE_RIGHT) ;

  mmgButton  *AddVMEButton = new mmgButton(m_Dialog, ID_ADDVME, "add vme", p, wxSize(80,20));
  mmgButton  *ZoomOutButton = new mmgButton(m_Dialog, ID_ZOOMOUT, "zoom out x2", p, wxSize(80,20));
  mmgButton  *CameraResetButton = new mmgButton(m_Dialog, ID_CAMERARESET, "reset camera", p, wxSize(80,20));
  mmgButton  *GoBackButton = new mmgButton(m_Dialog, ID_GOBACK, "go back", p, wxSize(80,20));
  mmgButton  *ok = new mmgButton(m_Dialog, ID_OK, "ok", p, wxSize(80,20));
  mmgButton  *cancel = new mmgButton(m_Dialog, ID_CANCEL, "cancel", p, wxSize(80,20));
  mmgButton  *debug = new mmgButton(m_Dialog, ID_DEBUG, "debug", p, wxSize(80,20));

  scaleValueTxt->SetValidator(mmgValidator(this,ID_SCALEVALUETXT,ok)) ;
  scaleUnitsTxt->SetValidator(mmgValidator(this,ID_SCALEUNITSTXT,ok)) ;
  AddVMEButton->SetValidator(mmgValidator(this,ID_ADDVME,ok));
  ZoomOutButton->SetValidator(mmgValidator(this,ID_ZOOMOUT,ok));
  CameraResetButton->SetValidator(mmgValidator(this,ID_CAMERARESET,ok));
  GoBackButton->SetValidator(mmgValidator(this,ID_GOBACK,ok));
  ok->SetValidator(mmgValidator(this,ID_OK,ok));
  cancel->SetValidator(mmgValidator(this,ID_CANCEL,cancel));
  debug->SetValidator(mmgValidator(this,ID_DEBUG,debug));

  wxBoxSizer *h_sizer0 = new wxBoxSizer(wxHORIZONTAL);
  h_sizer0->Add(help, 0, wxRIGHT);	

  wxBoxSizer *h_sizer1 = new wxBoxSizer(wxHORIZONTAL);
  h_sizer1->Add(AddVMEButton, 0, wxRIGHT);	
  h_sizer1->Add(ZoomOutButton, 0, wxRIGHT);	
  h_sizer1->Add(CameraResetButton, 0, wxRIGHT);	
  h_sizer1->Add(GoBackButton, 0, wxRIGHT);	
  h_sizer1->Add(debug, 0, wxRIGHT);	

  wxBoxSizer *h_sizer2 = new wxBoxSizer(wxHORIZONTAL);
  h_sizer2->Add(scaleStaticTxt, 0, wxRIGHT) ;
  h_sizer2->Add(scaleValueTxt, 0, wxRIGHT) ;
  h_sizer2->Add(scaleUnitsTxt, 0, wxRIGHT) ;

  wxBoxSizer *h_sizer3 = new wxBoxSizer(wxHORIZONTAL);
  h_sizer3->Add(ok, 0, wxRIGHT);
  h_sizer3->Add(cancel, 0, wxRIGHT);

  wxBoxSizer *v_sizer =  new wxBoxSizer( wxVERTICAL );
  v_sizer->Add(m_Rwi->m_RwiBase, 1,wxEXPAND);
  v_sizer->Add(h_sizer0, 0, wxEXPAND | wxALL,5);
  v_sizer->Add(h_sizer1, 0, wxEXPAND | wxALL,5);
  v_sizer->Add(h_sizer2, 0, wxEXPAND | wxALL,5);
  v_sizer->Add(h_sizer3, 0, wxEXPAND | wxALL,5);

  m_Dialog->Add(v_sizer, 1, wxEXPAND);

  // set position of dialog
  m_Dialog->SetPosition(wxPoint(20,20)) ;

  m_Rwi->CameraUpdate();


  // Set the interactor style to trackball camera
  vtkInteractorStyleTrackballCamera* style = vtkInteractorStyleTrackballCamera::New() ;
  m_Rwi->m_RwiBase->SetInteractorStyle(style) ;
  style->Delete() ;

  // add observer to catch vtk start render event
  vtkStartRenderCallback *startRenderCallback = vtkStartRenderCallback::New() ;         // instantiate callback to convert vtk event to maf event
  GetRenderer()->AddObserver(vtkCommand::StartEvent, startRenderCallback) ;             // set callback to get start event from renderer
  startRenderCallback->SetListener(this) ;                                              // set self as listener to callback
  startRenderCallback->SetMafEventId(ID_START_RENDER) ;                                 // set event id to be thrown by callback
  startRenderCallback->Delete() ;

  // add observer to catch vtk mouse click event
  vtkMouseClickCallback *mouseClickCallback = vtkMouseClickCallback::New() ;           // instantiate callback to convert vtk event to maf event
  GetInteractor()->AddObserver(vtkCommand::LeftButtonPressEvent, mouseClickCallback) ;  // set callback to get mouse event from interactor
  mouseClickCallback->SetListener(this) ;                                              // set self as listener to callback
  mouseClickCallback->SetMafEventId(ID_MOUSE_CLICK) ;                                 // set event id to be thrown by callback
  mouseClickCallback->Delete() ;

  // save the current view so we can go back to it
  GetMultiscaleUtility()->SaveInitialView(GetRenderer()) ;
}



//----------------------------------------------------------------------------
void lhpOpMultiscaleExplore::DeleteOpDialog()
//----------------------------------------------------------------------------
{
  cppDEL(m_Rwi); 
  cppDEL(m_Dialog);
}



//----------------------------------------------------------------------------
// Create the vtk pipeline
void lhpOpMultiscaleExplore::CreateSurfacePipeline(mafVME* vme, vtkRenderer *renderer)
//----------------------------------------------------------------------------
{
  // create new pipe and add to list
  lhpMultiscaleSurfacePipeline* pipe = new lhpMultiscaleSurfacePipeline(vme, GetRenderer()) ;

  // add to list of pipes and create multiscale actor
  m_surfacePipes.push_back(pipe) ;
  GetMultiscaleUtility()->AddMultiscaleActor(pipe->GetActor(), pipe->GetMapper(), MSCALE_DATA_ACTOR) ;
}


//----------------------------------------------------------------------------
// Create the vtk pipeline
void lhpOpMultiscaleExplore::CreateTokenPipeline(vtkRenderer *renderer)
//----------------------------------------------------------------------------
{
  // create new pipe and add to list
  lhpMultiscaleTokenPipeline* pipe = new lhpMultiscaleTokenPipeline(GetRenderer(), m_nextTokenColor++) ;

  // add to list of pipes and create multiscale actor
  m_tokenPipes.push_back(pipe) ;
  GetMultiscaleUtility()->AddMultiscaleActor(pipe->GetActor(), pipe->GetMapper(), MSCALE_TOKEN) ;
}



//----------------------------------------------------------------------------
// Add new vme to scene.
// Adds vme to list, creates multiscale actors for data and tokens and creates visual pipes.
void lhpOpMultiscaleExplore::AddVmeToScene(mafVME* vme)
//----------------------------------------------------------------------------
{
  // add vme to list, create pipeline and multiscale actor
  AddVME(vme) ;
  CreateSurfacePipeline(vme, GetRenderer()) ;
  int dataActorId = GetMultiscaleUtility()->GetNumberOfActors() - 1 ;

  // create pipeline and multiscale actor for corresponding token
  CreateTokenPipeline(GetRenderer()) ;
  int tokenActorId = GetMultiscaleUtility()->GetNumberOfActors() - 1 ;

  // Put token in same position as actor
  vtkActor* actor_data = GetMultiscaleUtility()->GetMultiscaleActor(dataActorId)->GetActor() ;
  vtkActor* actor_token = GetMultiscaleUtility()->GetMultiscaleActor(tokenActorId)->GetActor() ;
  GetMultiscaleUtility()->GetActorCoordsUtility()->MoveToCenter(actor_token, actor_data) ;

  // Set token to default size
  //this->SetTokenSize(GetRenderer(), tokenActorId, TOKENSIZE) ;

  // Associate data actor and token together as a pair
  GetMultiscaleUtility()->SetActorTokenPair(dataActorId, tokenActorId) ;

  // Reset camera to view all actors currently in scene
  GetMultiscaleUtility()->ResetCameraFitAll(GetRenderer()) ;

  // Must save the camera view again, else the saved view will not contain
  // the required info for the new actors.
  GetMultiscaleUtility()->SaveInitialView(GetRenderer()) ;

  // Render the new scene
  GetRenderer()->GetRenderWindow()->Render() ;
  m_Rwi->CameraUpdate() ;
}



//----------------------------------------------------------------------------
// Event Handler
void lhpOpMultiscaleExplore::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {	
    case ID_ADDVME:
      {
        // Create pointer to the Accept() function.
        // We have to use a static version of the function so that it can be cast to long.
        bool (*acceptFunc)(mafNode*) = &lhpOpMultiscaleExplore::AcceptStatic ;

        // raise event to call up select vme dialog
        // The VME_CHOOSE event is handled by mafLogicWithManagers
        mafEvent e(this,VME_CHOOSE);  // create choose event 
        e.SetArg((long)acceptFunc) ;  // pass the accept function to the event.
        mafEventMacro(e);
        mafVME* vme = mafVME::SafeDownCast(e.GetVme());

        // Add vme to scene
        if (vme != NULL)
          AddVmeToScene(vme) ;

        break;
      }

    case ID_ZOOMOUT:
      OnZoomOut(GetRenderer()) ;
      break;

    case ID_CAMERARESET:
      OnCameraReset(GetRenderer()) ;
      break ;

    case ID_GOBACK:
      OnGoBack(GetRenderer()) ;
      break ;

    case ID_DEBUG:
      {
        // Write system state to file
        //std::fstream thing ;
        //thing.open("C:/Documents and Settings/Nigel.DB6ZB32J/My Documents/Visual Studio Projects/MAF/Multiscale2/thing.txt", thing.out | thing.app) ;
        //OnDebug(thing, GetRenderer()) ;
        //thing.close() ;
        break ;
      }

    case ID_OK:
      m_Dialog->EndModal(wxID_OK);
      break;

    case ID_CANCEL:
      m_Dialog->EndModal(wxID_CANCEL);
      break;

    case ID_START_RENDER:
      OnStartRender(GetRenderer()) ;
      break ;

    case ID_MOUSE_CLICK:
      OnMouseClick() ;
      break ;

    default:
      mafEventMacro(*e);
      break; 
    }
  }
  else{
    mafLogMessage("event") ;
  }
}



//------------------------------------------------------------------------------
// Set the token size
void lhpOpMultiscaleExplore::SetTokenSize(vtkRenderer* renderer, int tokenId, int tokenSize)
//------------------------------------------------------------------------------
{
  // get token and corresponding actor
  int actorId = GetMultiscaleUtility()->GetDataActorCorrespondingToToken(tokenId) ;
  vtkActor* actor = GetMultiscaleUtility()->GetMultiscaleActor(actorId)->GetActor() ;
  vtkActor* token = GetMultiscaleUtility()->GetMultiscaleActor(tokenId)->GetActor() ;

  // reset size of token
  GetMultiscaleUtility()->GetActorCoordsUtility()->SetActorDisplaySize(token, renderer, tokenSize) ;

  // reposition the token on the actor because the resizing changes the position slightly
  GetMultiscaleUtility()->GetActorCoordsUtility()->MoveToCenter(token, actor) ;

}


//------------------------------------------------------------------------------
// ZoomOut - Simple zoom out by x2
void lhpOpMultiscaleExplore::OnZoomOut(vtkRenderer* renderer)
//------------------------------------------------------------------------------
{
  vtkCamera *camera = renderer->GetActiveCamera() ;

  // get the current scale and desired new scale
  double scale = GetMultiscaleUtility()->GetCameraUtility()->CalculateScale(camera) ;
  double scaleNew = scale * 2.0 ;

  // calculate the new view angle
  double FL = camera->GetDistance() ;
  double viewAngleRad = 2.0 * atan(0.5*scaleNew/FL) ;
  double viewAngle = 180.0 / 3.14159 * viewAngleRad ;
  camera->SetViewAngle(viewAngle) ;

  // this is a dialog event so we need to manually call a render when we have finished
  renderer->GetRenderWindow()->Render() ;
  m_Rwi->CameraUpdate() ;

  std::fstream thing ;
  thing.open("C:/Documents and Settings/Nigel.DB6ZB32J/My Documents/Visual Studio Projects/MAF/Multiscale2/thing.txt", thing.out | thing.app) ;
  OnDebug(thing, GetRenderer()) ;
  thing.close() ;

}



//------------------------------------------------------------------------------
// ZoomReset - reset camera to top level
void lhpOpMultiscaleExplore::OnCameraReset(vtkRenderer* renderer)
//------------------------------------------------------------------------------
{
  // reset the camera to the start view
  GetMultiscaleUtility()->ClearViewSaves() ;      // clears all states except the intial view
  GetMultiscaleUtility()->RestoreView(renderer) ;

  // Set any visible tokens to the default size
  for (int i = 0 ;   i < GetMultiscaleUtility()->GetNumberOfActors() ;  i++){
    lhpMultiscaleActor *ma = GetMultiscaleUtility()->GetMultiscaleActor(i) ;
    vtkActor *actor = ma->GetActor() ;

    if ((ma->GetActorType() == MSCALE_TOKEN) && (actor->GetVisibility() == 1))
      SetTokenSize(renderer, i, TOKENSIZE) ;
  }

  // set the attention on all the actors
  GetMultiscaleUtility()->SetAttentionAll(true) ;

  // this is a dialog event so we need to manually call a render when we have finished
  renderer->GetRenderWindow()->Render() ;
  m_Rwi->CameraUpdate() ;
}



//------------------------------------------------------------------------------
// GoBack - This event requires us to go back up the actor stack
void lhpOpMultiscaleExplore::OnGoBack(vtkRenderer* renderer)
//------------------------------------------------------------------------------
{
  if (GetMultiscaleUtility()->CameraSameSinceLastSave(renderer)){
    // The camera has not moved since the last save so we should go straight back to the previous pick.
    // reset the camera to the last saved view but one
    GetMultiscaleUtility()->RestoreView(renderer) ;
  }
  else{
    // The camera has moved so we should go back to the last pick.
    // Reset the camera to the last saved view without deleting saved view.
    GetMultiscaleUtility()->RestoreViewWithoutDelete(renderer) ;
  }

  // Set any visible tokens to the default size
  for (int i = 0 ;   i < GetMultiscaleUtility()->GetNumberOfActors() ;  i++){
    lhpMultiscaleActor *ma = GetMultiscaleUtility()->GetMultiscaleActor(i) ;
    vtkActor *actor = ma->GetActor() ;

    if ((ma->GetActorType() == MSCALE_TOKEN) && (actor->GetVisibility() == 1))
      SetTokenSize(renderer, i, TOKENSIZE) ;
  }

  // this is a dialog event so we need to manually call a render when we have finished
  renderer->GetRenderWindow()->Render() ;
  m_Rwi->CameraUpdate() ;

}



//------------------------------------------------------------------------------
// Start Render handler.
// This is where we look at the scene and decide which actors and tokens should be visible.
// Nowhere else should change the visibility of the actors.
void lhpOpMultiscaleExplore::OnStartRender(vtkRenderer *renderer)
//------------------------------------------------------------------------------
{
  int i, j ;


  // Get the current scale and display in the dialog
  // This displays the scale in tidy units.
  int iscale ;
  std::ostrstream units ;
  std::ostrstream value ;

  double scale = GetMultiscaleUtility()->GetCameraUtility()->CalculateScale(renderer->GetActiveCamera()) ;
  GetMultiscaleUtility()->ConvertScaleToTidyUnits(scale, &iscale, units) ;
  value << iscale << std::ends ;

  wxTextCtrl* valueTxtCtrl = dynamic_cast<wxTextCtrl*>(m_Dialog->FindWindow(ID_SCALEVALUETXT)) ;
  wxTextCtrl* unitsTxtCtrl = dynamic_cast<wxTextCtrl*>(m_Dialog->FindWindow(ID_SCALEUNITSTXT)) ;

  valueTxtCtrl->Clear() ;
  valueTxtCtrl->AppendText(value.str()) ;
  unitsTxtCtrl->Clear() ;
  unitsTxtCtrl->AppendText(units.str()) ;


  // We check all the data actors (not tokens) to see if they have crossed the size thresholds
  // NB SIZELOWER and SIZEUPPER are the hysteresis bounds of the threshold screen size.
  for (i = 0 ;   i < GetMultiscaleUtility()->GetNumberOfActors() ;  i++){
    lhpMultiscaleActor *ma = GetMultiscaleUtility()->GetMultiscaleActor(i) ;
    vtkActor *actor = ma->GetActor() ;

    // Get the size of the actor
    // Note that we define the actor screen size with the view-independent GetMaxSizeDisplayAnyView()
    // because we don't want the actor to change just because is is being viewed down a long thin axis.
    // To be consistent we should do the same for the tokens, but as they don't have a long axis it doesn't matter.
    double screenSize = GetMultiscaleUtility()->GetActorCoordsUtility()->GetMaxSizeDisplayAnyView(actor, renderer) ;
    double worldSize = GetMultiscaleUtility()->GetActorCoordsUtility()->GetMaxSizeWorld(actor) ;
 
    if (ma->GetActorType() == MSCALE_DATA_ACTOR){
      // check if actor has gone below the size threshold
      if (screenSize < SIZELOWER){
        if (actor->GetVisibility() == 1 || ma->GetScaleStatus() != TOO_SMALL)
          OnActorTooSmall(renderer, i) ;
      }
      // check if actor is in the scale range (actors with attention have no upper size limit)
      else if (ma->GetAttention() && (screenSize > SIZEUPPER)){
        if (actor->GetVisibility() == 0 || ma->GetScaleStatus() != IN_SCALE)
          OnActorInScale(renderer, i) ;
      }
      // check if actor is in the scale range (actors without attention have to also be smaller than current scale)
      else if (!ma->GetAttention() && (screenSize > SIZEUPPER) && (worldSize <= (double)TOO_LARGE_FACTOR*scale)){
        if (actor->GetVisibility() == 0 || ma->GetScaleStatus() != IN_SCALE)
          OnActorInScale(renderer, i) ;
      }
      // check if actor without attention has gone too large
      else if (!ma->GetAttention() && (worldSize > (double)TOO_LARGE_FACTOR*scale)){
        if (ma->GetScaleStatus() != TOO_LARGE)
          OnActorTooLarge(renderer, i) ;
      }
    }
  }

  // Now we check if any of the visible tokens need to be resized
  for (i = 0 ;   i < GetMultiscaleUtility()->GetNumberOfActors() ;  i++){
    lhpMultiscaleActor *ma = GetMultiscaleUtility()->GetMultiscaleActor(i) ;
    vtkActor *actor = ma->GetActor() ;

    // get the screen size of the actor
    double screenSize = GetMultiscaleUtility()->GetActorCoordsUtility()->GetMaxSizeDisplay(actor, renderer) ;

    if ((ma->GetActorType() == MSCALE_TOKEN) && (actor->GetVisibility() == 1)){
      if (ma->GetScreenSizeMode() == FIXED_SIZE){
        // fixed size - reset to default size
        SetTokenSize(renderer, i, TOKENSIZE) ;
      }
      else if (ma->GetScreenSizeMode() == LIMITED_SIZE){
        // limited size - keep size between max or min
        if (screenSize < TOKENSIZEMIN)
          SetTokenSize(renderer, i, TOKENSIZEMIN) ;
        else if (screenSize > TOKENSIZEMAX)
          SetTokenSize(renderer, i, TOKENSIZEMAX) ;
      }
    }
  }

  // Check if any pairs of tokens are touching or not touching
  for (i = 0 ;   i < GetMultiscaleUtility()->GetNumberOfActors() ;  i++){
    for (j = i+1 ;  j < GetMultiscaleUtility()->GetNumberOfActors() ;  j++){
      lhpMultiscaleActor *mai = GetMultiscaleUtility()->GetMultiscaleActor(i) ;
      lhpMultiscaleActor *maj = GetMultiscaleUtility()->GetMultiscaleActor(j) ;
      if ((mai->GetActorType() == MSCALE_TOKEN) && (maj->GetActorType() == MSCALE_TOKEN)){
        vtkActor *actori = mai->GetActor() ;
        vtkActor *actorj = maj->GetActor() ;

        double distApart = GetMultiscaleUtility()->DistanceApartPixelUnits(actori, actorj, renderer) ;
        if (distApart < TOKENSIZEMIN){
          if (!GetMultiscaleUtility()->TokensListedAsTouching(i,j)){
            // tokens were not touching, but now are - call event handler
            OnTokensOverlap(renderer, i, j) ;
          }
        }
        else{
          if (GetMultiscaleUtility()->TokensListedAsTouching(i,j)){
            // tokens were touching, but now are not - call event handler
            OnTokensSeparated(renderer, i, j) ;
          }
        }
      }
    }
  }
}



//------------------------------------------------------------------------------
// Actor gone below threshold size
void lhpOpMultiscaleExplore::OnActorTooSmall(vtkRenderer* renderer, int actorId)
//------------------------------------------------------------------------------
{
  // get actor and token
  int tokenId = GetMultiscaleUtility()->GetTokenCorrespondingToDataActor(actorId) ;
  vtkActor* actor = GetMultiscaleUtility()->GetMultiscaleActor(actorId)->GetActor() ;
  vtkActor* token = GetMultiscaleUtility()->GetMultiscaleActor(tokenId)->GetActor() ;

  // make token visible instead of actor
  actor->SetVisibility(0) ;
  token->SetVisibility(1) ;

  // set size and position of token
  GetMultiscaleUtility()->GetActorCoordsUtility()->SetActorDisplaySize(token, renderer, TOKENSIZE) ;
  GetMultiscaleUtility()->GetActorCoordsUtility()->MoveToCenter(token, actor) ;

  // set flag to indicate that this actor is now below the scale threshold
  GetMultiscaleUtility()->GetMultiscaleActor(actorId)->SetScaleStatus(TOO_SMALL) ;
}


//------------------------------------------------------------------------------
// Actor is in visible scale range
void lhpOpMultiscaleExplore::OnActorInScale(vtkRenderer* renderer, int actorId)
//------------------------------------------------------------------------------
{
  // get actor and token
  int tokenId = GetMultiscaleUtility()->GetTokenCorrespondingToDataActor(actorId) ;
  vtkActor* actor = GetMultiscaleUtility()->GetMultiscaleActor(actorId)->GetActor() ;
  vtkActor* token = GetMultiscaleUtility()->GetMultiscaleActor(tokenId)->GetActor() ;

  // make actor visible and token invisible
  actor->SetVisibility(1) ;
  token->SetVisibility(0) ;

  // set flag to indicate that this actor is now above the scale threshold
  GetMultiscaleUtility()->GetMultiscaleActor(actorId)->SetScaleStatus(IN_SCALE) ;
}



//------------------------------------------------------------------------------
// Actor gone too large for scale
void lhpOpMultiscaleExplore::OnActorTooLarge(vtkRenderer* renderer, int actorId)
//------------------------------------------------------------------------------
{
  // get actor and token
  int tokenId = GetMultiscaleUtility()->GetTokenCorrespondingToDataActor(actorId) ;
  vtkActor* actor = GetMultiscaleUtility()->GetMultiscaleActor(actorId)->GetActor() ;
  vtkActor* token = GetMultiscaleUtility()->GetMultiscaleActor(tokenId)->GetActor() ;

  // make both actor and token invisible
  actor->SetVisibility(0) ;
  token->SetVisibility(0) ;

  // set flag to indicate that this actor is now too large
  GetMultiscaleUtility()->GetMultiscaleActor(actorId)->SetScaleStatus(TOO_LARGE) ;
}



//------------------------------------------------------------------------------
// Tokens overlap
void lhpOpMultiscaleExplore::OnTokensOverlap(vtkRenderer* renderer, int tokenId1, int tokenId2)
//------------------------------------------------------------------------------
{
  // get token actors
  vtkActor *tokenActor1 = GetMultiscaleUtility()->GetMultiscaleActor(tokenId1)->GetActor() ;
  vtkActor *tokenActor2 = GetMultiscaleUtility()->GetMultiscaleActor(tokenId2)->GetActor() ;


  // get corresponding data actors and their size
  int actorId1 = GetMultiscaleUtility()->GetDataActorCorrespondingToToken(tokenId1) ;
  int actorId2 = GetMultiscaleUtility()->GetDataActorCorrespondingToToken(tokenId2) ;

  vtkActor *actor1 = GetMultiscaleUtility()->GetMultiscaleActor(actorId1)->GetActor() ;
  vtkActor *actor2 = GetMultiscaleUtility()->GetMultiscaleActor(actorId2)->GetActor() ;

  double siz1 = GetMultiscaleUtility()->GetActorCoordsUtility()->GetMaxSizeWorld(actor1) ;
  double siz2 = GetMultiscaleUtility()->GetActorCoordsUtility()->GetMaxSizeWorld(actor2) ;


  // Make a record of the touching pair and make the one with the smaller data actor invisible
  // If they are the same size, the data actor with the lower index is considered larger.
  GetMultiscaleUtility()->AddTouchingTokens(tokenId1, tokenId2) ;
  if (siz1 > siz2)
    tokenActor2->SetVisibility(0) ;
  else if (siz1 < siz2)
    tokenActor1->SetVisibility(0) ;
  else{
    if (actorId1 < actorId2)
      tokenActor2->SetVisibility(0) ;
    else
      tokenActor1->SetVisibility(0) ;
  }
}


//------------------------------------------------------------------------------
// Tokens separated
void lhpOpMultiscaleExplore::OnTokensSeparated(vtkRenderer* renderer, int tokenId1, int tokenId2)
//------------------------------------------------------------------------------
{
  // get token actors
  vtkActor *tokenActor1 = GetMultiscaleUtility()->GetMultiscaleActor(tokenId1)->GetActor() ;
  vtkActor *tokenActor2 = GetMultiscaleUtility()->GetMultiscaleActor(tokenId2)->GetActor() ;


  // get corresponding data actors and their size
  int actorId1 = GetMultiscaleUtility()->GetDataActorCorrespondingToToken(tokenId1) ;
  int actorId2 = GetMultiscaleUtility()->GetDataActorCorrespondingToToken(tokenId2) ;

  vtkActor *actor1 = GetMultiscaleUtility()->GetMultiscaleActor(actorId1)->GetActor() ;
  vtkActor *actor2 = GetMultiscaleUtility()->GetMultiscaleActor(actorId2)->GetActor() ;

  double siz1 = GetMultiscaleUtility()->GetActorCoordsUtility()->GetMaxSizeWorld(actor1) ;
  double siz2 = GetMultiscaleUtility()->GetActorCoordsUtility()->GetMaxSizeWorld(actor2) ;


  // erase the record of the touching pair
  GetMultiscaleUtility()->RemoveTouchingTokens(tokenId1, tokenId2) ;

  // make tokens visible if their data actors are not currently visible and they are not touched by any tokens of larger actors
  if ((actor1->GetVisibility() == 0) && (GetMultiscaleUtility()->GetNumberOfTouchingTokensLarger(tokenId1) == 0))
    tokenActor1->SetVisibility(1) ;
  if ((actor2->GetVisibility() == 0) && (GetMultiscaleUtility()->GetNumberOfTouchingTokensLarger(tokenId2) == 0))
    tokenActor2->SetVisibility(1) ;
}


//------------------------------------------------------------------------------
// Mouse click handler
void lhpOpMultiscaleExplore::OnMouseClick()
//------------------------------------------------------------------------------
{
  int i, j, xscreen, yscreen ;
  double boundsD[6] ;

  std::vector<ACTORINFO>pickedActors ;

  vtkRenderWindowInteractor *RWI = GetInteractor() ;
  vtkRenderer *renderer = GetRenderer() ;

  // get position of event
  RWI->GetLastEventPosition(xscreen, yscreen) ;

  // list all the visible actors which are targeted by the click (there may be more than one)
  // note that we consider all visible actors - not just those in use by callback, because
  // the pickable actor might be hidden behind an unpickable one.
  int nactors = GetMultiscaleUtility()->GetNumberOfActors() ;
  for (i = 0 ;   i < nactors ;  i++){
    lhpMultiscaleActor *ma = GetMultiscaleUtility()->GetMultiscaleActor(i) ;
    vtkActor *actor = ma->GetActor() ;

    if (actor->GetVisibility() == 1){
      // get bounds of actor in display coords
      GetMultiscaleUtility()->GetActorCoordsUtility()->GetBoundsDisplay(actor, renderer, boundsD) ;

      // compare bounding box with event coords
      if ((xscreen >= boundsD[0]) && (xscreen <= boundsD[1]) && (yscreen >= boundsD[2]) && (yscreen <= boundsD[3])){
        // get depth of actor in view coords
        double centerV[3] ;
        GetMultiscaleUtility()->GetActorCoordsUtility()->GetCenterView(actor, renderer, centerV) ;

        ACTORINFO info ;
        info.actorId = i ;
        info.depth = centerV[2] ;

        // note index of picked actor
        pickedActors.push_back(info) ;
      }
    }
  }

  if (pickedActors.size() == 0)
    return ;


  // find targeted actor which is closest to the viewer
  int jnearest = 0 ;
  for (j = 0 ;  j < (int)pickedActors.size() ;  j++){
    if (pickedActors[j].depth < pickedActors[jnearest].depth)
      jnearest = j ;
  }
  i = pickedActors[jnearest].actorId ;


  // if the nearest targeted actor is a pickable token, call pick method
  if (GetMultiscaleUtility()->GetMultiscaleActor(i)->GetActorType() == MSCALE_TOKEN)
    OnPick(renderer, i) ;
}



//------------------------------------------------------------------------------
// Pick token
void lhpOpMultiscaleExplore::OnPick(vtkRenderer* renderer, int tokenId)
//------------------------------------------------------------------------------
{
  lhpMultiscaleUtility *MSU = GetMultiscaleUtility() ;

  // is token currently touching any other tokens
  int ntouching = MSU->GetNumberOfTouchingTokens(tokenId) ;

  // get token and corresponding object
  int actorId = MSU->GetDataActorCorrespondingToToken(tokenId) ;
  vtkActor* actor = MSU->GetMultiscaleActor(actorId)->GetActor() ;
  vtkActor* token = MSU->GetMultiscaleActor(tokenId)->GetActor() ;

  // temporarily allow the token to change screen size so we can see it zoom
  ScreenSizeMode saveMode = MSU->GetMultiscaleActor(tokenId)->GetScreenSizeMode() ;
  MSU->GetMultiscaleActor(tokenId)->SetScreenSizeMode(VARIABLE_SIZE) ;

  if (ntouching == 0){
    // zoom in on actor corresponding to picked token
    MSU->GetCameraUtility()->ZoomCameraOnActor(actor, renderer) ;

    // set the picked actor to have the attention
    MSU->SetAttentionAll(false) ;
    MSU->GetMultiscaleActor(actorId)->SetAttention(true) ;
  }
  else{
    // get collection of touching actors
    vtkActorCollection *AC = vtkActorCollection::New() ;
    MSU->GetTouchingDataActorsAsCollection(tokenId, AC) ;

    // Set the attention to all the actors in the collection,
    // because you don't want any to disappear when you zoom in.
    // Do this before you zoom, while the tokens are still touching !
    std::vector<int> idlist ;
    MSU->GetTouchingDataActorsAsIdList(tokenId, idlist) ;
    MSU->SetAttentionAll(false) ;
    MSU->SetAttentionToList(idlist, true) ;

    // zoom in on the collection
    MSU->GetCameraUtility()->ZoomCameraOnActorCollection(AC, renderer) ;

    // delete actor collection
    AC->Delete() ;


  }

  // set the token's screen size back to original behaviour
  MSU->GetMultiscaleActor(tokenId)->SetScreenSizeMode(saveMode) ;

  // save the camera params and attention flags
  MSU->SaveView(renderer) ;

}


//------------------------------------------------------------------------------
// Debug handler
void lhpOpMultiscaleExplore::OnDebug(std::ostream& os, vtkRenderer *renderer)
//------------------------------------------------------------------------------
{
  // print state of system as held by multiscale utility
  GetMultiscaleUtility()->PrintSelf(os, 0) ;

  // print positions and sizes of actors
  os << "size and postion of actors" << std::endl ;
  double sizW[4], sizD[3], posW[4], posD[3], posV[3] ;
  for (int i = 0 ;  i < GetMultiscaleUtility()->GetNumberOfActors() ;  i++){
    os << i << std::endl ;
    vtkActor* actor = GetMultiscaleUtility()->GetMultiscaleActor(i)->GetActor() ;
    this->GetMultiscaleUtility()->GetActorCoordsUtility()->GetCenterWorld(actor, posW) ;
    this->GetMultiscaleUtility()->GetActorCoordsUtility()->GetCenterView(actor, renderer, posV) ;
    this->GetMultiscaleUtility()->GetActorCoordsUtility()->GetCenterDisplay(actor, renderer, posD) ;
    this->GetMultiscaleUtility()->GetActorCoordsUtility()->GetSizeWorld(actor, sizW) ;
    this->GetMultiscaleUtility()->GetActorCoordsUtility()->GetSizeDisplay(actor, renderer, sizD) ;
    os << "center (world)   " ; lhpMultiscaleVectorMath::PrintVector(os, posW) ;
    os << "center (view)    " ; lhpMultiscaleVectorMath::PrintVector(os, posV) ;
    os << "center (display) " ; lhpMultiscaleVectorMath::PrintVector(os, posD) ;
    os << "size (world)   " ; lhpMultiscaleVectorMath::PrintVector(os, sizW) ;
    os << "size (display) " ; lhpMultiscaleVectorMath::PrintVector(os, sizD) ;
    os << std::endl ;
  }
  os << std::endl ;
}
