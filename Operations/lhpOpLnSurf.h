/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpLnSurf.h,v $
  Language:  C++
  Date:      $Date: 2008-02-19 11:22:33 $
  Version:   $Revision: 1.3 $
  Authors:   Fedor Moiseev / Vladik Aranov
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#ifndef __lhpOpLnSurf_H__
#define __lhpOpLnSurf_H__

//----------------------------------------------------------------------------
// Includes:
//----------------------------------------------------------------------------
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMESurface;
class mafVMEPolyline;

//----------------------------------------------------------------------------
// lhpOpRefSys :
//----------------------------------------------------------------------------
/** */
class lhpOpLnSurf: public mafOp
{

public:
  mafTypeMacro(lhpOpLnSurf, mafOp)
  lhpOpLnSurf(const mafString& label= _R(""));
 ~lhpOpLnSurf(); 

  virtual void OnEvent(mafEventBase *maf_event);
  mafOp* Copy();

  bool Accept(mafNode* vme);   
  void OpRun();
  void OpDo();
  void OpUndo();

protected: 
  enum 
  {
    ID_RHO_SPL = MINID,
    ID_RHO_SRF,
    ID_SGM_SRF,
    ID_DIM_SPL,
    ID_DIMX_SRF,
    ID_DIMY_SRF,
    ID_GEN_LIST,
    ID_PARSE_NAME,
    ID_LAST
  };

private:
  double          m_rhoLine;
  double          m_rhoSurf;
  double          m_sgmSurf;
  int             m_splDim;
  int             m_xDim;
  int             m_yDim;
  int             m_generateLinesSurfaces;
  int             m_parseNames;
  mafVMESurface   *m_Surface;
  mafVMEPolyline  *m_Muscles;
  mafVMEPolyline  *m_Tendons;
};
#endif
