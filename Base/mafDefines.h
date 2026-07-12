#pragma once

//----------------------------------------------------------------------------
// Pragmas: disable the "singned/unsigned mismatch" warning
//----------------------------------------------------------------------------
#pragma warning( disable : 4018 )
#pragma warning( disable : 4251 )

//----------------------------------------------------------------------------
// Includes: mafConfigure should be first, mafIncludeWX second
//----------------------------------------------------------------------------
#include "mafConfigure.h"

#ifdef MAF_USE_WX
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
#endif

#include "Base/mfLog.h"

#include <string.h>
#include <typeinfo>
#include <iosfwd>
#include <sstream>

typedef double mafTimeStamp; ///< type for time varying data timestamps (not for pipelines timestamps!)
typedef intptr_t mafID; ///< type for IDs inside MAF @todo to be changed to support 64bit IDs
typedef std::type_info mafTypeID; ///< type for mafObject's class type IDs

enum MAF_EXPORT MAF_RETURN_VALUES_ID
{
  MAF_OK = 0,
  MAF_ERROR,
  MAF_WAIT,
  MAF_USER_RETURN_VALUE
};
#define ID_NO_EVENT 0
/*enum MAF_NO_EVENT_ID
{
  ID_NO_EVENT = 0
};
*/
#define MAF_STRING_BUFFER_SIZE 2048
/*enum MAF_DEFAULT_BUFFER_SIZE
{
  MAF_STRING_BUFFER_SIZE = 2048
};
*/
//------------------------------------------------------------------------------
// Global Variables
//------------------------------------------------------------------------------
class mafDeviceButtonsPadMouse;
MAF_EXPORT mafDeviceButtonsPadMouse *GetGlobalMouse();
MAF_EXPORT void SetGlobalMouse(mafDeviceButtonsPadMouse *);


//------------------------------------------------------------------------------
// Global Functions
//------------------------------------------------------------------------------
/** 
  reliable comparison test for floating point numbers. Extracted from article:
  "Work Around Floating-Point Accuracy/Comparison Problems" Article ID: Q69333
  of MSDN library*/
MAF_EXPORT bool mafEquals(double x, double y);
MAF_EXPORT bool mafFloatEquals(float x, float y);

/** retrieve the double value with the desired precision*/
MAF_EXPORT double mafRoundToPrecision(double val, unsigned prec);

/** return true if it's little endian*/
MAF_EXPORT bool mafIsLittleEndian(void);

/** wait for given milliseconds */
MAF_EXPORT void mafSleep(int msec);

//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------

/** Delete a VTK object */
#define vtkDEL(a) do{if (a) { (a)->Delete(); a = NULL; }}while(0)

/** Allocate a new VTK object: don't worry, New is a static member function! */
#define vtkNEW(a) a=(a)->New()

/** delete a new() allocated object */
#define cppDEL(a) do{if (a) { delete a; a = NULL;}}while(0)

/** Shortcut for type checking */
#define IsMAFType(type_name) IsA(type_name::GetStaticTypeId())

/** This macros is used to declare a new Id and should be placed in a .h file*/
#define MAF_ID_IMP(idname) const mafID idname = mmuIdFactory::GetNextId(#idname);
/** This macros is used to define a new Id and should be placed in a .cpp file*/
#define MAF_ID_DEC(idname) static const mafID idname;
/** This macros is used to declare a new global Id and should be placed in a .h file*/
#define MAF_ID_GLOBAL(idname) extern const mafID idname;

/** This macro is used to define a group of IDs and should be placed in a .h file. Ids declaration with MAF_ID_DEC. */
// Not Used !!
//#define MAF_ID_GROUP(groupname,num) const mafID groupname = mmuIdFactory::AllocIdGroup(#groupname,num);

/** This macro is used to declare the base ID of a  group of ids. */
#define MAF_ID_CLASS_DEC(baseClass) static const mafID BaseID;
/** This macro is used to define the base ID of a  group of ids. */
#define MAF_ID_CLASS_IMP(baseClass,num) const mafID baseClass::BaseID = mmuIdFactory::AllocIdGroup("#baseClass::BaseID",num);

#define MAF_ID_LOC(name,idname) static const mafID name=mmuIdFactory::GetId(#idname);

/** These macros are used to retrieve the base ID of a group of events */
#define mafGetEventClassId(event,baseClass) (event->GetID()-baseClass::BaseID)
#define mafGetEventGroupId(event,baseID) (event->GetID()-baseID)
#define mafEvalGroupId(baseClass,id) (baseClass::BaseID+id)

/** Helper macro used for testing */  
#define MAF_TEST(a) if (!(a)) \
{ \
  std::cerr << "Test failed at line " \
  << __LINE__ << " : " << #a << std::endl; \
  return MAF_ERROR; \
}
