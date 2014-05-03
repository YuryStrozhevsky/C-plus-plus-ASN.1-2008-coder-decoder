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
#ifndef _ABF41DE2_8824_46AE_94CE_552B79A68D99
#define _ABF41DE2_8824_46AE_94CE_552B79A68D99
//**************************************************************************
template< typename In, typename Out >
struct SID_value_block : virtual public hex_block_type< In, Out >
{
 unsigned long long value_dec;

 bool is_first_sid;

 SID_value_block();
 
 virtual std::string block_name();

 virtual MSXML2::IXMLDOMElementPtr to_xml( MSXML2::IXMLDOMDocumentPtr pDoc );
 virtual bool from_xml( MSXML2::IXMLDOMElementPtr element );

 virtual In decode_BER( unsigned long long max_block_len );
 virtual unsigned long long encode_BER( bool size_only = false );
};
//**************************************************************************
template< typename In, typename Out >
SID_value_block< In, Out >::SID_value_block() : value_dec( 0 ),
                                                is_first_sid( false )
{
}
//**************************************************************************
template< typename In, typename Out >
std::string SID_value_block< In, Out >::block_name()
{
 return "sid_block";
}
//**************************************************************************
template< typename In, typename Out >
MSXML2::IXMLDOMElementPtr SID_value_block< In, Out >::to_xml( MSXML2::IXMLDOMDocumentPtr pDoc )
{
 MSXML2::IXMLDOMElementPtr result = block_type< In, Out >::to_xml( pDoc );
 if( error[ 0 ] != 0x00 )
  return result;

 if( !block_length )
  return result;

 MSXML2::IXMLDOMElementPtr is_first_sid_xml = pDoc->createElement( L"is_first_sid" );
 is_first_sid_xml->appendChild( pDoc->createTextNode( ( is_first_sid )? L"true" : L"false" ) );
 result->appendChild( is_first_sid_xml );

 MSXML2::IXMLDOMElementPtr is_hex_only_xml = pDoc->createElement( L"is_hex_only" );
 is_hex_only_xml->appendChild( pDoc->createTextNode( ( is_hex_only )? L"true" : L"false" ) );
 result->appendChild( is_hex_only_xml );

 if( is_hex_only )
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

  MSXML2::IXMLDOMElementPtr value_hex_xml = pDoc->createElement( L"value_hex" );
  value_hex_xml->appendChild( pDoc->createTextNode( whex ) );
  result->appendChild( value_hex_xml );

  free( whex );
 }
 else
 {
  wchar_t buf[255];
  _ui64tow_s( value_dec, buf, 255, 10 );

  MSXML2::IXMLDOMElementPtr value_dec_xml = pDoc->createElement( L"value_dec" );
  value_dec_xml->appendChild( pDoc->createTextNode( buf ) );
  result->appendChild( value_dec_xml ); 
 }

 return result;
}
//**************************************************************************
template< typename In, typename Out >
bool SID_value_block< In, Out >::from_xml( MSXML2::IXMLDOMElementPtr element )
{
 if( !element )
  return false;

 MSXML2::IXMLDOMElementPtr is_first_sid_xml = element->selectSingleNode( L"is_first_sid" );
 if( !is_first_sid_xml )
  return false;

 if( is_first_sid_xml->text == ( _bstr_t )L"true" )
  is_first_sid = true;
 else
 {
  if( is_first_sid_xml->text == ( _bstr_t )L"false" )
   is_first_sid = false;
  else
   return false;
 }

 MSXML2::IXMLDOMElementPtr is_hex_only_xml = element->selectSingleNode( L"is_hex_only" );
 if( !is_hex_only_xml )
  return false;

 if( is_hex_only_xml->text == ( _bstr_t )L"true" )
  is_hex_only = true;
 else
 {
  if( is_hex_only_xml->text == ( _bstr_t )L"false" )
   is_hex_only = false;
  else
   return false;
 }

 if( is_hex_only )
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
 else
 {
  MSXML2::IXMLDOMElementPtr value_dec_xml = element->selectSingleNode( L"value_dec" );
  if( !value_dec_xml )
   return false;

  value_dec = _wtoi64( ( wchar_t* ) value_dec_xml->text );
  if( errno && !value_dec )
   return false;
 }

 return true;
}
//**************************************************************************
template< typename In, typename Out >
In SID_value_block< In, Out >::decode_BER( unsigned long long max_block_len )
{
 if( !max_block_len )
  return stream_in;

 if( stream_in == In() )
 {
  sprintf_s( error, 255, "End of input reached before message was fully decoded" );
  return stream_in;
 }

 value_hex = ( unsigned char* ) malloc( ( size_t )max_block_len );
 if( !value_hex )
  return In();

 In end = stream_in;
 unsigned char* data = value_hex;

 while( (unsigned char)(*end) & 0x80 )
 {
  if( block_length > ( max_block_len - 1 ) )
  {
   sprintf_s( error, 255, "End of input reached before message was fully decoded" );
   return end;
  }

  *data = (unsigned char)(*end) & 0x7F;

  data++;
  end++;
  block_length++;

  if( end == In() )
  {
   sprintf_s( error, 255, "End of input reached before message was fully decoded" );
   return end;
  }
 }

 // The least octet
 if( end == In() )
 {
  sprintf_s( error, 255, "End of input reached before message was fully decoded" );
  return end;
 }
 *data = (unsigned char)(*end) & 0x7F;
 block_length++;
 end++;

 if( !value_hex[ 0 ] )
  warnings.push_back( "Needlessly long format of SID encoding" );

 if( block_length <= 8 )
  from_base( value_hex, &value_dec, 7, ( unsigned char )block_length );
 else
 {
  is_hex_only = true;
  warnings.push_back( "Too big SID for decoding, hex only" );
 }

 return end;
}
//**************************************************************************
template< typename In, typename Out >
unsigned long long SID_value_block< In, Out >::encode_BER( bool size_only )
{
 if( is_hex_only )
 {
  if( !size_only )
  {
   for( unsigned long long i = 0; i < ( block_length - 1 ); i++ )
    stream_out = value_hex[ i ] | 0x80;

   stream_out = value_hex[ block_length - 1 ];
  }

  return block_length;
 }

 unsigned char* encoded;

 int size = to_base( value_dec, &encoded, 7 );
 if( !size )
 {
  sprintf_s( error, 255, "Error during encoding SID value" );
  free( encoded );
  return 0;
 }

 if( !size_only )
 {
  for( int i = 0; i < ( size - 1 ); i++ )
   stream_out = encoded[ i ] | 0x80;

  stream_out = encoded[ size - 1 ];
 }

 free( encoded );

 return size;
}
//**************************************************************************
template< typename In, typename Out >
struct OID_value_block : virtual public value_block_type< In, Out >
{
 std::vector< SID_value_block< In, Out >* > value;

 virtual ~OID_value_block();

 virtual MSXML2::IXMLDOMElementPtr to_xml( MSXML2::IXMLDOMDocumentPtr pDoc );
 virtual bool from_xml( MSXML2::IXMLDOMElementPtr element );

 virtual In decode_BER( unsigned long long max_block_len );
 virtual unsigned long long encode_BER( bool size_only = false );
};
//**************************************************************************
template< typename In, typename Out >
OID_value_block< In, Out >::~OID_value_block()
{
 for( std::vector< SID_value_block< In, Out >* >::iterator i = value.begin(); i != value.end(); i++ ) 
  delete (*i);
}
//**************************************************************************
template< typename In, typename Out >
MSXML2::IXMLDOMElementPtr OID_value_block< In, Out >::to_xml( MSXML2::IXMLDOMDocumentPtr pDoc )
{
 MSXML2::IXMLDOMElementPtr result = block_type< In, Out >::to_xml( pDoc );
 if( error[ 0 ] != 0x00 )
  return result;

 std::string oid;

 MSXML2::IXMLDOMElementPtr sid_blocks_xml = pDoc->createElement( L"sid_blocks" );

 bool is_hex_only = false;

 for( std::vector< SID_value_block< In, Out >* >::iterator i = value.begin(); i != value.end(); i++ )
 {
  is_hex_only |= (*i)->is_hex_only;

  MSXML2::IXMLDOMElementPtr element = (*i)->to_xml( pDoc );
  sid_blocks_xml->appendChild( element );

  if( i != value.begin() )
   oid += ".";

  if( (*i)->is_hex_only )
  { 
   char* buf = ( char* ) malloc( ( size_t ) ( (*i)->block_length + 1 ) );
   if( !buf )
    return result;

   memcpy( buf, (*i)->value_hex, ( size_t )(*i)->block_length );
   buf[ (*i)->block_length ] = 0x00;

   std::string temp_oid = "{" + hex_codes( (*i)->value_hex, (*i)->block_length ) + "}";

   if( (*i)->is_first_sid )
    oid = "2.{" + temp_oid + " - 80}";
   else
    oid += temp_oid;
   
   free( buf );
  }
  else
  {
   unsigned long long sid = (*i)->value_dec;

   if( (*i)->is_first_sid )
   {
    if( (*i)->value_dec <= 39 )
     oid = "0.";
    else
    {
     if( (*i)->value_dec <= 79 ) 
     {
      oid = "1.";
      sid -= 40;
     }
     else
     {
      oid = "2.";
      sid -= 80;
     }
    }
   }

   char buf[255];
   _ui64toa_s( sid, buf, 255, 10 );
   oid += buf;
  }
 }

 result->appendChild( sid_blocks_xml );

 MSXML2::IXMLDOMElementPtr is_hex_only_xml = pDoc->createElement( L"is_hex_only" );
 is_hex_only_xml->appendChild( pDoc->createTextNode( ( is_hex_only )? L"true" : L"false" ) );
 result->appendChild( is_hex_only_xml );

 wchar_t* whex = ( wchar_t* ) malloc( ( oid.length() + 1 ) * sizeof( wchar_t ) );
 memset( whex, 0x00, ( oid.length() + 1 ) * sizeof( wchar_t ) );

 size_t converted = 0;
 errno_t error_num = mbstowcs_s( &converted, whex, oid.length() + 1, oid.c_str(), oid.length() );
 if( error_num )
 {
  free( whex );
  return NULL;
 }

 MSXML2::IXMLDOMElementPtr value_xml = pDoc->createElement( L"value_dec" );
 value_xml->appendChild( pDoc->createTextNode( whex ) );

 free( whex );
 
 result->appendChild( value_xml );

 return result;
}
//**************************************************************************
template< typename In, typename Out >
bool OID_value_block< In, Out >::from_xml( MSXML2::IXMLDOMElementPtr element )
{
 if( !element )
  return false;

 bool is_hex_only = false;

 MSXML2::IXMLDOMElementPtr is_hex_only_xml = element->selectSingleNode( L"is_hex_only" );
 if( !is_hex_only_xml )
  return false;

 if( is_hex_only_xml->text == ( _bstr_t )L"true" )
  is_hex_only = true;
 else
 {
  if( is_hex_only_xml->text == ( _bstr_t )L"false" )
   is_hex_only = false;
  else
   return false;
 }

 if( is_hex_only )
 {
  MSXML2::IXMLDOMNodeListPtr list = element->selectNodes( L"./sid_blocks/sid_block" );
  
  for( int i = 0; i < list->length; i++ )
  {
   SID_value_block< In, Out >* sid = new SID_value_block< In, Out >();
   sid->from_xml( list->item[ i ] );
   value.push_back( sid );
  }

  return true;
 }

 MSXML2::IXMLDOMNodePtr value_dec_xml = element->selectSingleNode( L"value_dec" );
 if( !value_dec_xml )
  return false;

 std::wstring str_value = ( wchar_t* )value_dec_xml->firstChild->text;
 std::wstring sid;

 std::wstring::size_type pos1 = 0, pos2;
 bool flag = false;

 do
 {
  pos2 = str_value.find( L".", pos1 );
  sid.assign( str_value, pos1, pos2 - pos1 );
  pos1 = pos2 + 1;

  SID_value_block< In, Out >* sid_block = NULL;

  if( flag )
  {
   sid_block = value[ 0 ];
   unsigned long long plus = 0;

   switch( sid_block->value_dec )
   {
    case 0:
     break;
    case 1: 
     plus = 40;
     break;
    case 2:
     plus = 80;
     break;
    default:
     return false;
   }

   sid_block->value_dec = _wtoi64( sid.c_str() ) + plus;

   flag = false;
  }
  else
  {
   sid_block = new SID_value_block< In, Out >();
   sid_block->value_dec = _wtoi64( sid.c_str() );

   if( !value.size() )
   {
    sid_block->is_first_sid = true;
    flag = true;
   }

   value.push_back( sid_block );
  }
   
 }while( pos2 != std::wstring::npos );

 return true;
}
//**************************************************************************
template< typename In, typename Out >
In OID_value_block< In, Out >::decode_BER( unsigned long long max_block_len )
{
 In end = stream_in;

 while( max_block_len )
 {
  if( end == In() )
  {
   sprintf_s( error, 255, "End of input reached before message was fully decoded" );
   return end;
  }

  SID_value_block< In, Out >* sid_type = new SID_value_block< In, Out >(); 
  sid_type->stream_in = end;

  end = sid_type->decode_BER( max_block_len );
  if( sid_type->error[ 0 ] != 0x00 )
  {
   block_length = 0;
   sprintf_s( error, 255, sid_type->error );

   delete sid_type;

   return end;
  }

  if( !(value.size()) )
   sid_type->is_first_sid = true;

  block_length += sid_type->block_length;
  max_block_len -= sid_type->block_length;

  value.push_back( sid_type );

  if( end == In() )
   break;
 }

 return end;
}
//**************************************************************************
template< typename In, typename Out >
unsigned long long OID_value_block< In, Out >::encode_BER( bool size_only )
{
 unsigned long long result = 0;
 unsigned long long encoded = 0;

 for( std::vector< SID_value_block< In, Out >* >::iterator i = value.begin(); i != value.end(); i++ )
 {
  (*i)->stream_out = stream_out;
  encoded = (*i)->encode_BER( size_only );
  if( !encoded )
  {
   sprintf_s( error, 255, (*i)->error );
   return 0;
  }

  result += encoded;
 }

 return result;
}
//**************************************************************************
template< typename In, typename Out >
struct OID_type : virtual public BASIC_type< In, Out >
{
 OID_type();

 static std::string block_name_static();
 virtual std::string block_name();
};
//**************************************************************************
template< typename In, typename Out >
OID_type< In, Out >::OID_type()
{
 tag_class = 1;
 tag_number = 6;

 value_block = new OID_value_block< In, Out >();
}
//**************************************************************************
template< typename In, typename Out >
std::string OID_type< In, Out >::block_name_static()
{
 return "OBJECT IDENTIFIER";
}
//**************************************************************************
template< typename In, typename Out >
std::string OID_type< In, Out >::block_name()
{
 return block_name_static();
}
//**************************************************************************
template< typename In, typename Out >
struct ROID_value_block : virtual public OID_value_block< In, Out >
{
 virtual In decode_BER( unsigned long long max_block_len );
};
//**************************************************************************
template< typename In, typename Out >
In ROID_value_block< In, Out >::decode_BER( unsigned long long max_block_len )
{
 In end = stream_in;

 while( max_block_len )
 {
  if( end == In() )
  {
   sprintf_s( error, 255, "End of input reached before message was fully decoded" );
   return end;
  }

  SID_value_block< In, Out >* sid_type = new SID_value_block< In, Out >(); 
  sid_type->stream_in = end;

  end = sid_type->decode_BER( max_block_len );
  if( sid_type->error[ 0 ] != 0x00 )
  {
   block_length = 0;
   sprintf_s( error, 255, sid_type->error );

   delete sid_type;

   return end;
  }

  block_length += sid_type->block_length;
  max_block_len -= sid_type->block_length;

  value.push_back( sid_type );

  if( end == In() )
   break;
 }

 return end;
}
//**************************************************************************
template< typename In, typename Out >
struct ROID_type : virtual public BASIC_type< In, Out >
{
 ROID_type();

 static std::string block_name_static();
 virtual std::string block_name();
};
//**************************************************************************
template< typename In, typename Out >
ROID_type< In, Out >::ROID_type()
{
 tag_class = 1;
 tag_number = 13;

 value_block = new ROID_value_block< In, Out >();
}
//**************************************************************************
template< typename In, typename Out >
std::string ROID_type< In, Out >::block_name_static()
{
 return "RELATIVE OBJECT IDENTIFIER";
}
//**************************************************************************
template< typename In, typename Out >
std::string ROID_type< In, Out >::block_name()
{
 return block_name_static();
}
//**************************************************************************
#endif