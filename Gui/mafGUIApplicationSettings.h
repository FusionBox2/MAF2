/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafGUIApplicationSettings.h,v $
Language:  C++
Date:      $Date: 2011-05-25 10:03:24 $
Version:   $Revision: 1.1.2.5 $
Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafGUIApplicationSettings_H__
#define __mafGUIApplicationSettings_H__

#include "mafGUISettings.h"

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// mafGUIApplicationSettings :
//----------------------------------------------------------------------------
/**
   Class Name: mafGUIApplicationSettings.
   Class that contains several application options like: 
   - logging to file
   - logging verbosity
   - logging directory
   - Undo warning
   - passphrase for data encryption
   - default image type when saving a snapshot
*/
class MAF_EXPORT mafGUIApplicationSettings : public mafGUISettings
{
public:
  /** constructor .*/
	mafGUIApplicationSettings(mafObserver *Listener, const mafString &label = _("Application"));
  /** destructor .*/
	~mafGUIApplicationSettings(); 

  /** GUI IDs */
  enum APP_SETTINGS_WIDGET_ID
  {
    ID_LOG_TO_FILE = MINID,
    ID_LOG_VERBOSE,
    ID_LOG_DIR,
    ID_WARN_UNDO,
    ID_USE_DEFAULT_PASSPHRASE,
    ID_PASSPHRASE,
		IMAGE_TYPE_ID,
  };

  /** Image Type IDs */
	enum IMAGE_TYPE_LIST
	{
		JPG = 0,
		BMP,
    PNG,
	};

  /** Answer to the messages coming from interface. */
  void OnEvent(mafEventBase *maf_event);

  /** Return true if Log To File is enabled.*/
  bool GetLogToFileStatus() {return m_LogToFile != 0;};

  /** Set the log status flag.*/
  void SetLogFileStatus(int log_status);

  /** Return true if Verbose Log is enabled.*/
  bool GetLogVerboseStatus() {return m_VerboseLog != 0;};
  
  /** Set the log verbose status flag.*/
  void SetLogVerboseStatus(int log_verbose);

  /** Return the folder in which store the log files.*/
  mafString &GetLogFolder() {return m_LogFolder;};

  /** Set a folder name for the log file.*/
  void SetLogFolder(mafString log_folder);

  /** Return the status of the WarnUser flag.*/
  bool GetWarnUserFlag() {return m_WarnUserFlag != 0;};

  /** Return the status of the default pass phrase usage.*/
  bool UseDefaultPassPhrase() {return m_UseDefaultPasPhrase != 0;};

  /** Set the flag to use the default passphrase with which encrypt saved data.
  If the flag is '0' means that no default passphrase is used, so the second optional argument has to be set.*/
  void SetUseDefaultPassPhrase(int use_default, mafString passphrase = "");

  /** Return the Pass phrase used to encrypt/decrypt files.*/
  mafString &GetPassPhrase() {return m_PassPhrase;};

  /** Set the custom passphrase to encrypt data.*/
  void SetPassPhrase(mafString pass_phrase);

  /** Select image type during saving of the views*/
  void SetImageTypeId(int imageTypeId){m_ImageTypeId = imageTypeId;}

	/** Select image type during saving of the views*/
	int GetImageTypeId(){return m_ImageTypeId;};

  /** Enable the possibility to choose log directory. */
  void EnableLogDirChoices(){m_EnableLogDirChoices = true;};
  /** Disable the possibility to choose log directory. */
  void DisableLogDirChoices(){m_EnableLogDirChoices = false;};

protected:
  /** Create the GUI for the setting panel.*/
  void CreateGui();

  /** Initialize the application settings.*/
  void InitializeSettings();

  /** Used to enable/disable items according to the current widgets state.*/
  void EnableItems();

  // Log variables
  int     m_LogToFile; ///< Flag used to store the On-Off state of log to file mechanism.
  int     m_VerboseLog;///< Flag used to store the On-Off state of verbose logging to file.
  mafString m_LogFolder;///< Flag used to store the log folder. 

  int m_WarnUserFlag; ///< Warn user flag on not supported undo operations

  // Encryption variables
  int       m_UseDefaultPasPhrase;
  mafString m_PassPhrase;

	int       m_ImageTypeId;

  bool m_EnableLogDirChoices;
};
#endif
