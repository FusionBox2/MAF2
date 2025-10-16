// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause
/**
 * @class   vtkMetashapeOBJReader
 * @brief   read Wavefront .obj files
 *
 * vtkMetashapeOBJReader is a source object that reads Wavefront .obj
 * files. The output of this source object is polygonal data.
 * @sa
 * vtkOBJImporter
 */

#ifndef vtkMetashapeOBJReader_h
#define vtkMetashapeOBJReader_h

#include "vtkAbstractPolyDataReader.h"
#include "vtkMAFConfigure.h" // For export macro
#include "vtkResourceStream.h"   // For vtkResourceStream

VTK_ABI_NAMESPACE_BEGIN
class VTK_vtkMAF_EXPORT vtkMetashapeOBJReader : public vtkAbstractPolyDataReader
{
public:
  static vtkMetashapeOBJReader* New();
  vtkTypeMacro(vtkMetashapeOBJReader, vtkAbstractPolyDataReader);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Get first comment in the file.
   * Comment may be multiple lines. # and leading spaces are removed.
   */
  vtkGetStringMacro(Comment);

  ///@{
  /**
   * Specify stream to read from
   * When selecting input method, `Stream` has an higher priority than `Filename`.
   * If both are null, reader outputs nothing.
   */
  vtkSetSmartPointerMacro(Stream, vtkResourceStream);
  vtkGetSmartPointerMacro(Stream, vtkResourceStream);
  ///@}

protected:
  vtkMetashapeOBJReader();
  ~vtkMetashapeOBJReader() override;

  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

  /**
   * Set comment string. Internal use only.
   */
  vtkSetStringMacro(Comment);

  char* Comment;
  vtkSmartPointer<vtkResourceStream> Stream;

private:
  vtkSmartPointer<vtkResourceStream> Open();

  vtkMetashapeOBJReader(const vtkMetashapeOBJReader&) = delete;
  void operator=(const vtkMetashapeOBJReader&) = delete;
};

VTK_ABI_NAMESPACE_END
#endif
