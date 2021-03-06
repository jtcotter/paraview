/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkTemporalDataSet.cxx,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkTemporalDataSet.h"

#include "vtkDataSet.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkInformationIntegerKey.h"

#include "vtkObjectFactory.h"

vtkCxxRevisionMacro(vtkTemporalDataSet, "$Revision: 1.2 $");
vtkStandardNewMacro(vtkTemporalDataSet);

//----------------------------------------------------------------------------
vtkTemporalDataSet::vtkTemporalDataSet()
{
  this->Information->Set(vtkDataObject::DATA_EXTENT_TYPE(), 
                         VTK_TIME_EXTENT);
}

//----------------------------------------------------------------------------
vtkTemporalDataSet::~vtkTemporalDataSet()
{
}

//----------------------------------------------------------------------------
vtkTemporalDataSet* vtkTemporalDataSet::GetData(vtkInformation* info)
{
  return
    info? vtkTemporalDataSet::SafeDownCast(info->Get(DATA_OBJECT())) : 0;
}

//----------------------------------------------------------------------------
vtkTemporalDataSet*
vtkTemporalDataSet::GetData(vtkInformationVector* v, int i)
{
  return vtkTemporalDataSet::GetData(v->GetInformationObject(i));
}

//----------------------------------------------------------------------------
void vtkTemporalDataSet::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

