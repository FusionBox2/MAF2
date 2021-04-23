/*=========================================================================

 Program: MAF2
 Module: mafOpValidateTree
 Authors: Paolo Quadrani , Stefano Perticoni
 
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


#include "mafOpValidateTree.h"
#include "mafDecl.h"
#include "mafEvent.h"
#include "mafGUI.h"

#include "mafVMEStorage.h"
#include "mafEventIO.h"
#include "mafDataVector.h"
#include "mafVMEItem.h"
#include "mafVMEGenericAbstract.h"
#include "mafVMERoot.h"
#include "mafNodeIterator.h"
#include "mafVMEExternalData.h"
#include "vtkDirectory.h"

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpValidateTree);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpValidateTree::mafOpValidateTree(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
  m_MSFPath = _R("");
}
//----------------------------------------------------------------------------
mafOpValidateTree::~mafOpValidateTree()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* mafOpValidateTree::Copy()   
//----------------------------------------------------------------------------
{
	return new mafOpValidateTree(GetLabel());
}
//----------------------------------------------------------------------------
void mafOpValidateTree::OpRun()
//----------------------------------------------------------------------------
{
  int result = ValidateTree();
  if(m_TestMode == false)
  {
    if (result == mafOpValidateTree::VALIDATE_SUCCESS)
    {
      wxMessageBox(_("Tree validation terminated succesfully!!."), _("Info"));
    }
    else if (result == mafOpValidateTree::VALIDATE_WARNING)
    {
      wxMessageBox(_("Tree Patched!! In log area you can find details."), _("Warning"));
    }
    else
    {
      wxMessageBox(_("Tree invalid!! In log area you can find details."), _("Warning"));
    }
  }
  mafEventMacro(mafEvent(this,OP_RUN_OK));
}
//----------------------------------------------------------------------------
int mafOpValidateTree::ValidateTree()
//----------------------------------------------------------------------------
{
  m_MSFTreeAbsFileNamesSet.clear();
  assert(m_MSFTreeAbsFileNamesSet.size() == 0);

  int result = mafOpValidateTree::VALIDATE_SUCCESS;

  mafNode *node;
  mafVMERoot *root = mafVMERoot::SafeDownCast(m_Input->GetRoot());
  assert(root != NULL);
  int max_item_id = root->GetMaxItemId();
  int max_node_id = root->GetMaxNodeId();

  mafNodeIterator *iter = root->NewIterator();
  try
  {
    for (node = iter->GetFirstNode(); node; node = iter->GetNextNode())
    {
      // check node ID
      bool valid = node->IsValid();
      if (!valid && !node->IsMAFType(mafVMERoot))
      {
        ErrorLog(mafOpValidateTree::INVALID_NODE, node->GetName().GetCStr());
        node->UpdateId();
        if (result != mafOpValidateTree::VALIDATE_ERROR)
        {
          result = mafOpValidateTree::VALIDATE_WARNING;
        }
      }
      int numLinks = node->GetNumberOfLinks();
      if (numLinks > 0)
      {
        // check node links
        mafNode::mafLinksMap::iterator lnk_it;
        for (lnk_it = node->GetLinks()->begin(); lnk_it != node->GetLinks()->end(); lnk_it++)
        {
          if (lnk_it->second.m_Node == NULL)
          {
            ErrorLog(mafOpValidateTree::LINK_NULL, node->GetName().GetCStr(), lnk_it->first.GetCStr());
            result = mafOpValidateTree::VALIDATE_ERROR;
            continue;
          }
          valid = root->IsInTree(lnk_it->second.m_Node);
          if (!valid)
          {
            ErrorLog(mafOpValidateTree::LINK_NOT_PRESENT, lnk_it->second.m_Node->GetName().GetCStr());
            result = mafOpValidateTree::VALIDATE_ERROR;
          }
          valid = lnk_it->second.m_Node->IsValid();
          if (!valid && !lnk_it->second.m_Node->IsMAFType(mafVMERoot))
          {
            ErrorLog(mafOpValidateTree::INVALID_NODE, lnk_it->second.m_Node->GetName().GetCStr());
            result = mafOpValidateTree::VALIDATE_ERROR;
          }
        }
      }
      // check data (binary data files are associated to that file that 
      // have the DataVector => inherit from mafVMEGenericAbstract
      mafString urlString = _R("");
      mafString archiveFilename = _R("");
      wxString absFilename = "";
      mafVMEItem *item = NULL;
      bool singleFileMode = false;
      mafVMEGenericAbstract *vme = mafVMEGenericAbstract::SafeDownCast(node);

      if (vme != NULL && vme->IsA("mafVMEExternalData"))
      {
        mafVMEExternalData *ed = mafVMEExternalData::SafeDownCast(vme);
        mafString fileName = _R(ed->GetFileName());
        mafString extension = _R(ed->GetExtension());

        if (m_MSFPath.IsEmpty())
        {
          mafEventIO e(this,NODE_GET_STORAGE);
          vme->ForwardUpEvent(e);
          mafVMEStorage *storage = mafVMEStorage::SafeDownCast(e.GetStorage());
          if (storage != NULL)
          {
            m_MSFPath = storage->GetURL();
            m_MSFPath.ExtractPathName();
          }
        }

        wxString absFilename = (m_MSFPath + _R("\\") + fileName + _R(".") + extension).toWx();
        absFilename.Replace("/","\\");

        if (!mafFileExists(mafWxToString(absFilename)))
        {
          ErrorLog(mafOpValidateTree::BINARY_FILE_NOT_PRESENT, absFilename.c_str());
          result = mafOpValidateTree::VALIDATE_ERROR;
        }
        else
        {
          m_MSFTreeAbsFileNamesSet.insert(mafWxToString(absFilename).toStd());
#pragma message ("a bit hacky")
        }
      }
      else if (vme != NULL) // any other vme type
      {         
        mafDataVector *dv = vme->GetDataVector();
        if (dv)
        {
          for (int t = 0; t < dv->GetNumberOfItems(); t++)
          {
            if (m_MSFPath.IsEmpty())
            {
              mafEventIO e(this,NODE_GET_STORAGE);
              vme->ForwardUpEvent(e);
              mafVMEStorage *storage = mafVMEStorage::SafeDownCast(e.GetStorage());
              if (storage != NULL)
              {
                m_MSFPath = storage->GetURL();
                m_MSFPath.ExtractPathName();
              }
            }
            // binary files exists => VME is animated on data
            item = dv->GetItemByIndex(t);
            if (item == NULL)
            {
              ErrorLog(mafOpValidateTree::ITEM_NOT_PRESENT, vme->GetName().GetCStr());
              result = mafOpValidateTree::VALIDATE_ERROR;
            }
            else
            {
              int item_id = item->GetId();
              if (max_item_id < item_id)
              {
                ErrorLog(mafOpValidateTree::MAX_ITEM_ID_PATCHED, vme->GetName().GetCStr());
                root->SetMaxItemId(item_id);
                if (result != mafOpValidateTree::VALIDATE_ERROR)
                {
                  result = mafOpValidateTree::VALIDATE_WARNING;
                }
              }

              urlString = _R(item->GetURL());
              if (urlString.IsEmpty())
              {
                ErrorLog(mafOpValidateTree::URL_EMPTY, vme->GetName().GetCStr());
                result = mafOpValidateTree::VALIDATE_ERROR;
              }
              //archiveFilename = item->GetArchiveFileName();
              singleFileMode = dv->GetSingleFileMode();
              if (singleFileMode && archiveFilename.IsEmpty())
              {
                ErrorLog(mafOpValidateTree::ARCHIVE_FILE_NOT_PRESENT, vme->GetName().GetCStr());
                result = mafOpValidateTree::VALIDATE_ERROR;
              }
              absFilename = (m_MSFPath + _R("\\")).toWx();
              absFilename.Replace("/","\\");

              if (singleFileMode)
              {
                absFilename << archiveFilename.toWx();
              }
              else
              {
                absFilename << urlString.toWx();
              }

              if (!mafFileExists(mafWxToString(absFilename)))
              {
                ErrorLog(mafOpValidateTree::BINARY_FILE_NOT_PRESENT, absFilename.c_str());
                result = mafOpValidateTree::VALIDATE_ERROR;
              }
              else
              {
                m_MSFTreeAbsFileNamesSet.insert(mafWxToString(absFilename).toStd());
#pragma message("a bit hacky")
              }
            } // item != NULL
          } // for() on items
        } // if(dv)
      } // if(vme) vme is inherited from a mafVMEGenericAbstract => has binary data file
    } // for () on node into the tree
  } // try
  catch (...)
  {
    ErrorLog(mafOpValidateTree::EXCEPTION_ON_ITERATOR, iter->GetCurrentNode()->GetName().GetCStr());
  }
  iter->Delete();
 
  return result;
}
//----------------------------------------------------------------------------
void mafOpValidateTree::ErrorLog(int error_num, const char *node_name, const char *description)
//----------------------------------------------------------------------------
{
  switch(error_num)
  {
    case mafOpValidateTree::INVALID_NODE:
      mafLogMessage(_M(_L("Patched Node '") + mafString(_R(node_name)) + _L("' with an invalid ID!!")));
    break;
    case mafOpValidateTree::LINK_NOT_PRESENT:
      mafLogMessage(_M(_L("Link node '") + mafString(_R(node_name)) + _L("' is not present into the tree")));
    break;
    case mafOpValidateTree::LINK_NULL:
      mafLogMessage(_M(_L("Link '") + mafString(_R(description)) + _L("' of node '") + mafString(_R(node_name)) + _L("' is null")));
    break;
    case mafOpValidateTree::EXCEPTION_ON_ITERATOR:
      mafLogMessage(_M(_L("Exception occurred during iteration on node '") + mafString(_R(node_name)) + _L("'")));
    break;
    case mafOpValidateTree::ITEM_NOT_PRESENT:
      mafLogMessage(_M(_L("Item not present in node '") + mafString(_R(node_name)) + _L("'")));
    break;
    case mafOpValidateTree::MAX_ITEM_ID_PATCHED:
      mafLogMessage(_M(_L("Item of Node '") + mafString(_R(node_name)) + _L("' has caused Max item Id to be patched!!")));
    break;
    case mafOpValidateTree::URL_EMPTY:
      mafLogMessage(_M(_L("URL referring to binary data not present in node '") + mafString(_R(node_name)) + _L("'")));
    break;
    case mafOpValidateTree::BINARY_FILE_NOT_PRESENT:
      mafLogMessage(_M(_L("Binary data file '") + mafString(_R(node_name)) + _L("' not present")));
    break;
    case mafOpValidateTree::ARCHIVE_FILE_NOT_PRESENT:
      mafLogMessage(_M(_L("Archive data file not present for node '") + mafString(_R(node_name)) + _L("'")));
    break;
  }
}

int mafOpValidateTree::GetMSFTreeABSFileNamesSet( std::set<std::string> &fileNamesSet )
{
  int result = this->ValidateTree(); // needed to fill m_MSFTreeBinaryFilesSet ivar
  
  if (result != mafOpValidateTree::VALIDATE_SUCCESS)
  {
    fileNamesSet.clear();
    mafLogMessage(_M("MSF Tree is invalid: GetMSFTreeABSFileNamesSet returning an empty set"));
    return MAF_ERROR;
  }
 
  fileNamesSet = m_MSFTreeAbsFileNamesSet;
  return MAF_OK;
 
}