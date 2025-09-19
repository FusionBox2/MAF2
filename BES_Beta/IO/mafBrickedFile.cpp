#include "mafBrickedFile.h"

mafBrickedFile::mafBrickedFile() = default;

mafBrickedFile::~mafBrickedFile()
{
	DeallocateBuffers();

	if (m_BrickFile)
	{
		m_BrickFile->Close();
		m_BrickFile->Delete();
	}
}