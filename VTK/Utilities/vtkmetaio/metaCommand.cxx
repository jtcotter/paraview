/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: metaCommand.cxx,v $
  Language:  C++
  Date:      $Date: 2007/05/27 14:04:13 $
  Version:   $Revision: 1.12 $

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifdef _MSC_VER
#pragma warning(disable:4702)
#endif

#include "metaCommand.h"

#include <stdio.h>
#include <string>

#if (METAIO_USE_NAMESPACE)
namespace METAIO_NAMESPACE {
#endif

MetaCommand::
MetaCommand()
{
  m_HelpCallBack = NULL;
  m_OptionVector.clear();
  m_Version = "Not defined";
  m_Date = "Not defined";
  m_Name = "";
  m_Author = "Not defined";
  m_Description = "";
  m_ParsedOptionVector.clear();
  m_Verbose = true;
  m_FailOnUnrecognizedOption = false;
}


/** Extract the date from the $Date: 2007/05/27 14:04:13 $ cvs command */
METAIO_STL::string MetaCommand::
ExtractDateFromCVS(METAIO_STL::string date)
{
  METAIO_STL::string newdate;
  for(int i=7;i<(int)date.size()-1;i++)
    {
    newdate += date[i];
    }
  return newdate.c_str();
}

void MetaCommand::
SetDateFromCVS(METAIO_STL::string cvsDate)
  {
  this->SetDate( this->ExtractDateFromCVS( cvsDate ).c_str() );
  }

/** Extract the version from the $Revision: 1.12 $ cvs command */
METAIO_STL::string MetaCommand::
ExtractVersionFromCVS(METAIO_STL::string version)
{
  METAIO_STL::string newversion;
  for(int i=11;i<(int)version.size()-1;i++)
    {
    newversion += version[i];
    }
  return newversion.c_str();
}

void MetaCommand::
SetVersionFromCVS(METAIO_STL::string cvsVersion)
  {
  this->SetVersion( this->ExtractVersionFromCVS( cvsVersion ).c_str() );
  }


/** */
bool MetaCommand::
SetOption(Option option)
{
  // need to add some tests here to check if the option is not defined yet
  m_OptionVector.push_back(option);
  return true;
}

bool MetaCommand::
SetOption(METAIO_STL::string name,
                            METAIO_STL::string tag,
                            bool required,
                            METAIO_STL::string description,
                            METAIO_STL::vector<Field> fields)
{
  // need to add some tests here to check if the option is not defined yet
  if(tag == "")
    {
    METAIO_STREAM::cout << "Tag cannot be empty : use AddField() instead." 
                        << METAIO_STREAM::endl;
    return false;
    }

  Option option;
  option.name = name;
  option.tag = tag;
  option.fields = fields;
  option.required = required;
  option.description = description;
  option.userDefined = false;
  option.complete = false;

  m_OptionVector.push_back(option);
  return true;
}


bool MetaCommand::
SetOption(METAIO_STL::string name,
          METAIO_STL::string tag,
          bool required,
          METAIO_STL::string description,
          TypeEnumType type,
          METAIO_STL::string defVal,
          DataEnumType externalData)
{
  // need to add some tests here to check if the option is not defined yet
  if(tag == "")
    {
    METAIO_STREAM::cout << "Tag cannot be empty : use AddField() instead." 
                        << METAIO_STREAM::endl;
    return false;
    }

  Option option;
  option.tag = tag;
  option.name = name;
  option.required = required;
  option.description = description;
  option.userDefined = false;
  option.complete = false;

  // Create a field without description as a flag
  Field field;
  if(type == LIST)
    {
    field.name = "NumberOfValues";
    }
  else
    {
    field.name = name;
    }
  field.externaldata = externalData;
  field.type = type;
  field.value = defVal;
  field.userDefined = false;
  field.required = true;
  field.rangeMin = "";
  field.rangeMax = "";
  option.fields.push_back(field);

  m_OptionVector.push_back(option);
  return true;
}


/** Add a field */
bool MetaCommand::
AddField(METAIO_STL::string name,
         METAIO_STL::string description,
         TypeEnumType type,
         DataEnumType externalData,
         METAIO_STL::string rangeMin,
         METAIO_STL::string rangeMax)
{
  // need to add some tests here to check if the option is not defined yet
  Option option;
  option.tag = "";

  // Create a field without description with the specified type
  Field field;
  field.name = name;
  field.type = type;
  field.required = true;
  field.userDefined = false;
  field.externaldata = externalData;
  field.rangeMin = rangeMin;
  field.rangeMax = rangeMax;
  option.fields.push_back(field);

  option.required = true;
  option.name = name;
  option.description = description;
  option.userDefined = false;
  option.complete = false;

  m_OptionVector.push_back(option);
  return true;
}

/** For backward compatibility */
bool MetaCommand:: 
AddField(METAIO_STL::string name,
         METAIO_STL::string description,
         TypeEnumType type,
         bool externalData)
{
  if(externalData)
    {
    return this->AddField(name,description,type,DATA_IN);
    }
  else
    {
    return this->AddField(name,description,type,DATA_NONE);
    }
}

/** Collect all the information until the next tag 
  * \warning this function works only if the field is of type String */ 
void MetaCommand::
SetOptionComplete(METAIO_STL::string optionName,
                  bool complete)
{
  OptionVector::iterator it = m_OptionVector.begin();
  while(it != m_OptionVector.end())
    {
    if((*it).name == optionName)
      {
      (*it).complete = complete;
      return;
      }
    it++;
    }
 }

/** Add a field to a given an option */
bool MetaCommand::
AddOptionField(METAIO_STL::string optionName,
               METAIO_STL::string name,
               TypeEnumType type,
               bool required,
               METAIO_STL::string defVal,
               METAIO_STL::string description,
               DataEnumType externalData)
{ 
  OptionVector::iterator it = m_OptionVector.begin();
  while(it != m_OptionVector.end())
    {
    if((*it).name == optionName)
      {
      // Create a field without description with the specified type
      Field field;
      field.name = name;
      field.type = type;
      field.required = required;
      field.value = defVal;
      field.description = description;
      field.userDefined = false;
      field.externaldata = externalData;
      field.rangeMin = "";
      field.rangeMax = "";
    
      // If this is the first field in the list we replace the current field
      if((*it).fields[0].type == FLAG)
        {
        (*it).fields[0] = field;
        }
      else
        {
        (*it).fields.push_back(field);
        }
      return true;
      }
    it++;
    }
  return false;
}

/** Set the range of an option */
bool MetaCommand::
SetOptionRange(METAIO_STL::string optionName,
               METAIO_STL::string name,
               METAIO_STL::string rangeMin,
               METAIO_STL::string rangeMax)
{
  OptionVector::iterator it = m_OptionVector.begin();
  while(it != m_OptionVector.end())
    {
    if((*it).name == optionName)
      {
      METAIO_STL::vector<Field> & fields = (*it).fields;
      METAIO_STL::vector<Field>::iterator itField = fields.begin();
      while(itField != fields.end())
        {
        if((*itField).name == name)
          {
          (*itField).rangeMin = rangeMin;
          (*itField).rangeMax = rangeMax;
          return true;
          }
        itField++;
        }
      }
    it++;
    }
  return false;
}


/** Return the value of the option as a boolean */
bool MetaCommand::
GetValueAsBool(METAIO_STL::string optionName,
               METAIO_STL::string fieldName)
{
  METAIO_STL::string fieldname = fieldName;
  if(fieldName == "")
    {
    fieldname = optionName;
    }
 
  OptionVector::const_iterator it = m_OptionVector.begin();
  while(it != m_OptionVector.end())
    {
    if((*it).name == optionName)
      {
      METAIO_STL::vector<Field>::const_iterator itField = (*it).fields.begin();
      while(itField != (*it).fields.end())
        {
        if((*itField).name == fieldname)
          {
          if((*itField).value == "true"
            || (*itField).value == "1"
            || (*itField).value == "True"
            || (*itField).value == "TRUE"
            )
            {
            return true;
            }
          return false;
          }
        itField++;
        }
      }
    it++;
    }
  return false;
}


/** Return the value of the option as a bool */
bool MetaCommand::
GetValueAsBool(Option option,
               METAIO_STL::string fieldName)
{
  METAIO_STL::string fieldname = fieldName;
  if(fieldName == "")
    {
    fieldname = option.name;
    }

  METAIO_STL::vector<Field>::const_iterator itField = option.fields.begin();
  while(itField != option.fields.end())
    {
    if((*itField).name == fieldname)
      {
      if((*itField).value == "true"
         || (*itField).value == "1"
         || (*itField).value == "True"
         || (*itField).value == "TRUE"
        )
        {
        return true;
        }
      return false;
      }
    itField++;
    }
  return 0;
}

/** Return the value of the option as a float */
float MetaCommand::
GetValueAsFloat(METAIO_STL::string optionName,
                METAIO_STL::string fieldName)
{
  METAIO_STL::string fieldname = fieldName;
  if(fieldName == "")
    {
    fieldname = optionName;
    }
 
  OptionVector::const_iterator it = m_OptionVector.begin();
  while(it != m_OptionVector.end())
    {
    if((*it).name == optionName)
      {
      METAIO_STL::vector<Field>::const_iterator itField = (*it).fields.begin();
      while(itField != (*it).fields.end())
        {
        if((*itField).name == fieldname)
          {
          return (float)atof((*itField).value.c_str());
          }
        itField++;
        }
      }
    it++;
    }
  return 0;
}

/** Return the value of the option as a float */
float MetaCommand::
GetValueAsFloat(Option option,
                METAIO_STL::string fieldName)
{
  METAIO_STL::string fieldname = fieldName;
  if(fieldName == "")
    {
    fieldname = option.name;
    }

  METAIO_STL::vector<Field>::const_iterator itField = option.fields.begin();
  while(itField != option.fields.end())
    {
    if((*itField).name == fieldname)
      {
      return (float)atof((*itField).value.c_str());
      }
    itField++;
    }
  return 0;
}

/** Return the value of the option as a int */
int MetaCommand::
GetValueAsInt(METAIO_STL::string optionName,
              METAIO_STL::string fieldName)
{
  METAIO_STL::string fieldname = fieldName;
  if(fieldName == "")
    {
    fieldname = optionName;
    }
 
  OptionVector::const_iterator it = m_OptionVector.begin();
  while(it != m_OptionVector.end())
    {
    if((*it).name == optionName)
      {
      METAIO_STL::vector<Field>::const_iterator itField = (*it).fields.begin();
      while(itField != (*it).fields.end())
        {
        if((*itField).name == fieldname)
          {
          return atoi((*itField).value.c_str());
          }
        itField++;
        }
      }
    it++;
    }
  return 0;
}

/** Return the value of the option as a int */
int MetaCommand::
GetValueAsInt(Option option,
              METAIO_STL::string fieldName)
{
  METAIO_STL::string fieldname = fieldName;
  if(fieldName == "")
    {
    fieldname = option.name;
    }

  METAIO_STL::vector<Field>::const_iterator itField = option.fields.begin();
  while(itField != option.fields.end())
    {
    if((*itField).name == fieldname)
      {
      return atoi((*itField).value.c_str());
      }
    itField++;
    }
  return 0;
}

/** Return the value of the option as a string */
METAIO_STL::string MetaCommand::
GetValueAsString(METAIO_STL::string optionName,
                 METAIO_STL::string fieldName)
{
  METAIO_STL::string fieldname = fieldName;
  if(fieldName == "")
    {
    fieldname = optionName;
    }
 
  OptionVector::const_iterator it = m_OptionVector.begin();
  while(it != m_OptionVector.end())
    {
    if((*it).name == optionName)
      {
      METAIO_STL::vector<Field>::const_iterator itField = (*it).fields.begin();
      while(itField != (*it).fields.end())
        {
        if((*itField).name == fieldname)
          {
          return (*itField).value;
          }
        itField++;
        }
      }
    it++;
    }
  return "";
}

/** Return the value of the option as a string */
METAIO_STL::string MetaCommand::
GetValueAsString(Option option,
                 METAIO_STL::string fieldName)
{
  METAIO_STL::string fieldname = fieldName;
  if(fieldName == "")
    {
    fieldname = option.name;
    }

  METAIO_STL::vector<Field>::const_iterator itField = option.fields.begin();
  while(itField != option.fields.end())
    {
    if((*itField).name == fieldname)
      {
      return (*itField).value;
      }
    itField++;
    }
  return "";
}

/** Return the value of the option as a list of strings */
METAIO_STL::list<METAIO_STL::string> MetaCommand::
GetValueAsList( Option option )
{
  METAIO_STL::list<METAIO_STL::string> results;
  results.clear();
  METAIO_STL::vector<Field>::const_iterator itField = option.fields.begin();
  itField++;
  while(itField != option.fields.end())
    {
    results.push_back((*itField).value);
    itField++;
    }
  return results;
}

METAIO_STL::list< METAIO_STL::string > MetaCommand::
GetValueAsList( METAIO_STL::string optionName )
{
  OptionVector::const_iterator it = m_OptionVector.begin();
  while(it != m_OptionVector.end())
    {
    if((*it).name == optionName)
      {
      return this->GetValueAsList( *it );
      }
    it++;
    }
  METAIO_STL::list< METAIO_STL::string > empty;
  empty.clear();
  return empty;
}

bool MetaCommand::
GetOptionWasSet(Option option)
{
  if(option.userDefined)
    {
    return true;
    }
  return false;
}

bool MetaCommand::
GetOptionWasSet( METAIO_STL::string optionName)
{
  OptionVector::const_iterator it = m_ParsedOptionVector.begin();
  while(it != m_ParsedOptionVector.end())
    {
    if((*it).name == optionName)
      {
      return true;
      }
    it++;
    }
  return false;
}

/** List the current options */
void MetaCommand::
ListOptions()
{
  OptionVector::const_iterator it = m_OptionVector.begin();
  int i=0;
  while(it != m_OptionVector.end())
    {
    METAIO_STREAM::cout << "Option #" << i << METAIO_STREAM::endl;
    METAIO_STREAM::cout << "   Name: " <<  (*it).name.c_str() 
                        << METAIO_STREAM::endl;
    if((*it).tag.size() > 0)
      {
      METAIO_STREAM::cout << "   Tag: " << (*it).tag.c_str() 
                          << METAIO_STREAM::endl;
      }
    METAIO_STREAM::cout << "   Description: " << (*it).description.c_str() 
                        << METAIO_STREAM::endl;
    if((*it).required)
      {
      METAIO_STREAM::cout << "   Required: true" << METAIO_STREAM::endl;
      }
    else
      {
      METAIO_STREAM::cout << "   Required: false" << METAIO_STREAM::endl;
      }
    METAIO_STREAM::cout << "   Number of expeted values: " 
                        << (*it).fields.size() 
                        << METAIO_STREAM::endl;
    
    METAIO_STL::vector<Field>::const_iterator itField = (*it).fields.begin();
    while(itField != (*it).fields.end())
      {
      METAIO_STREAM::cout << "      Field Name: " <<  (*itField).name.c_str() 
                          << METAIO_STREAM::endl;
      METAIO_STREAM::cout << "      Description: " 
                          << (*itField).description.c_str() 
                          << METAIO_STREAM::endl;
      METAIO_STREAM::cout << "      Type: " 
                          << this->TypeToString((*itField).type).c_str()
                          << METAIO_STREAM::endl;
      METAIO_STREAM::cout << "      Value: " << (*itField).value.c_str() 
                          << METAIO_STREAM::endl;
      
      if((*itField).externaldata)
        {
        METAIO_STREAM::cout << "      External Data: true" 
                            << METAIO_STREAM::endl;
        }
      else
        {
        METAIO_STREAM::cout << "      External Data: false" 
                            << METAIO_STREAM::endl;
        }

      if((*itField).required)
        {
        METAIO_STREAM::cout << "      Required: true" << METAIO_STREAM::endl;
        }
      else
        {
        METAIO_STREAM::cout << "      Required: false" << METAIO_STREAM::endl;
        }
      itField++;
      }
    METAIO_STREAM::cout << METAIO_STREAM::endl;
    i++;
    it++;
    }
  if(m_HelpCallBack != NULL)
    {
    m_HelpCallBack();
    }
}

/** List the current options in xml format */
void MetaCommand::
ListOptionsXML()
{
  OptionVector::const_iterator it = m_OptionVector.begin();
  int i=0;
  while(it != m_OptionVector.end())
    {
    METAIO_STREAM::cout << "<option>" << METAIO_STREAM::endl;
    METAIO_STREAM::cout << "<number>" << i << "</number>" 
                        << METAIO_STREAM::endl;
    METAIO_STREAM::cout << "<name>" << (*it).name.c_str() << "</name>" 
                        << METAIO_STREAM::endl;
    METAIO_STREAM::cout << "<tag>" << (*it).tag.c_str() << "</tag>" 
                        << METAIO_STREAM::endl;
    METAIO_STREAM::cout << "<description>" << (*it).description.c_str() 
                        << "</description>" << METAIO_STREAM::endl;
    METAIO_STREAM::cout << "<required>";
    if((*it).required)
      {
      METAIO_STREAM::cout << "1</required>" << METAIO_STREAM::endl;
      }
    else
      {
      METAIO_STREAM::cout << "0</required>" << METAIO_STREAM::endl;
      }

    METAIO_STREAM::cout << "<nvalues>" << (*it).fields.size() << "</nvalues>" 
                        << METAIO_STREAM::endl;
    
    METAIO_STL::vector<Field>::const_iterator itField = (*it).fields.begin();
    while(itField != (*it).fields.end())
      {
      METAIO_STREAM::cout << "<field>" << METAIO_STREAM::endl;
      METAIO_STREAM::cout << "<name>" << (*itField).name.c_str() << "</name>" 
                          << METAIO_STREAM::endl;
      METAIO_STREAM::cout << "<description>" << (*itField).description.c_str() 
                          << "</description>" << METAIO_STREAM::endl;
      METAIO_STREAM::cout << "<type>" 
                          << this->TypeToString((*itField).type).c_str() 
                          << "</type>" << METAIO_STREAM::endl;
      METAIO_STREAM::cout << "<value>" << (*itField).value.c_str() 
                          << "</value>" << METAIO_STREAM::endl; 
      METAIO_STREAM::cout << "<external>";
      if((*itField).externaldata == DATA_IN)
        {
        METAIO_STREAM::cout << "1</external>" << METAIO_STREAM::endl;
        }
      else if((*itField).externaldata == DATA_OUT)
        {
        METAIO_STREAM::cout << "2</external>" << METAIO_STREAM::endl;
        }
      else
        {
        METAIO_STREAM::cout << "0</external>" << METAIO_STREAM::endl;
        }
      METAIO_STREAM::cout << "<required>";
      if((*itField).required)
        {
        METAIO_STREAM::cout << "1</required>" << METAIO_STREAM::endl;
        }
      else
        {
        METAIO_STREAM::cout << "0</required>" << METAIO_STREAM::endl;
        }


      METAIO_STREAM::cout << "</field>" << METAIO_STREAM::endl;
      itField++;
      }
    METAIO_STREAM::cout << "</option>" << METAIO_STREAM::endl;
    i++;
    it++;
    }
}

/** Internal small XML parser */
METAIO_STL::string MetaCommand::
GetXML(const char* buffer, const char* desc, unsigned long pos)
{
  METAIO_STL::string begin = "<";
  begin += desc;
  begin += ">";
  METAIO_STL::string end = "</";
  end += desc;
  end += ">";

  METAIO_STL::string buf = buffer;

  long int posb = buf.find(begin,pos);
  if(posb == -1)
    {
    return "";
    }
  long int pose = buf.find(end,posb);
  if(pose == -1)
    {
    return "";
    }

  return buf.substr(posb+begin.size(),pose-posb-begin.size());
}

/** Given an XML buffer fill in the command line arguments */
bool MetaCommand::
ParseXML(const char* buffer)
{
  m_OptionVector.clear();
  METAIO_STL::string buf = this->GetXML(buffer,"option",0);
  long pos = 0;
  while(buf.size() > 0)
    {
    Option option;
    option.name = this->GetXML(buf.c_str(),"name",0);
    option.tag = this->GetXML(buf.c_str(),"tag",0);
    option.description = this->GetXML(buf.c_str(),"description",0);
    if(atoi(this->GetXML(buf.c_str(),"required",0).c_str()) == 0)
      {
      option.required = false;
      }
    else
      {
      option.required = true;
      }
    unsigned int n = atoi(this->GetXML(buf.c_str(),"nvalues",0).c_str());

    // Now check the fields
    long posF = buf.find("<field>");
    for(unsigned int i=0;i<n;i++)
      {
      METAIO_STL::string f = this->GetXML(buf.c_str(),"field",posF);
      Field field;
      field.userDefined = false;
      field.name = this->GetXML(f.c_str(),"name",0);
      field.description = this->GetXML(f.c_str(),"description",0);
      field.value = this->GetXML(f.c_str(),"value",0);
      field.type = this->StringToType(this->GetXML(f.c_str(),"type",0).c_str());
      if(atoi(this->GetXML(f.c_str(),"external",0).c_str()) == 0)
        {
        field.externaldata = DATA_NONE;
        }
      else
        {
        if(atoi(this->GetXML(f.c_str(),"external",0).c_str()) == 1)
          {
          field.externaldata = DATA_IN;
          }
        else
          {
          field.externaldata = DATA_OUT;
          }
        }
      if(atoi(this->GetXML(f.c_str(),"required",0).c_str()) == 0)
        {
        field.required = false;
        }
      else
        {
        field.required = true;
        }

      option.fields.push_back(field);
      posF += f.size()+8;
      }

    m_OptionVector.push_back(option);

    pos += buf.size()+17;
    buf = this->GetXML(buffer,"option",pos);
    }

  return true;
}


/** List the current options */
void MetaCommand::
ListOptionsSimplified()
{
  METAIO_STREAM::cout << " System tags: " << METAIO_STREAM::endl
            << "   [ -v ] or [ -h ]" 
            << METAIO_STREAM::endl
            << "      = List options in short format" 
            << METAIO_STREAM::endl
            << "   [ -V ] or [ -H ]" 
            << METAIO_STREAM::endl
            << "      = List options in long format" 
            << METAIO_STREAM::endl
            << "   [ -vxml ] or [ -hxml ] or [ -exportXML ]" 
            << METAIO_STREAM::endl
            << "      = List options in xml format for BatchMake" 
            << METAIO_STREAM::endl
            << "   [ -vgad ] or [ -hgad ] or [ -exportGAD ]" 
            << METAIO_STREAM::endl
            << "      = List options in Grid Application Description format" 
            << METAIO_STREAM::endl
            << "   [ -version ]" 
            << METAIO_STREAM::endl
            << "      = return the version number" 
            << METAIO_STREAM::endl
            << "   [ -date ]" 
            << METAIO_STREAM::endl
            << "      = return the cvs checkout date" 
            << METAIO_STREAM::endl;
  int count = 0;
  int ntags = 0;
  int nfields = 0;
  OptionVector::const_iterator it;
  it = m_OptionVector.begin();
  while(it != m_OptionVector.end())
    {
    if((*it).tag.size() > 0)
      {
      ntags++;
      }
    else
      {
      nfields++;
      }
    ++it;
    }
  while(count<2)
    {
    if(count == 0)
      {
      if(ntags > 0)
        {
        METAIO_STREAM::cout << " Command tags: " 
                            << METAIO_STREAM::endl;
        }
      else
        {
        count++;
        }
      }
    if(count == 1)
      {
      if(nfields > 0)
        {
        METAIO_STREAM::cout << " Command fields: " 
                            << METAIO_STREAM::endl;
        }
      else
        {
        count++;
        }
      }
    count++;
    it = m_OptionVector.begin();
    while(it != m_OptionVector.end())
      {
      if((count == 1 && (*it).tag.size() > 0) 
         || (count == 2 && (*it).tag.size() == 0)) 
        {
        if(!(*it).required)
          {
          METAIO_STREAM::cout << "   [ ";
          }
        else
          {
          METAIO_STREAM::cout << "   ";
          }
        if((*it).tag.size() > 0)
          {
          METAIO_STREAM::cout << "-" << (*it).tag.c_str() << " ";
          }
        METAIO_STL::vector<Field>::const_iterator itField =
                                                  (*it).fields.begin();
        while(itField != (*it).fields.end())
          {
          // only display the type if it's not a FLAG
          if((*itField).type != FLAG) 
            {
            if((*itField).required)
              {
              METAIO_STREAM::cout << "< ";
              }
            else
              {
              METAIO_STREAM::cout << "[ ";
              }
    
            METAIO_STREAM::cout << (*itField).name.c_str();
         
            if((*itField).required)
              {
              METAIO_STREAM::cout << " > ";
              }
            else
              {
              METAIO_STREAM::cout << " ] ";
              }
            }
          itField++;
          }
      
        if(!(*it).required)
          {
          METAIO_STREAM::cout << "]";
          }
        METAIO_STREAM::cout << METAIO_STREAM::endl;
    
        if((*it).description.size()>0)
          {
          METAIO_STREAM::cout << "      = " << (*it).description.c_str();
          METAIO_STREAM::cout << METAIO_STREAM::endl;
          itField = (*it).fields.begin();
          while(itField != (*it).fields.end())
            {
            if((*itField).description.size() > 0
               || (*itField).value.size() > 0)
              {
              METAIO_STREAM::cout << "        With: " 
                                  << (*itField).name.c_str();
              if((*itField).description.size() > 0)
                {
                METAIO_STREAM::cout << " = " << (*itField).description.c_str();
                }
              if((*itField).value.size() > 0)
                {
                METAIO_STREAM::cout << " (Default = " 
                                    << (*itField).value.c_str() << ")";
                }
              METAIO_STREAM::cout << METAIO_STREAM::endl;
              }
            itField++;
            }
          }
        }
      it++;
      }
    }
  
  if(m_HelpCallBack != NULL)
    {
    m_HelpCallBack();
    }
}

/** Get the option by "-"+tag */
bool MetaCommand::
OptionExistsByMinusTag(METAIO_STL::string minusTag)
{
  OptionVector::const_iterator it = m_OptionVector.begin();
  while(it != m_OptionVector.end())
    { 
    METAIO_STL::string tagToSearch = "-";
    tagToSearch += (*it).tag;
    if(tagToSearch == minusTag)
      {
      return true;
      }
    it++;
    }

  return false;
  
}


/** Get the option by "-"+tag */
MetaCommand::Option * MetaCommand::
GetOptionByMinusTag(METAIO_STL::string minusTag)
{
  OptionVector::iterator it = m_OptionVector.begin();
  while(it != m_OptionVector.end())
    { 
    METAIO_STL::string tagToSearch = "-";
    tagToSearch += (*it).tag;
    if(tagToSearch == minusTag)
      {
      return &(*it);
      }
    it++;
    }
  return NULL;
}

/** Get the option by tag */
MetaCommand::Option * MetaCommand::
GetOptionByTag(METAIO_STL::string minusTag)
{
  OptionVector::iterator it = m_OptionVector.begin();
  while(it != m_OptionVector.end())
    {
    if((*it).tag == minusTag)
      {
      return &(*it);
      }
    it++;
    }
  return NULL;
}

/** Return the option id. i.e the position in the vector */
long MetaCommand::
GetOptionId(Option* option)
{
  OptionVector::iterator it = m_OptionVector.begin();
  unsigned long i = 0;
  while(it != m_OptionVector.end())
    {
    if(&(*it) == option)
      {
      return i;
      }
    i++;
    it++;
    }
  return -1;
}

/** Export the current command line arguments to a Grid Application
 *  Description file */
bool MetaCommand::
ExportGAD(bool dynamic)
{
  METAIO_STREAM::cout << "Exporting GAD file...";

  OptionVector options = m_OptionVector;
  if(dynamic)
    {
    options = m_ParsedOptionVector;
    }

  if(m_Name=="")
    {
    METAIO_STREAM::cout << "Set the name of the application using SetName()" 
                        << METAIO_STREAM::endl;
    return false;
    }

  METAIO_STL::string filename = m_Name;
  filename += ".gad.xml";

  METAIO_STREAM::ofstream file;
#ifdef __sgi
  file.open(filename.c_str(), METAIO_STREAM::ios::out);
#else
  file.open(filename.c_str(), METAIO_STREAM::ios::binary 
                              | METAIO_STREAM::ios::out);
#endif
  if(!file.rdbuf()->is_open())
    {
    METAIO_STREAM::cout << "Cannot open file for writing: " 
                        << filename.c_str() <<  METAIO_STREAM::endl;
    return false;
    }
  
  file << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" << METAIO_STREAM::endl;
  file << "<gridApplication" << METAIO_STREAM::endl;
  file << "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"" 
       << METAIO_STREAM::endl;
  file << "xsi:noNamespaceSchemaLocation=\"grid-application-description.xsd\"" 
       << METAIO_STREAM::endl;
  file << "name=\"" << m_Name.c_str() << "\"" << METAIO_STREAM::endl;
  file << "description=\"" << m_Description.c_str() << "\">" 
       << METAIO_STREAM::endl;
  file << "<applicationComponent name=\"Client\" remoteExecution=\"true\">" 
       << METAIO_STREAM::endl;
  file << "<componentActionList>" << METAIO_STREAM::endl;
  file << METAIO_STREAM::endl;

  unsigned int order = 1;
  // Write out the input data to be transfered
  OptionVector::const_iterator it = options.begin();
  while(it != options.end())
    {
    METAIO_STL::vector<Field>::const_iterator itFields = (*it).fields.begin();
    while(itFields != (*it).fields.end())
      {
      if((*itFields).externaldata == DATA_IN)
        {
        file << " <componentAction type=\"DataRelocation\" order=\"" << order 
             << "\">" << METAIO_STREAM::endl;
        file << "  <parameter name=\"Name\" value=\"" 
             << (*itFields).name.c_str()
             <<"\"/>" << METAIO_STREAM::endl;
        file << "  <parameter name=\"Host\" value=\"hostname\"/>" 
             << METAIO_STREAM::endl;
        file << "  <parameter name=\"Description\" value=\"" 
             << (*itFields).description.c_str() << "\"/>" 
             << METAIO_STREAM::endl;
        file << "  <parameter name=\"Direction\" value=\"In\"/>" 
             << METAIO_STREAM::endl;
        file << "  <parameter name=\"Protocol\" value=\"gsiftp\"/>" 
             << METAIO_STREAM::endl;
        file << "  <parameter name=\"SourceDataPath\" value=\"" 
             << (*itFields).value.c_str() << "\"/>" << METAIO_STREAM::endl;

        METAIO_STL::string datapath = (*itFields).value;
        long int slash = datapath.find_last_of("/");
        if(slash>0)
          {
          datapath = datapath.substr(slash+1,datapath.size()-slash-1);
          }
        slash = datapath.find_last_of("\\");
        if(slash>0)
          {
          datapath = datapath.substr(slash+1,datapath.size()-slash-1);
          }
        file << "  <parameter name=\"DestDataPath\" value=\"" 
             << datapath.c_str() << "\"/>" << METAIO_STREAM::endl;
        file << " </componentAction>" << METAIO_STREAM::endl;
        file << METAIO_STREAM::endl;
        order++;
        }
      itFields++;
      }
    it++;
    }

  file << " <componentAction type=\"JobSubmission\" order=\"" << order << "\">" 
       << METAIO_STREAM::endl;
  file << "  <parameter name=\"Executable\" value=\"" 
       << m_ExecutableName.c_str() << "\"/>" << METAIO_STREAM::endl;
  file << "  <parameter name=\"Arguments\"  value=\"";
  // Write out the command line arguments
  it = options.begin();
  while(it != options.end())
    {
    if(it != options.begin())
      {
      file << " ";
      }
    file << "{" << (*it).name.c_str() << "}";
    it++;
    }
  file << "\"/>" << METAIO_STREAM::endl;
  // Write out the arguments that are not data
  it = options.begin();
  while(it != options.end())
    {
    // Find if this is a non data field
    bool isData = false;
    METAIO_STL::vector<Field>::const_iterator itFields = (*it).fields.begin();
    while(itFields != (*it).fields.end())
      {
      if((*itFields).externaldata != DATA_NONE)
        {
        isData = true;
        break;
        }
      itFields++;
      }

    if(isData)
      {
      it++;
      continue;
      }

    file << "   <group name=\"" << (*it).name.c_str();
    file << "\" syntax=\"";
    
    if((*it).tag.size()>0)
      {
      file << "-" << (*it).tag.c_str() << " ";
      }

    itFields = (*it).fields.begin();
    while(itFields != (*it).fields.end())
      {
      if(itFields != (*it).fields.begin())
        {
        file << " ";
        }
      file << "{" << (*it).name.c_str() << (*itFields).name.c_str() << "}";
      itFields++;
      }  
    file << "\"";
    
    if(!(*it).required)
      {
      file << " optional=\"true\"";
      
      // Add if the option was selected
      if((*it).userDefined)
        {
        file << " selected=\"true\"";
        }
      else
        {
        file << " selected=\"false\"";
        }
      }
    
    file << ">" << METAIO_STREAM::endl; 

    // Now writes the value of the arguments 
    itFields = (*it).fields.begin();
    while(itFields != (*it).fields.end())
      {
      file << "    <argument name=\"" << (*it).name.c_str() 
           << (*itFields).name.c_str();
      file << "\" value=\"" << (*itFields).value.c_str();
      file << "\" type=\"" << this->TypeToString((*itFields).type).c_str();
      file << "\"";
      
      if((*itFields).rangeMin != "")
        {
        file << " rangeMin=\"" << (*itFields).rangeMin.c_str() << "\"";
        }

      if((*itFields).rangeMax != "")
        {
        file << " rangeMax=\"" << (*itFields).rangeMax.c_str() << "\"";
        } 
      file << "/>" << METAIO_STREAM::endl;
      itFields++;
      }
    file << "  </group>" << METAIO_STREAM::endl;
    it++;
    }
  file << " </componentAction>" << METAIO_STREAM::endl;
  order++;
  file << METAIO_STREAM::endl;
  // Write out the input data to be transfered
  it = options.begin();
  while(it != options.end())
    {
    METAIO_STL::vector<Field>::const_iterator itFields = (*it).fields.begin();
    while(itFields != (*it).fields.end())
      {
      if((*itFields).externaldata == DATA_OUT)
        {
        file << " <componentAction type=\"DataRelocation\" order=\"" << order 
             << "\">" << METAIO_STREAM::endl;
        file << "  <parameter name=\"Name\" Value=\"" 
             << (*itFields).name.c_str()
             <<"\"/>" << METAIO_STREAM::endl;
        file << "  <parameter name=\"Host\" Value=\"hostname\"/>" 
             << METAIO_STREAM::endl;
        file << "  <parameter name=\"Description\" value=\"" 
             << (*itFields).description.c_str() << "\"/>" 
             << METAIO_STREAM::endl;
        file << "  <parameter name=\"Direction\" value=\"Out\"/>" 
             << METAIO_STREAM::endl;
        file << "  <parameter name=\"Protocol\" value=\"gsiftp\"/>" 
             << METAIO_STREAM::endl;
        METAIO_STL::string datapath = (*itFields).value;
        long int slash = datapath.find_last_of("/");
        if(slash>0)
          {
          datapath = datapath.substr(slash+1,datapath.size()-slash-1);
          }
        slash = datapath.find_last_of("\\");
        if(slash>0)
          {
          datapath = datapath.substr(slash+1,datapath.size()-slash-1);
          }
        file << "  <parameter name=\"SourceDataPath\" value=\"" 
             << datapath.c_str() << "\"/>" << METAIO_STREAM::endl;
        file << "  <parameter name=\"DestDataPath\" value=\"" 
             << (*itFields).value.c_str() << "\"/>" << METAIO_STREAM::endl;
        file << " </componentAction>" << METAIO_STREAM::endl;
        file << METAIO_STREAM::endl;
        order++;
        }
      itFields++;
      }
    it++;
    }
  file << "    </componentActionList>" << METAIO_STREAM::endl;
  file << "  </applicationComponent>" << METAIO_STREAM::endl;
  file << "</gridApplication>" << METAIO_STREAM::endl;

  file.close();

  METAIO_STREAM::cout << "done" << METAIO_STREAM::endl;
  return true;
}


/** Parse the command line */
bool MetaCommand::
Parse(int argc, char* argv[])
{  
  m_ExecutableName = argv[0];

  long int slash = m_ExecutableName.find_last_of("/");
  if(slash>0)
    {
    m_ExecutableName = m_ExecutableName.substr(slash+1,
                                               m_ExecutableName.size()-slash-1);
    }
  slash = m_ExecutableName.find_last_of("\\");
  if(slash>0)
    {
    m_ExecutableName = m_ExecutableName.substr(slash+1,
                                               m_ExecutableName.size()-slash-1);
    }

  // Fill in the results
  m_ParsedOptionVector.clear();
  bool inArgument = false;
  METAIO_STL::string tag = "";
  METAIO_STL::string args;
  
  unsigned int currentField = 0; // current field position
  int currentOption = 0; // id of the option to fill
  unsigned int valuesRemaining=0;
  unsigned int optionalValuesRemaining=0;
  bool isComplete = false; // check if the option should be parse until
                           // the next tag is found
  METAIO_STL::string completeString = "";

  bool exportGAD = false;
  for(unsigned int i=1;i<(unsigned int)argc;i++)
    {
    if(!strcmp(argv[i],"-V") || !strcmp(argv[i],"-H"))
      {
      METAIO_STREAM::cout << "Usage : " << argv[0] << METAIO_STREAM::endl; 
      this->ListOptions();
      continue;
      }
    // List the options if using -v
    if(!strcmp(argv[i],"-v") || !strcmp(argv[i],"-h"))
      {
      METAIO_STREAM::cout << "Usage : " << argv[0] << METAIO_STREAM::endl; 
      this->ListOptionsSimplified();
      continue;
      }
    if(!strcmp(argv[i],"-vxml") 
       || !strcmp(argv[i],"-hxml")
       || !strcmp(argv[i],"-exportXML"))
      {
      this->ListOptionsXML();
      continue;
      }
    if(!strcmp(argv[i],"-version"))
      {
      METAIO_STREAM::cout << "Version: " << m_Version.c_str() 
                          << METAIO_STREAM::endl;
      continue;
      }
    if(!strcmp(argv[i],"-date"))
      {
      METAIO_STREAM::cout << "Date: " << m_Date.c_str() 
                          << METAIO_STREAM::endl;
      continue;
      }
    if(!strcmp(argv[i],"-exportGAD")
       || !strcmp(argv[i],"-vgad")
       || !strcmp(argv[i],"-hgad"))
      {
      this->ExportGAD();
      exportGAD = true;
      continue;
      }

    // If this is a tag
    if(argv[i][0] == '-' && (atof(argv[i])==0) && (strlen(argv[i])>1))
      {
      // if we have a tag before the expected values we throw an exception
      if(valuesRemaining!=0)
        {
        if(!isComplete)
          {
          if(optionalValuesRemaining >0)
            {
            valuesRemaining = 0;
            m_OptionVector[currentOption].userDefined = true;
            m_ParsedOptionVector.push_back(m_OptionVector[currentOption]);
            }
          else
            {
            METAIO_STREAM::cout << "Found tag " << argv[i] 
                              << " before end of value list!" 
                              << METAIO_STREAM::endl;
            return false;
            }
          }
        else
          {
          m_OptionVector[currentOption].fields[0].value = completeString;
          m_OptionVector[currentOption].fields[0].userDefined = true;
          m_OptionVector[currentOption].userDefined = true;
          m_ParsedOptionVector.push_back(m_OptionVector[currentOption]);
          valuesRemaining = 0;
          }
        }
      inArgument = false;
      // New tag so we add the previous values to the tag
      tag = argv[i];

      // Check if the tag is in the list
      if(this->OptionExistsByMinusTag(tag))
        {
        inArgument = true;
        
        // We check the number of mandatory and optional values for
        // this tag
        METAIO_STL::vector<Field>::const_iterator fIt =
                                 this->GetOptionByMinusTag(tag)->fields.begin();
        while(fIt != this->GetOptionByMinusTag(tag)->fields.end())
          {
          if(!(*fIt).required)
            {
            optionalValuesRemaining++;
            }
          valuesRemaining++;
          fIt++;
          }
        currentOption = this->GetOptionId(this->GetOptionByMinusTag(tag));
      
        if(currentOption < 0)
          {
          METAIO_STREAM::cout << "Error processing tag " << tag.c_str()
                              << ".  Tag exists but cannot find its Id."
                              << METAIO_STREAM::endl;
          }
        else
          {
          isComplete = m_OptionVector[currentOption].complete;

          if(m_OptionVector[currentOption].fields[0].type == FLAG)
            {
            // the tag exists by default
            m_OptionVector[currentOption].fields[0].value = "true"; 
            valuesRemaining = 0;
            optionalValuesRemaining = 0;
            inArgument = false;
            }
          else if(m_OptionVector[currentOption].fields[0].type == LIST)
            {
            inArgument = true;        
            unsigned int valuesInList = (int)atoi(argv[++i]);
            m_OptionVector[currentOption].fields[0].value = argv[i];
            valuesRemaining += valuesInList-1;
            char optName[255];
            for(unsigned int j=0; j<valuesInList; j++)
              {
              sprintf(optName, "%03d", j);
              this->AddOptionField( m_OptionVector[currentOption].name,
                                    optName, STRING );
              }
            }
          args = "";
          }
        }
      else 
        {
        if(m_Verbose)
          {
          METAIO_STREAM::cout << "The tag " << tag.c_str() 
                              << " is not a valid argument : skipping this tag" 
                              << METAIO_STREAM::endl;
          }
        if(m_FailOnUnrecognizedOption)
          {
          return false;
          }
        }
      if(inArgument)
        {
        i++;
        }
      }
    else if(!inArgument) // If this is a field
      {
      // Look for the field to add
      OptionVector::iterator it = m_OptionVector.begin();
      unsigned long pos = 0;
      bool found = false;
      while(it != m_OptionVector.end())
        {
        if((pos >= currentField) && ((*it).tag==""))
          {
          currentOption = pos;
          valuesRemaining = (*it).fields.size();
          found = true;
          break;
          }
        pos++;
        it++;
        }

      if(!found && m_Verbose)
        {
        METAIO_STREAM::cout 
                  << "Too many arguments specified in your command line! "
                  << "Skipping extra argument: " << argv[i] 
                  << METAIO_STREAM::endl;
        }
      
      inArgument=true;
      currentField=currentOption+1;
      }

    // We collect the values
    if(isComplete && (int)i<argc)
      {
      if(completeString.size()==0)
        {
        completeString = argv[i];
        }
      else
        {
        completeString += " ";
        completeString += argv[i];
        }
      }
    else if(inArgument && i<(unsigned int)argc && (valuesRemaining>0))
      {
      // We check that the current value is not a tag.
      // This might be the case when we have optional fields
      if(this->OptionExistsByMinusTag(argv[i]) && optionalValuesRemaining>0)
        {
        valuesRemaining = 0;
        optionalValuesRemaining = 0;
        i--; // the outter loop will take care of incrementing it.
        }

      else if(currentOption >=0 && currentOption <(int)(m_OptionVector.size()))
        {
        unsigned long s = m_OptionVector[currentOption].fields.size();

        // We change the value only if this is not a tag
        if(this->OptionExistsByMinusTag(argv[i]))
          {
          METAIO_STREAM::cout << "Option " 
                              << m_OptionVector[currentOption].name.c_str()
                              << " expect a value and got tag: " << argv[i] 
                              << METAIO_STREAM::endl;
          this->ListOptionsSimplified();
          return false;
          }

        m_OptionVector[currentOption].fields[s-(valuesRemaining)].value = argv[i];
       
        m_OptionVector[currentOption].fields[s-(valuesRemaining)].userDefined =
                                                                           true;

       if(!m_OptionVector[currentOption].fields[s-(valuesRemaining)].required)
         {
         optionalValuesRemaining--;
         }

        valuesRemaining--;
        }
      else if(valuesRemaining>0)
        {
        valuesRemaining--;
        }
      }
    else if(valuesRemaining==optionalValuesRemaining  
            && i==(unsigned int)argc && (optionalValuesRemaining>0)) 
    // if this is the last argument and all the remaining values are optionals
      {
      if(this->OptionExistsByMinusTag(argv[i-1]) )
        {
        valuesRemaining = 0;
        optionalValuesRemaining = 0;
        } 
      }

    if(valuesRemaining == 0)
      {
      inArgument = false;
      m_OptionVector[currentOption].userDefined = true;
      m_ParsedOptionVector.push_back(m_OptionVector[currentOption]);
      }

    } // end loop command line arguments

  if(isComplete) // If we are still in the isComplete mode we add the option
    {
    m_OptionVector[currentOption].fields[0].value = completeString;
    m_OptionVector[currentOption].fields[0].userDefined = true;
    m_OptionVector[currentOption].userDefined = true;
    m_ParsedOptionVector.push_back(m_OptionVector[currentOption]);
    valuesRemaining = 0;
    }

  if(optionalValuesRemaining >0 && optionalValuesRemaining==valuesRemaining)
    {
    valuesRemaining = 0;
    m_OptionVector[currentOption].userDefined = true;
    m_ParsedOptionVector.push_back(m_OptionVector[currentOption]);
    }

  if(valuesRemaining>0)
    {
    METAIO_STREAM::cout << "Not enough parameters for " 
                        << m_OptionVector[currentOption].name.c_str()
                        << METAIO_STREAM::endl;
    METAIO_STREAM::cout << "Usage: " << argv[0] << METAIO_STREAM::endl;
    this->ListOptionsSimplified();

    return false;
    }

  // Check if the options with required arguments are defined
  OptionVector::iterator it = m_OptionVector.begin();
  bool requiredAndNotDefined = false;
  while(it != m_OptionVector.end())
    {
    if((*it).required)
      {
      // First check if the option is actually defined
      if(!(*it).userDefined)
        {
        METAIO_STREAM::cout << "Option " << (*it).name.c_str()
                            << " is required but not defined" 
                            << METAIO_STREAM::endl;
        requiredAndNotDefined = true;
        it++;
        continue;
        }

      // Check if the values are defined
      METAIO_STL::vector<Field>::const_iterator itFields = (*it).fields.begin();
      bool defined = true;
      while(itFields != (*it).fields.end())
        {
        if((*itFields).value == "")
          {
          defined = false;
          }
        itFields++;
        }

      if(!defined)
        {
        if((*it).tag.size()>0)
          {
          METAIO_STREAM::cout << "Field " << (*it).tag.c_str() 
                              << " is required but not defined" 
                              << METAIO_STREAM::endl;
          }
        else
          {
          METAIO_STREAM::cout << "Field " << (*it).name.c_str() 
                              << " is required but not defined" 
                              << METAIO_STREAM::endl;
          }
        requiredAndNotDefined = true;
        }
      }
    it++;
    }

  if(requiredAndNotDefined)
    {
    METAIO_STREAM::cout << "Command: " << argv[0] << METAIO_STREAM::endl;
    this->ListOptionsSimplified();
    return false;
    }

  // Check if the values are in range (if the range is defined)
  OptionVector::iterator itParsed = m_ParsedOptionVector.begin();
  bool valueInRange = true;
  while(itParsed != m_ParsedOptionVector.end())
    {
    METAIO_STL::vector<Field>::const_iterator itFields = (*itParsed).fields
                                                                    .begin();
    while(itFields != (*itParsed).fields.end())
      {
      // Check only if this is a number
      if(((*itFields).type == INT ||
        (*itFields).type == FLOAT ||
        (*itFields).type == CHAR)
        && ((*itFields).value != "")
        )
        {
        // Check the range min
        if(
          (((*itFields).rangeMin != "")
          && (atof((*itFields).rangeMin.c_str())
              > atof((*itFields).value.c_str())))
          ||
          (((*itFields).rangeMax != "")
          && (atof((*itFields).rangeMax.c_str())
              < atof((*itFields).value.c_str())))
          )
          {
          METAIO_STREAM::cout << (*itParsed).name.c_str() 
                    << "." << (*itFields).name.c_str()
                    << " : Value (" << (*itFields).value.c_str() << ") "
                    << "is not in the range [" << (*itFields).rangeMin.c_str()
                    << "," << (*itFields).rangeMax.c_str()
                    << "]" << METAIO_STREAM::endl;
          valueInRange = false;
          }
        } 
      itFields++;
      }
    itParsed++;
    }

  if(!valueInRange)
    {
    return false;
    }

  // If everything is ok
  if(exportGAD)
    {
    this->ExportGAD(true);
    return false; // prevent from running the application
    }

  return true;  
}

/** Return the string representation of a type */
METAIO_STL::string MetaCommand::TypeToString(TypeEnumType type)
{
  switch(type)
    {
    case INT:
      return "int";
    case FLOAT:
      return "float";
    case STRING:
      return "string";
    case LIST:
      return "list";
    case FLAG:
      return "flag";
    case BOOL:
      return "boolean";
    default:
      return "not defined";
    }
}



/** Return a type given a string */
MetaCommand::TypeEnumType MetaCommand::StringToType(const char* type)
{
  if(!strcmp(type,"int"))
    {
    return INT;
    }
  else if(!strcmp(type,"float"))
    {
    return FLOAT;
    }
  else if(!strcmp(type,"string"))
    {
    return STRING;
    }
  else if(!strcmp(type,"list"))
    {
    return LIST;
    }
  else if(!strcmp(type,"flag"))
    {
    return FLAG;
    }

  return INT; // by default

}

#if (METAIO_USE_NAMESPACE)
};
#endif

