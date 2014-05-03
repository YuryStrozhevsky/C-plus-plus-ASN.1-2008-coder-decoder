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
#ifndef _C8A3B24B_5E3F_4CEF_BBA1_F1029408A589
#define _C8A3B24B_5E3F_4CEF_BBA1_F1029408A589
//**************************************************************************
template< typename In, typename Out >
struct OCTETSTRING_value_block : virtual public CONSTRUCTED_value_block< In, Out >,
                                 virtual public hex_block_type< In, Out >
{
 bool is_constructed;
 
 OCTETSTRING_value_block();
 
 virtual MSXML2::IXMLDOMElementPtr to_xml( MSXML2::IXMLDOMDocumentPtr pDoc );
 virtual bool from_xml( MSXML2::IXMLDOMElementPtr element );

 virtual In decode_BER( unsigned long long max_block_len );
 virtual unsigned long long encode_BER( bool size_only = false );
};
//**************************************************************************
template< typename In, typename Out >
OCTETSTRING_value_block< In, Out >::OCTETSTRING_value_block() : is_constructed( false )
{
}
//**************************************************************************
template< typename In, typename Out >
MSXML2::IXMLDOMElementPtr OCTETSTRING_value_block< In, Out >::to_xml( MSXML2::IXMLDOMDocumentPtr pDoc )
{
 if( is_constructed )
  return CONSTRUCTED_value_block< In, Out >::to_xml( pDoc );
 else
 {
  MSXML2::IXMLDOMElementPtr result = block_type< In, Out >::to_xml( pDoc );
  if( error[ 0 ] != 0x00 )
   return result;

  if( !block_length )
   return result;

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

  MSXML2::IXMLDOMElementPtr value_xml = pDoc->createElement( L"value_hex" );
  value_xml->appendChild( pDoc->createTextNode( whex ) );

  free( whex );
 
  result->appendChild( value_xml );

  return result;
 }
 
 return NULL;
}
//**************************************************************************
template< typename In, typename Out >
bool OCTETSTRING_value_block< In, Out >::from_xml( MSXML2::IXMLDOMElementPtr element )
{
 if( !element )
  return false;

 if( is_constructed )
 {
  if( !CONSTRUCTED_value_block< In, Out >::from_xml( element ) )
   return false;

  for( std::vector< BASIC_type< In, Out >* >::iterator i = value.begin(); i != value.end(); i++ )
  {
   if( (*i)->block_name() != OCTETSTRING_type< In, Out >::block_name_static() )
   {
    value.erase( value.begin(), value.end() );
    sprintf_s( error, 255, "OCTET STRING may consists of OCTET STRINGs only" );
    return false;
   }
  }
 }
 else
 {
  MSXML2::IXMLDOMElementPtr value_hex_xml = element->selectSingleNode( L"value_hex" );
  if( !value_hex_xml )
   return false;

  std::wstring str_hex = ( wchar_t* ) value_hex_xml->text;
  str_hex = wreplace( str_hex.c_str(), L" ", L"" );

  if( !from_whex_codes( str_hex.c_str(), str_hex.length(), &value_hex ) )
   return false;

  block_length = str_hex.length() >> 1;
 }

 return false;
}
//**************************************************************************
template< typename In, typename Out >
In OCTETSTRING_value_block< In, Out >::decode_BER( unsigned long long max_block_len )
{
 if( is_constructed )
 {
  In end = CONSTRUCTED_value_block< In, Out >::decode_BER( max_block_len );
  if( error[ 0 ] != 0x00 )
   return In();

  for( std::vector< BASIC_type< In, Out >* >::iterator i = value.begin(); i != value.end(); i++ )
  {
   if( (*i)->block_name() == EOC_type< In, Out >::block_name_static() )
   {
    if( is_indefinite_form )
     break;
    else
    {
     value.erase( value.begin(), value.end() );
     sprintf_s( error, 255, "EOC is unexpected, OCTET STRING may consists of OCTET STRINGs only" );
     return In();
    }
   }

   if( (*i)->block_name() != OCTETSTRING_type< In, Out >::block_name_static() )
   {
    value.erase( value.begin(), value.end() );
    sprintf_s( error, 255, "OCTET STRING may consists of OCTET STRINGs only" );
    return In();
   }
  }

  return end;
 }
 else
 {
  In end = hex_block_type::decode_BER( max_block_len );
  if( error[ 0 ] != 0x00 )
   return In();

  return end;
 }
 
 return In();
}
//**************************************************************************
template< typename In, typename Out >
unsigned long long OCTETSTRING_value_block< In, Out >::encode_BER( bool size_only )
{
 if( is_constructed )
  return CONSTRUCTED_value_block< In, Out >::encode_BER( size_only );
 else
 {
  if( size_only )
   return block_length;
   
  if( !block_length )
   return 0;

  for( unsigned long long i = 0; i < block_length; i++ )
   stream_out = value_hex[ i ];

  return block_length;
 }
 
 return 0;
}
//**************************************************************************
template< typename In, typename Out >
struct OCTETSTRING_type : virtual public BASIC_type< In, Out >
{
 static std::string block_name_static();
 virtual std::string block_name();

 OCTETSTRING_type();

 virtual bool from_xml( MSXML2::IXMLDOMElementPtr element );
 virtual In decode_BER( unsigned long long max_block_len );
};
//**************************************************************************
template< typename In, typename Out >
OCTETSTRING_type< In, Out >::OCTETSTRING_type()
{
 tag_class = 1;
 tag_number = 4;

 value_block = new OCTETSTRING_value_block< In, Out >();
}
//**************************************************************************
template< typename In, typename Out >
std::string OCTETSTRING_type< In, Out >::block_name_static()
{
 return "OCTET STRING";
}
//**************************************************************************
template< typename In, typename Out >
std::string OCTETSTRING_type< In, Out >::block_name()
{
 return block_name_static();
}
//**************************************************************************
template< typename In, typename Out >
bool OCTETSTRING_type< In, Out >::from_xml( MSXML2::IXMLDOMElementPtr element )
{
 id_block = new identification_block_type< In, Out >();
 if( !id_block->from_xml( element->selectSingleNode( L"./identification_block" ) ) )
  return false;

 if( ( id_block->tag_class != tag_class ) ||
     ( id_block->tag_number != tag_number ) )
 {
  if( ( tag_class != ( unsigned char ) ( -1 ) ) &&
      ( tag_number != ( unsigned long long ) ( -1 ) ) )
  {
   std::string str = "Wrong \"tag_class\" or \"tag_number\" for ASN.1 type " + block_name();
   sprintf_s( id_block->error, 255, str.c_str() );
   return false;
  }
 }

 len_block = new length_block_type< In, Out >();
 if( !len_block->from_xml( element->selectSingleNode( L"./length_block" ) ) )
  return false;

 OCTETSTRING_value_block< In, Out >* vb = dynamic_cast< OCTETSTRING_value_block< In, Out >* >( value_block );
 if( vb == NULL )
  return false;
 
 vb->is_constructed = id_block->is_constructed;
 vb->is_indefinite_form = len_block->is_indefinite_form;

 if( value_block != NULL )
  if( !value_block->from_xml( element->selectSingleNode( L"./value_block" ) ) )
   return false;

 return true;
}
//**************************************************************************
template< typename In, typename Out >
In OCTETSTRING_type< In, Out >::decode_BER( unsigned long long max_block_len )
{
 if( !max_block_len ) // Ability to encode empty OCTET STRING
  return stream_in;

 OCTETSTRING_value_block< In, Out >* vb = dynamic_cast< OCTETSTRING_value_block< In, Out >* >( value_block );
 if( vb == NULL )
  return false;
 
 vb->is_constructed = id_block->is_constructed;
 vb->is_indefinite_form = len_block->is_indefinite_form;

 return BASIC_type< In, Out >::decode_BER( max_block_len );
}
//**************************************************************************
#endif