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
#ifndef _A1ED10DE_1D07_4EC4_D57_9B82EB54D320
#define _A1ED10DE_1D07_4EC4_D57_9B82EB54D320
//**************************************************************************
template< typename In, typename Out >
struct INTEGER_value_block : virtual public value_block_type< In, Out >,
                             virtual public hex_block_type< In, Out >
{
 long long value_dec;

 INTEGER_value_block();

 virtual MSXML2::IXMLDOMElementPtr to_xml( MSXML2::IXMLDOMDocumentPtr pDoc );
 virtual bool from_xml( MSXML2::IXMLDOMElementPtr element );

 virtual In decode_BER( unsigned long long max_block_len );
 virtual unsigned long long encode_BER( bool size_only = false );
};
//**************************************************************************
template< typename In, typename Out >
INTEGER_value_block< In, Out >::INTEGER_value_block() : value_dec( 0 )
{
}
//**************************************************************************
template< typename In, typename Out >
MSXML2::IXMLDOMElementPtr INTEGER_value_block< In, Out >::to_xml( MSXML2::IXMLDOMDocumentPtr pDoc )
{
 MSXML2::IXMLDOMElementPtr result = block_type< In, Out >::to_xml( pDoc );
 if( error[ 0 ] != 0x00 )
  return result;

 if( !block_length )
  return result;

 MSXML2::IXMLDOMElementPtr value_xml;

 if( !is_hex_only )
 {
  wchar_t buf[255];
  _i64tow_s( value_dec, buf, 255, 10 );

  value_xml = pDoc->createElement( L"value_dec" );
  value_xml->appendChild( pDoc->createTextNode( buf ) );
 }
 else
 {
  std::string codes = hex_codes( value_hex, block_length );

  wchar_t* whex = ( wchar_t* ) malloc( ( codes.length() + 1 ) * sizeof( wchar_t ) );
  memset( whex, 0x00, ( codes.length() + 1 ) * sizeof( wchar_t ) );

  size_t converted = 0;
  errno_t error_num = mbstowcs_s( &converted, whex, codes.length() + 1, codes.c_str(), codes.length() );
  if( error_num )
  {
   free( whex );
   return result;
  }

  value_xml = pDoc->createElement( L"value_hex" );
  value_xml->appendChild( pDoc->createTextNode( whex ) );

  free( whex );
 }

 result->appendChild( value_xml );

 return result;
}
//**************************************************************************
template< typename In, typename Out >
bool INTEGER_value_block< In, Out >::from_xml( MSXML2::IXMLDOMElementPtr element )
{
 if( !element )
  return false;

 MSXML2::IXMLDOMNodePtr value_dec_xml = element->selectSingleNode( L"value_dec" );
 if( value_dec_xml )
 {
  value_dec = _wtoi64( ( wchar_t* )value_dec_xml->firstChild->text );
  if( errno && !value_dec )
   return false;

  block_length = encode_tc( value_dec, &value_hex );
 }
 else
 {
  MSXML2::IXMLDOMNodePtr value_hex_xml = element->selectSingleNode( L"value_hex" );
  if( !value_hex_xml )
   return false;

  is_hex_only = true;

  std::wstring result = wreplace( (wchar_t*)value_hex_xml->firstChild->text, L" ", L"" );

  from_whex_codes( result.c_str(), result.length(), &value_hex );
  block_length = result.length() >> 1;
 }

 return true;
}
//**************************************************************************
template< typename In, typename Out >
In INTEGER_value_block< In, Out >::decode_BER( unsigned long long max_block_len )
{
 In end = hex_block_type::decode_BER( max_block_len );
 if( error[ 0 ] != 0x00 )
  return In();

 if( block_length > 8 )
 {
  warnings.push_back( "Too big INTEGER for decoding, hex only" );
  is_hex_only = true;
 }
 else
  value_dec = decode_tc( value_hex, ( unsigned char )block_length, &warnings );

 return end;
}
//**************************************************************************
template< typename In, typename Out >
unsigned long long INTEGER_value_block< In, Out >::encode_BER( bool size_only )
{
 if( !block_length )
  return 0;

 unsigned long long size = 0;

 if( !is_hex_only )
 {
  unsigned char* encoded;

  size = encode_tc( value_dec, &encoded );
  if( !size )
  {
   sprintf_s( error, 255, "Error during encoding INTEGER value" );
   return 0;
  }

  if( !size_only )
  {
   for( unsigned long long i = 0; i < size; i++ )
    stream_out = encoded[ i ];
  }

  free( encoded );
 }
 else
 {
  size = block_length;

  if( !size_only )
  {
   for( unsigned long long i = 0; i < size; i++ )
    stream_out = value_hex[ i ];
  }
 }

 return size;
}
//**************************************************************************
template< typename In, typename Out >
struct INTEGER_type : virtual public BASIC_type< In, Out >
{
 INTEGER_type();

 static std::string block_name_static();
 virtual std::string block_name();
};
//**************************************************************************
template< typename In, typename Out >
INTEGER_type< In, Out >::INTEGER_type()
{
 tag_class = 1;
 tag_number = 2;

 value_block = new INTEGER_value_block< In, Out >();
}
//**************************************************************************
template< typename In, typename Out >
std::string INTEGER_type< In, Out >::block_name_static()
{
 return "INTEGER";
}
//**************************************************************************
template< typename In, typename Out >
std::string INTEGER_type< In, Out >::block_name()
{
 return block_name_static();
}
//**************************************************************************
template< typename In, typename Out >
struct ENUMERATED_type : virtual public INTEGER_type< In, Out >
{
 ENUMERATED_type();

 static std::string block_name_static();
 virtual std::string block_name();
};
//**************************************************************************
template< typename In, typename Out >
ENUMERATED_type< In, Out >::ENUMERATED_type()
{
 tag_class = 1;
 tag_number = 10;
}
//**************************************************************************
template< typename In, typename Out >
std::string ENUMERATED_type< In, Out >::block_name_static()
{
 return "ENUMERATED";
}
//**************************************************************************
template< typename In, typename Out >
std::string ENUMERATED_type< In, Out >::block_name()
{
 return block_name_static();
}
//**************************************************************************
#endif