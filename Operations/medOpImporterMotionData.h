#pragma once

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafDefines.h" 
#include "mafOp.h"

#include <wx/busyinfo.h>

#include "mafDecl.h"
#include "mafEvent.h"
#include "mafVME.h"
#include "mafTagArray.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;                      
class mafEvent;
//----------------------------------------------------------------------------
// medOpImporterMotionData :
//----------------------------------------------------------------------------
/** */
template <class MotionReader>
class medOpImporterMotionData : public mafOp
{
public:
  medOpImporterMotionData(const mafString& label, const mafString& pgdWildc, const mafString& dicWildc) :  mafOp(label), m_PgdWildc(pgdWildc), m_DicWildc(dicWildc)
        //----------------------------------------------------------------------------
  {
    m_OpType	= OPTYPE_IMPORTER;
    m_Canundo	= true;
    m_File		= _R("");
    m_Dict		= _R("");
    m_FileDir = mafGetApplicationDirectory() + _R("/Data/External/");
    m_DictDir = mafGetApplicationDirectory() + _R("/Config/Dictionary/");
  }
      //----------------------------------------------------------------------------
  ~medOpImporterMotionData( ) override
  //----------------------------------------------------------------------------
  {
    // Must unregister in order to avoid leaks or data loss
    if (m_Output)
    {
      m_Vme->UnRegister(m_Output);
      m_Output = NULL;
    }
    m_Vme = NULL;    
  }
  //----------------------------------------------------------------------------
  mafOp* Copy() override
  /** restituisce una copia di se stesso, serve per metterlo nell'undo stack */
    //----------------------------------------------------------------------------
  {
    //non devo incrementare l'id counter --- vfc le operazioni sono gia inserite nei menu;
    medOpImporterMotionData *cp = new medOpImporterMotionData(GetLabel(), m_PgdWildc, m_DicWildc);
    cp->m_Canundo = m_Canundo;
    cp->m_OpType = m_OpType;
    cp->SetListener(GetListener());
    cp->m_Next = NULL;

    cp->m_File = m_File;
    cp->m_Dict = m_Dict;
    cp->m_Vme = m_Vme;
    return cp;
  }

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode* node) override {return true;};

  //----------------------------------------------------------------------------
  /** Builds operation's interface. */
  void OpRun() override
  //----------------------------------------------------------------------------
  {
    int result = OP_RUN_CANCEL;
    m_File = _R("");
    m_Dict = _R("");

    mafString f = mafGetOpenFile(m_FileDir,m_PgdWildc); 
    if(!f.empty())
    {
      m_File = f;
      f = mafGetOpenFile(m_DictDir,m_DicWildc,_R("Open Dictionary")); 
      if(!f.empty())
      {
        m_Dict = f;
        SetDictionaryFlagOn();
        result = OP_RUN_OK;
      }
      else
      {
        SetDictionaryFlagOff();
        result = OP_RUN_OK;
      }
    }
    if (result == OP_RUN_OK)
    {
	    Import();
    }
    OpStop(result);
  }
  //----------------------------------------------------------------------------
  /** Execute the operation. */
  void Import()
  //----------------------------------------------------------------------------
  {
    if (!m_TestMode)
    {
      wxBusyInfo wait("Please wait, working...");
    }

    MotionReader* reader = nullptr;
    mafNEW(reader);
    reader->SetFileName(m_File.GetCStr());
    reader->SetDictionaryFileName(m_Dict.GetCStr());

    if (GetDictionaryFlag()==1)
      reader->DictionaryOn();
    else
      reader->DictionaryOff();

    reader->Read();

    m_Vme = reader;

    mafString path, name, ext;
    mafSplitPath(m_File,&path,&name,&ext);
    m_Vme->SetName(name);

    mafTagItem tag_Nature;
    tag_Nature.SetName(_R("VME_NATURE"));
    tag_Nature.SetValue(_R("NATURAL"));

    m_Vme->GetTagArray()->SetTag(tag_Nature); 

    // Must register in order to preserve output for do/undo operation (since it is a smart pointer)
    m_Output = m_Vme;
  }
  //----------------------------------------------------------------------------
  /** Set file name. */
  void SetFileName(const mafString& filename)   
    //----------------------------------------------------------------------------
  {
    m_File = filename;
  }
  //----------------------------------------------------------------------------
  /** Set dictionary name. */
  void SetDictionaryName(const mafString& dicname)   
    //----------------------------------------------------------------------------
  {
    m_Dict = dicname;
  }
  //----------------------------------------------------------------------------
  /** Set dictionary flag. */
  void SetDictionaryFlagOn()   
    //----------------------------------------------------------------------------
  {
    this->m_DictionaryAvailable = 1;
  }
  //----------------------------------------------------------------------------
  /** Set dictionary flag. */
  void SetDictionaryFlagOff()   
    //----------------------------------------------------------------------------
  {
    this->m_DictionaryAvailable = 0;
  }
  //----------------------------------------------------------------------------
  /** Get dictionary flag. */
  int GetDictionaryFlag()   
    //----------------------------------------------------------------------------
  {
    return this->m_DictionaryAvailable;
  }
  //----------------------------------------------------------------------------
  //** Makes the undo for the operation.
  void OpDo() override
    //----------------------------------------------------------------------------
  {
    m_Vme->ReparentTo(GetInput());
  }
	//----------------------------------------------------------------------------
  //** Makes the undo for the operation.
  void OpUndo() override
  //----------------------------------------------------------------------------
  {
    m_Vme->ReparentTo(nullptr);
  }
  

protected:
  mafString m_FileDir;
	mafString m_DictDir;
	mafString m_File;
	mafString m_Dict;
  mafString m_PgdWildc;
  mafString m_DicWildc;
	mafVME  *m_Vme = nullptr;
	int m_DictionaryAvailable = 0;
};
