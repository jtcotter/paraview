/*=========================================================================

  Program:   ParaView
  Module:    $RCSfile: vtkSMPropertyIterator.cxx,v $

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkSMPropertyIterator.h"

#include "vtkObjectFactory.h"
#include "vtkSMCompoundProxy.h"
#include "vtkSMProxy.h"
#include "vtkSMProxyInternals.h"

vtkStandardNewMacro(vtkSMPropertyIterator);
vtkCxxRevisionMacro(vtkSMPropertyIterator, "$Revision: 1.8 $");

struct vtkSMPropertyIteratorInternals
{
  vtkSMProxyInternals::PropertyInfoMap::iterator PropertyIterator;
  vtkSMProxyInternals::ExposedPropertyInfoMap::iterator 
    ExposedPropertyIterator;
};

//---------------------------------------------------------------------------
vtkSMPropertyIterator::vtkSMPropertyIterator()
{
  this->Proxy = 0;
  this->Internals = new vtkSMPropertyIteratorInternals;
  this->TraverseSubProxies = 1;
}

//---------------------------------------------------------------------------
vtkSMPropertyIterator::~vtkSMPropertyIterator()
{
  this->SetProxy(0);
  delete this->Internals;
}

//---------------------------------------------------------------------------
void vtkSMPropertyIterator::SetProxy(vtkSMProxy* proxy)
{
  if (proxy && proxy->IsA("vtkSMCompoundProxy"))
    {
    vtkSMProxy* mainProxy = 
      static_cast<vtkSMCompoundProxy*>(proxy)->GetMainProxy();
    if (mainProxy)
      {
      proxy = mainProxy;
      }
    }

  if (this->Proxy != proxy)
    {
    if (this->Proxy != NULL) { this->Proxy->UnRegister(this); }
    this->Proxy = proxy;
    if (this->Proxy != NULL) 
      { 
      this->Proxy->Register(this); 
      this->Begin();
      }
    this->Modified();
    }
}

//---------------------------------------------------------------------------
void vtkSMPropertyIterator::Begin()
{
  if (!this->Proxy)
    {
    vtkErrorMacro("Proxy is not set. Can not perform operation: Begin()");
    return;
    }

  this->Internals->PropertyIterator = 
    this->Proxy->Internals->Properties.begin(); 

  this->Internals->ExposedPropertyIterator =
    this->Proxy->Internals->ExposedProperties.begin();
}

//---------------------------------------------------------------------------
int vtkSMPropertyIterator::IsAtEnd()
{
  if (!this->Proxy)
    {
    vtkErrorMacro("Proxy is not set. Can not perform operation: IsAtEnd()");
    return 1;
    }
  if (this->TraverseSubProxies)
    {
    if ( this->Internals->PropertyIterator == this->Proxy->Internals->Properties.end()
      && this->Internals->ExposedPropertyIterator == 
      this->Proxy->Internals->ExposedProperties.end())
      {
      return 1;
      }
    }
  else
    {
    if ( this->Internals->PropertyIterator == this->Proxy->Internals->Properties.end() ) 
      {
      return 1;
      }
    }
  return 0;
}

//---------------------------------------------------------------------------
void vtkSMPropertyIterator::Next()
{
  if (!this->Proxy)
    {
    vtkErrorMacro("Proxy is not set. Can not perform operation: Next()");
    return;
    }

  // If we are still in the root proxy, move to the next element.
  if (this->Internals->PropertyIterator != 
    this->Proxy->Internals->Properties.end())
    {
    this->Internals->PropertyIterator++;
    return;
    // Consider the end case when the this->Internals->PropertyIterator 
    // is pointing to the last element before the above iterator increment.
    // Then the iterator is now pointing off the last element (after the
    // this->Internals->PropertyIterator++). But that's still okay,
    // since this->Internals->ExposedPropertyIterator is already
    // initialized to point to the first exposed property 
    // and the hence the next GetKey()/GetProperty() call
    // will correctly return the first exposed property.
    }

  if (!this->TraverseSubProxies)
    {
    return;
    }
  
  if (this->Internals->ExposedPropertyIterator !=
    this->Proxy->Internals->ExposedProperties.end())
    {
    this->Internals->ExposedPropertyIterator++;
    }
}

//---------------------------------------------------------------------------
const char* vtkSMPropertyIterator::GetKey()
{
  if (!this->Proxy)
    {
    vtkErrorMacro("Proxy is not set. Can not perform operation: GetKey()");
    return 0;
    }

  if (this->Internals->PropertyIterator != 
      this->Proxy->Internals->Properties.end())
    {
    return this->Internals->PropertyIterator->first.c_str();
    }

  if (this->TraverseSubProxies)
    {
    if (this->Internals->ExposedPropertyIterator !=
      this->Proxy->Internals->ExposedProperties.end())
      {
      // return the exposed name.
      return this->Internals->ExposedPropertyIterator->first.c_str();
      }
    }

  return 0;
}

//---------------------------------------------------------------------------
vtkSMProperty* vtkSMPropertyIterator::GetProperty()
{
  if (!this->Proxy)
    {
    vtkErrorMacro("Proxy is not set. Can not perform operation: GetProperty()");
    return 0;
    }
  if (this->Internals->PropertyIterator != 
      this->Proxy->Internals->Properties.end())
    {
    return this->Internals->PropertyIterator->second.Property.GetPointer();
    }

  if (this->TraverseSubProxies)
    {
    if (this->Internals->ExposedPropertyIterator !=
      this->Proxy->Internals->ExposedProperties.end())
      {
      vtkSMProxy* proxy = this->Proxy->GetSubProxy(
        this->Internals->ExposedPropertyIterator->second.SubProxyName.c_str());
      if (!proxy)
        {
        return 0;
        }
      return proxy->GetProperty(
        this->Internals->ExposedPropertyIterator->second.PropertyName.c_str());
      }
    }
  return 0;
}

//---------------------------------------------------------------------------
void vtkSMPropertyIterator::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "TraverseSubProxies: " << this->TraverseSubProxies << endl;
  os << indent << "Proxy: " << this->Proxy << endl;
}
