/*=========================================================================

 Program: MAF2
 Module: mafVMEExternalData
 Authors: Marco Petrone - Roberto Mucci
 
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

#include "mafVMEExternalData.h"

#include <wx/filefn.h>

#include "mafTagArray.h"
#include "mafStorage.h"
#include "mafEventIO.h"
#include "ftk/IO/StorageElement.h"

#include <vtkIndent.h>

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEExternalData)
//-------------------------------------------------------------------------


mafVMEExternalData::mafVMEExternalData()
{
}

mafVMEExternalData::~mafVMEExternalData()
{
}

//-------------------------------------------------------------------------
int mafVMEExternalData::DeepCopy(mafNode *a)
//-------------------------------------------------------------------------
{
  if (Superclass::DeepCopy(a)==MAF_OK)
  {
    mafVMEExternalData *external = mafVMEExternalData::SafeDownCast(a);
    this->SetCurrentPath(external->GetCurrentPath());
    return MAF_OK;
  }  
  return MAF_ERROR;
}

//-------------------------------------------------------------------------
bool mafVMEExternalData::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{
  bool ret = false;
  if (Superclass::Equals(vme))
  {
    mafVMEExternalData *external = mafVMEExternalData::SafeDownCast(vme);
    ret = (m_MSFPath == external->GetCurrentPath());
  }
  return ret;
}

//-------------------------------------------------------------------------
void mafVMEExternalData::SetExtension(const char *ext)
//-------------------------------------------------------------------------
{
  mafTagItem item;

  item.SetName(_R("EXTDATA_EXTENSION"));
  item.SetValue(_R(ext));
  this->GetTagArray()->SetTag(item);
}

//-------------------------------------------------------------------------
void mafVMEExternalData::SetFileName(const char *filename)
//-------------------------------------------------------------------------
{
  mafTagItem item;
  
  item.SetName(_R("EXTDATA_FILENAME"));
  item.SetValue(_R(filename));
  this->GetTagArray()->SetTag(item);
}

//-------------------------------------------------------------------------
const char* mafVMEExternalData::GetExtension()
//-------------------------------------------------------------------------
{
  if(mafTagItem *item=this->GetTagArray()->GetTag(_R("EXTDATA_EXTENSION")))
    return item->GetValue().GetCStr();
  return NULL;
}

//-------------------------------------------------------------------------
const char *mafVMEExternalData::GetFileName()
//-------------------------------------------------------------------------
{
  if(mafTagItem* item=this->GetTagArray()->GetTag(_R("EXTDATA_FILENAME")))
    return item->GetValue().GetCStr();
  return NULL;
}

//-------------------------------------------------------------------------
mafString mafVMEExternalData::GetAbsoluteFileName()
//-------------------------------------------------------------------------
{
  mafString filename = m_MSFPath;

  if (m_MSFPath.empty())
  {
    InitializeCurrentPath();
    filename = m_MSFPath;
  }
  
  filename.append(_R("\\"));
  filename.append(_R(this->GetFileName()));
  filename.append(_R("."));
  filename.append(_R(this->GetExtension()));

  if (mafFileExists(filename))
  {
    return filename;
  }
  else
  {
    // if file not exists, than the file is not stored yet and I must use TmpPath
    GetTmpPath();
    filename = m_TmpPath;
    filename.append(_R("\\"));
    filename.append(_R(this->GetFileName()));
    filename.append(_R("."));
    filename.append(_R(this->GetExtension()));
    return filename;
  }
}

//-------------------------------------------------------------------------
mafString mafVMEExternalData::GetTmpPath()
//-------------------------------------------------------------------------
{
  mafString tmpPath = this->m_TmpPath;
  return tmpPath;
}

//-------------------------------------------------------------------------
mafString mafVMEExternalData::GetCurrentPath()
//-------------------------------------------------------------------------
{
  InitializeCurrentPath();
  mafString currentPath= this->m_MSFPath;
  return currentPath;
}

//-------------------------------------------------------------------------
void mafVMEExternalData::SetMimeType(const char *mimetype)
//-------------------------------------------------------------------------
{
  mafTagItem item;
  item.SetName(_R("EXTDATA_MIMETYPE"));
  item.SetValue(_R(mimetype));
  this->GetTagArray()->SetTag(item);
}
 
//-------------------------------------------------------------------------
const char* mafVMEExternalData::GetMimeType()
//-------------------------------------------------------------------------
{
  if(mafTagItem *item=this->GetTagArray()->GetTag(_R("EXTDATA_MIMETYPE")))
    return item->GetValue().GetCStr();
  return NULL;
}

//-----------------------------------------------------------------------
void mafVMEExternalData::InternalStore(mafStorageElementBuilder& parent)
//-----------------------------------------------------------------------
{
  InitializeCurrentPath();
  
  mafString fileNameOrigin =  GetAbsoluteFileName(); 
  mafString fileNameTarget = m_MSFPath + _R("\\") + _R(GetFileName()) + _R(".") + _R(GetExtension());

  if (fileNameOrigin != fileNameTarget)
  {
    bool copySuccess = mafFileCopy(fileNameOrigin, fileNameTarget);
    if (!copySuccess)
    {
      mafErrorMessage( _M("Error copying external file!") );
      return;
    }
  }
  this->SetCurrentPath(m_MSFPath);
  Superclass::InternalStore(parent);
}

//-----------------------------------------------------------------------
void mafVMEExternalData::InternalRestore(const mafStorageElement& node)
//-----------------------------------------------------------------------
{
  Superclass::InternalRestore(node);
  m_TmpPath = node.GetStorage()->GetURL();
  ExtractPathName(m_TmpPath);
  
  m_MSFPath.clear();
}

//-----------------------------------------------------------------------
void mafVMEExternalData::InitializeCurrentPath()
//-----------------------------------------------------------------------
{
  mafEventIO e(this,NODE_GET_STORAGE);
  ForwardUpEvent(e);
  mafStorage *storage=e.GetStorage();
  if (storage != NULL)
  {
    m_MSFPath = storage->GetURL();
    ExtractPathName(m_MSFPath);
  }
}

//-------------------------------------------------------------------------
void mafVMEExternalData::PrintSelf(std::ostream& os,const int indent)
//-------------------------------------------------------------------------
{
  if (m_MSFPath.empty())
  {
    InitializeCurrentPath();
  }

  mafVME::Print(os,indent);
  os << indent << "m_MSFPath: "  << this->m_MSFPath.GetCStr() << "\n";
  os << indent << "FileName: " << this->GetFileName() << "\n";
  os << indent << "File Extension: " << this->GetExtension() << "\n" ;
	os << indent << "MIME Type: " << this->GetMimeType() << "\n";
}
