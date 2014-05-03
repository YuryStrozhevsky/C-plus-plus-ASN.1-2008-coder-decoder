/*
 * Copyright (c) 2012 Yury Strozhevsky <www.strozhevsky.com>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <windows.h>

#include <stdio.h>
#include <stdlib.h>

#include <atlbase.h>
#include <atlenc.h>

#import <msxml6.dll>

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <fstream>

#pragma warning( disable : 4250 )

#include "internal_utf8stream.h"
#include "internal_base64.h"
#include "common.h"
#include "internal_basictypes.h"
#include "internal_transformfuncs.h"
#include "type_integer.h"
#include "type_objid.h"
#include "type_real.h"
#include "type_boolean.h"
#include "type_null.h"
#include "type_eoc.h"
#include "type_bitstring.h"
#include "type_octetstring.h"
#include "type_sequence.h"
#include "type_strings.h"
#include "type_date_times.h"

#include "resource.h"
//**************************************************************************
struct file_element
{
 std::wstring format;
 std::wstring name;
};
struct file_group
{
 file_element* in;
 file_element* out;

 file_group();
 ~file_group();
};
file_group::file_group() : in( NULL),
                           out( NULL )
{
}
file_group::~file_group()
{
 if( in != NULL )
  delete in;

 if( out != NULL )
  delete out;
}
std::vector< file_group* > configuration;
//**************************************************************************
void usage()
{
 std::cout << "compli - fully-compliant ASN.1 BER decoder/encoder, v2.0" << std::endl;
 std::cout << "(c) 2012, Yury Strozhevsky, www.strozhevsky.com" << std::endl;
 std::cout << std::endl << "Usage:" << std::endl;
 std::cout << "\tcompli.exe <configiration XML file>" << std::endl;
}
//**************************************************************************
int init_configuration( char** argv )
{
  MSXML2::IXMLDOMDocument2Ptr pXSDDoc;
  HRESULT hr = pXSDDoc.CreateInstance(__uuidof(MSXML2::DOMDocument60), NULL, CLSCTX_INPROC_SERVER);

  pXSDDoc->async = VARIANT_FALSE;
  pXSDDoc->validateOnParse = VARIANT_FALSE;

  HRSRC xsd_res = ::FindResource(NULL, MAKEINTRESOURCE( IDR_CONFIGXSD ), RT_RCDATA);
  if( !xsd_res )
   return 0;

  unsigned int xsd_res_size = ::SizeofResource( NULL, xsd_res );
  HGLOBAL xsd_res_global_data = ::LoadResource( NULL, xsd_res );
  if( !xsd_res_global_data )
   return 0;

  char* xsd_res_data = ( char* )::LockResource( xsd_res_global_data );

  wchar_t* wxsd = ( wchar_t* ) malloc( ( xsd_res_size + 1 ) * sizeof( wchar_t ) );
  if( !wxsd )
   return 0;

  size_t converted = 0;
  errno_t errnum = mbstowcs_s( &converted, wxsd, xsd_res_size + 1, xsd_res_data, xsd_res_size );
  if( errnum && !converted )
   return 0;

  VARIANT_BOOL result = pXSDDoc->loadXML( wxsd );

  free( wxsd );

  if( result == VARIANT_FALSE )
  {
   std::cout << "Failed to load resource with XSD schema, exiting" << std::endl;
   return 0;
  }

  MSXML2::IXMLDOMSchemaCollectionPtr xsd_schema;
  hr = xsd_schema.CreateInstance( __uuidof(MSXML2::XMLSchemaCache60), NULL, CLSCTX_INPROC_SERVER );
  hr = xsd_schema->add(L"", pXSDDoc.GetInterfacePtr() );
  if( FAILED( hr ) )
  {
   std::cout << "Unable to load CompliXML.xsd schema file, exiting" << std::endl;
   return 0;
  }

 MSXML2::IXMLDOMDocument3Ptr xml_document;
 hr = xml_document.CreateInstance( __uuidof(MSXML2::DOMDocument60), NULL, CLSCTX_INPROC_SERVER );

 size_t len = strlen( argv[ 1 ] );
 wchar_t* whex = ( wchar_t* ) malloc( ( len + 1 ) * sizeof( wchar_t ) );

 converted = 0;
 errno_t error_num = mbstowcs_s( &converted, whex, len + 1, argv[ 1 ], len );
 if( error_num )
 {
  free( whex );
  return 0;
 }

 VARIANT_BOOL loaded = xml_document->load( whex );
 free( whex );

 if( loaded == VARIANT_FALSE )
 {
  std::cout << "Unable to load configuration file: " << argv[ 1 ] << ", exiting" << std::endl;
  return 0;
 }

 xml_document->schemas = xsd_schema.GetInterfacePtr();

 MSXML2::IXMLDOMParseErrorPtr pError = xml_document->validate();
 if (pError->errorCode != 0)
 {
  std::cout << "Problem during XML validation of config file: " << std::endl;
  std::cout << "---------------------------------------------" << std::endl;
  char* error = to_console_cp( (char*)pError->Getreason() );
  if( error )
  {
   std::cout << error << std::endl;
   std::cout << "--------------------------------------------" << std::endl;
   free( error );
  }

  return 0;
 }

 MSXML2::IXMLDOMNodeListPtr list = xml_document->selectNodes( L"/config/file_group" );
 if( !list )
  return 0;

 for( int i = 0; i < list->length; i++ )
 {
  file_group* fg = new file_group();

  MSXML2::IXMLDOMNodePtr in_xml = list->item[ i ]->selectSingleNode( L"in" );
  if( !in_xml )
   return 0;

  MSXML2::IXMLDOMElementPtr format_xml = in_xml->selectSingleNode( L"format" );
  if( !format_xml )
   return 0;

  fg->in = new file_element();
  fg->in->format = ( wchar_t* ) format_xml->text;

  MSXML2::IXMLDOMElementPtr name_xml = in_xml->selectSingleNode( L"name" );
  if( !name_xml )
   return 0;

  fg->in->name = ( wchar_t* ) name_xml->text;

  MSXML2::IXMLDOMNodePtr out_xml = list->item[ i ]->selectSingleNode( L"out" );
  if( !out_xml )
   return 0;

  MSXML2::IXMLDOMElementPtr format_out_xml = out_xml->selectSingleNode( L"format" );
  if( !format_out_xml )
   return 0;

  fg->out = new file_element();
  fg->out->format = ( wchar_t* ) format_out_xml->text;

  MSXML2::IXMLDOMElementPtr name_out_xml = out_xml->selectSingleNode( L"name" );
  if( !name_out_xml )
   return 0;

  fg->out->name = ( wchar_t* ) name_out_xml->text;

  configuration.push_back( fg );
 }

 return 1;
}
//**************************************************************************
void process_file_group( file_group* fg, MSXML2::IXMLDOMSchemaCollectionPtr xsd_schema )
{
 //-------------------------------------------------
 if( fg->in->format == L"XML" )
 {
  MSXML2::IXMLDOMDocument3Ptr xml_document;
  HRESULT hr = xml_document.CreateInstance( __uuidof(MSXML2::DOMDocument60), NULL, CLSCTX_INPROC_SERVER );
  xml_document->async = VARIANT_FALSE;
  xml_document->validateOnParse = VARIANT_FALSE;

  xml_document->load( fg->in->name.c_str() );

  xml_document->schemas = xsd_schema.GetInterfacePtr();

  MSXML2::IXMLDOMParseErrorPtr pError = xml_document->validate();
  if (pError->errorCode != 0)
  {
   std::cout << "Problem during XML validation of input file: " << std::endl;
   std::cout << "--------------------------------------------" << std::endl;
   std::wcout << L"File name: " << fg->in->name << std::endl << std::endl;
   char* error = to_console_cp( (char*)pError->Getreason() );
   if( error )
   {
    std::cout << error << std::endl;
    std::cout << "--------------------------------------------" << std::endl;
    free( error );
   }

   return;
  }

  if( fg->out->format == L"BER" )
  {
   char* name = to_acp( fg->out->name.c_str() );

   std::ofstream ostream( name, std::ios::binary );
   ostream.unsetf( std::ios_base::skipws );

   free( name );

   std::ostreambuf_iterator< char > out( ostream );

   encode_result* result = XML_BER( out, xml_document->documentElement );
   if( result != NULL )
    delete result;

   ostream.flush();
   ostream.close();
  }
  if( fg->out->format == L"BER64")
  {
   char* name = to_acp( fg->out->name.c_str() );

   std::ofstream ostream( name, std::ios::binary );
   ostream.unsetf( std::ios_base::skipws );

   free( name );

   std::ostreambuf_iterator< char > out( ostream );

   encode_result* result = XML_BER64( out, xml_document->documentElement );
   if( result != NULL )
    delete result;

   ostream.flush();
   ostream.close();
  }
  if( fg->out->format == L"XER")
  {
  }
  if( fg->out->format == L"XML")
  {
   xml_document->save( fg->out->name.c_str() );
  }
 }
 //-------------------------------------------------
 if( fg->in->format == L"BER" )
 {
  MSXML2::IXMLDOMDocument3Ptr xml_document, xslt_document;
  HRESULT hr = xml_document.CreateInstance( __uuidof(MSXML2::DOMDocument60), NULL, CLSCTX_INPROC_SERVER );
  hr = xslt_document.CreateInstance( __uuidof(MSXML2::DOMDocument60), NULL, CLSCTX_INPROC_SERVER );

  char* name_in = to_acp( fg->in->name.c_str() );
  char* name_out = to_acp( fg->out->name.c_str() );

  if( fg->out->format == L"XML" )
  {
   std::ifstream coded_file( name_in, std::ios_base::binary );
   if( !coded_file.good() )
   {
    std::string message = "Unable to open ";
    message += name_in;
    message += " file!";
    std::cout << message << std::endl;
    
    free( name_in );
    free( name_out );
    
    return;
   }

   coded_file.unsetf( std::ios_base::skipws );

   coded_file.seekg( 0, std::ios::end );
   unsigned long long max_block_len = coded_file.tellg();
   coded_file.seekg( 0, std::ios::beg );

   BER_XML( std::istreambuf_iterator< char >( coded_file ), max_block_len, xml_document );

   coded_file.close();

   HRSRC xslt_res = ::FindResource(NULL, MAKEINTRESOURCE( IDR_XSLT ), RT_RCDATA);
   if( !xslt_res )
   {
    free( name_in );
    free( name_out );
    return;
   }

   unsigned int xslt_res_size = ::SizeofResource( NULL, xslt_res );
   HGLOBAL xslt_res_global_data = ::LoadResource(NULL, xslt_res);
   if( !xslt_res_global_data )
   {
    free( name_in );
    free( name_out );
    return;
   }

   char* xslt_res_data = ( char* )::LockResource( xslt_res_global_data );

   wchar_t* wxslt = ( wchar_t* ) malloc( ( xslt_res_size + 1 ) * sizeof( wchar_t ) );
   if( !wxslt )
   {
    free( name_in );
    free( name_out );
    return;
   }

   size_t converted = 0;
   errno_t errnum = mbstowcs_s( &converted, wxslt, xslt_res_size + 1, xslt_res_data, xslt_res_size );
   if( errnum && !converted )
   {
    free( name_in );
    free( name_out );
    return;
   }

   VARIANT_BOOL xslt_result = xslt_document->loadXML( wxslt );
   free( wxslt );
   if( xslt_result == VARIANT_FALSE )
   {
    free( name_in );
    free( name_out );
    return;
   }

   std::wofstream ostream( name_out, std::ios::binary );
   ostream.unsetf( std::ios_base::skipws );
   ostream.imbue( std::locale( ostream.getloc(), new utf8_conversion() ) );
   std::wstring transformed = ( wchar_t* )xml_document->transformNode( xslt_document );
   ostream << transformed << std::endlu;
   ostream.flush();
   ostream.close();
  }
  if( fg->out->format == L"BER64")
  {
   std::ifstream istream( name_in, std::ios_base::binary );
   istream.unsetf( std::ios_base::skipws );

   std::ofstream ostream( name_out, std::ios::binary );
   ostream.unsetf( std::ios_base::skipws );

   base64_encode( std::istreambuf_iterator< char >( istream ), std::istreambuf_iterator< char >(), std::ostreambuf_iterator< char > ( ostream ) );

   ostream.flush();
   ostream.close();

   istream.close();
  }
  if( fg->out->format == L"XER")
  {
  }
  if( fg->out->format == L"BER")
  {
   std::ifstream istream( name_in, std::ios_base::binary );
   istream.unsetf( std::ios_base::skipws );

   std::ofstream ostream( name_out, std::ios::binary );
   ostream.unsetf( std::ios_base::skipws );

   std::copy( std::istreambuf_iterator< char >( istream ), std::istreambuf_iterator< char >(), std::ostreambuf_iterator< char > ( ostream ) );

   ostream.flush();
   ostream.close();

   istream.close();
  }
  
  free( name_in );
  free( name_out );
 }
 //-------------------------------------------------
 if( fg->in->format == L"BER64" )
 {
  MSXML2::IXMLDOMDocument3Ptr xml_document, xslt_document;
  HRESULT hr = xml_document.CreateInstance( __uuidof(MSXML2::DOMDocument60), NULL, CLSCTX_INPROC_SERVER );
  hr = xslt_document.CreateInstance( __uuidof(MSXML2::DOMDocument60), NULL, CLSCTX_INPROC_SERVER );

  char* name_in = to_acp( fg->in->name.c_str() );
  char* name_out = to_acp( fg->out->name.c_str() );

  if( fg->out->format == L"BER" )
  {
   std::ifstream istream( name_in, std::ios_base::binary );
   istream.unsetf( std::ios_base::skipws );

   std::ofstream ostream( name_out, std::ios::binary );
   ostream.unsetf( std::ios_base::skipws );

   base64_decode( std::istreambuf_iterator< char >( istream ), std::istreambuf_iterator< char >(), std::ostreambuf_iterator< char > ( ostream ) );

   ostream.flush();
   ostream.close();

   istream.close();
  }
  if( fg->out->format == L"XML")
  {
   std::ifstream coded_file( name_in, std::ios_base::binary );
   if( !coded_file.good() )
   {
    std::string message = "Unable to open ";
    message += name_in;
    message += " file!";
    std::cout << message << std::endl;
    
    free( name_in );
    free( name_out );
    
    return;
   }

   coded_file.unsetf( std::ios_base::skipws );

   coded_file.seekg( 0, std::ios::end );
   unsigned long long max_block_len = coded_file.tellg();
   coded_file.seekg( 0, std::ios::beg );

   BER64_XML( std::istreambuf_iterator< char >( coded_file ), max_block_len, xml_document );

   coded_file.close();

   HRSRC xslt_res = ::FindResource(NULL, MAKEINTRESOURCE( IDR_XSLT ), RT_RCDATA);
   if( !xslt_res )
   {
    free( name_in );
    free( name_out );
    return;
   }

   unsigned int xslt_res_size = ::SizeofResource( NULL, xslt_res );
   HGLOBAL xslt_res_global_data = ::LoadResource(NULL, xslt_res);
   if( !xslt_res_global_data )
   {
    free( name_in );
    free( name_out );
    return;
   }

   char* xslt_res_data = ( char* )::LockResource( xslt_res_global_data );

   wchar_t* wxslt = ( wchar_t* ) malloc( ( xslt_res_size + 1 ) * sizeof( wchar_t ) );
   if( !wxslt )
   {
    free( name_in );
    free( name_out );
    return;
   }

   size_t converted = 0;
   errno_t errnum = mbstowcs_s( &converted, wxslt, xslt_res_size + 1, xslt_res_data, xslt_res_size );
   if( errnum && !converted )
   {
    free( name_in );
    free( name_out );
    return;
   }

   VARIANT_BOOL xslt_result = xslt_document->loadXML( wxslt );
   free( wxslt );
   if( xslt_result == VARIANT_FALSE )
   {
    free( name_in );
    free( name_out );
    return;
   }

   std::wofstream ostream( name_out, std::ios::binary );
   ostream.unsetf( std::ios_base::skipws );
   ostream.imbue( std::locale( ostream.getloc(), new utf8_conversion() ) );
   std::wstring transformed = ( wchar_t* )xml_document->transformNode( xslt_document );
   ostream << transformed << std::endlu;
   ostream.flush();
   ostream.close();
  }
  if( fg->out->format == L"XER")
  {
  }
  if( fg->out->format == L"BER64")
  {
   std::ifstream istream( name_in, std::ios_base::binary );
   istream.unsetf( std::ios_base::skipws );

   std::ofstream ostream( name_out, std::ios::binary );
   ostream.unsetf( std::ios_base::skipws );

   std::copy( std::istreambuf_iterator< char >( istream ), std::istreambuf_iterator< char >(), std::ostreambuf_iterator< char > ( ostream ) );

   ostream.flush();
   ostream.close();

   istream.close();
  }

  free( name_in );
  free( name_out );
 }
 //-------------------------------------------------
 if( fg->in->format == L"XER" )
 {
  MSXML2::IXMLDOMDocument3Ptr xml_document;
  HRESULT hr = xml_document.CreateInstance( __uuidof(MSXML2::DOMDocument60), NULL, CLSCTX_INPROC_SERVER );

  xml_document->load( fg->in->name.c_str() );

  if( fg->out->format == L"BER" )
  {
  }
  if( fg->out->format == L"BER64")
  {
  }
  if( fg->out->format == L"XML")
  {
  }
  if( fg->out->format == L"XER")
  {
   xml_document->save( fg->out->name.c_str() );
  }
 }
 //-------------------------------------------------
}
//**************************************************************************
int main( int argc, char** argv )
{
 if( argc != 2 )
 {
  usage();
  return 0;
 }

 std::cout << "\"compli\" - fully-compliant ASN.1 BER decoder/encoder, v2.0" << std::endl;
 std::cout << "(c) 2012, Yury Strozhevsky, www.strozhevsky.com" << std::endl << std::endl;

 HRESULT hr = CoInitialize( NULL );
 if( FAILED( hr ) )
 {
  std::cout << "Something wrong with COM initialization, exiting" << std::endl;
  return 0;
 }

 if( !init_configuration( argv ) )
 {
  std::cout << "Unable to process configuration file!" << std::endl;
  return 0;
 }

 { // additional brackets in order to give MSXML smart pointers a chance to de-init before "CoUninitialize()"
  MSXML2::IXMLDOMDocument2Ptr pXSDDoc;
  hr = pXSDDoc.CreateInstance(__uuidof(MSXML2::DOMDocument60), NULL, CLSCTX_INPROC_SERVER);

  pXSDDoc->async = VARIANT_FALSE;
  pXSDDoc->validateOnParse = VARIANT_FALSE;

  HRSRC xsd_res = ::FindResource(NULL, MAKEINTRESOURCE( IDR_XSD ), RT_RCDATA);
  if( !xsd_res )
   return 0;

  unsigned int xsd_res_size = ::SizeofResource( NULL, xsd_res );
  HGLOBAL xsd_res_global_data = ::LoadResource(NULL, xsd_res );
  if( !xsd_res_global_data )
   return 0;

  char* xsd_res_data = ( char* )::LockResource( xsd_res_global_data );

  wchar_t* wxsd = ( wchar_t* ) malloc( ( xsd_res_size + 1 ) * sizeof( wchar_t ) );
  if( !wxsd )
   return 0;

  size_t converted = 0;
  errno_t errnum = mbstowcs_s( &converted, wxsd, xsd_res_size + 1, xsd_res_data, xsd_res_size );
  if( errnum && !converted )
   return 0;

  VARIANT_BOOL result = pXSDDoc->loadXML( wxsd );

  free( wxsd );

  if( result == VARIANT_FALSE )
  {
   std::cout << "Failed to load resource with XSD schema, exiting" << std::endl;
   return 0;
  }

  MSXML2::IXMLDOMSchemaCollectionPtr xsd_schema;
  hr = xsd_schema.CreateInstance( __uuidof(MSXML2::XMLSchemaCache60), NULL, CLSCTX_INPROC_SERVER );
  hr = xsd_schema->add(L"", pXSDDoc.GetInterfacePtr() );
  if( FAILED( hr ) )
  {
   std::cout << "Unable to load CompliXML.xsd schema file, exiting" << std::endl;
   return 0;
  }

  for( std::vector< file_group* >::iterator i = configuration.begin(); i != configuration.end(); i++ )
   process_file_group( *i, xsd_schema );

  for( std::vector< file_group* >::iterator i = configuration.begin(); i != configuration.end(); i++ )
   delete (*i);
 }

 CoUninitialize();

 return 0;
}
//**************************************************************************
