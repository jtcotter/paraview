/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkHierarchicalDataExtractDataSets.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkHierarchicalDataExtractDataSets - extract a number of datasets
// .SECTION Description
// Legacy class. Use vtkMultiGroupDataExtractDataSets instead.
//
// .SECTION See Also
// vtkMultiGroupDataExtractDataSets

#ifndef __vtkHierarchicalDataExtractDataSets_h
#define __vtkHierarchicalDataExtractDataSets_h

#include "vtkMultiGroupDataExtractDataSets.h"

//BTX
struct vtkHierarchicalDataExtractDataSetsInternals;
//ETX

class VTK_GRAPHICS_EXPORT vtkHierarchicalDataExtractDataSets : public vtkMultiGroupDataExtractDataSets 
{
public:
  vtkTypeRevisionMacro(vtkHierarchicalDataExtractDataSets,vtkMultiGroupDataExtractDataSets);
  void PrintSelf(ostream& os, vtkIndent indent);
  static vtkHierarchicalDataExtractDataSets *New();

protected:
  vtkHierarchicalDataExtractDataSets();
  ~vtkHierarchicalDataExtractDataSets();

private:
  vtkHierarchicalDataExtractDataSets(const vtkHierarchicalDataExtractDataSets&);  // Not implemented.
  void operator=(const vtkHierarchicalDataExtractDataSets&);  // Not implemented.
};

#endif

