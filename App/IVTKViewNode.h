#pragma once

#include "ftkConfigure.h"

#include <vtkAssembly.h>
#include <vtkSmartPointer.h>

BEGIN_FTK_NAMESPACE

class IVTKViewNode
{
public:
	virtual ~IVTKViewNode() = default;

	virtual IVTKViewNode* parent() const = 0;

	virtual std::vector<IVTKViewNode*> children() const = 0;

	virtual vtkSmartPointer<vtkAssembly> getAssembly(size_t) const = 0;
};

END_FTK_NAMESPACE
