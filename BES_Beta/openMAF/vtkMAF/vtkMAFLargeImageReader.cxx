/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: vtkMAFLargeImageReader.cxx,v $ 
  Language: C++ 
  Date: $Date: 2008-06-23 16:43:55 $ 
  Version: $Revision: 1.1 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/

#pragma warning(disable: 4996)	//depricated

#include "vtkMAFLargeImageReader.h"
#include "vtkMAFLargeImageData.h"
#include "vtkMAFFileDataProvider.h"

#include "vtkObjectFactory.h"

vtkCxxRevisionMacro(vtkMAFLargeImageReader, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkMAFLargeImageReader);

#include "mafMemDbg.h"

//ctor
vtkMAFLargeImageReader::vtkMAFLargeImageReader()
{
	this->SampleRate[0] = this->SampleRate[1] = this->SampleRate[2] = 1;
	this->AutoSampleRate = true;
	this->MemoryLimit = 1700;		//some magic constant given by Anupam

	this->DataScalarType = VTK_SHORT;
	this->NumberOfScalarComponents = 1;

	this->DataOrigin[0] = this->DataOrigin[1] = this->DataOrigin[2] = 0.0;

	this->DataSpacing[0] = this->DataSpacing[1] = this->DataSpacing[2] = 1.0;

	this->DataExtent[0] = this->DataExtent[2] = this->DataExtent[4] = 0;
	this->DataExtent[1] = this->DataExtent[3] = this->DataExtent[5] = 0;

	this->DataIncrements[0] = this->DataIncrements[1] = 
		this->DataIncrements[2] = this->DataIncrements[3] = 1;

	this->FileName = NULL;

	this->HeaderSize = 0;
	this->ManualHeaderSize = 0;

	// Left over from short reader
	this->SwapBytes = 0;
	this->FileLowerLeft = 0;
	this->FileDimensionality = 2;

	for (int idx = 0; idx < 3; ++idx)
	{
		this->DataVOI[idx*2] = this->DataVOI[idx*2 + 1] = 0;
	}

	this->DataMask = 0xffff;
}

vtkMAFLargeImageReader::~vtkMAFLargeImageReader()
{
	if (this->FileName)
	{
		delete [] this->FileName;
		this->FileName = NULL;
	}
}

//Initializes data providers for the given image data set
/*virtual*/ void vtkMAFLargeImageReader::InitializeDataProviders(vtkMAFLargeImageData* ds)
{
	vtkMAFFileDataProvider* fp;
	vtkMAFLargeDataProvider* pp = ds->GetPointDataProvider();	 
	if (pp != NULL)
		fp = vtkMAFFileDataProvider::SafeDownCast(pp);
	else
	{
		//no provider at all, create the default one
		fp = vtkMAFFileDataProvider::New();
		ds->SetPointDataProvider(pp = fp);
		pp->Delete();	//we no longer need it
	}
		
	if (fp != NULL && (fp->GetFileName() == NULL || 
		strcmp(fp->GetFileName(), this->GetFileName()) != 0))
	{
		//open a new file
		if (!fp->OpenFile(this->GetFileName()))
		{
			vtkErrorMacro(<< "Cannot open file: " << this->GetFileName());
		}
	}

	pp->SetHeaderSize(this->GetHeaderSize());
	pp->SetSwapBytes(this->GetSwapBytes() != 0);
}

//This method should fill the data providers in ds by the
//currently selected extent
/*virtual*/ void vtkMAFLargeImageReader::PopulateDataProviders(vtkMAFLargeImageData* ds)
{
	//nothing to do for vtkMAFFileDataProvider, etc.
}

// By default, UpdateInformation calls this method to copy information
// unmodified from the input to the output.
/*virtual*/ void vtkMAFLargeImageReader::ExecuteInformation()
{	
	//default computation of output extent
	vtkMAFLargeImageData *output = this->GetOutput();	

	//first, let us initialize data providers
	this->InitializeDataProviders(output);
	
	//set the dimensions of underlaying file
	//NB: Extent will be set to UpdateExtent by the caller
	output->SetWholeExtent(this->DataExtent);
	output->SetUpdateExtentToWholeExtent();

	// set the whole extent (area of interest in the image data) to our VOI
	if ((this->DataVOI[0] | this->DataVOI[1] | this->DataVOI[2] |
		this->DataVOI[3] | this->DataVOI[4] | this->DataVOI[5]) != 0)	
		output->SetVOI(this->DataVOI);	//if it was set
	
	// set the spacing
	output->SetSpacing(this->DataSpacing);

	// set the origin.
	output->SetOrigin(this->DataOrigin);

	output->SetScalarType(this->DataScalarType);
	output->SetNumberOfScalarComponents(this->NumberOfScalarComponents);
	output->SetDataLowerLeft(this->FileLowerLeft != 0);
	output->SetDataMask(this->DataMask);	
	output->SetMemoryLimit(this->MemoryLimit);
}

//----------------------------------------------------------------------------
// This function reads a data from a file.  The datas extent/axes
// are assumed to be the same as the file extent/order.
void vtkMAFLargeImageReader::ExecuteData(vtkDataObject *output)
{	
	if (!this->FileName)
	{
		vtkErrorMacro("A valid FileName must be specified.");
		return;
	}

	//This call initializes the output: it calls ExecuteInformation()
	//to copy Extents, Spacing, etc. and calls vtkMAFLargeImageData::AllocateScalars   
	//that updates the descriptor of point scalar data with the provided 
	//information (if the descriptor does not exist, it is created)
	vtkMAFLargeImageData *data = this->AllocateOutputData(output);

	int* ext = data->GetExtent();
	vtkDebugMacro("Reading extent: " << ext[0] << ", " << ext[1] << ", " 
		<< ext[2] << ", " << ext[3] << ", " << ext[4] << ", " << ext[5]);
	
	PopulateDataProviders(data);
}


#pragma region vtkMAFLargeImageReader stuff
//----------------------------------------------------------------------------
// This function sets the name of the file. 
void vtkMAFLargeImageReader::SetFileName(const char *name)
{
	if ( this->FileName && name && (!strcmp(this->FileName,name)))
	{
		return;
	}
	if (!name && !this->FileName)
	{
		return;
	}
	if (this->FileName)
	{
		delete [] this->FileName;
	}

	if (name)
	{
		this->FileName = new char[strlen(name) + 1];
		strcpy(this->FileName, name);
	}
	else
	{
		this->FileName = NULL;
	}

	this->Modified();
}

void vtkMAFLargeImageReader::SetDataByteOrderToBigEndian()
{
#ifndef VTK_WORDS_BIGENDIAN
	this->SwapBytesOn();
#else
	this->SwapBytesOff();
#endif
}

void vtkMAFLargeImageReader::SetDataByteOrderToLittleEndian()
{
#ifdef VTK_WORDS_BIGENDIAN
	this->SwapBytesOn();
#else
	this->SwapBytesOff();
#endif
}

#include "vtkImageReader2.h"
void vtkMAFLargeImageReader::SetDataByteOrder(int byteOrder)
{
	if ( byteOrder == VTK_FILE_BYTE_ORDER_BIG_ENDIAN )
	{
		this->SetDataByteOrderToBigEndian();
	}
	else
	{
		this->SetDataByteOrderToLittleEndian();
	}
}

int vtkMAFLargeImageReader::GetDataByteOrder()
{
#ifdef VTK_WORDS_BIGENDIAN
	if ( this->SwapBytes )
	{
		return VTK_FILE_BYTE_ORDER_LITTLE_ENDIAN;
	}
	else
	{
		return VTK_FILE_BYTE_ORDER_BIG_ENDIAN;
	}
#else
	if ( this->SwapBytes )
	{
		return VTK_FILE_BYTE_ORDER_BIG_ENDIAN;
	}
	else
	{
		return VTK_FILE_BYTE_ORDER_LITTLE_ENDIAN;
	}
#endif
}

const char *vtkMAFLargeImageReader::GetDataByteOrderAsString()
{
#ifdef VTK_WORDS_BIGENDIAN
	if ( this->SwapBytes )
	{
		return "LittleEndian";
	}
	else
	{
		return "BigEndian";
	}
#else
	if ( this->SwapBytes )
	{
		return "BigEndian";
	}
	else
	{
		return "LittleEndian";
	}
#endif
}


//----------------------------------------------------------------------------
void vtkMAFLargeImageReader::PrintSelf(ostream& os, vtkIndent indent)
{
	int idx;

	this->Superclass::PrintSelf(os,indent);

	// this->File, this->Colors need not be printed  
	os << indent << "FileName: " <<
		(this->FileName ? this->FileName : "(none)") << "\n";

	os << indent << "DataScalarType: " 
		<< vtkImageScalarTypeNameMacro(this->DataScalarType) << "\n";
	os << indent << "NumberOfScalarComponents: " 
		<< this->NumberOfScalarComponents << "\n";

	os << indent << "File Dimensionality: " << this->FileDimensionality << "\n";

	os << indent << "File Lower Left: " << 
		(this->FileLowerLeft ? "On\n" : "Off\n");

	os << indent << "Swap Bytes: " << (this->SwapBytes ? "On\n" : "Off\n");

	os << indent << "DataIncrements: (" << this->DataIncrements[0];
	for (idx = 1; idx < 2; ++idx)
	{
		os << ", " << this->DataIncrements[idx];
	}
	os << ")\n";

	os << indent << "DataExtent: (" << this->DataExtent[0];
	for (idx = 1; idx < 6; ++idx)
	{
		os << ", " << this->DataExtent[idx];
	}
	os << ")\n";

	os << indent << "DataSpacing: (" << this->DataSpacing[0];
	for (idx = 1; idx < 3; ++idx)
	{
		os << ", " << this->DataSpacing[idx];
	}
	os << ")\n";

	os << indent << "DataOrigin: (" << this->DataOrigin[0];
	for (idx = 1; idx < 3; ++idx)
	{
		os << ", " << this->DataOrigin[idx];
	}
	os << ")\n";

	os << indent << "HeaderSize: " << this->HeaderSize << "\n";
}


//----------------------------------------------------------------------------
// Manual initialization.
void vtkMAFLargeImageReader::SetHeaderSize(unsigned long size)
{
	if (size != this->HeaderSize)
	{
		this->HeaderSize = size;
		this->Modified();
	}
	this->ManualHeaderSize = 1;
}


//----------------------------------------------------------------------------
// This function opens a file to determine the file size, and to
// automatically determine the header size.
void vtkMAFLargeImageReader::ComputeDataIncrements()
{
	int idx;
	vtkIdType64 fileDataLength;

	// Determine the expected length of the data ...
	switch (this->DataScalarType)
	{
	case VTK_FLOAT:
		fileDataLength = sizeof(float);
		break;
	case VTK_DOUBLE:
		fileDataLength = sizeof(double);
		break;
	case VTK_INT:
		fileDataLength = sizeof(int);
		break;
	case VTK_UNSIGNED_INT:
		fileDataLength = sizeof(unsigned int);
		break;
	case VTK_LONG:
		fileDataLength = sizeof(long);
		break;
	case VTK_UNSIGNED_LONG:
		fileDataLength = sizeof(unsigned long);
		break;
	case VTK_SHORT:
		fileDataLength = sizeof(short);
		break;
	case VTK_UNSIGNED_SHORT:
		fileDataLength = sizeof(unsigned short);
		break;
	case VTK_CHAR:
		fileDataLength = sizeof(char);
		break;
	case VTK_UNSIGNED_CHAR:
		fileDataLength = sizeof(unsigned char);
		break;
	default:
		vtkErrorMacro(<< "Unknown DataScalarType");
		return;
	}

	fileDataLength *= this->NumberOfScalarComponents;

	// compute the fileDataLength (in units of bytes)
	for (idx = 0; idx < 3; ++idx)
	{
		this->DataIncrements[idx] = fileDataLength;
		fileDataLength = fileDataLength *
			(this->DataExtent[idx*2+1] - this->DataExtent[idx*2] + 1);
	}

	this->DataIncrements[3] = fileDataLength;
}


#include <sys/stat.h>
unsigned long vtkMAFLargeImageReader::GetHeaderSize()
{
	if (!this->FileName)
	{
		vtkErrorMacro(<<"Either a FileName or FilePattern must be specified.");
		return 0;
	}

	if (!this->ManualHeaderSize)
	{
		this->ComputeDataIncrements();

		// make sure we figure out a filename to open
		struct stat statbuf;
		stat(this->FileName, &statbuf);

		return (int)(statbuf.st_size -
			this->DataIncrements[this->GetFileDimensionality()]);
	}

	return this->HeaderSize;
}


//----------------------------------------------------------------------------
// Set the data type of pixels in the file.  
// If you want the output scalar type to have a different value, set it
// after this method is called.
void vtkMAFLargeImageReader::SetDataScalarType(int type)
{
	if (type == this->DataScalarType)
	{
		return;
	}

	this->Modified();
	this->DataScalarType = type;
	// Set the default output scalar type
	this->GetOutput()->SetScalarType(this->DataScalarType);
}
#pragma endregion

#pragma region vtkImageReader stuff
void vtkMAFLargeImageReader::ComputeTransformedExtent(int inExtent[6],
											  int outExtent[6])
{
	int idx;
	int dataExtent[6];

	memcpy (outExtent, inExtent, 6 * sizeof (int));
	memcpy (dataExtent, this->DataExtent, 6 * sizeof(int));

	for (idx = 0; idx < 6; idx += 2)
	{
		if (outExtent[idx] > outExtent[idx+1]) 
		{
			int temp = outExtent[idx];
			outExtent[idx] = outExtent[idx+1];
			outExtent[idx+1] = temp;
		}
		// do the slide to 000 origin by subtracting the minimum extent
		outExtent[idx] -= dataExtent[idx];
		outExtent[idx+1] -= dataExtent[idx];
	}

	vtkDebugMacro(<< "Transformed extent are:" 
		<< outExtent[0] << ", " << outExtent[1] << ", "
		<< outExtent[2] << ", " << outExtent[3] << ", "
		<< outExtent[4] << ", " << outExtent[5]);
}

void vtkMAFLargeImageReader::ComputeInverseTransformedExtent(int inExtent[6],
													 int outExtent[6])
{
	int idx;

	memcpy (outExtent, inExtent, 6 * sizeof (int));
	for (idx = 0; idx < 6; idx += 2)
	{
		// do the slide to 000 origin by subtracting the minimum extent
		outExtent[idx] += this->DataExtent[idx];
		outExtent[idx+1] += this->DataExtent[idx];
	}

	vtkDebugMacro(<< "Inverse Transformed extent are:" 
		<< outExtent[0] << ", " << outExtent[1] << ", "
		<< outExtent[2] << ", " << outExtent[3] << ", "
		<< outExtent[4] << ", " << outExtent[5]);
}

#pragma endregion