/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: vtkMAFLargeDataProvider.cxx,v $ 
  Language: C++ 
  Date: $Date: 2008-06-23 16:43:55 $ 
  Version: $Revision: 1.1 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
  Abstract class - every large data provider (memory mapped, distributed, etc.
  is supposed to be derived from this class
  =========================================================================*/
#pragma warning(disable: 4996)	//depricated

#include "vtkMAFLargeDataProvider.h"
#include "vtkObjectFactory.h"
#include "vtkFieldData.h"
#include "vtkIntArray.h"
#include "vtkByteSwap.h"



vtkCxxRevisionMacro(vtkMAFLargeDataProvider, "$Revision: 1.1 $");

#include "mafMemDbg.h"
#include <assert.h>

vtkMAFLargeDataProvider::vtkMAFLargeDataProvider()
{
	TagArray = vtkFieldData::New();
	
	HeaderSize = 0;
	SwapBytes = false;

	InitializeDescriptors();
}

vtkMAFLargeDataProvider::~vtkMAFLargeDataProvider() 
{
	TagArray->Delete();	
	InitializeDescriptors();
}

// Release all descriptors but do not delete this object.
// Also, the underlaying data is untouched.
/*virtual*/ void vtkMAFLargeDataProvider::InitializeDescriptors()
{
	StringToIntMap::iterator it = m_DescriptorsMap.begin();
	while (it != m_DescriptorsMap.end())
	{
		delete[] it->first;
		it++;
	}

	m_DescriptorsMap.clear();

	for (int i = 0; i < (int)m_Descriptors.size(); i++)
	{
		if (m_Descriptors[i] != NULL)
			m_Descriptors[i]->UnRegister(this);
	}
	
	m_Descriptors.clear();

	memset(m_SpecDescPos, -1, sizeof(m_SpecDescPos));
	this->Modified();
}

// Return this object's modified time.
/*virtual*/ unsigned long vtkMAFLargeDataProvider::GetMTime()
{
	unsigned long mt = Superclass::GetMTime();
	for (int i = 0; i < (int)m_Descriptors.size(); i++) 
	{
		unsigned long t = m_Descriptors[i]->GetMTime();
		if (t > mt)
			mt = t;
	}

	return mt;
}

//Sets the data array descriptor having the specified name
//The caller is supposed to Delete the given array when it is no longer needed
/*virtual*/ int vtkMAFLargeDataProvider::AddDescriptor(vtkMAFDataArrayDescriptor* dad)
{	
	assert(dad != NULL);

	if (dad == NULL)
		return -1;

	dad->Register(this);
	m_Descriptors.push_back(dad);
	int index = (int)m_Descriptors.size() - 1;

	ReplaceLookupName(NULL, dad->GetName(), index);
	this->Modified();	//we have changed it
	return index;
}

//Returns index of the descriptor given by name, or -1 if no such descriptor
//exists in the collection
/*virtual*/ int vtkMAFLargeDataProvider::GetIndexOfDescriptor(const char* name)
{
	assert(name != NULL);

	StringToIntMap::iterator it = m_DescriptorsMap.find(name);
	if (it == m_DescriptorsMap.end())
		return -1;		//not found

	return it->second;
}

// Remove an array (with the given name) from the list of arrays.
/*virtual*/ void vtkMAFLargeDataProvider::RemoveDescriptor(int i)
{
	if (i < 0 || i >= this->GetNumberOfDescriptors())		
		return;		//invalid index

	StringToIntMap::iterator itrem, it = m_DescriptorsMap.begin();
	while (it != m_DescriptorsMap.end())
	{
		if (it->second >= i) 
		{
			//this is index that must be shifted
			if (it->second == i) 
			{			
				//this is the item to be removed
				delete const_cast<char*>(it->first);
				itrem = it;
			}

			it->second--;
		}

		it++;
	}

	m_DescriptorsMap.erase(itrem);
	m_Descriptors[i]->UnRegister(this);
	m_Descriptors.erase(m_Descriptors.begin() + i);

	//fix m_SpecDescPos
	for (int j = 0; j < vtkDataSetAttributes::NUM_ATTRIBUTES; j++) 
	{
		if (m_SpecDescPos[j] >= i)
		{
			if (m_SpecDescPos[j] == i)
				m_SpecDescPos[j] = -1;	//remove it
			else
				m_SpecDescPos[j]--;
		}
	}
}

//Gets the descriptor with the specified name for the given descriptor types
//(i.e., SCALARS, VECTORS, etc.). If name is NULL, it returns the active descriptor
//of that type. Unlike vtkDataSetAttributes, if the descriptor could not be found, 
//it is not created.
/*virtual*/ vtkMAFDataArrayDescriptor* vtkMAFLargeDataProvider::GetDescriptor(int type, const char* name)
{
	assert(type >= 0 && type < vtkDataSetAttributes::NUM_ATTRIBUTES);

	if (name == NULL || *name != '\0')
		return GetDescriptor(m_SpecDescPos[type]);

	//if name is specified, try to find it
	return GetDescriptor(name);
}

//Sets the descriptor for the given type (e.g., SCALARS, VECTORS etc.),
//returns -1 if an error occurs, otherwise,
//it returns the index of descriptor (that can be used e.g. in GetDescriptor)
/*virtual*/ int vtkMAFLargeDataProvider::SetDescriptor(int type, vtkMAFDataArrayDescriptor* dad)
{
	assert(type >= 0 && type < vtkDataSetAttributes::NUM_ATTRIBUTES);
	int curPos = m_SpecDescPos[type];

	// If there is an existing attribute, replace it
	if (curPos < 0)
		curPos = AddDescriptor(dad);
	else
	{
		if (GetDescriptor(curPos) == dad)		
			return curPos;	//the same, no change
		
		if (dad == NULL)
			RemoveDescriptor(curPos);
		else
		{
			//we will have to modify it
			vtkMAFDataArrayDescriptor* old = m_Descriptors[curPos];
			ReplaceLookupName(old->GetName(), dad->GetName());
			old->UnRegister(this);			

			m_Descriptors[curPos] = dad;
			dad->Register(this);
		}
	}
	
	m_SpecDescPos[type] = curPos;
	this->Modified();
	return curPos;
}


//returns index of the special descriptor
/*virtual*/ int vtkMAFLargeDataProvider::GetIndexOfDescriptor(int type)
{
	assert(type >= 0 && type < vtkDataSetAttributes::NUM_ATTRIBUTES);
	
	return m_SpecDescPos[type];
}

//sets index of the special descriptor
/*virtual*/ void vtkMAFLargeDataProvider::SetIndexOfDescriptor(int type, int idx)
{
	assert(type >= 0 && type < vtkDataSetAttributes::NUM_ATTRIBUTES);
	assert(idx >= 0 && idx < (int)m_Descriptors.size());

	m_SpecDescPos[type] = idx;
}

//replaces the name used in the lookup table, if old_name is NULL, then new entry
//refereed to the given index is created
void vtkMAFLargeDataProvider
	::ReplaceLookupName(const char* old_name, const char* new_name, int index)
{	
	if (old_name == NULL && new_name == NULL)
		return;	//nothing to be replaced, lets quit
	
	if (old_name != NULL)	
	{
		StringToIntMap::iterator it = m_DescriptorsMap.find(old_name);
		if (it != m_DescriptorsMap.end())
		{
			delete[] it->first;
			m_DescriptorsMap.erase(it);
		}
	}

	//construct the copy of string
	char* new_name_cp = NULL;
	if (new_name != NULL)
	{
		assert(index >= 0);

		new_name_cp = new char[(int)strlen(new_name) + 1];
		strcpy(new_name_cp, new_name);

		m_DescriptorsMap.insert(StringToIntMap::value_type(new_name_cp, index));	
	}	
}

//Constructs a new vtkDataArray object and fills it with a range of tuples from 
//the data array at index idx, starting at the specified index.
//If count is longer than the data array, the remainder of the data array is copied.
//NB: The caller is responsible for the deletion of constructed object.
/*virtual*/ vtkDataArray* vtkMAFLargeDataProvider
	::GetDataArray(int idx, vtkIdType64 startIndex, int countTuples)
{
	vtkMAFDataArrayDescriptor* desc = GetDescriptor(idx);
	if (desc == NULL)
		return NULL;

	vtkDataArray* retArray = vtkDataArray::CreateDataArray(desc->GetDataType());
	retArray->SetNumberOfComponents(desc->GetNumberOfComponents());
	
	GetDataArray(idx, retArray, startIndex, countTuples);
	return retArray;
}

//Fills the given buffer with a range of tuples from 
//the data array at index idx, starting at the specified index.
//If count is longer than the data array, the remainder of the data array is copied.	
//NB: buffer must be created by the caller (and be compatible with the descriptor)
//If the buffer is not capable to hold the data, it is enlarged automatically
/*virtual*/ void vtkMAFLargeDataProvider
	::GetDataArray(int idx, vtkDataArray* buffer, vtkIdType64 startIndex, int countTuples)
{	
	vtkMAFDataArrayDescriptor* desc = GetDescriptor(idx);
	if (desc == NULL)
		buffer->Reset();	//set the array empty
	else
	{
		//readjust the amount of tuples to be taken
		vtkIdType64 size = desc->GetNumberOfTuples();
		vtkIdType64 endIndex = countTuples < 0 ? size : startIndex + countTuples;
		if (endIndex >= size)
			countTuples = endIndex - startIndex;

		//get the buffer to direct access
		int numComps = desc->GetNumberOfComponents(); 
    buffer->SetNumberOfComponents(numComps);    
    buffer->SetNumberOfTuples(countTuples);        

		GetDataArray(idx, buffer->GetVoidPointer(0), countTuples*numComps, startIndex*numComps);
	}
}

//Fills the given buffer with elements from the data array at index idx, 
//starting at the specified !element! index. Buffer must be capable to hold these elements.
//The routine returns the number of stored elements (may be less than count, if the
//amount of data available is smaller than requested)
/*virtual*/ vtkIdType64 vtkMAFLargeDataProvider::GetDataArray(int idx, void* buffer, 
	vtkIdType64 count, vtkIdType64 startIndex)
{
	vtkMAFDataArrayDescriptor* desc = GetDescriptor(idx);
	if (desc == NULL)
		return 0;

	if (desc->GetDataType() == VTK_BIT) {
		vtkErrorMacro(<< "Binary arrays are not supported by vtkMAFLargeDataProvider.");
		return 0;	//bit arrays are not supported
	}

	//readjust the amount of elements to be taken
	vtkIdType64 size = desc->GetSize();
	vtkIdType64 endIndex = startIndex + count;
	if (endIndex >= size)
		count = size - startIndex;	

	//compute position in bytes	into the underlaying data source
	int elemSize = desc->GetDataTypeSize();
	vtkIdType64 ofset = GetOffset(idx, startIndex*elemSize);
	vtkIdType64 read = ReadBinaryData(ofset, buffer, count*elemSize);

	// handle swapping
	if (GetSwapBytes())
		vtkByteSwap::SwapVoidRange(buffer, read, elemSize);

	return count / elemSize;
}

//Gets the global offset for the offset da_ofs into the data array 
//denoted by the index da_idx. Global offset is needed by ReadBinaryData
//and WriteBinaryData methods
/*virtual*/ vtkIdType64 vtkMAFLargeDataProvider::GetOffset(int da_idx, vtkIdType64 da_ofs)
{
	if (this->GetMTime() > m_OffsetsComputeTime)
	{
		//we have to recalculate global offsets table
		m_Offsets.clear();
		
		vtkIdType64 ofs = this->GetHeaderSize();
		m_Offsets.push_back(ofs);
		
		for (int i = 0; i < (int)m_Descriptors.size(); i++)
		{
			vtkMAFDataArrayDescriptor* desc = m_Descriptors[i];
			ofs += desc->GetActualMemorySize();
			m_Offsets.push_back(ofs);
		}

		m_OffsetsComputeTime.Modified();
	}

	return da_ofs + m_Offsets[da_idx];
}

//Called by GetScalars(), etc. See public GetDataArray
/*virtual*/ vtkDataArray* vtkMAFLargeDataProvider::GetDataArray(int type, const char* name, 
									   vtkIdType64 startIndex, int countTuples)
{
	assert(type >= 0 && type < vtkDataSetAttributes::NUM_ATTRIBUTES);

	if (name == NULL || *name != '\0')
		return GetDataArray(m_SpecDescPos[type], startIndex, countTuples);

	//if name is specified, try to find it
	return GetDataArray(name, startIndex, countTuples);
}

//Called by GetScalars(), etc. See public GetDataArray
/*virtual*/ void vtkMAFLargeDataProvider::GetDataArray(int type, const char* name, 
							vtkDataArray* buffer, vtkIdType64 startIndex, int countTuples)
{
	assert(type >= 0 && type < vtkDataSetAttributes::NUM_ATTRIBUTES);

	if (name == NULL || *name != '\0')
		GetDataArray(m_SpecDescPos[type], buffer, startIndex, countTuples);
	else
		//if name is specified, try to find it
		GetDataArray(name, buffer, startIndex, countTuples);
}

//Stores the data from the given buffer into the data array 
//denoted by the index, starting at the specified index.
/*virtual*/ void vtkMAFLargeDataProvider::SetDataArray(int da_idx, vtkDataArray* buffer, vtkIdType64 startIndex)
{
	vtkMAFDataArrayDescriptor* desc = GetDescriptor(da_idx);
	if (desc == NULL) {
		assert(false);
		return;			//error
	}

	//get the buffer to direct access
	int numComps = desc->GetNumberOfComponents();
	vtkIdType64 buff_sz = buffer->GetMaxId() + 1;	

	SetDataArray(da_idx, buffer->GetVoidPointer(0), buff_sz, startIndex*numComps);
}

//Stores the elements from the given buffer into the data array 
//denoted by the index, starting at the specified index.
//NB: count is given in number of elements (not bytes)
/*virtual*/ void vtkMAFLargeDataProvider::SetDataArray(int da_idx, void* buffer, vtkIdType64 count,
													vtkIdType64 startIndex)
{
	vtkMAFDataArrayDescriptor* desc = GetDescriptor(da_idx);
	if (desc == NULL) {
		assert(false);
		return;
	}

	if (desc->GetDataType() == VTK_BIT) {
		assert(false);
		vtkErrorMacro(<< "Binary arrays are not supported by vtkMAFLargeDataProvider.");
		return;	//bit arrays are not supported
	}

	//compute position in bytes	into the underlaying data source
	int elemSize = desc->GetDataTypeSize();
	vtkIdType64 ofset = GetOffset(da_idx, startIndex*elemSize);

	char* buf = (char*)buffer;
	
	// handle swapping
	if (GetSwapBytes())
	{
		buf = new char[count*elemSize];
		memcpy(buf, buffer, count);
		vtkByteSwap::SwapVoidRange(buf, count, count*elemSize);
	}

	WriteBinaryData(ofset, buffer, count*elemSize);

	if (GetSwapBytes())
		delete[] buf;

	this->Modified();
}

//Called by SetScalars(), etc. See public SetDataArray
/*virtual*/ void vtkMAFLargeDataProvider::SetDataArray(int type, const char* name, 
													vtkDataArray* buffer, 
													vtkIdType64 startIndex)
{
	assert(type >= 0 && type < vtkDataSetAttributes::NUM_ATTRIBUTES);

	if (name == NULL || *name != '\0')
		SetDataArray(m_SpecDescPos[type], buffer, startIndex);
	else
		//if name is specified, try to find it
		SetDataArray(name, buffer, startIndex);
}

// Description:
// Shallow copy, copies only references (increments refenrece counters)
void vtkMAFLargeDataProvider::ShallowCopy(vtkMAFLargeDataProvider *src)
{
	if (src != NULL)
	{
		InternalDataCopy(src);

		for (int i = 0; i < (int)src->m_Descriptors.size(); i++)
		{
			src->m_Descriptors[i]->Register(this);
			this->m_Descriptors.push_back(src->m_Descriptors[i]);
		}
	}
}

//Deep copy. NB: Data arrays ARE NOT COPIED
void vtkMAFLargeDataProvider::DeepCopy(vtkMAFLargeDataProvider *src)
{
	if (src != NULL)
	{
		InternalDataCopy(src);

		for (int i = 0; i < (int)src->m_Descriptors.size(); i++)
		{
			vtkMAFDataArrayDescriptor* sd = src->m_Descriptors[i];
			vtkMAFDataArrayDescriptor* dd = vtkMAFDataArrayDescriptor::New();
			dd->SetDataType(sd->GetDataType());
			dd->SetName(sd->GetName());
			dd->SetNumberOfComponents(sd->GetNumberOfComponents());
			dd->SetNumberOfTuples(sd->GetNumberOfTuples());
			
			this->m_Descriptors.push_back(dd);
		}
	}
}

void vtkMAFLargeDataProvider::InternalDataCopy(vtkMAFLargeDataProvider *src)
{
	this->InitializeDescriptors();

	memcpy(this->m_SpecDescPos, src->m_SpecDescPos, sizeof(m_SpecDescPos));
	this->m_OffsetsComputeTime = src->m_OffsetsComputeTime;
	this->SwapBytes = src->SwapBytes;
	this->HeaderSize = src->HeaderSize;

	for (int i = 0; i < (int)src->m_Offsets.size(); i++)
	{
		this->m_Offsets.push_back(src->m_Offsets[i]);
	}

	for (StringToIntMap::iterator it = src->m_DescriptorsMap.begin();
		it != src->m_DescriptorsMap.end(); it++)
	{
		this->ReplaceLookupName(NULL, it->first, it->second);
	}
}