/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEPGDData.h,v $
  Language:  C++
  Date:      $Date: 2007-08-22 14:01:40 $
  Version:   $Revision: 1.1 $
  Authors:   Stefano Perticoni - porting Fedor Moiseev
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
  ULB    - University Libre de Bruxelles
=========================================================================*/
#ifndef __mafVMEPGDData_h
#define __mafVMEPGDData_h

#include "lhpDefines.h"

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVMEGroup.h"
#include <fstream>

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafVMELandmarkCloud;

/** mafVMEPGDData - 
*/
class  mafVMEPGDData : public mafVMEGroup  
{
public:
  mafTypeMacro(mafVMEPGDData,mafVMEGroup);
  
  /** Set RAW motion data  Dictionary file name*/
  void SetDictionaryFileName(const char *name);
  
  /** Get RAW motion data  Dictionary file name*/
  const char *GetDictionaryName() {return this->m_DictionaryFileName.GetCStr();}

  /** Set RAW motion data file name*/
  void mafVMEPGDData::SetFileName(const char *name);
  
  /** Get RAW motion data file name*/
  const char *GetFileName() {return this->m_FileName.GetCStr(); }	
  
  /** Parse C3D motion data file and fill VME Tree*/
  int Read();

	/**
	Use DictionaryOn() to use a dictionary otherwise
	only one segment will be created and all the landmarks 
	will be appended to it.*/
	void SetDictionary(int Dict){m_Dictionary = Dict;}
	int GetDictionary(){return m_Dictionary;}

	void DictionaryOn () { this->SetDictionary((int)1);}
	void DictionaryOff () { this->SetDictionary((int)0);}

protected:
  mafVMEPGDData();
  virtual ~mafVMEPGDData();

  /**
  //Attitude vector to roll pitch conversion; input and
  //output in degrees.		*/
  void AttVec2RPY(double RxRyRzIn[3], double RxRyRzOut[3]);

	/**
  //Attitude vector to rotation matrix; input in degrees;
	//only the rotation 3x3 submatrix is written.*/
	void AttVec2Matrix(double RxRyRzIn[3], vtkMatrix4x4 *pMatrix);



  mafString              m_DictionaryFileName; 
  mafString              m_FileName; 
  int                    m_Dictionary;
  mafVMELandmarkCloud    *m_CurrentDlc;
	mafVMELandmarkCloud    *m_Dlc;

  mafVMELandmarkCloud    *m_PGD_DLCloud;
  std::vector<mafString> *m_PGDLMNamesTagArray;
  FILE                   * m_pPGDFile;


  void Read_C3D_Header(
    unsigned short int	*num_markers, 
    unsigned short int	*num_channels,
    unsigned short int	*first_field,
    unsigned short int	*last_field,
    float				*scale_factor,
    unsigned short int	*start_record_num,
    unsigned short int	*frames_per_field,
    float				*video_rate,
    FILE				*infile);


  int Read_C3D_Data(
    unsigned short	num_markers,				// number of marker trajectories
    unsigned short	num_analog_channels,		// number of analog channels
    unsigned short	first_field,				// first frame to read
    unsigned short	last_field,					// last frame to read
    unsigned short	start_byte,					// starting record number
    unsigned short	analog_frames_per_field,    // analog samples/ video frame
    float scale_factor,
    std::vector<mafString> *lm_names_tagarray,
    FILE			*infile);

  void Read_C3D_Parameters(std::vector<mafString> *mlabels,   FILE	*infile);

private:
  mafVMEPGDData(const mafVMEPGDData&);  // Not implemented.
  void operator=(const mafVMEPGDData&);  // Not implemented.

  /**
  //Removes trailing spaces from the end of a string*/
  char* strrtrim( char* s);
  //mafVMERoot *root;
  int Dictionary;
  //wxString DictionaryFileName;
};
#endif
