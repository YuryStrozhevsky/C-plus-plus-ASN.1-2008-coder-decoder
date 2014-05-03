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
#ifndef _C0F3C217_84C5_4E2E_B8BC_A4F8F9EB2C0A
#define _C0F3C217_84C5_4E2E_B8BC_A4F8F9EB2C0A
//**************************************************************************
template< typename In, typename Out >
struct BOOLEAN_value_block : virtual public value_block_type< In, Out >,
                             virtual public hex_block_type< In, Out >
{
 bool value;

 BOOLEAN_value_block();

 virtual MSXML2::IXMLDOMElementPtr to_xml( MSXML2::IXMLDOMDocumentPtr pDoc );
 virtual bool from_xml( MSXML2::IXMLDOMElementPtr element );

 virtual In decode_BER( unsigned long long max_block_len );
};
//**************************************************************************
template< typename In, typename Out >
BOOLEAN_value_block< In, Out >::BOOLEAN_value_block() : value( false )
{
}
//**************************************************************************
template< typename In, typename Out >
MSXML2::IXMLDOMElementPtr BOOLEAN_value_block< In, Out >::to_xml( MSXML2::IXMLDOMDocumentPtr pDoc )
{
 MSXML2::IXMLDOMElementPtr result = block_type< In, Out >::to_xml( pDoc );
 if( error[ 0 ] != 0x00 )
  return result;

 MSXML2::IXMLDOMElementPtr value_hex_xml = pDoc->createElement( L"value_hex" );

 std::string hex = hex_codes( value_hex, block_length );

 wchar_t* whex = ( wchar_t* ) malloc( ( hex.length() + 1 ) * sizeof( wchar_t ) );
 memset( whex, 0x00, ( hex.length() + 1 ) * sizeof( wchar_t ) );

 size_t converted = 0;
 errno_t error_num = mbstowcs_s( &converted, whex, hex.length() + 1, hex.c_str(), hex.length() );
 if( error_num )
 {
  free( whex );
  return result;
 }

 value_hex_xml->appendChild( pDoc->createTextNode( whex ) );
  
 free( whex );

 result->appendChild( value_hex_xml );

 return result;
}
//**************************************************************************
template< typename In, typename Out >
bool BOOLEAN_value_block< In, Out >::from_xml( MSXML2::IXMLDOMElementPtr element )
{
 if( !element )
  return false;

 is_hex_only = true;

 MSXML2::IXMLDOMNodePtr value_hex_xml = element->selectSingleNode( L"value_hex" );
 if( !value_hex_xml )
  return false;

 std::wstring str_hex = ( wchar_t* ) value_hex_xml->text;
 str_hex = wreplace( str_hex.c_str(), L" ", L"" );

 if( !from_whex_codes( str_hex.c_str(), str_hex.length(), &value_hex ) )
  return false;

 block_length = str_hex.length() >> 1;

 long long result = decode_tc( value_hex, ( unsigned char )block_length, &warnings );

 if( result != 0 )
  value = true;
 else
  value = false;

 if( block_length > 1 )
  warnings.push_back( "BOOLEAN value encoded in more then 1 octet" );

 return true;
}
//**************************************************************************
template< typename In, typename Out >
In BOOLEAN_value_block< In, Out >::decode_BER( unsigned long long max_block_len )
{
 In end = hex_block_type::decode_BER( max_block_len );
 if( error[ 0 ] != 0x00 )
  return In();

 is_hex_only = true;

 long long result = decode_tc( value_hex, ( unsigned char )block_length, &warnings );

 if( result != 0 )
  value = true;
 else
  value = false;

 if( block_length > 1 )
  warnings.push_back( "BOOLEAN value encoded in more then 1 octet" );

 return end;
}
//**************************************************************************
template< typename In, typename Out >
struct BOOLEAN_type : virtual public BASIC_type< In, Out >
{
 BOOLEAN_type();

 static std::string block_name_static();
 virtual std::string block_name();
};
//**************************************************************************
template< typename In, typename Out >
BOOLEAN_type< In, Out >::BOOLEAN_type()
{
 tag_class = 1;
 tag_number = 1;

 value_block = new BOOLEAN_value_block< In, Out >();
}
//**************************************************************************
template< typename In, typename Out >
std::string BOOLEAN_type< In, Out >::block_name_static()
{
 return "BOOLEAN";
}
//**************************************************************************
template< typename In, typename Out >
std::string BOOLEAN_type< In, Out >::block_name()
{
 return block_name_static();
}
//**************************************************************************
#endif