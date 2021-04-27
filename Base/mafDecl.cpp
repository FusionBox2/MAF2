/*=========================================================================

 Program: MAF2
 Module: mafDecl
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDecl.h"
#include <wx/image.h>
#include <wx/uri.h>
#include <wx/filename.h>
#include "mafIncludeWX.h"
#include "mmuIdFactory.h"
#include <math.h>

MAF_ID_IMP(REMOTE_COMMAND_CHANNEL)


// int MAFExpertMode = TRUE;

static bool yelding;
//----------------------------------------------------------------------------
void mafYield()
//----------------------------------------------------------------------------
{
   if(!yelding)
   {
       yelding=true;
       wxYield();
       yelding=false;
   }
}

/* to be removed (Marco)
//----------------------------------------------------------------------------
mafVmeBaseTypes mafGetBaseType(mafVME* vme)
//----------------------------------------------------------------------------
{
  assert(vme);
	     if(vme->GetTagArray()->FindTag("MAF_TOOL_VME") != -1)  return VME_TOOL;
			 if(vme->IsA("mflVMEPointSet")==1)                      return VME_POINTSET;
	else if(vme->IsA("mflVMEWidgetLine") ==1)                       return VME_WIDGET; //SIL. 18-11-2004: 
	else if(vme->IsA("mflVMESurface") ==1)                      return VME_SURFACE;
	else if(vme->IsA("mflVMEImage")   ==1)                      return VME_IMAGE;

  else if(vme->IsA("mflVMEExFieldScalar")     ==1)            return VME_EX_FIELD_SCALAR;//BEZ. 20-7-2004: 
	else if(vme->IsA("mflVMEExFieldVector")     ==1)            return VME_EX_FIELD_VECTOR;//BEZ. 20-7-2004: 
  else if(vme->IsA("mflVMEExFieldProfile")     ==1)           return VME_EX_FIELD_PROFILE;//BEZ. 20-7-2004:
	else if(vme->IsA("mflVMEExField")     ==1)                  return VME_EX_FIELD;       //BEZ. 20-7-2004: 

  else if(vme->IsA("mflVMEGrayVolume")  ==1)                  return VME_GRAY_VOLUME;
	else if(vme->IsA("mflVMEVolume")  ==1)                      return VME_VOLUME;
	else if(vme->IsA("mflVMEGizmo")   ==1)                      return VME_GIZMO;
  else if(vme->IsA("mflVMEExternalData") ==1)                 return VME_EXTERNAL_DATA;
	else if(vme->IsA("mflVMEfem")     ==1)                      return VME_FEM;
	else if(vme->IsA("mflVMEScalar")     ==1)                   return VME_SCALAR;
	else return VME_GENERIC;
}
*/

//----------------------------------------------------------------------------
mafString  mafGetDirName(const mafString& initial, const mafString& title, wxWindow *parent)
//----------------------------------------------------------------------------
{
  wxDirDialog dialog(parent, title.toWx(), initial.toWx(), wxDD_DEFAULT_STYLE | wxDD_NEW_DIR_BUTTON);

  dialog.SetReturnCode(wxID_OK);
  int result = dialog.ShowModal();
  mafYield(); // wait for the dialog to disappear
  if(result != wxID_OK)
    return mafString();
  mafString res = mafWxToString(dialog.GetPath());
  res.ParsePathName();
  return res;
}

//----------------------------------------------------------------------------
mafString mafGetOpenFile(const mafString& initial, const mafString& wild, const mafString& title, wxWindow *parent)
//----------------------------------------------------------------------------
{
  wxString path, name, ext;
  wxFileName::SplitPath(initial.toWx(),&path,&name,&ext);

  if (!name.IsEmpty() && !ext.IsEmpty()) name.Append(".").Append(ext);

  mafString wildcard=wild;
  wildcard+=_R("|All Files (*.*)|*.*");
 
  wxFileDialog dialog(parent, title.toWx(), path, name, wildcard.toWx(), wxFD_OPEN|wxFD_FILE_MUST_EXIST);

  dialog.SetReturnCode(wxID_OK);
	int result = dialog.ShowModal();
  mafYield(); // wait for the dialog to disappear
  if(result != wxID_OK)
    return mafString();
  mafString res = mafWxToString(dialog.GetPath());
  res.ParsePathName();
  return res;
}

//----------------------------------------------------------------------------
void mafGetOpenMultiFiles(const mafString& initial, const mafString& wild, std::vector<mafString>& files, const mafString& title, wxWindow *parent)
//----------------------------------------------------------------------------
{
  wxString path, name, ext;
  wxFileName::SplitPath(initial.toWx(),&path,&name,&ext);
  if (!name.IsEmpty() && !ext.IsEmpty()) name.Append(".").Append(ext);
  mafString wildcard = wild;
  wildcard += _R("|All Files (*.*)|*.*");
 
  wxFileDialog dialog(parent, title.toWx(), path, name, wildcard.toWx(), wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_MULTIPLE);

  dialog.SetReturnCode(wxID_OK);
	int result = dialog.ShowModal();
  mafYield(); // wait for the dialog to disappear
  wxArrayString wxfiles;
	(result == wxID_OK) ? dialog.GetPaths(wxfiles) : wxfiles.Empty();
	for (int i=0;i<wxfiles.GetCount();i++)
  {
    mafString f = mafWxToString(wxfiles[i]);
    f.ParsePathName();
    files.push_back(f);
  }
}
//----------------------------------------------------------------------------
mafString mafGetSaveFile(const mafString& initial, const mafString& wild, const mafString& title, wxWindow *parent)
//----------------------------------------------------------------------------
{
  wxString path, name, ext;
  wxFileName::SplitPath(initial.toWx(),&path,&name,&ext);
  if (!name.IsEmpty() && !ext.IsEmpty()) name.Append(".").Append(ext);
  mafString wildcard = wild;
  mafString defaultname = _R("newMAFfile");
  wildcard += _R("|All Files (*.*)|*.*");
  //wxFileDialog dialog(parent,title.GetCStr(), path, name, wildcard.GetCStr(), wxSAVE|wxOVERWRITE_PROMPT|wxHIDE_READONLY);
  wxFileDialog dialog(parent,title.toWx(), initial.toWx(), defaultname.toWx(), wildcard.toWx(), wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
  dialog.SetReturnCode(wxID_OK);
	int result = dialog.ShowModal();
  mafYield(); // wait for the dialog to disappear
  if(result != wxID_OK)
    return mafString();
  mafString res = mafWxToString(dialog.GetPath());
  res.ParsePathName();
  return res;
}
//----------------------------------------------------------------------------
mafString mafGetApplicationDirectory()
//----------------------------------------------------------------------------
{
	static mafString app_dir;
	if (app_dir.IsEmpty())
	{
		wxString cd = wxGetCwd();
		wxSetWorkingDirectory(wxT(".."));
		app_dir = mafWxToString(wxGetCwd());
		wxSetWorkingDirectory(cd);
	}
  mafString dir = app_dir;
  dir.ParsePathName();
	return dir;
}
//----------------------------------------------------------------------------
bool IsRemote(const mafString& filename, mafString &protocol_used)
//----------------------------------------------------------------------------
{
  bool is_remote = false;
  wxURI data_uri(filename.toWx());
  if(data_uri.HasScheme())
    protocol_used = mafWxToString(data_uri.GetScheme());
  else
    is_remote = false; //try to treat it as a local file!!

  if (!protocol_used.IsEmpty())
  {
    is_remote = protocol_used.Equals(_R("http"))  ||
                protocol_used.Equals(_R("ftp"))   ||
                protocol_used.Equals(_R("https"));
  }
  return is_remote;
}
//----------------------------------------------------------------------------
float RoundValue(float f_in, int decimal_digits)
//----------------------------------------------------------------------------
{
  float f_tmp = f_in * pow((double)10,(double)decimal_digits);
  int b = ( f_tmp >= 0 ) ? static_cast<int>( f_tmp + .5):static_cast<int>( f_tmp - .5);
  return b / pow((double)10,(double)decimal_digits);
}
//----------------------------------------------------------------------------
double RoundValue(double d_in, int decimal_digits)
//----------------------------------------------------------------------------
{
  double d_tmp = d_in * pow((double)10,(double)decimal_digits);
  int b = ( d_tmp >= 0 ) ? static_cast<int>( d_tmp + .5):static_cast<int>( d_tmp - .5);
  return b / pow((double)10,(double)decimal_digits);
}
//----------------------------------------------------------------------------
void mafFormatDataSize( long long size, mafString& szOut )
//----------------------------------------------------------------------------
{
  const mafString SZUN[] = {_R("B"), _R("KB"), _R("MB"), _R("GB"), _R("")};
  const int LIMITS[] = { 16384, 4096, 1024, INT_MAX};

  int idx = 0;
  double nsize = (double)size;
  while (!SZUN[idx].IsEmpty())
  {
    if (nsize < LIMITS[idx])
      break;

    nsize /= 1024;
    idx++;
  }

  szOut = mafString::Format(_R("%g "), RoundValue(nsize)) + SZUN[idx];
}
//----------------------------------------------------------------------------
wxBitmap mafGrayScale(wxBitmap bmp)
//----------------------------------------------------------------------------
{
  wxImage img = bmp.ConvertToImage();
  unsigned char *p = img.GetData();
  unsigned char *max = p + img.GetWidth() * img.GetHeight() * 3;
  unsigned char *r, *g, *b;
  unsigned int gray ;
  while( p < max )
  {
    r = p++;
    g = p++;
    b = p++;
    gray = *r + *g + *b;
    *r = *g = *b = gray / 3;
  }
  return wxBitmap(img);
}
//----------------------------------------------------------------------------
wxBitmap mafRedScale(wxBitmap bmp)
//----------------------------------------------------------------------------
{
  wxImage img = bmp.ConvertToImage();
  unsigned char *p = img.GetData();
  unsigned char *max = p + img.GetWidth() * img.GetHeight() * 3;
  unsigned char *r, *g, *b;
  unsigned int red;
  unsigned int gray;
  while( p < max )
  {
    r = p++;
    g = p++;
    b = p++;
    gray = *r + *g + *b;
    red = *r * 1.6;
    *r = *g = *b = gray / 3;
    *r = red > 255 ? 255 : red;
  }
  return wxBitmap(img);
}
//----------------------------------------------------------------------------
wxColour mafRandomColor()
//----------------------------------------------------------------------------
{
  static long i = 117;
  i = (i*13)%16+1;
  switch( i )
  {
    case 1:  return wxColour(229,166,215); break;
    case 2:  return wxColour(224,86 ,86 ); break;
    case 3:  return wxColour(224,143,87 ); break;
    case 4:  return wxColour(224,198,87 ); break;
    case 5:  return wxColour(198,224,87 ); break;
    case 6:  return wxColour(143,224,87 ); break;
    case 7:  return wxColour(87 ,224,87 ); break;
    case 8:  return wxColour(87 ,224,143); break;
    case 9:  return wxColour(87 ,224,198); break;
    case 10: return wxColour(88 ,199,195); break;
    case 11: return wxColour(87 ,224,198); break;
    case 12: return wxColour(88 ,88 ,225); break;
    case 13: return wxColour(140,88 ,225); break;
    case 14: return wxColour(196,88 ,225); break;
    case 15: return wxColour(225,188,199); break;
    default: return wxColour(224,87 ,143); break;
  };
}
//----------------------------------------------------------------------------
mafString  mafIdString(int id)
//----------------------------------------------------------------------------
{
    mafString s;
    switch(id)
    {
     case MENU_START:           s=_R("MENU_START"); break; 
     case MENU_FILE_START:      s=_R("MENU_FILE_START"); break; 
     case MENU_FILE_NEW:        s=_R("MENU_FILE_NEW"); break; 
     case MENU_FILE_OPEN:       s=_R("MENU_FILE_OPEN"); break; 
     case MENU_FILE_SAVE:       s=_R("MENU_FILE_SAVE"); break; 
     case MENU_FILE_SAVEAS:     s=_R("MENU_FILE_SAVEAS"); break;
     case MENU_FILE_UPLOAD:     s=_R("MENU_FILE_UPLOAD"); break;
     case MENU_FILE_MERGE:      s=_R("MENU_FILE_MERGE"); break; 
     case MENU_FILE_PRINT:      s=_R("MENU_FILE_PRINT"); break;
     case MENU_FILE_PRINT_PREVIEW:s=_R("MENU_FILE_PRINT_PREVIEW"); break;
     case MENU_FILE_PRINT_SETUP:s=_R("MENU_FILE_PRINT_SETUP"); break;
     case MENU_FILE_PRINT_PAGE_SETUP:s=_R("MENU_FILE_PRINT_PAGE_SETUP"); break;
     case MENU_FILE_QUIT:       s=_R("MENU_FILE_QUIT"); break; 
     case MENU_FILE_END:        s=_R("MENU_FILE_END"); break; 
     case MENU_EDIT_START:      s=_R("MENU_EDIT_START"); break; 
     case MENU_OP:              s=_R("MENU_OP"); break; 
     case MENU_OPTION_DEVICE_SETTINGS: s=_R("MENU_OPTION_DEVICE_SETTINGS"); break;
     case MENU_OPTION_LOCALE_SETTINGS: s=_R("MENU_OPTION_LOCALE_SETTINGS"); break;
     //case MENU_EDIT_UNDO:	      s=_R("MENU_EDIT_UNDO"); break; 
     //case MENU_EDIT_REDO:	      s=_R("MENU_EDIT_REDO"); break; 
     //case MENU_EDIT_DELETE:	    s=_R("MENU_EDIT_DELETE"); break; 
     //case MENU_EDIT_CUT:	      s=_R("MENU_EDIT_CUT"); break; 
     //case MENU_EDIT_COPY:	      s=_R("MENU_EDIT_COPY"); break; 
     //case MENU_EDIT_PASTE:	    s=_R("MENU_EDIT_PASTE"); break; 
     case MENU_EDIT_FIND_VME:   s=_R("MENU_EDIT_FIND_VME"); break; 
     case MENU_EDIT_END:	      s=_R("MENU_EDIT_END"); break; 

     case MENU_VIEW_START:	    s=_R("MENU_VIEW_START"); break; 
     case SASH_START:	          s=_R("SASH_START"); break; 
     case MENU_VIEW_TOOLBAR:	  s=_R("MENU_VIEW_TOOLBAR"); break; 
     case MENU_VIEW_LOGBAR:	    s=_R("MENU_VIEW_LOGBAR"); break; 
     case MENU_VIEW_SIDEBAR:	  s=_R("MENU_VIEW_SIDEBAR"); break; 
     case MENU_VIEW_TIMEBAR:	  s=_R("MENU_VIEW_TIMEBAR"); break; 
     case MENU_OPTION_APPLICATION_SETTINGS: s = _R("MENU_OPTION_APPLICATION_SETTINGS"); break;
     case MENU_OPTION_MEASURE_UNIT_SETTINGS: s = _R("MENU_OPTION_MEASURE_UNIT_SETTINGS"); break;
     case COLLABORATE_ENABLE:   s = _R("COLLABORATE_ENABLE"); break;
     case SEND_VIEW_LAYOUT:     s = _R("SEND_VIEW_LAYOUT"); break;
     case SASH_END:	            s=_R("SASH_END"); break; 
     case MENU_LAYOUT_START:	  s=_R("MENU_LAYOUT_START"); break; 
     case LAYOUT_ONE:	          s=_R("LAYOUT_ONE"); break; 
     case LAYOUT_TWO_VERT:	    s=_R("LAYOUT_TWO_VERT"); break; 
     case LAYOUT_TWO_HORZ:	    s=_R("LAYOUT_TWO_HORZ"); break; 
     case LAYOUT_THREE_UP:	    s=_R("LAYOUT_THREE_UP"); break; 
     case LAYOUT_THREE_DOWN:	  s=_R("LAYOUT_THREE_DOWN"); break; 
     case LAYOUT_THREE_LEFT:	  s=_R("LAYOUT_THREE_LEFT"); break; 
     case LAYOUT_THREE_RIGHT:	  s=_R("LAYOUT_THREE_RIGHT"); break; 
     case LAYOUT_FOUR:	        s=_R("LAYOUT_FOUR"); break; 
     case LAYOUT_LOAD:	        s=_R("LAYOUT_LOAD"); break; 
     case LAYOUT_SAVE:	        s=_R("LAYOUT_SAVE"); break; 
     case MENU_LAYOUT_END:	    s=_R("MENU_LAYOUT_END"); break; 
     case MENU_VIEW_END:	      s=_R("MENU_VIEW_END"); break; 

     case MENU_SETTINGS_START:	s=_R("MENU_SETTINGS_START"); break; 
     case ID_APP_SETTINGS:	    s=_R("ID_APP_SETTINGS"); break; 
     case ID_TIMEBAR_SETTINGS:	s=_R("ID_TIMEBAR_SETTINGS"); break; 
     case ID_VIEW_SETTINGS:	    s=_R("ID_VIEW_SETTINGS"); break; 
     case MENU_SETTINGS_END:	  s=_R("MENU_SETTINGS_END"); break; 

     case EVT_START:	          s=_R("EVT_START"); break; 
     case UPDATE_UI:	          s=_R("UPDATE_UI"); break; 
     case UPDATE_PROPERTY:      s=_R("UPDATE_PROPERTY"); break; 

     case VME_ADD:	            s=_R("VME_ADD"); break; 
     case VME_ADDED:	          s=_R("VME_ADDED"); break; 
     case VME_REMOVE:	          s=_R("VME_REMOVE"); break; 
     case VME_REMOVING:	        s=_R("VME_REMOVING"); break; 
     case VME_SELECT:	          s=_R("VME_SELECT"); break; 
     case VME_SELECTED:	        s=_R("VME_SELECTED"); break;
     case VME_DCLICKED:         s=_R("VME_DCLICKED"); break;
     case VME_SHOW:	            s=_R("VME_SHOW"); break; 
     case VME_TRANSFORM:	      s=_R("VME_TRANSFORM"); break; 
     case VME_MODIFIED:	        s=_R("VME_MODIFIED"); break; 
     case VME_CHOOSE:						s=_R("VME_CHOOSE"); break; 
     case VME_REF_SYS_CHOOSED:  s=_R("VME_REF_SYS_CHOOSED"); break;
     case VME_FEM_DATA_CHOOSED: s=_R("VME_FEM_DATA_CHOOSED"); break;
     case VME_PICKED:	          s=_R("VME_PICKED"); break; 
     case VME_PICKING:	        s=_R("VME_PICKING"); break; 
     case VME_VISUAL_MODE_CHANGED:s=_R("VME_VISUAL_MODE_CHANGED"); break;
     case CREATE_STORAGE:       s=_R("CREATE_STORAGE"); break; 

     case VIEW_QUIT:            s=_R("VIEW_QUIT"); break;
     case VIEW_CREATE:          s=_R("VIEW_CREATE"); break; 
     case VIEW_CREATED:         s=_R("VIEW_CREATED"); break; 
     case VIEW_DELETE:          s=_R("VIEW_DELETE"); break; 
     case VIEW_SELECT:          s=_R("VIEW_SELECT"); break; 
     case VIEW_SELECTED:        s=_R("VIEW_SELECTED"); break; 
     case VIEW_CLICKED:         s=_R("VIEW_CLICKED"); break; 
     case VIEW_MAXIMIZE:        s=_R("VIEW_CLICKED"); break; 
     case VIEW_RESIZE:          s=_R("VIEW_RESIZE"); break; 
     case VIEW_SAVE_IMAGE:      s=_R("VIEW_SAVE_IMAGE"); break; 

     case PER_PUSH:             s=_R("PER_PUSH"); break; 
     case PER_POP:              s=_R("PER_POP"); break; 
     case CAMERA_FRONT:	        s=_R("CAMERA_FRONT"); break; 
     case CAMERA_BACK:	        s=_R("CAMERA_BACK"); break; 
     case CAMERA_LEFT:	        s=_R("CAMERA_LEFT"); break; 
     case CAMERA_RIGHT:	        s=_R("CAMERA_RIGHT"); break; 
     case CAMERA_TOP:	          s=_R("CAMERA_TOP"); break; 
     case CAMERA_BOTTOM:	      s=_R("CAMERA_BOTTOM"); break; 
     case CAMERA_PERSPECTIVE:	  s=_R("CAMERA_PERSPECTIVE"); break; 
     case CAMERA_RX_FRONT:	    s=_R("CAMERA_RX_FRONT"); break; 
     case CAMERA_RX_LEFT:	      s=_R("CAMERA_RX_LEFT"); break; 
     case CAMERA_RX_RIGHT:	    s=_R("CAMERA_RX_RIGHT"); break; 
    // modified by Stefano 21-9-2004 (begin)
    case CAMERA_RXFEM_XNEG:     s=_R("CAMERA_RXFEM_XNEG");break;
    case CAMERA_RXFEM_XPOS:     s=_R("CAMERA_RXFEM_XPOS");break;
    case CAMERA_RXFEM_YNEG:     s=_R("CAMERA_RXFEM_YNEG");break;
    case CAMERA_RXFEM_YPOS:     s=_R("CAMERA_RXFEM_YPOS");break;
    case CAMERA_RXFEM_ZNEG:     s=_R("CAMERA_RXFEM_ZNEG");break;
    case CAMERA_RXFEM_ZPOS:     s=_R("CAMERA_RXFEM_ZPOS");break;
    // modified by Stefano 21-9-2004 (end)
		 case CAMERA_BLEND:					s=_R("CAMERA_BLEND");break;
     case CAMERA_CT:						s=_R("CAMERA_CT"); break; 
     case CAMERA_OS_X:	        s=_R("CAMERA_OS_X"); break; 
     case CAMERA_OS_Y:	        s=_R("CAMERA_OS_Y"); break; 
     case CAMERA_OS_Z:	        s=_R("CAMERA_OS_Z"); break; 
     case CAMERA_OS_P:	        s=_R("CAMERA_OS_P"); break; 
     case CAMERA_UPDATE:	      s=_R("CAMERA_UPDATE"); break; 
     case CAMERA_RESET:	        s=_R("CAMERA_RESET"); break; 
		 case CAMERA_FIT:						s=_R("CAMERA_FIT"); break; 
		 case CAMERA_FLYTO:					s=_R("CAMERA_FLYTO"); break; 
     case LINK_CAMERA_TO_INTERACTOR: s=_R("LINK_CAMERA_TO_INTERACTOR"); break; 
     case DEVICE_ADD:           s=_R("DEVICE_ADD"); break; 
     case DEVICE_REMOVE:        s=_R("DEVICE_REMOVE"); break; 
     case DEVICE_GET:           s=_R("DEVICE_GET"); break; 
     case INTERACTOR_ADD:       s=_R("INTERACTOR_ADD"); break; 
     case INTERACTOR_REMOVE:    s=_R("INTERACTOR_REMOVE"); break; 

     case OP_RUN:	              s=_R("OP_RUN"); break; 
     case OP_RUN_OK:	          s=_R("OP_RUN_OK"); break; 
     case OP_RUN_CANCEL:	      s=_R("OP_RUN_CANCEL"); break; 
     case OP_RUN_STARTING:	    s=_R("OP_RUN_STARTING"); break; 
     case OP_RUN_TERMINATED:	  s=_R("OP_RUN_TERMINATED"); break; 
     case OP_SHOW_GUI:	        s=_R("OP_SHOW_GUI"); break; 
     case OP_HIDE_GUI:	        s=_R("OP_HIDE_GUI"); break; 
     case OP_FORCE_STOP:	      s=_R("OP_FORCE_STOP"); break; 
     case REMOTE_PARAMETER:     s=_R("REMOTE_PARAMETER"); break; 

     case EVT_END:	            s=_R("EVT_END"); break; 
     case MENU_END:	            s=_R("MENU_END"); break; 

		 case SHOW_CONTEXTUAL_MENU: s=_R("SHOW_CONTEXTUAL_MENU"); break;


     case WIDGETS_START:	      s=_R("WIDGETS_START"); break; 
     case ID_LAYOUT:	          s=_R("ID_LAYOUT"); break; 
     case ID_CLOSE_SASH:	      s=_R("ID_CLOSE_SASH"); break; 
     case ID_LABEL_CLICK:	      s=_R("ID_LABEL_CLICK"); break; 
     case ID_DISPLAY_LIST:	    s=_R("ID_DISPLAY_LIST"); break; 
     case ID_SETTINGS:					s=_R("ID_SETTINGS"); break; 

     case IMPORT_FILE:          s=_R("IMPORT_FILE"); break;
     case CLEAR_UNDO_STACK:     s=_R("CLEAR_UNDO_STACK"); break;

     case WIDGETS_END:	        s=_R("WIDGETS_END"); break; 
     case PIC_START:	          s=_R("PIC_START"); break; 
     case TIME_SET:	            s=_R("TIME_SET"); break; 
     case TIME_PLAY:	          s=_R("TIME_PLAY"); break; 
     case TIME_STOP:	          s=_R("TIME_STOP"); break; 
     case TIME_PREV:	          s=_R("TIME_PREV"); break; 
     case TIME_NEXT:	          s=_R("TIME_NEXT"); break; 
     case TIME_BEGIN:	          s=_R("TIME_BEGIN"); break; 
     case TIME_END:	            s=_R("TIME_END"); break; 
     case MOVIE_RECORD:         s=_R("MOVIE_RECORD"); break; 
        
     case MCH_START:            s=_R("MCH_START"); break;
     case MCH_UP:               s=_R("MCH_UP"); break;
     case MCH_DOWN:             s=_R("MCH_DOWN"); break;
     case MCH_INPUT:            s=_R("MCH_INPUT"); break;
     case MCH_OUTPUT:           s=_R("MCH_OUTPUT"); break;
     case MCH_NODE:             s=_R("MCH_NODE"); break;
     case MCH_ANY:              s=_R("MCH_ANY"); break;
     case MCH_VTK:              s=_R("MCH_VTK"); break;
     case MCH_END:              s=_R("MCH_END"); break;

     case NODE_START:               s=_R("NODE_START"); break;
     case NODE_DETACHED_FROM_TREE:  s=_R("NODE_DETACHED_FROM_TREE"); break;
     case NODE_ATTACHED_TO_TREE:    s=_R("NODE_ATTACHED_TO_TREE"); break;
     case NODE_DESTROYED:           s=_R("NODE_DESTROYED"); break;
     case NODE_GET_STORAGE:         s=_R("NODE_GET_STORAGE"); break;
     case NODE_GET_ROOT:            s=_R("NODE_GET_ROOT"); break;
     case VME_TIME_SET:             s=_R("VME_TIME_SET"); break;
     case VME_MATRIX_CHANGED:       s=_R("VME_MATRIX_CHANGED"); break;
     case VME_MATRIX_PREUPDATE:     s=_R("VME_MATRIX_PREUPDATE"); break;
     case VME_MATRIX_UPDATE:        s=_R("VME_MATRIX_UPDATE"); break;
     case VME_ABSMATRIX_UPDATE:     s=_R("VME_ABSMATRIX_UPDATE"); break;
     case VME_OUTPUT_DATA_PREUPDATE:s=_R("VME_OUTPUT_DATA_PREUPDATE"); break;
     case VME_OUTPUT_DATA_UPDATE:   s=_R("VME_OUTPUT_DATA_UPDATE"); break;
     case VME_OUTPUT_DATA_CHANGED:  s=_R("VME_OUTPUT_DATA_CHANGED"); break;
     case VME_GET_NEWITEM_ID:       s=_R("VME_GET_NEWITEM_ID"); break;
     case NODE_END:                 s=_R("NODE_END"); break;

     case AVATAR_ADDED:         s=_R("AVATAR_ADDED"); break;
     case AVATAR_REMOVED:       s=_R("AVATAR_REMOVED"); break;

     case MATRIX_UPDATED:       s=_R("MATRIX_UPDATED"); break;

// const used by mafGUI and mafGUIValidator
     case MINID:	              s=_R("MINID"); break; 
     case MAXID:	              s=_R("MAXID"); break; 
     case ID_GUI_UPDATE:	      s=_R("ID_GUI_UPDATE"); break; 
     default:
      s += mafToString(id);
     break;
    }
    return s; 
}
//----------------------------------------------------------------------------
int* GetMAFExpertMode()
//----------------------------------------------------------------------------
{
  static int MAFExpertMode = TRUE;
  return &MAFExpertMode;
}
