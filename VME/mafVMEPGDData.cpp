/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEPGDData.cpp,v $
  Language:  C++
  Date:      $Date: 2007-12-01 11:53:22 $
  Version:   $Revision: 1.2 $
  Authors:   Stefano Perticoni - porting Fedor Moiseev
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
  ULB    - University Libre de Bruxelles
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafVMEPGDData.h"


#include "mafVMELandmarkCloud.h"  
#include "mafVMELandmark.h"  
#include "mafVMEItem.h"
#include "mafTagArray.h"
#include "mafPlotMath.h"
#include "mafMatrixVector.h"

#include "vtkObjectFactory.h"
#include "vtkCommand.h"
#include "vtkMath.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"

#include "itkRawMotionImporterUtility.h"

#include <vcl_fstream.h>
#include <vcl_string.h>
#include <vnl\vnl_matrix.h>
#include <vnl\vnl_vector.h>

#include <iostream>
#include <string>


//----------------------------------------------------------------------------
// constant
//----------------------------------------------------------------------------
//If there is no dictionary or if the dictionary
//does not define a NOT_USED value I use
//the value -9999.00 to identify landmarks that
//are not visible at a given timestamp.
const double CONST_NOT_USED_IOR_MAL = -9999.00;


//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEPGDData)
//-------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafVMEPGDData::mafVMEPGDData()
//----------------------------------------------------------------------------
{
  m_DefaultRadius = 15;
  mafNEW(m_PGD_DLCloud);
  m_PGD_DLCloud->SetRadius(m_DefaultRadius);
  m_PGD_DLCloud->SetDefaultVisibility(0);  //modified by Marco. 3-10-2003

  m_Dictionary = 0;
}
//----------------------------------------------------------------------------
mafVMEPGDData::~mafVMEPGDData()
//----------------------------------------------------------------------------
{
  mafDEL(m_PGD_DLCloud);
}

//----------------------------------------------------------------------------
void mafVMEPGDData::SetDictionaryFileName(const char *name)
//----------------------------------------------------------------------------
{
  m_DictionaryFileName = name;
  DictionaryOn();
  Modified();
}

//----------------------------------------------------------------------------
void mafVMEPGDData::SetFileName(const char *name)
//----------------------------------------------------------------------------
{
  m_FileName = name;
  Modified();
}

//----------------------------------------------------------------------------
int mafVMEPGDData::Read()
//-----------------------------------------------------------------------
{
  m_PGDLMNamesTagArray = new std::vector<mafString>;
  m_pPGDFile = fopen(m_FileName, "rb");

  mafTagItem  TmpTagItem;
  mafTagArray *PGDDictionaryTagArray = mafTagArray::New();
  std::ifstream dict_iff;

  dict_iff.open(m_DictionaryFileName.GetCStr());
  if (dict_iff.is_open() == 0 || dict_iff.eof())
    return -1;

  char buf[128];
  char tmpString1[128],tmpString2[128];

  do
  {
    dict_iff.getline(buf,128,'\n');

    sscanf(buf,"%s %s", tmpString1, tmpString2);
    TmpTagItem.SetName(tmpString1);
    TmpTagItem.SetValue(tmpString2, 0);
    PGDDictionaryTagArray->SetTag(TmpTagItem);
  }
  while (!dict_iff.eof());
  dict_iff.close();


  //Parse DOF Section

  char LandmarkName[128];
  char tmpstring[128];
  int tmp1,tmp2;
  float X, Y, Z;
  float R_x, R_y, R_z;
  int nof; //nof:number of frames;
  float si,tos; // si: sample interval; tos: time offset;
  mafVME *TmpVMEFound = NULL;


  std::ifstream pgd_iff;
  pgd_iff.open( m_FileName.GetCStr());
  if( pgd_iff.is_open() == 0)
  {
    //vtkErrorMacro("Cannot Open file:"<<m_FileName.GetCStr());
    return -1;
  }

  double length_coef = 1.0;
  double angle_coef  = 1.0;
  //find Length measure units
  do
  {
    pgd_iff.getline(buf,128,'\n');
  }
  while( !strstr(buf,"$KIUL") );
  pgd_iff.getline(buf,128,'\n');
  if(toupper(buf[0]) == 'M' && toupper(buf[1]) == 'M')
    length_coef = 1.0;
  else if(toupper(buf[0]) == 'C' && toupper(buf[1]) == 'M')
    length_coef = 10.0;
  else //if(toupper(buf[0]) == 'M' && toupper(buf[1]) == '\0')
    length_coef = 1000.0;

  //find Angle measure units
  do
  {
    pgd_iff.getline(buf,128,'\n');
  }
  while( !strstr(buf,"$KIUA") );
  pgd_iff.getline(buf,128,'\n');
  if(toupper(buf[0]) == 'D' && toupper(buf[1]) == 'E'  && toupper(buf[2]) == 'G')
    angle_coef = 1.0;
  else //if(toupper(buf[0]) == 'R' && toupper(buf[1]) == 'A'  && toupper(buf[1]) == 'D')
    angle_coef = 180.0 / vtkMath::Pi();



  //find CAL section
  do
  {
    pgd_iff.getline(buf,128,'\n');
  }
  while( !strstr(buf,"!CAL") );


  //parse CAL section
  do
  {	
    if( strstr(buf,"!CAL"))
    {
      sscanf(buf,"!CAL%4s", LandmarkName);

      pgd_iff.getline(buf,128,'\n');
      sscanf(buf,"%f %f %f",&X,&Y,&Z);

      X *= length_coef;
      Y *= length_coef;
      Z *= length_coef;

      //Handle difference between 2 char and 3 char 
      //landmark names
      if	(LandmarkName[3] == '-')
      {
        LandmarkName[3] = '\0';
      }

      //Find segment name from landmark name  
      mafTagItem *tag = PGDDictionaryTagArray->GetTag(LandmarkName);
      if(tag != NULL)
      {
        const char *SegmentName = tag->GetValue();

        sprintf(tmpstring, "%s", SegmentName);

        //Search for segment [Side|SegmentName] in VME
        if	(NULL == (TmpVMEFound =  mafVME::SafeDownCast(FindInTreeByName(tmpstring))))
        {	
          //if the segment does not exist then create its VME	

          mafVMELandmarkCloud * TmpVME = mafVMELandmarkCloud::New();

          //modified by Vladik: 8-03-2005
          //TmpVME->Open();
          TmpVME->SetName(tmpstring);
          TmpVME->SetRadius(m_DefaultRadius);
          //mafEventMacro(mafEvent(this,VME_CREATE_CLIENT_DATA,m_cloud));
          AddChild(TmpVME);		

          // reimplemented by MARCO
          TmpVME->AppendLandmark(X,Y,Z,LandmarkName);

          //modified by Vladik: 8-03-2005
          /*mafVMELandmark *Landmark = mafVMELandmark::New();
          Landmark->SetName(LandmarkName);
          Landmark->ReparentTo(TmpVME);
          //Landmark->UpdateCurrentData();
          Landmark->SetAbsPose(X,Y,Z,0,0,0);*/


          //TmpVME->Delete();
        }
        else 
          //If the segment already exists append points
        {
          if (mafVMELandmarkCloud * TmpVME = mafVMELandmarkCloud::SafeDownCast(TmpVMEFound))
          {
            //modified by Vladik: 8-03-2005
            TmpVME->AppendLandmark(X,Y,Z,LandmarkName);
            /*mafVMELandmark *Landmark = mafVMELandmark::New();//we have a reference on the vme (we can call vtkDEL in the UNDO)
            Landmark->SetName(LandmarkName);
            Landmark->ReparentTo(TmpVME);
            //Landmark->UpdateCurrentData();
            Landmark->SetAbsPose(X,Y,Z,0,0,0);*/
            //mafEventMacro(mafEvent(this,VME_ADD,m_landmark));
          }
          else
          {
            //vtkErrorMacro("DEBUG: found a NULL or Wrong type VME in the tree...");
          }
        }	
      }
    }
    pgd_iff.getline(buf,128,'\n');
  }
  //stop when we arrive at DOF section
  while (!strstr(buf,"!DOF") && !strstr(buf,"!URP"));

  //Parse DOF or URP section
  do
  {
    char TmpSegmentName[128];
    char CurrentSegmentName[128];
    if(strstr(buf,"!DOF"))
      sscanf(buf,"!DOF%s %i %i;%i;%f;%f",CurrentSegmentName,&tmp1,&tmp2,&nof,&si,&tos);
    else
      sscanf(buf,"!URP%s %i %i;%i;%f;%f",CurrentSegmentName,&tmp1,&tmp2,&nof,&si,&tos);

    ///// Workaround to handle IOR PGD /////
    if (strlen(CurrentSegmentName) != 3)			
    {
      strncpy(TmpSegmentName, CurrentSegmentName, 3);
      TmpSegmentName[3] = '\0';
      sscanf(buf, "%*c%*c%*c%*c%*c%*c%*c%*c%*c%*c%*c%*c%i", &nof);
      strcpy(CurrentSegmentName, TmpSegmentName);
    }
    ///// End workaround /////

    //find the segment in VMETree
    TmpVMEFound =  mafVME::SafeDownCast(FindInTreeByName(CurrentSegmentName));

    // check if TmpVMEFound is NULL, and in that case create a new one
    if (TmpVMEFound == NULL) 
    {

      mafVMELandmarkCloud *TmpVME = mafVMELandmarkCloud::New();
      TmpVME->SetName(CurrentSegmentName);

      mafTagItem *v_ti = new mafTagItem("visible", 0.0);
      TmpVME->GetTagArray()->SetTag(*v_ti);
      TmpVMEFound=TmpVME;       
      AddChild(TmpVME);

      TmpVMEFound=TmpVME;

      TmpVME->Delete();
      delete v_ti;
    }
    vtkTransform * tmpTransform = vtkTransform::New();
    //mflMatrix *tmpMatrix = mflMatrix::New();
    vtkMatrix4x4 *tmpMatrix = vtkMatrix4x4::New();
    vtkMatrix4x4 *tmp_matrix = vtkMatrix4x4::New();
    int counter = 0;
    pgd_iff.getline(buf,128,'\n');

    do
    {	
      if(sscanf(buf,"%f %f %f %f %f %f",&R_x,&R_y,&R_z,&X,&Y,&Z) == 6)
      {
        R_x *= angle_coef;
        R_y *= angle_coef;
        R_z *= angle_coef;

        X *= length_coef;
        Y *= length_coef;
        Z *= length_coef;


        double AttVec[3] = {R_x, R_y, R_z};				

        tmp_matrix->Identity();
        //this->AttVec2Matrix(AttVec, tmp_matrix);	
        mafAttVecToVTKMat(AttVec, tmp_matrix);

        tmpTransform->PostMultiply();
        tmpTransform->SetMatrix(tmp_matrix);	
        tmpTransform->Translate(X, Y, Z);

        //create matrix with timestamp tos + si*i
        tmpMatrix->DeepCopy(tmpTransform->GetMatrix());

        // modified by Marco. 24-10-2003
        // Il time stamp cosü calcolato ø sempre 0!!! passando una mflMatrix a AppendKeyMatrix il
        // suo time stamp ø conservato, anche per questo i PGD non si muovevano!!!
        // Passandolgi una matrice vtkMatrix4x4 il time stamp ø calcolato automaticamente in modo 
        // progresivo, per cui il PGD si muove ma il tempo non ø quello estratto dal file PGD
        //tmpMatrix->SetTimeStamp(tos + counter*si);
        mafMatrix ttmp;
        ttmp.SetVTKMatrix(tmpMatrix);
        ttmp.SetTimeStamp(mafTimeStamp(counter));
        mafVMEGenericAbstract::SafeDownCast(TmpVMEFound)->GetMatrixVector()->AppendKeyMatrix(ttmp);
      }

      pgd_iff.getline(buf,128,'\n');
      counter++;
    }
    while (	!strstr(buf,"!DOF") && !strstr(buf,"!URP") && pgd_iff.eof() == 0);	
    tmpTransform->Delete();
    tmpTransform = NULL;
    tmpMatrix->Delete();
    tmpMatrix = NULL;
    tmp_matrix->Delete();
    tmp_matrix = NULL;
  }
  while (!pgd_iff.eof());
  pgd_iff.close();

  //cleaning
  PGDDictionaryTagArray->Delete();
  TmpVMEFound = NULL;


  delete m_PGDLMNamesTagArray;
  m_PGDLMNamesTagArray = NULL;
  fclose(m_pPGDFile);
  return 0;				
}

//----------------------------------------------------------------------------
void mafVMEPGDData::AttVec2RPY(double ThetaIn[3], double ThetaOut[3])
//----------------------------------------------------------------------------
{
  const double pi = vtkMath::Pi();
  const double eps = 2.2e-16;

  vnl_matrix<double> th(3, 1);

  th(0, 0) = ThetaIn[0]*(pi/180);
  th(1, 0) = ThetaIn[1]*(pi/180);
  th(2, 0) = ThetaIn[2]*(pi/180);

  vnl_matrix<double> temp = th.transpose() * th;

  temp.assert_size(1,1);
  double fi = sqrt(temp(0,0));

  double sinc = sin(fi)/fi;
  double cosc = (1 - cos(fi))/(fi * fi);

  vnl_matrix<double> A(3, 3);
  A(0,0) = 0;			A(0,1) = -th(2,0);	A(0,2) = th(1,0);
  A(1,0) = th(2,0);	A(1,1) = 0;		    A(1,2) = -th(0,0);
  A(2,0) = -th(1,0);	A(2,1) = th(0,0);	A(2,2) = 0;

  vnl_matrix<double> eye(3, 3);
  eye.set_identity();

  vnl_matrix<double> R(3, 3);
  R = cos(fi) * eye + sinc * A +
    cosc * (th * th.transpose());

  vnl_vector<double> rpy(3);

  if (fabs(R(0,0)) < eps && fabs (R(1,0)) < eps)
  {
    rpy(0) = 0;
    rpy(1) = pi / 2;
    rpy(2) = atan2(R(0,1), R(1,1)); 	
  }
  else
  {
    rpy(0) = atan2(R(1,0), R(0,0));
    rpy(1) = atan2 (- R(2,0) , sqrt( pow(R(0,0), 2) + pow (R(1,0), 2)));
    rpy(2) = atan2(R(2,1), R(2,2));
  }

  ThetaOut[0] = rpy(2) * (180 / pi);
  ThetaOut[1] = rpy(1) * (180 / pi);
  ThetaOut[2] = rpy(0) * (180 / pi);	
}

//----------------------------------------------------------------------------
void mafVMEPGDData::AttVec2Matrix(double ThetaIn[3], vtkMatrix4x4 *pMat)
//----------------------------------------------------------------------------
{
  const double pi = vtkMath::Pi();
  const double eps = 2.2e-16;

  vnl_matrix<double> th(3, 1);

  th(0, 0) = ThetaIn[0]*(pi/180);
  th(1, 0) = ThetaIn[1]*(pi/180);
  th(2, 0) = ThetaIn[2]*(pi/180);

  vnl_matrix<double> temp = th.transpose() * th;

  temp.assert_size(1,1);
  double fi = sqrt(temp(0,0));

  double sinc = sin(fi)/fi;
  double cosc = (1 - cos(fi))/(fi * fi);

  vnl_matrix<double> A(3, 3);
  A(0,0) = 0;			A(0,1) = -th(2,0);	A(0,2) = th(1,0);
  A(1,0) = th(2,0);	A(1,1) = 0;		    A(1,2) = -th(0,0);
  A(2,0) = -th(1,0);	A(2,1) = th(0,0);	A(2,2) = 0;

  vnl_matrix<double> eye(3, 3);
  eye.set_identity();

  vnl_matrix<double> R(3, 3);
  R = cos(fi) * eye + sinc * A +
    cosc * (th * th.transpose());

  //copy R into a pMat vtkMatrix
  for (int i = 0; i < 3; i++ )
  {		
    for(int j = 0; j < 3; j++)
    {		
      pMat->SetElement(i, j, R(i,j));
    }
  }
}


//----------------------------------------------------------------------------
char* mafVMEPGDData::strrtrim( char* s)
//----------------------------------------------------------------------------
{
  int i;

  if (s) {
    i = strlen(s); while ((--i)>0 && isspace(s[i]) ) s[i]=0;
  }
  return s;
}
