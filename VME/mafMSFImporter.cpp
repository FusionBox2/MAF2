/*=========================================================================

 Program: MAF2
 Module: mafMSFImporter
 Authors: Marco Petrone - Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafIncludeWX.h" // to be removed

#include "mafMSFImporter.h"
#include "mafVMERoot.h"
#include "mafVMEGenericAbstract.h"
#include "mafVMEGeneric.h"
#include "mafTagArray.h"
#include "mafTagItem.h"
#include "mafVMEItemVTK.h"
#include "mafDataVector.h"
#include "mafUtility.h"
#include "mafStorable.h"
#include "mafStorageElement.h"
#include "mmaMaterial.h"
#include "mmaMeter.h"
#include "mafMatrixVector.h"

#include "mafVMEGroup.h"
#include "mafVMEImage.h"
#include "mafVMELandmark.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMEMeter.h"
#include "mafVMEPointSet.h"
#include "mafVMEProber.h"
#include "mafVMERefSys.h"
#include "mafVMESlicer.h"
#include "mafVMESurface.h"
#include "mafVMEVolumeGray.h"
#include "mafVMEVolumeRGB.h"
#include "mafVMEExternalData.h"

#include "mafNodeIterator.h"

#include <vector>

//------------------------------------------------------------------------------
// mafMSFImporter
//------------------------------------------------------------------------------
namespace {
  bool StartsWith(const char *str1,const char *str2)
  {
    if ( !str1 || !str2 || strlen(str1) < strlen(str2) )
    {
      return false;
    }
    return !strncmp(str1, str2, strlen(str2));
  }
  int Compare(const char* str1, const char* str2)
    //----------------------------------------------------------------------------
  {
    if (!str1&&!str2)
      return 0;

    if ( !str1&&str2 )
    {
      return -1;
    }

    if ( !str2&&str1 )
    {
      return 1;
    }
    return strcmp(str1, str2);
  }

}

//------------------------------------------------------------------------------
int mafMSFImporter::InternalStore(mafStorageElement *node)
//------------------------------------------------------------------------------
{
  mafErrorMessage(_M("Writing MSF 1.x files is not supported!"));
  return MAF_ERROR;
}

//------------------------------------------------------------------------------
int mafMSFImporter::InternalRestore(mafStorageElement *node)
//------------------------------------------------------------------------------
{
  ///////////////////////////////////////////////
  // code to import the file from old MSF file //
  ///////////////////////////////////////////////
  mafVMERoot *root = mafVMERoot::New();
  if(!root)
    return MAF_ERROR;
  mafAutoPointer<mafVMERoot> root_ap = root;
  mafString root_name;
  if (node->GetAttribute(_R("Name"),root_name))
    root->SetName(root_name);

  mafID max_item_id;
  if (node->GetAttributeAsInteger(_R("MaxItemId"),max_item_id))
    root->SetMaxItemId(max_item_id);
  

  const mafStorageElement::ChildrenVector& children = node->GetChildren();

  for (int i=0;i<children.size();i++)
  {
    if (children[i]->GetName() == _R("TArray"))
    {
      if (RestoreTagArray(children[i],root->GetTagArray()) != MAF_OK)
      {
        mafErrorMacro("MSFImporter: error restoring Tag Array of node: \""<<root->GetName().GetCStr() <<"\"");
      }
    }
    else if (children[i]->GetName() == _R("VME"))
    {
      mafVME *child_vme=RestoreVME(children[i],root);
      if (child_vme==NULL)
      {
        mafErrorMacro("Error while restoring a VME (parent is the root)");
      }
      else
      {
        root->AddChild(child_vme);

        child_vme->UnRegister(NULL);
      }
    }    
  }
  mafNode *n = NULL;
  std::vector<mafNode *> link_list;
  mafNodeIterator *iter = root->NewIterator();
  // iteration for updating VME's ID
  for (n = iter->GetFirstNode(); n; n=iter->GetNextNode())
  {
    n->UpdateId();
  }
  // iteration for setting up linked vme
  for (n = iter->GetFirstNode(); n;n=iter->GetNextNode())
  {
    if (n->IsMAFType(mafVMEGeneric) && n->GetTagArray()->GetTag(_R("mflVMELink")))
    {
      link_list.push_back(n);
      mafTagItem *tag = n->GetTagArray()->GetTag(_R("VME_ALIAS_PATH"));
      mafNode *linkedVME = this->ParsePath(root, tag->GetValue().GetCStr());
      if (linkedVME != NULL)
      {
        mafID sub_id = -1;
        if (mafTagItem *ti = n->GetTagArray()->GetTag(_R("SUBLINK_ID")))
        {
          sub_id = (mafID)ti->GetValueAsDouble();
        }
        n->GetParent()->SetLink(n->GetName(), linkedVME, sub_id);
      }
    }
  }
  iter->Delete();
  // remove all mafVMEGeneric representing links
  for (int l=0;l<link_list.size();l++)
  {
    link_list[l]->ReparentTo(NULL);
  }
  link_list.clear();
  if(root->Initialize() == MAF_ERROR)
    return MAF_ERROR;
  SetRoot(root);
  return MAF_OK; 
}
//------------------------------------------------------------------------------
mafNode *mafMSFImporter::ParsePath(mafVMERoot *root,const char *path)
//------------------------------------------------------------------------------
{
  const char *str=path;
  mafNode *node=NULL;

  if (StartsWith(str,"/MSF"))
  {
    str+=4; // jump to next token

    node=root;
    for (;StartsWith(str,"/VME[");)
    {
      int idx;
      if (sscanf(str,"/VME[%d]",&idx)==EOF)
      {
        mafErrorMacro("Error Parsing XPATH string: \""<<str<<"\"");
        return NULL;
      }
      node=node->GetChild(idx);  

      if (node==NULL)
      {
        mafWarningMacro("Corrupted Link");
        return NULL;
      }

      // OK... this is not a true Regular Expression pareser!
      const char *next=strchr(str,']');
      if (next)
      {
        str=next+1;
      }
      else
      {
        mafErrorMacro("Error Parsing XPATH string");
        return NULL;
      }
    }

    // if string was not yet finished... parse error!!!
    if ((*str!=0)&&(!Compare(str,"/")))
    {
      mafErrorMacro("Error Parsing XPATH string: \""<<str<<"\"");
      return NULL;
    }
  }
  return node;
}

//------------------------------------------------------------------------------
mafVME *mafMSFImporter::RestoreVME(mafStorageElement *node, mafVME *parent)
//------------------------------------------------------------------------------
{
  mafVME *vme = NULL;
  // restore due attributes
  mafString vme_type;
  if (node->GetAttribute(_R("Type"),vme_type))
  {
    vme = CreateVMEInstance(vme_type);
    if (!vme)
      return NULL;

    mafString vme_name;

    if (node->GetAttribute(_R("Name"),vme_name))
    {
      vme->SetName(vme_name);
      // traverse children and restore TagArray, MatrixVector and VMEItems 
      mafStorageElement::ChildrenVector children;
      children = node->GetChildren();
      for (int i=0;i<children.size();i++)
      {
        // Restore a TagArray element
        //if (mafCString("TArray") == children[i]->GetName())
        if (children[i]->GetName() == _R("TArray"))
        {
          if (RestoreTagArray(children[i],vme->GetTagArray()) != MAF_OK)
          {
            mafErrorMacro("MSFImporter: error restoring Tag Array of node: \""<<vme->GetName().GetCStr() <<"\"");
            return NULL;
          }

          /////////////////////////////////////////// 
          // here should process VME-specific tags //
          ///////////////////////////////////////////
          mafTagArray *ta = vme->GetTagArray();
          if (ta->GetTag(_R("material")))
          {
            RestoreMaterial(vme);
          }
          if (ta->GetTag(_R("MAF_TOOL_VME")))
          {
            ta->DeleteTag(_R("MAF_TOOL_VME"));
          }
          if (vme_type == _R("mflVMEMeter"))
          {
            RestoreMeterAttribute(vme);
          }
          else if (vme_type == _R("mflVMELandmarkCloud") || vme_type == _R("mflVMERigidLandmarkCloud") || vme_type == _R("mflVMEDynamicLandmarkCloud"))
          {
            int num_lm = ((mafVMELandmarkCloud *)vme)->GetNumberOfLandmarks();
            double rad = ((mafVMELandmarkCloud *)vme)->GetRadius();
          }
        }
        // restore VME-Item element
        else if (children[i]->GetName() == _R("VItem"))
        {
          if (RestoreVItem(children[i],vme) != MAF_OK)
          {
            mafErrorMacro("MSFImporter: error restoring VME-Item of node: \""<<vme->GetName().GetCStr() <<"\"");
            return NULL;
          }
        }

        // restore MatrixVector element
        else if (children[i]->GetName()== _R("VMatrix"))
        {
          mafVMEGenericAbstract *vme_generic = mafVMEGenericAbstract::SafeDownCast(vme);
          if (vme_generic && RestoreVMatrix(children[i],vme_generic->GetMatrixVector()) != MAF_OK)
          {
            mafErrorMacro("MSFImporter: error restoring VME-Item of node: \""<<vme->GetName().GetCStr() <<"\"");
            return NULL;
          }
          if (mafVMEGroup::SafeDownCast(vme))
          {
            mafSmartPointer<mafMatrix> m;
            m->DeepCopy(mafVMEGroup::SafeDownCast(vme)->GetMatrixVector()->GetMatrix(0));
            mafVMEGroup::SafeDownCast(vme)->SetMatrix(*m);
          }
          
        }
        
        // restore children VMEs
        else if (children[i]->GetName() == _R("VME"))
        {
          mafVME *child_vme=RestoreVME(children[i],vme);
          if (child_vme==NULL)
          {
            mafErrorMacro("MSFImporter: error restoring child VME (parent=\""<<vme->GetName().GetCStr() <<"\")");
            continue;
          }

          // add the new VME as a child of the given parent node
          if (vme_type == _R("mflVMELandmarkCloud") || vme_type == _R("mflVMERigidLandmarkCloud") || vme_type == _R("mflVMEDynamicLandmarkCloud") && child_vme->IsMAFType(mafVMELandmark))
          {
            if(mafVMELandmark::SafeDownCast(child_vme))
            {
              ((mafVMELandmarkCloud *)vme)->SetLandmark((mafVMELandmark *)child_vme);
              child_vme->Delete();
              child_vme = NULL;
            }
            else
            {
              //mafErrorMacro("MSFImporter: error restoring child VME in landmark cloud:(parent=\""<<vme->GetName()<<"\")");
              //mafErrorMacro("MSFImporter: Child must be a landmark");
              vme->AddChild(child_vme);
            }
            
          }
          else if ((vme->IsMAFType(mafVMEMeter) || vme->IsMAFType(mafVMEProber)) && child_vme->GetTagArray()->GetTag(_R("mflVMELink")))
          {
            // this is a particular case for mafVMEMeter, in which the links are changed name
            if (child_vme->GetName() == _R("StartLink"))
            {
              child_vme->SetName(_R("StartVME"));
              if (vme->GetTagArray()->GetTag(_R("MFL_METER_START_VME_ID")))
              {
                child_vme->GetTagArray()->SetTag(mafTagItem(_R("SUBLINK_ID"),vme->GetTagArray()->GetTag(_R("MFL_METER_START_VME_ID"))->GetValue()));
                vme->GetTagArray()->DeleteTag(_R("MFL_METER_START_VME_ID"));
              }
            }
            else if (child_vme->GetName() == _R("EndLink1"))
            {
              child_vme->SetName(_R("EndVME1"));
              if (vme->GetTagArray()->GetTag(_R("MFL_METER_END_VME_1_ID")))
              {
                child_vme->GetTagArray()->SetTag(mafTagItem(_R("SUBLINK_ID"),vme->GetTagArray()->GetTag(_R("MFL_METER_END_VME_1_ID"))->GetValue()));
                vme->GetTagArray()->DeleteTag(_R("MFL_METER_END_VME_1_ID"));
              }
            }
            else if (child_vme->GetName() == _R("EndLink2"))
            {
              child_vme->SetName(_R("EndVME2"));
              if (vme->GetTagArray()->GetTag(_R("MFL_METER_END_VME_2_ID")))
              {
                child_vme->GetTagArray()->SetTag(mafTagItem(_R("SUBLINK_ID"),vme->GetTagArray()->GetTag(_R("MFL_METER_END_VME_2_ID"))->GetValue()));
                vme->GetTagArray()->DeleteTag(_R("MFL_METER_END_VME_2_ID"));
              }
            }
            else if (child_vme->GetName() == _R("SurfaceLink"))
            {
              child_vme->SetName(_R("Surface"));
            }
            else if (child_vme->GetName() == _R("VolumeLink"))
            {
              child_vme->SetName(_R("Volume"));
            }
            vme->AddChild(child_vme);
          }
          else
          {
            vme->AddChild(child_vme);
          }
        } // VMR restore
      }// Children
    } // Name
  } // Type
  return vme;
}

//------------------------------------------------------------------------------
mafVME *mafMSFImporter::CreateVMEInstance(const mafString &name)
//------------------------------------------------------------------------------
{
  if (
    name == _R("mafVMEGeneric")         ||
    name == _R("mflVMEAlias")
    )
  {
    return mafVMEGeneric::New();
  }
  else if (name == _R("mflVMELink"))
  {
    mafVME *link = mafVMEGeneric::New();
    link->GetTagArray()->SetTag(mafTagItem(_R("mflVMELink"),_R("1")));
    return link;
  }
  else if (name == _R("mflVMEExternalData"))
  {
    return mafVMEExternalData::New();
  }
  else if (name == _R("mflVMEGroup"))
  {
    return mafVMEGroup::New();
  }
  else if (name == _R("mflVMESurface"))
  {
    return mafVMESurface::New();
  }
  else if (name == _R("mflVMEGenericVolume"))
  {
    return mafVMEVolumeRGB::New();
  }
  else if (name == _R("mflVMEGrayVolume"))
  {
    return mafVMEVolumeGray::New();
  }
  else if (name == _R("mflVMELandmarkCloud") || name == _R("mflVMEDynamicLandmarkCloud")  || name == _R("mflVMERigidLandmarkCloud"))
  {
    return mafVMELandmarkCloud::New();
  }
  else if (name == _R("mflVMELandmark"))
  {
    return mafVMELandmark::New();
  }
  else if (name == _R("mflVMEImage"))
  {
    return mafVMEImage::New();
  }
  else if (name == _R("mflVMEMeter"))
  {
    return mafVMEMeter::New();
  }
  else if (name == _R("mflVMEPointSet"))
  {
    return mafVMEPointSet::New();
  }
  else if (name == _R("mflVMERefSys"))
  {
    return mafVMERefSys::New();
  }
  else if (name == _R("mflVMEMaps"))
  {
    return mafVMEProber::New();
  }
  else if (name == _R("mflVMESlicer"))
  {
    return mafVMESlicer::New();
  }
  else
  {
    mafErrorMacro("Unknown VME type: \""<<name.GetCStr()<<"\"");
    return NULL;
  }
}

//------------------------------------------------------------------------------
void mafMSFImporter::RestoreMeterAttribute(mafVME *vme)
//------------------------------------------------------------------------------
{
  mafVMEMeter *meter = mafVMEMeter::SafeDownCast(vme);
  if (meter)
  {
    mmaMeter *meter_attrib = meter->GetMeterAttributes();
    mafTagArray *meter_ta  = meter->GetTagArray();
    int num_tags = meter_ta->GetNumberOfTags();
    std::vector<mafString> tag_list;
    meter_ta->GetTagList(tag_list);
    mafTagItem *ti = NULL;
    double component;
    for (int t=0; t<num_tags; t++)
    {
      const mafString& tag_name = tag_list[t];
      if (tag_list[t].Equals(_R("MFL_METER_END_VME_1_ID")) || 
          tag_list[t].Equals(_R("MFL_METER_START_VME_ID")) ||
          tag_list[t].Equals(_R("MFL_METER_END_VME_2_ID")))
      {
        continue;
      }
      ti = meter_ta->GetTag(tag_name);
      component = ti->GetComponentAsDouble(0);
      if (tag_name.Equals(_R("MFL_METER_TYPE")))
      {
        meter_attrib->m_MeterMode = (int)component;
      }
      else if (tag_name.Equals(_R("MFL_METER_COLOR_MODE")))
      {
        meter_attrib->m_ColorMode = (int)component;
      }
      else if (tag_name.Equals(_R("MFL_METER_MEASURE_TYPE")))
      {
        meter_attrib->m_MeasureType = (int)component;
      }
      else if (tag_name.Equals(_R("MFL_METER_REPRESENTATION")))
      {
        meter_attrib->m_Representation = (int)component;
      }
      else if (tag_name.Equals(_R("MFL_METER_TUBE_CAPPING")))
      {
        meter_attrib->m_Capping = (int)component;
      }
      else if (tag_name.Equals(_R("MFL_METER_EVENT_THRESHOLD")))
      {
        meter_attrib->m_GenerateEvent = (int)component;
      }
      else if (tag_name.Equals(_R("MFL_METER_INIT_MEASURE")))
      {
        meter_attrib->m_InitMeasure = component;
      }
      else if (tag_name.Equals(_R("MFL_METER_DELTA_PERCENT")))
      {
        meter_attrib->m_DeltaPercent = (int)component;
      }
      else if (tag_name.Equals(_R("MFL_METER_LABEL_VISIBILITY")))
      {
        meter_attrib->m_LabelVisibility = (int)component;
      }
      else if (tag_name.Equals(_R("MFL_METER_RADIUS")))
      {
        meter_attrib->m_TubeRadius = component;
      }
      else if (tag_name.Equals(_R("MFL_METER_DISTANCE_RANGE")))
      {
        meter_attrib->m_DistanceRange[0] = component;
        meter_attrib->m_DistanceRange[1] = ti->GetComponentAsDouble(1);
      }
      else
        continue;
      meter_ta->DeleteTag(tag_name);
    }
  }
}

//------------------------------------------------------------------------------
void mafMSFImporter::RestoreMaterial(mafVME *vme)
//------------------------------------------------------------------------------
{
  mmaMaterial *material = (mmaMaterial *)vme->GetAttribute(_R("MaterialAttributes"));
  if (material == NULL)
  {
    material = mmaMaterial::New();
    vme->SetAttribute(_R("MaterialAttributes"), material);
  }

  mafTagItem *mat_item = vme->GetTagArray()->GetTag(_R("material"));
  material->m_MaterialName =  mat_item->GetComponent(MAT_NAME);
  material->m_Ambient[0] = mat_item->GetComponentAsDouble(MAT_AMBIENT_R);
  material->m_Ambient[1] = mat_item->GetComponentAsDouble(MAT_AMBIENT_G);
  material->m_Ambient[2] = mat_item->GetComponentAsDouble(MAT_AMBIENT_B);
  material->m_AmbientIntensity = mat_item->GetComponentAsDouble(MAT_AMBIENT_INTENSITY);
  material->m_Diffuse[0] = mat_item->GetComponentAsDouble(MAT_DIFFUSE_R);
  material->m_Diffuse[1] = mat_item->GetComponentAsDouble(MAT_DIFFUSE_G);
  material->m_Diffuse[2] = mat_item->GetComponentAsDouble(MAT_DIFFUSE_B);
  material->m_DiffuseIntensity = mat_item->GetComponentAsDouble(MAT_DIFFUSE_INTENSITY);
  material->m_Specular[0] = mat_item->GetComponentAsDouble(MAT_SPECULAR_R);
  material->m_Specular[1] = mat_item->GetComponentAsDouble(MAT_SPECULAR_G);
  material->m_Specular[2] = mat_item->GetComponentAsDouble(MAT_SPECULAR_B);
  material->m_SpecularIntensity = mat_item->GetComponentAsDouble(MAT_SPECULAR_INTENSITY);
  material->m_SpecularPower = mat_item->GetComponentAsDouble(MAT_SPECULAR_POWER);
  material->m_Opacity = mat_item->GetComponentAsDouble(MAT_OPACITY);
  material->m_Representation = (int)mat_item->GetComponentAsDouble(MAT_REPRESENTATION);
  material->UpdateProp();
  vme->GetTagArray()->DeleteTag(_R("material"));
}

//------------------------------------------------------------------------------
int mafMSFImporter::RestoreVItem(mafStorageElement *node, mafVME *vme)
//------------------------------------------------------------------------------
{
  mafTimeStamp item_time;
  if (node->GetAttributeAsDouble(_R("TimeStamp"),item_time))
  {
    mafString data_type;
    if (node->GetAttribute(_R("DataType"),data_type))
    {
      mafID item_id;
      if (node->GetAttributeAsInteger(_R("Id"),item_id))
      {
        mafString data_file;
        if (node->GetAttribute(_R("DataFile"),data_file))
        {
          mafSmartPointer<mafVMEItemVTK> vitem;
          mafStorageElement *tarray=node->FindNestedElement(_R("TArray"));
          mafVMEGeneric *vme_generic=mafVMEGeneric::SafeDownCast(vme);
          assert(vme_generic);
          if (tarray)
          {
            if (RestoreTagArray(tarray,vitem->GetTagArray())==MAF_OK)
            {
              /////////////////////////////////////////////// 
              // here should process VMEItem-specific tags //
              ///////////////////////////////////////////////
              mafTagArray *ta = vitem->GetTagArray();
              if (ta->GetTag(_R("MFL_CRYPTING")))
              {
                mafTagItem *ti = ta->GetTag(_R("MFL_CRYPTING"));
                vme->SetCrypting((int)ti->GetValueAsDouble());
                vitem->SetCrypting((int)ti->GetValueAsDouble() != 0);
                ta->DeleteTag(_R("MFL_CRYPTING"));
              }
              if (ta->GetTag(_R("VTK_DATASET_BOUNDS")))
              {
                double *b = vitem->GetBounds();
                mafTagItem *ti = ta->GetTag(_R("VTK_DATASET_BOUNDS"));
                for (int c=0;c<6;c++)
                {
                  b[c] = ti->GetComponentAsDouble(c);
                }
                ta->DeleteTag(_R("VTK_DATASET_BOUNDS"));
              }
            }
          } // tarray
          vitem->SetTimeStamp(item_time);
          vitem->SetId(item_id);
          vitem->SetURL(data_file.GetCStr());
          vitem->SetDataType(data_type);
          vme_generic->GetDataVector()->AppendItem(vitem);
          vme_generic->GetDataVector()->SetCrypting(vitem->GetCrypting());
          return MAF_OK;
        } // DataFile
      } // Id
    } // DataType
  } // TimeStamp
  return MAF_ERROR;
}

//------------------------------------------------------------------------------
int mafMSFImporter::RestoreVMatrix(mafStorageElement *node, mafMatrixVector *vmatrix)
//------------------------------------------------------------------------------
{
  // restore single matrices
  mafStorageElement::ChildrenVector children;
  children = node->GetChildren();

  vmatrix->RemoveAllItems();

  for (int i = 0;i<children.size();i++)
  {
    assert(children[i]->GetName() == _R("Matrix"));

    mafSmartPointer<mafMatrix> matrix;
    int restored_matrix = children[i]->RestoreMatrix(matrix);
    if (restored_matrix != MAF_ERROR)
    {
      vmatrix->AppendKeyMatrix(matrix);
    }
    else
    {
      return MAF_ERROR;
    }
  }
  return MAF_OK;
}

//------------------------------------------------------------------------------
int mafMSFImporter::RestoreTagArray(mafStorageElement *node, mafTagArray *tarray)
//------------------------------------------------------------------------------
{
  mafStorageElement::ChildrenVector children;
  children = node->GetChildren();

  for (int i = 0;i<children.size();i++)
  {
    if (children[i]->GetName()== _R("TItem"))
    {
      mafID num_of_comps;
      if (children[i]->GetAttributeAsInteger(_R("Mult"),num_of_comps))
      {
        mafString tag_name;
        if (children[i]->GetAttribute(_R("Tag"),tag_name))
        {
          mafString tag_type;
          if (children[i]->GetAttribute(_R("Type"),tag_type))
          {
            mafTagItem titem;
            titem.SetNumberOfComponents(num_of_comps);
            titem.SetName(tag_name);
    
            if (tag_type==_R("NUM"))
            {
              titem.SetType(mafTagItem::MAF_NUMERIC_TAG);
            }
            else if (tag_type==_R("STR"))
            {
              titem.SetType(mafTagItem::MAF_STRING_TAG);
            }
            else if (tag_type==_R("MIS"))
            {
              titem.SetType(mafTagItem::MAF_MISSING_TAG);
            }
            else
            {
              titem.SetType(atof(tag_type.GetCStr()));
            }

            mafStorageElement::ChildrenVector tag_comps;
            tag_comps = children[i]->GetChildren();
            int idx=0;
            for (int n = 0;n<tag_comps.size();n++)
            {
              if (tag_comps[n]->GetName() == _R("TC"))
              {
                mafString tc;
                tag_comps[n]->RestoreText(tc);
                titem.SetComponent(tc,idx);
                idx++;
              }
              else
              {
                mafErrorMacro("Error parning a TItem element inside a TagArray: expected <TC> sub element, found <"<<tag_comps[n]->GetName().GetCStr() <<">");
              } 
            } 
            tarray->SetTag(titem);
          } // Type
        } // Tag
      } // Mult
      //mafErrorMacro("Error parning a TItem element inside a TagArray: missing required Attribute");
    } // TItem
  }
  return MAF_OK;
}
