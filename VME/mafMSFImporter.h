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
#ifndef __mafMSFImporter_h__
#define __mafMSFImporter_h__

#include "mafStorage.h"
#include "mafParser.h"
#include "mafUtility.h"
#include "mafNodeManager.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafVMERoot;
class mafVME;
class mafNode;
class mafTagArray;
class mafMatrixVector;

/** utility class representing the MSF document.
 This class is used by mafMSFImporter to restore MSF 1.x files.*/
class MAF_EXPORT mafMSFImporter : public mafUtility, public mafNodeManager
{
public:
  mafMSFImporter(){}
  virtual ~mafMSFImporter() {}
  virtual int InternalStore(mafStorageElement *node);
  virtual int InternalRestore(mafStorageElement *node);
  
  enum MATERIAL_TAG_ID
  {
    MAT_NAME = 0,
    MAT_AMBIENT_R,
    MAT_AMBIENT_G,
    MAT_AMBIENT_B,
    MAT_AMBIENT_INTENSITY,
    MAT_DIFFUSE_R,
    MAT_DIFFUSE_G,
    MAT_DIFFUSE_B,
    MAT_DIFFUSE_INTENSITY,
    MAT_SPECULAR_R,
    MAT_SPECULAR_G,
    MAT_SPECULAR_B,
    MAT_SPECULAR_INTENSITY,
    MAT_SPECULAR_POWER,
    MAT_OPACITY,
    MAT_REPRESENTATION,
    MAT_NUM_COMP
  };

protected:
  mafVME *CreateVMEInstance(const mafString &name);
  mafVME *RestoreVME(mafStorageElement *node, mafVME *parent);
  int RestoreTagArray(mafStorageElement *node, mafTagArray *tarray);
  int RestoreVItem(mafStorageElement *node, mafVME *vme);
  int RestoreVMatrix(mafStorageElement *node, mafMatrixVector *vmatrix);

  /** 
  Parse the path string to rebuild links*/
  mafNode *ParsePath(mafVMERoot *root,const char *path);

  /** Restore material attributes */
  void RestoreMaterial(mafVME *vme);

  /** Restore meter attributes */
  void RestoreMeterAttribute(mafVME *vme);
};
#endif // _mafMSFImporter_h_
