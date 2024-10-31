/*=========================================================================

 Program: MAF2
 Module: mafVMEItem
 Authors: Marco Petrone - Paolo Quadrani
 
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


#include "mafVMEItem.h"
#include "ftk/VME/ItemFactory.h"
#include "mafFilesDirs.h"

#include "mafVMERoot.h"
#include "mafTagArray.h"
#include "mafIndent.h"
#include "mafStorageElement.h"
#include "mmuIdFactory.h"
#include "mafEventIO.h"
#include "mafStorage.h"
#include <math.h>
#include <assert.h>
#include "mafFilesDirs.h"


// bool mafVMEItem::m_GlobalCompareDataFlag=0;

MAF_ID_IMP(mafVMEItem::VME_ITEM_DATA_MODIFIED);

//-------------------------------------------------------------------------
mafCxxAbstractTypeMacro(mafVMEItem);
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEItem::mafVMEItem()
//-------------------------------------------------------------------------
{
  m_DataModified  = false;
  //m_VME=NULL;
  mafNEW(m_TagArray);
  
  m_Id            = -1;
  m_TimeStamp     = 0;
  m_Crypting      = false;
  m_IOMode        = DEFAULT;
  
  m_OutputMemory    = NULL;
  m_OutputMemorySize= 0;
  m_InputMemory     = NULL;
  m_InputMemorySize = 0;

  m_DataObserver= new mafVMEItemAsynchObserver();

  m_ReleaseOldFile  = true;
  m_IsLoadingData   = false;

  m_ArchiveFileName = _R("");
  m_ChecksumMD5     = _R("");
}

//-------------------------------------------------------------------------
mafVMEItem::~mafVMEItem()
//-------------------------------------------------------------------------
{
  cppDEL(m_DataObserver);
  SetURL(""); // this simply force to garbage collect the linked URL when the item is destroyed
  mafDEL(m_TagArray);
}

//-------------------------------------------------------------------------
bool mafVMEItem::GetCrypting()
//-------------------------------------------------------------------------
{
  return m_Crypting;
}

//-------------------------------------------------------------------------
void mafVMEItem::SetCrypting(bool flag)
//-------------------------------------------------------------------------
{
  m_Crypting = flag;
  Modified();
}

//-------------------------------------------------------------------------
void mafVMEItem::SetDataModified(bool flag)
//-------------------------------------------------------------------------
{
  m_DataModified=flag;
  Modified();
  InvokeEvent(VME_ITEM_DATA_MODIFIED);
}

//-------------------------------------------------------------------------
void mafVMEItem::GetBounds(double bounds[6])
//-------------------------------------------------------------------------
{
  UpdateBounds();
  m_Bounds.CopyTo(bounds);
}

//-------------------------------------------------------------------------
void mafVMEItem::GetBounds(mafOBB &bounds)
//-------------------------------------------------------------------------
{
  UpdateBounds();
  bounds=m_Bounds;
}

//-------------------------------------------------------------------------
double *mafVMEItem::GetBounds()
//-------------------------------------------------------------------------
{
  UpdateBounds();
  return m_Bounds.m_Bounds;
}


//-------------------------------------------------------------------------
mafTagArray *mafVMEItem::GetTagArray()
//-------------------------------------------------------------------------
{
  return m_TagArray;
}

//-------------------------------------------------------------------------
void mafVMEItem::DeepCopy(mafVMEItem *a)
//-------------------------------------------------------------------------
{
  assert(a);
  
  // copy member variables
  m_TagArray->DeepCopy(a->GetTagArray());
  SetTimeStamp(a->GetTimeStamp());
  m_DataType=a->GetDataType();
  //SetVME(a->GetVME());

  // subclasses should reimplement DeepCopy to copy data

  Modified();
  //m_UpdateTime.Modified();
  SetDataModified(true);
}

//-------------------------------------------------------------------------
void mafVMEItem::ShallowCopy(mafVMEItem *a)
//-------------------------------------------------------------------------
{
  assert(a);

  // make a copy of tags
  m_TagArray->DeepCopy(a->GetTagArray());

  SetTimeStamp(a->GetTimeStamp());
  m_DataType=a->GetDataType();
  //SetVME(a->GetVME());

  // subclass should copy also data pointer

  Modified();
  SetDataModified(true);
}

//-------------------------------------------------------------------------
bool mafVMEItem::Equals(mafVMEItem *o)
//-------------------------------------------------------------------------
{
  if (o==NULL || !o->IsA(GetTypeId()) || \
    m_DataType!=o->m_DataType || \
    !mafEquals(m_TimeStamp,o->m_TimeStamp)) // consider only 15 digits to avoid the dirty bit
  {
    return false;
  }

  UpdateBounds(); o->UpdateBounds();
  if (!m_Bounds.Equals(o->m_Bounds))
  {
    return false;
  }

  // must check the tags after updating the bounds, since UpdateBounds() could 
  // generate a new TAG for storing bounds.
  if (!GetTagArray()->Equals(o->GetTagArray()))
  {
    return false;
  }

  return true;
}

//-------------------------------------------------------------------------
void mafVMEItem::Print(std::ostream& os, const int tabs) const
//-------------------------------------------------------------------------
{
  mafIndent indent(tabs);

  Superclass::Print(os,indent);

  os << indent << "Contents:"<<std::endl;

  os << indent << "DataType: " << "\""<<m_DataType.GetCStr() <<"\""<<std::endl;

  os << indent << "TimeStamp: "<< GetTimeStamp()<<std::endl;

  os << indent << "TagArray:\n";
  m_TagArray->Print(os,indent.GetNextIndent());
}

//-------------------------------------------------------------------------
void mafVMEItem::InternalStore(mafStorageElementBuilder& parent)
//-------------------------------------------------------------------------
{
  parent[_R("URL")].SetValue(m_URL);
  parent[_R("Id")].SetValue(m_Id);
  parent[_R("DataType")].SetValue(m_DataType);
  parent[_R("TimeStamp")].SetValue(m_TimeStamp);
  parent[_R("Crypting")].SetValue(m_Crypting ? _R("true") : _R("false"));
  parent[_R("Bounds")].SetValue(mafToString(m_Bounds.m_Bounds, 6));
  parent[_R("TagArray")].SetValue(m_TagArray);
}

//-------------------------------------------------------------------------
void mafVMEItem::SetURL(const char *name)
//-------------------------------------------------------------------------
{
  if (!m_URL.empty()&&m_URL!=_R(name))
  {
    mafEventIO e(this,NODE_GET_STORAGE);
    mafEventMacro(e);
    mafStorage *storage=e.GetStorage();
    if (storage)
    {
      if (m_ReleaseOldFile)
        storage->ReleaseURL(m_URL); // remove old file
      SetDataModified(true); // force rewriting data 
    }
  }
  m_URL=_R(name);
}

//-------------------------------------------------------------------------
void mafVMEItem::InternalRestore(const mafStorageElement& node)
//-------------------------------------------------------------------------
{
  m_URL = node[_R("URL")].As<mafString>();
  m_Id = node[_R("Id")].As<int>();
  m_DataType = node[_R("DataType")].As<mafString>();
  m_TimeStamp = node[_R("TimeStamp")].As<double>();
  mafString crypting = node[_R("Crypting")].As<mafString>();
  mafParseVector(node[_R("Bounds")].As<mafString>(), m_Bounds.m_Bounds, 6);
  m_TagArray->Restore(node[_R("TagArray")]);
  m_Crypting = (crypting==_R("true")||crypting==_R("True")||crypting==_R("TRUE"))?true:false;
}

//-------------------------------------------------------------------------
int mafVMEItem::StoreData(const char *url)
//-------------------------------------------------------------------------
{
  return InternalStoreData(url);
}
//-------------------------------------------------------------------------
int mafVMEItem::RestoreData()
//-------------------------------------------------------------------------
{
  m_IsLoadingData = true;

  // To prevent errors when saving in different way of loaded data
  // archive file name have to be tested. It is not set before item is written
  // into the archive, so we have to switch into the DEFAULT mode to read correctly
  // the item and then restore the IO Mode.
  int old_mode = m_IOMode;
  if (m_ArchiveFileName.empty())
  {
    SetIOMode(DEFAULT);
  }
  int ret = InternalRestoreData();
  SetIOMode(old_mode);
  //m_IsLoadingData = false;
  m_IsLoadingData = ret != MAF_OK;
  return ret;
}

//-------------------------------------------------------------------------
int mafVMEItem::ExtractFileFromArchive(mafString &archive_fullname, mafString &item_file)
//-------------------------------------------------------------------------
{
	mafString path, name_;
	mafSplitPath(archive_fullname, &path, &name_);
	mafString itempath = path + _R("/") + item_file;
	if (!mafFileExists(itempath))
	{
		wxZipFSHandler* zipHandler = NULL;      ///< Handler for zip archive (used to open zmsf files)
		wxFileSystem* fileSystem = NULL;      ///< File system manager

		wxFSFile* zfileStream;
		wxZlibInputStream* zip_is;
		mafString pkg = _R("#zip:");
		mafString zipFile = archive_fullname + pkg + item_file;
		if (fileSystem == NULL)
			fileSystem = new wxFileSystem();

		if (zipHandler == NULL)
		{
			zipHandler = new wxZipFSHandler();
			fileSystem->AddHandler(zipHandler); // add the handler that manage zip protocol
			// (the handler to manage the local files protocol is already added to wxFileSystem)
		}

		//fileSystem->ChangePathTo(zipFile.toWx());

		auto yyy = fileSystem->OpenFile(zipFile.toWx());
		delete yyy;






		mafZIPOpen(archive_fullname);
	}
	wxFileInputStream iiin(itempath.toWx());
	m_InputMemorySize = iiin.GetSize();
	m_InputMemory = new char[m_InputMemorySize];
	iiin.Read((void*)m_InputMemory, (size_t)m_InputMemorySize);
	return MAF_OK;

	void* buffer = nullptr;
  size_t size = 0;
  if (!mafExtractZIP(archive_fullname, item_file, buffer, size))
  {
      return MAF_ERROR;
  }
  m_InputMemorySize = size;
  m_InputMemory = (const char*)buffer;
  return MAF_OK;
}

//-------------------------------------------------------------------------
void mafVMEItem::SetInputMemory(const char *int_str,unsigned long size)
//-------------------------------------------------------------------------
{
  m_InputMemory = int_str;
  m_InputMemorySize = size;
}

//-------------------------------------------------------------------------
void mafVMEItem::UpdateItemId()
//-------------------------------------------------------------------------
{
  // retrieve the tree root
  mafEventIO e(this,NODE_GET_ROOT);
  InvokeEvent(e);

  mafVMERoot *root = mafVMERoot::SafeDownCast(e.GetRoot());
  mafID itemId = root ? root->GetNextItemId():-1;

  SetId(itemId);
}
//----------------------------------------------------------------------------
bool* mafVMEItem::GetGlobalCompareDataFlag()
//----------------------------------------------------------------------------
{
  static bool globalCompareDataFlag = false;
  return &globalCompareDataFlag;
}
//----------------------------------------------------------------------------
void mafVMEItem::SetGlobalCompareDataFlag(bool f)
//----------------------------------------------------------------------------
{
  (*GetGlobalCompareDataFlag()) = f;
}

//----------------------------------------------------------------------------
//     ****************  mafVMEItemAsynchObserver  ****************
//----------------------------------------------------------------------------
MAF_ID_IMP(mafVMEItemAsynchObserver::VME_ITEM_DATA_DOWNLOADED);

//----------------------------------------------------------------------------
mafVMEItemAsynchObserver::mafVMEItemAsynchObserver()
//----------------------------------------------------------------------------
{
  m_Item = NULL;
}
//----------------------------------------------------------------------------
mafVMEItemAsynchObserver::~mafVMEItemAsynchObserver() 
//----------------------------------------------------------------------------
{
  m_Item = NULL;
}
//----------------------------------------------------------------------------
void mafVMEItemAsynchObserver::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (maf_event->GetId() == VME_ITEM_DATA_DOWNLOADED)
  {
    m_Item->ReadData(m_Filename);
  }
}

mafVMEItem* mafVMEItem::Create(const char* ItemType)
{
  if (auto object = ItemFactory::CreateItem(ItemType))
  {
    if (auto item = mafVMEItem::SafeDownCast(object))
    {
      return item;
    }
    delete object;
    return nullptr;
  }
  return nullptr;
}
