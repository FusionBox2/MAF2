/*=========================================================================

 Program: MAF2
 Module: mafDataVector
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


#include "mafDataVector.h"
#include <wx/zipstrm.h>
#include <wx/zstream.h>
#include <wx/sstream.h>
#include <wx/wfstream.h>
#include <wx/fs_zip.h>

#include "mafZipUtility.h"
#include "mmuTimeSet.h"
#include "mafStorageElement.h"
#include "mafStorage.h"
#include "mafEvent.h"
#include "mafEventIO.h"
#include "mafIndent.h"
#include "mafVMERoot.h"
#include "mafAttribute.h"

#include <fstream>

#define round(x) (x<0?ceil((x)-0.5):floor((x)+0.5))

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
/** Event ID used to know if the VME should serialize itself as a single or multiple binary files.*/
// static const mafID m_SingleFileDataId = mmuIdFactory::GetNextId("SINGLE_FILE_DATA");

//-----------------------------------------------------------------------
mafCxxTypeMacro(mafDataVector)
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
mafDataVector::mafDataVector()
//-----------------------------------------------------------------------
{
  m_Crypting        = false;
  m_DataModified    = false;
  m_SingleFileMode  = true;
  m_JustRestored    = false;
  m_VectorID        = -1;
  m_ArchiveName     = _R("");
}

//-----------------------------------------------------------------------
mafDataVector::~mafDataVector()
//-----------------------------------------------------------------------
{
  for (auto& elem : *this)
  {
    elem.second->SetListener(NULL); // detach items before destroying
  }
}
//-------------------------------------------------------------------------
bool mafDataVector::GetCrypting()
//-------------------------------------------------------------------------
{
  return m_Crypting;
}

//-------------------------------------------------------------------------
void mafDataVector::SetCrypting(bool flag)
//-------------------------------------------------------------------------
{
  if (m_Crypting == flag)
  {
    return;
  }
  m_Crypting = flag;
  Modified();
}

//-------------------------------------------------------------------------
mafID mafDataVector::GetVectorID()
//-------------------------------------------------------------------------
{
  if (m_VectorID < 0)
  {
    // retrieve the tree root
    mafEventIO e(this,NODE_GET_ROOT);
    InvokeEvent(e);

    mafVMERoot *root = mafVMERoot::SafeDownCast(e.GetRoot());
    m_VectorID = root ? root->GetNextItemId():-1;
  }
  return m_VectorID;
}

//-------------------------------------------------------------------------
void mafDataVector::UpdateVectorId()
//-------------------------------------------------------------------------
{
  // retrieve the tree root
  mafEventIO e(this,NODE_GET_ROOT);
  InvokeEvent(e);

  mafVMERoot *root = mafVMERoot::SafeDownCast(e.GetRoot());
  m_VectorID = root ? root->GetNextItemId():-1;
}

//-----------------------------------------------------------------------
void mafDataVector::ShallowCopy(mafDataVector *array)
//-----------------------------------------------------------------------
{
  RemoveAllItems();
  for (auto& elem : *array)
  {
    mafVMEItem *item=elem.second;
	  mafVMEItem *copy = item->NewInstance();
    assert(copy);
	  copy->ShallowCopy(item);
    AppendItem(copy); //  Changed by Losi 09.24.2009:
                      //  Before was AppendItem(item) this generated leaks because copy was unreferenced
  }

  Modified();
}

//-------------------------------------------------------------------------
void mafDataVector::DeepCopyVmeLarge(mafDataVector *o)
//-------------------------------------------------------------------------
{
  RemoveAllItems();
  for (auto& elem : *o)
  {
    mafVMEItem *m=elem.second;
    mafVMEItem *new_item=m->NewInstance();
    new_item->DeepCopyVmeLarge(m);
    AppendItem(new_item);
  }
  Modified();
}

//-----------------------------------------------------------------------
void mafDataVector::SetSingleFileMode(bool mode)
//-----------------------------------------------------------------------
{
  m_SingleFileMode = mode;
  Modified();
}

//-----------------------------------------------------------------------
void mafDataVector::AppendItem(mafVMEItem *m)
//-----------------------------------------------------------------------
{
  m->SetListener(this);
  Superclass::AppendItem(m);
}
//-----------------------------------------------------------------------
void mafDataVector::PrependItem(mafVMEItem *m)
//-----------------------------------------------------------------------
{
  m->SetListener(this);
  Superclass::PrependItem(m);
}
//-----------------------------------------------------------------------
void mafDataVector::InsertItem(mafVMEItem *m)
//-----------------------------------------------------------------------
{
  m->SetListener(this);
  Superclass::InsertItem(m);
}
//-----------------------------------------------------------------------
void mafDataVector::InternalStore(mafStorageElementBuilder& parent)
//-----------------------------------------------------------------------
{
  parent(_R("NumberOfItems")).SetValue(mafToString(GetNumberOfItems()));
  parent(_R("ItemTypeName")).SetValue(_R(GetItemTypeName()));

  // retrieve the tree root
  mafEventIO e(this,NODE_GET_ROOT);
  InvokeEvent(e);

  mafVMERoot *root = mafVMERoot::SafeDownCast(e.GetRoot());
  assert(root);

  m_VectorID = GetVectorID();

  // the DataVector ID
  parent(_R("VectorID")).SetValue(mafToString(m_VectorID));

  mafEventIO es(this,NODE_GET_STORAGE);
  InvokeEvent(es);
  mafStorage *storage = es.GetStorage();
  assert(storage);

  // define base file name for data files
  mafString base_url = storage->GetURL();
  auto last_dot = base_url.find_last_of('.');

  if (last_dot != mafString::npos)
  {
	  base_url.erase(last_dot);
  }
  mafString base_name = base_url;

  auto last_slash = base_name.find_last_of('/');
  if (last_slash != mafString::npos)
  {
    base_name.erase(0,last_slash + 1);
  }
  
  // this test force data to be written when the MSF filename has changed.
  // The case when data has just been loaded is avoided, since in that case
  // there is not yet an old filename to which the file was saved.
  bool base_name_changed = m_LastBaseURL != base_url && !m_JustRestored;
  bool release_old_files = !base_name_changed && !m_LastBaseURL.empty();
  
  // now check if the filename is (really) changed or if something has changed.
  // Notice that in mafVMEAbstractGaneric, when the storage URL is changed the 
  // filename change event it caught and a Modified() is forced to the m_DataVector, 
  // thus in case the MSF has just been loaded (m_JustReastored is true) but the
  // storage object's URL has been changed, the DataVector appears as being Modified
  // and data save to the new place is forced!
  if (base_name_changed || IsDataModified()) // if some data added or removed...
  {
    if (m_LastBaseURL != base_url)
    {
      /*// ask how to save the binary file (as setting of the application):
      // single file or multi file.
      mafEvent single_file_event(this, SINGLE_FILE_DATA);
      InvokeEvent(&single_file_event);
      SetSingleFileMode(single_file_event.GetBool());*/
      bool single_file_mode;
      single_file_mode = GetNumberOfItems() > 1;
      SetSingleFileMode(single_file_mode);
    }

    // store data
    bool new_data = false;
    
    std::vector<mafString> data_files;
    data_files.resize(GetNumberOfItems());
    
    int ret;

    // store single data elements into its own files or into a single file if m_SingleFileMode is true
    if (m_SingleFileMode)
    {
      // check if there is at least one item
      if (begin() != end())
      {
        //////////////////////////////////////////////////////////////////////////
        int resolvedURL = MAF_OK;
        mafString filename;
        resolvedURL = storage->ResolveInputURL(m_ArchiveName, filename);

        if (resolvedURL == MAF_OK && mafFileExists(filename))//Only if exist an archive where it is possible read the vtk data
        {
          mafEventMacro(mafEvent(this,PROGRESSBAR_SHOW));
          long progress = 0;

          std::vector<mafString> filesExtracted = ZIPOpen(filename);

          int dataIndex = 0;
          int step = round(this->GetNumberOfItems() / 100) + 1;
          for (auto& elem : *this)
          {
            if ((dataIndex % step == 0))
            {
              progress++;
              mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,(intptr_t)progress));
            }
			mafVMEItem* itemTmp = elem.second;
            int IOmode = itemTmp->GetIOMode();
            itemTmp->SetIOModeToDefault();
            itemTmp->UpdateData();
            itemTmp->SetIOMode(IOmode);

            dataIndex++;
          }
          
          mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,(intptr_t)100));
          mafEventMacro(mafEvent(this,PROGRESSBAR_HIDE));

          for (int i=0;i<filesExtracted.size();i++)
          {
            mafFileRemove(filesExtracted[i]);
          }
        }

        //////////////////////////////////////////////////////////////////////////

        m_ArchiveName = base_name;
        m_ArchiveName += _R(".");
        m_ArchiveName += mafToString(m_VectorID);
        m_ArchiveName += _R(".z");

        begin()->second->UpdateData();
        m_ArchiveName += _R(begin()->second->GetDataFileExtension());
        mafString tmp_archive;
        storage->GetTmpFile(tmp_archive);
        wxFileOutputStream out(tmp_archive.toWx());
        wxZipOutputStream zip(out);
        if (!out || !zip)
          return;

        for (auto& elem : *this)
        {
          auto item = elem.second;

          // set item ID if not yet set
          if (item->GetId() < 0)
            item->SetId(root->GetNextItemId());

          // data file URL is specified as a local filename
          mafString data_file_url;
          data_file_url += base_name + _R(".") + mafToString(item->GetId()) + _R(".") + _R(item->GetDataFileExtension()); // extension is defined by the kind of item itself

          item->SetCrypting(m_Crypting);
          item->SetIOModeToMemory();
          // if in SaveAs do not remove old filename...
          //if (!base_name_changed && !m_LastBaseURL.IsEmpty())
          if(release_old_files)
          {
            item->ReleaseOldFileOn();
          }
          else
          {
            item->ReleaseOldFileOff();
          }

          ret = item->StoreData(NULL); //Storing in memory not require to create a file.
          if (ret == MAF_ERROR)
          {
            return;
          }
          item->SetURL(data_file_url.GetCStr());
          item->ReleaseOldFileOn(); // restore to default

          item->SetArchiveFileName(m_ArchiveName);
          if (!item->StoreToArchive(zip))
          {
            mafMessage(_M(_L("Unable to write ") + data_file_url + _L(" into archive file ") + m_ArchiveName));
            ret = MAF_ERROR;
          }

          // Free the memory allocated from the string saved into the archive.
          item->ReleaseOutputMemory();

          switch (ret)
          {
            case MAF_OK: 
              new_data = true;
            break;  // new data written on disk
            case MAF_ERROR:
              return;   // I/O error while writing
          }
        }

        if (!zip.Close() || !out.Close())
        {
          return;
        }

        storage->StoreToURL(tmp_archive, m_ArchiveName);
      }
    }
    else
    {
      for (auto& elem : *this)
      {
        auto item = elem.second;
        item->UpdateData();

        // set item ID if not yet set
        if (item->GetId() < 0)
          item->SetId(root->GetNextItemId());

        // data file URL is specified as a local filename
        mafString data_file_url;
        data_file_url += base_name + _R(".") + mafToString(item->GetId()) + _R(".") + _R(item->GetDataFileExtension()); // extension is defined by the kind of item itself

        item->SetCrypting(m_Crypting);

        // if in SaveAs do not remove old filename...
        if(release_old_files)
        {
          item->ReleaseOldFileOn();
        }
        else
        {
          item->ReleaseOldFileOff();
        }

        item->SetIOModeToDefault();
        ret = item->StoreData(data_file_url.GetCStr());
        
        item->ReleaseOldFileOn(); // restore to default
        switch (ret)
        {
          case MAF_OK: 
            new_data = true;
          break;  // new data written on disk
          case MAF_ERROR:
            return;   // I/O error while writing
        }
      }
    }
    // update the last base name for next time writing
    m_LastBaseURL = base_url;
    m_JustRestored = false;
  }

  parent(_R("SingleFileMode")).SetValue(m_SingleFileMode ? _R("true") : _R("false"));
  if (m_SingleFileMode)
  {
    parent(_R("ArchiveFileName")).SetValue(m_ArchiveName);
  }

  // Store meta-data (meta-data is stored later to be able set some info about stored data files)
  for (auto& elem : *this)
  {
    parent[_R("VItem")].SetValue(elem.second.GetPointer());
  }

  m_DataModified = false;
}
//-----------------------------------------------------------------------
void mafDataVector::InternalRestore(const mafStorageElement& node)
//-----------------------------------------------------------------------
{
  mafID num_items = node(_R("NumberOfItems")).As<mafID>();
  mafString item_type = node(_R("ItemTypeName")).As<mafString>();
  mafString single_file = node(_R("SingleFileMode")).As<mafString>();
  m_VectorID = node(_R("VectorID")).As<mafID>();

  m_JustRestored = true;

  //if (//node.GetAttributeAsInteger(_R("NumberOfItems"), num_items) == MAF_OK &&
      //node.GetAttribute(_R("ItemTypeName"), item_type) == MAF_OK &&
      //node.GetAttribute(_R("SingleFileMode"), single_file) == MAF_OK
      //&& node.GetAttributeAsInteger(_R("VectorID"), m_VectorID) == MAF_OK
  //  )
  {
    SetItemTypeName(item_type.GetCStr());
    SetSingleFileMode(single_file == _R("true") || single_file == _R("True") || single_file == _R("TRUE"));
    if (m_SingleFileMode)
    {
      m_ArchiveName = node(_R("ArchiveFileName")).As<mafString>();
    }
  }
  //else
  {
    //return MAF_ERROR;
  }

  // restore items meta-data
  auto elements = node[_R("VItem")];

  assert(num_items == elements.GetNumItems()); // check the number of elements

  for (size_t i = 0; i < elements.GetNumItems(); i++)
  {
    mafVMEItem *obj = elements[i].As<mafVMEItem>();
    mafVMEItem *item = mafVMEItem::SafeDownCast(obj);
    assert(item);
    if (!item)
    {
      mafErrorMacro("Cannot restore VME-Item: Wrong object type or I/O problems");
      return;
    }
    if (m_SingleFileMode)
    {
      item->SetIOModeToMemory();
      item->SetArchiveFileName(m_ArchiveName);
    }
    AppendItem(item);
  }
  // data is restored on demand by single items!

  // force the flag to false to avoid data rewriting while storing
  m_DataModified = false;
}

//-----------------------------------------------------------------------
void mafDataVector::OnEvent(mafEventBase *maf_event)
//-----------------------------------------------------------------------
{
  if (maf_event->GetId() == mafVMEItem::VME_ITEM_DATA_MODIFIED)
  {
    m_DataModified=true;
    return;
  }
  mafEventMacro(*maf_event);
}
