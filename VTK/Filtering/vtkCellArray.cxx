/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkCellArray.cxx,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkCellArray.h"
#include "vtkObjectFactory.h"

vtkCxxRevisionMacro(vtkCellArray, "$Revision: 1.2 $");
vtkStandardNewMacro(vtkCellArray);

//----------------------------------------------------------------------------
vtkCellArray::vtkCellArray()
{
  this->Ia = vtkIdTypeArray::New();
  this->NumberOfCells = 0;
  this->InsertLocation = 0;
  this->TraversalLocation = 0;
}

//----------------------------------------------------------------------------
void vtkCellArray::DeepCopy (vtkCellArray *ca)
{
  // Do nothing on a NULL input.
  if (ca == NULL)
    {
    return;
    }

  this->Ia->DeepCopy(ca->Ia);
  this->NumberOfCells = ca->NumberOfCells;
  this->InsertLocation = ca->InsertLocation;
  this->TraversalLocation = ca->TraversalLocation;
}

//----------------------------------------------------------------------------
vtkCellArray::~vtkCellArray()
{
  this->Ia->Delete();
}


//----------------------------------------------------------------------------
// Returns the size of the largest cell. The size is the number of points
// defining the cell.
int vtkCellArray::GetMaxCellSize()
{
  int i, npts=0, maxSize=0;

  for (i=0; i<this->Ia->GetMaxId(); i+=(npts+1))
    {
    if ( (npts=this->Ia->GetValue(i)) > maxSize )
      {
      maxSize = npts;
      }
    }
  return maxSize;
}

//----------------------------------------------------------------------------
// Specify a group of cells.
void vtkCellArray::SetCells(vtkIdType ncells, vtkIdTypeArray *cells)
{
  if ( cells && cells != this->Ia )
    {
    this->Modified();
    this->Ia->Delete();
    this->Ia = cells;
    this->Ia->Register(this);

    this->NumberOfCells = ncells;
    this->InsertLocation = cells->GetMaxId() + 1;
    this->TraversalLocation = 0;
    }
}

//----------------------------------------------------------------------------
unsigned long vtkCellArray::GetActualMemorySize()
{
  return this->Ia->GetActualMemorySize();
}

//----------------------------------------------------------------------------
void vtkCellArray::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Number Of Cells: " << this->NumberOfCells << endl;
  os << indent << "Insert Location: " << this->InsertLocation << endl;
  os << indent << "Traversal Location: " << this->TraversalLocation << endl;
}
