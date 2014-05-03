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
#ifndef _7D997C54_DF84_4A05_8BFA_D7E939FB2264
#define _7D997C54_DF84_4A05_8BFA_D7E939FB2264
//**************************************************************************
template< typename In, typename Out >
struct REAL_information_block : virtual public value_block_type< In, Out >,
                                virtual public hex_block_type< In, Out >
{
 unsigned char base;
 bool is_special_value;
 unsigned char scaling_factor;
 bool is_negative;
 unsigned char exponent_format;
 unsigned char NR_form;

 unsigned char content_value; // value of content for "inf_block" (for using in case of "SpecialValue")

 REAL_information_block();

 virtual std::string block_name();

 virtual MSXML2::IXMLDOMElementPtr to_xml( MSXML2::IXMLDOMDocumentPtr pDoc );
 virtual bool from_xml( MSXML2::IXMLDOMElementPtr element );

 virtual In decode_BER( unsigned long long max_block_len );
 virtual unsigned long long encode_BER( bool size_only = false );
};
//**************************************************************************
template< typename In, typename Out >
REAL_information_block< In, Out >::REAL_information_block() : base( 0 ),
                                                              is_special_value(false),
                                                              scaling_factor(0),
                                                              is_negative(false),
                                                              exponent_format(0),
                                                              NR_form(0),
                                                              content_value(0)
{
}
//**************************************************************************
template< typename In, typename Out >
std::string REAL_information_block< In, Out >::block_name()
{
 return "information_block";
}
//**************************************************************************
template< typename In, typename Out >
MSXML2::IXMLDOMElementPtr REAL_information_block< In, Out >::to_xml( MSXML2::IXMLDOMDocumentPtr pDoc )
{
 MSXML2::IXMLDOMElementPtr result = block_type< In, Out >::to_xml( pDoc );
 if( error[ 0 ] != 0x00 )
  return result;

 MSXML2::IXMLDOMElementPtr is_special_value_xml = pDoc->createElement( L"is_special_value" );
 is_special_value_xml->appendChild( pDoc->createTextNode( ( is_special_value ) ? L"true" : L"false" ) );
 result->appendChild( is_special_value_xml );

 if( is_special_value )
 {
  MSXML2::IXMLDOMElementPtr special_value_xml = pDoc->createElement( L"special_value" );
  switch( content_value )
  {
   case 0x40: // PLUS-INFINITY
    special_value_xml->appendChild( pDoc->createTextNode( L"PLUS-INFINITY" ) );
    break;
   case 0x41: // MINUS-INFINITY
    special_value_xml->appendChild( pDoc->createTextNode( L"MINUS-INFINITY" ) );
    break;
   case 0x42: // NOT-A-NUMBER
    special_value_xml->appendChild( pDoc->createTextNode( L"NOT-A-NUMBER" ) );
    break;
   case 0x43: // minus zero
    special_value_xml->appendChild( pDoc->createTextNode( L"minus zero" ) );
    break;
   default:
    special_value_xml->appendChild( pDoc->createTextNode( L"Unknown special REAL value" ) );
  }

  result->appendChild( special_value_xml );
  
  return result;
 }
 
 MSXML2::IXMLDOMElementPtr base_xml = pDoc->createElement( L"base" );
 switch( base )
 {
  case 2:
   base_xml->appendChild( pDoc->createTextNode( L"2" ) );
   break;
  case 8:
   base_xml->appendChild( pDoc->createTextNode( L"8" ) );
   break;
  case 16:
   base_xml->appendChild( pDoc->createTextNode( L"16" ) );
   break;
  case 10:
   base_xml->appendChild( pDoc->createTextNode( L"10" ) );
   break;
  default:
   return result;
 }
 result->appendChild( base_xml );

 if( base != 10 )
 {
  MSXML2::IXMLDOMElementPtr scaling_factor_xml = pDoc->createElement( L"scaling_factor" );
  switch( scaling_factor )
  {
   case 0:
    scaling_factor_xml->appendChild( pDoc->createTextNode( L"0" ) );
    break;
   case 1:
    scaling_factor_xml->appendChild( pDoc->createTextNode( L"1" ) );
    break;
   case 2:
    scaling_factor_xml->appendChild( pDoc->createTextNode( L"2" ) );
    break;
   case 3:
    scaling_factor_xml->appendChild( pDoc->createTextNode( L"3" ) );
    break;
   default:
    return result;
  }
  result->appendChild( scaling_factor_xml );

  MSXML2::IXMLDOMElementPtr is_negative_xml = pDoc->createElement( L"is_negative" );
  is_negative_xml->appendChild( pDoc->createTextNode( ( is_negative ) ? L"true" : L"false" ) );
  result->appendChild( is_negative_xml );

  MSXML2::IXMLDOMElementPtr exponent_format_xml = pDoc->createElement( L"exponent_format" );
  switch( exponent_format )
  {
   case 1:
    exponent_format_xml->appendChild( pDoc->createTextNode( L"1" ) );
    break;
   case 2:
    exponent_format_xml->appendChild( pDoc->createTextNode( L"2" ) );
    break;
   case 3:
    exponent_format_xml->appendChild( pDoc->createTextNode( L"3" ) );
    break;
   case 4:
    exponent_format_xml->appendChild( pDoc->createTextNode( L"4" ) );
    break;
   default:
    return result;
  }
  result->appendChild( exponent_format_xml );
 }
 else
 {
  MSXML2::IXMLDOMElementPtr NR_form_xml = pDoc->createElement( L"NR_form" );
  switch( NR_form )
  {
   case 1:
    NR_form_xml->appendChild( pDoc->createTextNode( L"1" ) );
    break;
   case 2:
    NR_form_xml->appendChild( pDoc->createTextNode( L"2" ) );
    break;
   case 3:
    NR_form_xml->appendChild( pDoc->createTextNode( L"3" ) );
    break;
   default:
    return result;
  }
  result->appendChild( NR_form_xml );
 }

 return result;
}
//**************************************************************************
template< typename In, typename Out >
bool REAL_information_block< In, Out >::from_xml( MSXML2::IXMLDOMElementPtr element )
{
 if( !element )
  return false;

 MSXML2::IXMLDOMElementPtr is_special_value_xml = element->selectSingleNode( L"is_special_value" );
 if( !is_special_value_xml )
  return false;

 if( is_special_value_xml->text == ( _bstr_t )L"true" )
  is_special_value = true;
 else
 {
  if( is_special_value_xml->text == ( _bstr_t )L"false" )
   is_special_value = false;
  else
   return false;
 }

 if( is_special_value )
 {
  MSXML2::IXMLDOMElementPtr special_value_xml = element->selectSingleNode( L"special_value" );
  if( !special_value_xml )
   return false;

  if( special_value_xml->text == ( _bstr_t )L"PLUS-INFINITY" )
   content_value = 0x40;
  if( special_value_xml->text == ( _bstr_t )L"MINUS-INFINITY" )
   content_value = 0x41;
  if( special_value_xml->text == ( _bstr_t )L"NOT-A-NUMBER" )
   content_value = 0x42;
  if( special_value_xml->text == ( _bstr_t )L"minus zero" )
   content_value = 0x43;
  if( content_value == 0x00 )
   return false;
  else
   return true;
 }

 MSXML2::IXMLDOMElementPtr base_xml = element->selectSingleNode( L"base" );
 if( !base_xml )
  return false;

 base =  _wtoi( ( wchar_t* ) base_xml->text );
 if( base == 0 )
  return false;

 MSXML2::IXMLDOMElementPtr scaling_factor_xml = element->selectSingleNode( L"scaling_factor" );
 if( !scaling_factor_xml )
  return false;

 scaling_factor = _wtoi( ( wchar_t* ) scaling_factor_xml->text );
 if( errno && !scaling_factor )
  return false;

 MSXML2::IXMLDOMElementPtr is_negative_xml = element->selectSingleNode( L"is_negative" );
 if( !is_negative_xml )
  return false;

 if( is_negative_xml->text == ( _bstr_t )L"true" )
  is_negative = true;
 else
 {
  if( is_negative_xml->text == ( _bstr_t )L"false" )
   is_negative = false;
  else
   return false;
 }

 if( base != 10 )
 {
  MSXML2::IXMLDOMElementPtr exponent_format_xml = element->selectSingleNode( L"exponent_format" );
  if( !exponent_format_xml )
   return false;

  exponent_format = _wtoi( ( wchar_t* ) exponent_format_xml->text );
  if( errno && !exponent_format )
   return false;
 }
 else
 {
  MSXML2::IXMLDOMElementPtr NR_form_xml = element->selectSingleNode( L"NR_form" );
  if( !NR_form_xml )
   return false;

  NR_form = _wtoi( ( wchar_t* ) NR_form_xml->text );
  if( errno && !NR_form )
   return false;
 }

 return true;
}
//**************************************************************************
template< typename In, typename Out >
In REAL_information_block< In, Out >::decode_BER( unsigned long long max_block_len )
{
 In end = hex_block_type::decode_BER( 1 );
 if( error[ 0 ] != 0x00 )
  return In();

 //--------------------------------------
 if( ( value_hex[ 0 ] & 0xC0 ) == 0) // decimal encoding
 {
  base = 10;

  switch( value_hex[ 0 ] & 0x3F )
  {
   case 0x01:
    NR_form = 1;
    break;
   case 0x02:
    NR_form = 2;
    break;
   case 0x03:
    NR_form = 3;
    break;
   default:
    NR_form = 0xFF; // impossible case;
    sprintf_s( error, 255, "Wrong NR form" );
    return In();
  }

  return end;
 }

 if( value_hex[ 0 ] & 0x80 ) // binary encoding
 {
  // Has a meaning only in case of binary encoding
  if( value_hex[ 0 ] & 0x40 )
   is_negative = true;
  else
   is_negative = false;

  switch( value_hex[ 0 ] & 0x30 )
  {
   case 0x00:
    base = 2;
    break;
   case 0x10:
    base = 8;
    break;
   case 0x20:
    base = 16;
    break;
   case 0x30:
    block_length = 0;
    sprintf_s( error, 255, "Encoded reserved value with BASE (11)" );
    return In(); // such value must be reserved for future use
    break;
   default:
    base = 0xFF; // impossible case
    sprintf_s( error, 255, "Strange base for REAL" );
    return In(); // such value must be reserved for future use
  }
 }

 if( ( value_hex[ 0 ] & 0x80 ) == 0x00 )
 {
  if( value_hex[ 0 ] & 0x40 ) // special value
  {
   is_special_value = true;
   content_value = value_hex[ 0 ];

   switch( content_value )
   {
    case 0x40: // PLUS-INFINITY
    case 0x41: // MINUS-INFINITY
    case 0x42: // NOT-A-NUMBER
    case 0x43: // minus zero
     break;
    default:
     sprintf_s( error, 255, "Unknown special REAL value" );
     return In();
   }

   if( max_block_len != 1 )
    warnings.push_back( "Special REAL value should be encoded by 1 octet only" );

   return end;
  }
  else
   is_special_value = false;
 }

 scaling_factor = ( value_hex[ 0 ] & 0x0C ) >> 2;

 switch( value_hex[ 0 ] & 0x03 )
 {
  case 0x00:
   exponent_format = 1;
   break;
  case 0x01:
   exponent_format = 2;
   break;
  case 0x02:
   exponent_format = 3;
   break;
  case 0x03:
   exponent_format = 4;
   break;
  default:
   exponent_format = 0xFF; // impossible case
   sprintf_s( error, 255, "Wrong exponent format" );
   return In();
 }
 //--------------------------------------

 return end;
}
//**************************************************************************
template< typename In, typename Out >
unsigned long long REAL_information_block< In, Out >::encode_BER( bool size_only )
{
 unsigned char ch = 0;

 if( is_special_value )
  if( !size_only )
   stream_out = content_value;
  else
   return 1;

 switch( base )
 {
  case 2:
   ch |= 0x80;
   break;
  case 8:
   ch |= 0x90;
   break;
  case 16:
   ch |= 0xA0;
   break;
  case 10:
   switch( NR_form )
   {
    case 1:
     ch = 0x01;
     break;
    case 2:
     ch = 0x02;
     break;
    case 3:
     ch = 0x03;
     break;
    default:
     sprintf_s( error, 255, "Wrong \"NR_form\" value for encoding" );
     return 0;
   }
   break;
  default:
   sprintf_s( error, 255, "Wrong \"base\" value for encoding" );
   return 0;
 }

 if( base != 10 )
 {
  if( is_negative )
   ch |= 0x40;

  switch( scaling_factor )
  {
   case 0:
    break;
   case 1:
    ch |= 0x04;
    break;
   case 2:
    ch |= 0x08;
    break;
   case 3:
    ch |= 0x0C;
    break;
   default:
   sprintf_s( error, 255, "Wrong \"scaling_factor\" value for encoding" );
   return 0;
  }

  switch( exponent_format )
  {
   case 1:
    break;
   case 2:
    ch |= 0x01;
    break;
   case 3:
    ch |= 0x02;
    break;
   case 4:
    ch |= 0x03;
    break;
   default:
   sprintf_s( error, 255, "Wrong \"exponent_format\" value for encoding" );
   return 0;
  }
 }

 if( !size_only )
 {
  stream_out = ch;
  return 1;
 }

 return 1;
}
//**************************************************************************

template< typename In, typename Out >
struct REAL_exponent_block : virtual public value_block_type< In, Out >,
                             virtual public hex_block_type< In, Out >
{
 long long value_dec;
 unsigned char exponent_format;

 REAL_exponent_block();
 REAL_exponent_block( unsigned char _exponent_format );

 virtual std::string block_name();

 virtual MSXML2::IXMLDOMElementPtr to_xml( MSXML2::IXMLDOMDocumentPtr pDoc );
 virtual bool from_xml( MSXML2::IXMLDOMElementPtr element );

 virtual In decode_BER( unsigned long long max_block_len );
 virtual unsigned long long encode_BER( bool size_only = false );
};
//**************************************************************************
template< typename In, typename Out >
REAL_exponent_block< In, Out >::REAL_exponent_block() : value_dec( 0 ),
                                                        exponent_format( 0 )
{
}
//**************************************************************************
template< typename In, typename Out >
REAL_exponent_block< In, Out >::REAL_exponent_block( unsigned char _exponent_format ) : value_dec( 0 ),
                                                                                        exponent_format( _exponent_format )
{
}
//**************************************************************************
template< typename In, typename Out >
std::string REAL_exponent_block< In, Out >::block_name()
{
 return "exponent_block";
}
//**************************************************************************
template< typename In, typename Out >
MSXML2::IXMLDOMElementPtr REAL_exponent_block< In, Out >::to_xml( MSXML2::IXMLDOMDocumentPtr pDoc )
{
 MSXML2::IXMLDOMElementPtr result = block_type< In, Out >::to_xml( pDoc );
 if( error[ 0 ] != 0x00 )
  return result;

 MSXML2::IXMLDOMElementPtr is_hex_only_xml = pDoc->createElement( L"is_hex_only" );
 is_hex_only_xml->appendChild( pDoc->createTextNode( ( is_hex_only ) ? L"true" : L"false" ) );
 result->appendChild( is_hex_only_xml );

 if( is_hex_only )
 {
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
 }
 else
 {
  MSXML2::IXMLDOMElementPtr value_dec_xml = pDoc->createElement( L"value_dec" );

  wchar_t value_dec_buf[ 255 ];
  memset( value_dec_buf, 0x00, 255 * sizeof( wchar_t ) );

  errno_t error_num = _i64tow_s( value_dec, value_dec_buf, 255, 10 );
  if( error_num )
   return result;

  value_dec_xml->appendChild( pDoc->createTextNode( value_dec_buf ) );

  result->appendChild( value_dec_xml );
 }

 return result;
}
//**************************************************************************
template< typename In, typename Out >
bool REAL_exponent_block< In, Out >::from_xml( MSXML2::IXMLDOMElementPtr element )
{
 if( !element )
  return false;

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

 if( !is_hex_only )
 {
  MSXML2::IXMLDOMElementPtr value_dec_xml = element->selectSingleNode( L"value_dec" );
  if( !value_dec_xml )
   return false;

  value_dec = _wtoi64( ( wchar_t* ) value_dec_xml->text );
  if( errno && !value_dec )
   return false;
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

 return true;
}
//**************************************************************************
template< typename In, typename Out >
In REAL_exponent_block< In, Out >::decode_BER( unsigned long long max_block_len )
{
 switch( exponent_format )
 {
  case 1: // simple form
   max_block_len = 1;
   break;
  case 2: // two octets encode exponent
   max_block_len = 2;
   break;
  case 3: // three octets encode exponent
   max_block_len = 3;
   break;
  case 4: // next octet encode length of exponent block
   max_block_len = (unsigned char)*stream_in;
   max_block_len++;

   if( max_block_len > 8 ) // unable to decode such big integer
   {
    is_hex_only = true;
    warnings.push_back( "Too big REAL exponent block for decoding, hex only" );
   }

   break;
  default: // something wrong
   value_dec = 0;
   block_length = 0;
   sprintf_s( error, 255, "Strange value of exponent format" );
   return In();
 }

 In end = hex_block_type::decode_BER( max_block_len );
 if( error[ 0 ] != 0x00 )
  return In();

 if( !is_hex_only )
  value_dec = decode_tc( value_hex + ( ( exponent_format == 4 ) ? 1 : 0 ), 
                         ( unsigned char )( block_length - ( ( exponent_format == 4 ) ? 1 : 0 ) ), 
                         &warnings );

 return end;
}
//**************************************************************************
template< typename In, typename Out >
unsigned long long REAL_exponent_block< In, Out >::encode_BER( bool size_only )
{
 if( !is_hex_only )
 {
  unsigned char* encoded;
  int size = encode_tc( value_dec, &encoded );

  if( !size_only )
  {
   for( int i = 0; i < size; i++ )
    stream_out = encoded[ i ];
  }

  free( encoded );

  return size;
 }
 else
 {
  if( size_only )
   return block_length;
  
  for( unsigned long long i = 0; i < block_length; i++ )
   stream_out = value_hex[ i ];

  return block_length;
 }

 return 0;
}
//**************************************************************************

template< typename In, typename Out >
struct REAL_mantissa_block : virtual public value_block_type< In, Out >,
                             virtual public hex_block_type< In, Out >
{
 unsigned long long value_dec;

 unsigned char base;
 unsigned char scaling_factor;
 unsigned char NR_form;

 REAL_mantissa_block();
 REAL_mantissa_block( unsigned char _base, 
                      unsigned char _scaling_factor,
                      unsigned char _NR_form );

 virtual std::string block_name();

 virtual MSXML2::IXMLDOMElementPtr to_xml( MSXML2::IXMLDOMDocumentPtr pDoc );
 virtual bool from_xml( MSXML2::IXMLDOMElementPtr element );

 virtual In decode_BER( unsigned long long max_block_len );
 virtual unsigned long long encode_BER( bool size_only = false );
};
//**************************************************************************
template< typename In, typename Out >
REAL_mantissa_block< In, Out >::REAL_mantissa_block() : value_dec( 0 ),
                                                        base( 0 ),
                                                        scaling_factor( 0 ),
                                                        NR_form( 0 )
{
}
//**************************************************************************
template< typename In, typename Out >
REAL_mantissa_block< In, Out >::REAL_mantissa_block( unsigned char _base,
                                                     unsigned char _scaling_factor,
                                                     unsigned char _NR_form ) : value_dec( 0 ),
                                                                                base( _base ),
                                                                                scaling_factor( _scaling_factor ),
                                                                                NR_form( _NR_form )
{
}
//**************************************************************************
template< typename In, typename Out >
std::string REAL_mantissa_block< In, Out >::block_name()
{
 return "mantissa_block";
}
//**************************************************************************
template< typename In, typename Out >
MSXML2::IXMLDOMElementPtr REAL_mantissa_block< In, Out >::to_xml( MSXML2::IXMLDOMDocumentPtr pDoc )
{
 MSXML2::IXMLDOMElementPtr result = block_type< In, Out >::to_xml( pDoc );
 if( error[ 0 ] != 0x00 )
  return result;

 MSXML2::IXMLDOMElementPtr is_hex_only_xml = pDoc->createElement( L"is_hex_only" );
 is_hex_only_xml->appendChild( pDoc->createTextNode( ( is_hex_only ) ? L"true" : L"false" ) );
 result->appendChild( is_hex_only_xml );

 if( is_hex_only )
 {
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
  result->appendChild( value_hex_xml );
  
  free( whex );
 }
 else
 {
  MSXML2::IXMLDOMElementPtr value_dec_xml = pDoc->createElement( L"value_dec" );

  wchar_t value_dec_buf[ 255 ];
  memset( value_dec_buf, 0x00, 255 * sizeof( wchar_t ) );

  errno_t error_num = _ui64tow_s( value_dec, value_dec_buf, 255, 10 );
  if( error_num )
   return result;

  value_dec_xml->appendChild( pDoc->createTextNode( value_dec_buf ) );

  result->appendChild( value_dec_xml );
 }

 return result;
}
//**************************************************************************
template< typename In, typename Out >
bool REAL_mantissa_block< In, Out >::from_xml( MSXML2::IXMLDOMElementPtr element )
{
 if( !element )
  return false;

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

 if( !is_hex_only )
 {
  MSXML2::IXMLDOMElementPtr value_dec_xml = element->selectSingleNode( L"value_dec" );
  if( !value_dec_xml )
   return false;

  value_dec = _wtoi64( ( wchar_t* ) value_dec_xml->text );
  if( errno && !value_dec )
   return false;

  block_length = to_base( value_dec, &value_hex, 8 );
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

 return true;
}
//**************************************************************************
template< typename In, typename Out >
In REAL_mantissa_block< In, Out >::decode_BER( unsigned long long max_block_len )
{
 In end = hex_block_type::decode_BER(  max_block_len );
 if( error[ 0 ] != 0x00 )
  return In();

 if( base != 10 ) // binary format only
 {
  if( block_length > 8 )
  {
   warnings.push_back( "Too big REAL mantissa for decoding, hex only" );
   is_hex_only = true;
  }
  else
   from_base( value_hex, &value_dec, 8, ( unsigned char )block_length );
 }
 else // decimal format
 {
  is_hex_only = true;
  
  char decimal[ 255 ];  
  memset( decimal, 0x00, 255 );
  
  memcpy( decimal, value_hex, ( size_t )block_length );
  
  std::string decimal_string = decimal;
  trim( decimal_string ); // trimming all space characters
  
  std::string sign = "";
  
  std::string::size_type pos = decimal_string.find_first_of( "+-" );
  if( pos != std::string::npos )
  {
   if( pos == 0 )
   {
    sign.assign( decimal_string, pos, 1 );
    decimal_string.assign( decimal_string, 1, decimal_string.length() - 1 ); 
   }
  }
  
  trim( decimal_string, "0", 0 ); // trim all leading zeros

  if( !decimal_string.length() )
   warnings.push_back( "REAL values for +0 and -0 can not be encoded in decimal format" );
  
  std::string admitted = "+-0123456789";
    
  switch( NR_form )
  {
   case 1:
    if( decimal_string.find_first_not_of( admitted ) != std::string::npos )
     warnings.push_back( "Wrong encoded NR1 form" );

    break;
   case 2:
    admitted += ".,";

    if( decimal_string.find_first_not_of( admitted ) != std::string::npos )
     warnings.push_back( "Wrong encoded NR2 form" );

    if( decimal_string.find_first_of( ".," ) == std::string::npos )
     warnings.push_back( "NR2 form must has a decimal mark" );
    
    break;
   case 3:
    admitted += ".,Ee";

    if( decimal_string.find_first_not_of( admitted ) != std::string::npos )
     warnings.push_back( "Wrong encoded NR3 form" );
    
    if( decimal_string.find_first_of( ".," ) == std::string::npos )
     warnings.push_back( "NR3 form must has a decimal mark" );
    
    if( decimal_string.find_first_of( "eE" ) == std::string::npos )
     warnings.push_back( "NR3 form must has a exponent mark" );

    break;
   default:
    ;
  }
  
  decimal_string = sign + decimal_string;
  
  memset( value_hex, 0x00, ( size_t )block_length );
  memcpy( value_hex, decimal_string.c_str(), decimal_string.length() );

  block_length = decimal_string.length();
 }

 return stream_in;
}
//**************************************************************************
template< typename In, typename Out >
unsigned long long REAL_mantissa_block< In, Out >::encode_BER( bool size_only )
{
 if( !is_hex_only )
 {
  if( !block_length )
   return 0;

  unsigned char* encoded;
  int size = to_base( value_dec, &encoded, 8 );

  if( !size_only )
  {
   for( int i = 0; i < size; i++ )
    stream_out = encoded[ i ];
  }

  free( encoded );

  return size;
 }
 else
 {
  if( size_only )
   return block_length;
  
  for( unsigned long long i = 0; i < block_length; i++ )
   stream_out = value_hex[ i ];

  return block_length;
 }

 return 0;
}
//**************************************************************************

template< typename In, typename Out >
struct REAL_value_block : virtual public value_block_type< In, Out >
{
 double value_dec;

 REAL_information_block< In, Out >* real_inf_block;
 REAL_exponent_block< In, Out >* real_exp_block;
 REAL_mantissa_block< In, Out >* real_mantissa_block;

 REAL_value_block();
 virtual ~REAL_value_block();

 virtual std::string block_name();

 virtual MSXML2::IXMLDOMElementPtr to_xml( MSXML2::IXMLDOMDocumentPtr pDoc );
 virtual bool from_xml( MSXML2::IXMLDOMElementPtr element );

 virtual In decode_BER( unsigned long long max_block_len );
 virtual unsigned long long encode_BER( bool size_only = false );

 virtual void calculate_value();
};
//**************************************************************************
template< typename In, typename Out >
REAL_value_block< In, Out >::REAL_value_block() : value_dec( 0 ),
                                                  real_inf_block( NULL ),
                                                  real_exp_block( NULL ),
                                                  real_mantissa_block( NULL )
{
}
//**************************************************************************
template< typename In, typename Out >
REAL_value_block< In, Out >::~REAL_value_block()
{
 if( real_inf_block != NULL )
  delete real_inf_block;

 if( real_exp_block != NULL )
  delete real_exp_block;

 if( real_mantissa_block != NULL )
  delete real_mantissa_block;
}
//**************************************************************************
template< typename In, typename Out >
std::string REAL_value_block< In, Out >::block_name()
{
 return "value_block";
}
//**************************************************************************
template< typename In, typename Out >
MSXML2::IXMLDOMElementPtr REAL_value_block< In, Out >::to_xml( MSXML2::IXMLDOMDocumentPtr pDoc )
{
 MSXML2::IXMLDOMElementPtr result = block_type< In, Out >::to_xml( pDoc );

 if( real_inf_block == NULL )
  return result;

 result->appendChild( real_inf_block->to_xml( pDoc ) );
 if( real_inf_block->is_special_value )
  return result;

 if( real_inf_block->error[ 0 ] != 0x00 )
  return result;
  
 if( real_exp_block != NULL )
 {
  result->appendChild( real_exp_block->to_xml( pDoc ) );
  if( real_exp_block->error[ 0 ] != 0x00 )
   return result;
 }

 if( real_mantissa_block != NULL )
 {
  result->appendChild( real_mantissa_block->to_xml( pDoc ) );
  if( real_mantissa_block->error[ 0 ] != 0x00 )
   return result;
 }

 bool condition = real_inf_block->is_hex_only;
 condition |= ( real_exp_block == NULL ) ? false : real_exp_block->is_hex_only;
 condition |= ( real_mantissa_block == NULL ) ? false : real_mantissa_block->is_hex_only;

 if( !condition ) 
 {
  MSXML2::IXMLDOMElementPtr value_dec_xml = pDoc->createElement( "value_dec" );

  wchar_t wbuf[255];
  swprintf_s( wbuf, 255, L"%f", value_dec );

  value_dec_xml->appendChild( pDoc->createTextNode( wbuf ) );
  result->appendChild( value_dec_xml );
 }
 else
 {
  std::string encoding;
  MSXML2::IXMLDOMElementPtr value_hex_xml = pDoc->createElement( "value_hex" );

  if( real_inf_block->is_special_value )
  {
   switch( real_inf_block->content_value )
   {
    case 0x40: // PLUS-INFINITY
     encoding += "PLUS-INFINITY";

     break;
    case 0x41: // MINUS-INFINITY
     encoding += "MINUS-INFINITY";

     break;
    case 0x42: // NOT-A-NUMBER
     encoding += "NOT-A-NUMBER";

     break;
    case 0x43: // minus zero
     encoding += "minus zero";

     break;
    default:
     encoding += "Unknown special REAL value";
   }
  }
  else
  {
   if( real_inf_block->base == 10 )
   {
    char* value = ( char* ) malloc( ( size_t )( real_mantissa_block->block_length + 1 ) );
    if( !value )
     return result;

    memcpy( value, real_mantissa_block->value_hex, ( size_t )real_mantissa_block->block_length );
    value[ real_mantissa_block->block_length ] = 0x00;

    encoding += value;

    free( value );
   }
   else
   {
    if( real_mantissa_block->is_hex_only )
     encoding += "{" + hex_codes( real_mantissa_block->value_hex, real_mantissa_block->block_length ) + "}";
    else
    {
     char value_dec_buf[ 255 ];
     memset( value_dec_buf, 0x00, 255 );

     errno_t error_num = _ui64toa_s( real_mantissa_block->value_dec, value_dec_buf, 255, 10 );
     if( error_num )
      return result;

     encoding += value_dec_buf;
    }

    encoding += " * ";

    switch( real_inf_block->base )
    {
     case 2:
      encoding += "2^";
      break;
     case 8:
      encoding += "8^";
      break;
     case 16:
      encoding += "16^";
      break;
     default:
      ;
    }

    if( real_exp_block->is_hex_only )
     encoding += "{" + hex_codes( real_exp_block->value_hex, real_exp_block->block_length ) + "}";
    else
    {
     char value_dec_buf[ 255 ];
     memset( value_dec_buf, 0x00, 255 );

     errno_t error_num = _i64toa_s( real_exp_block->value_dec, value_dec_buf, 255, 10 );
     if( error_num )
      return result;

     encoding += value_dec_buf;
    }

    if( ( real_inf_block->scaling_factor ) && ( real_mantissa_block->is_hex_only ) )
    {
     char buf[ 2 ];
     buf[ 0 ] = 0x30 | real_inf_block->scaling_factor;
     buf[ 1 ] = 0x00;

     encoding += " * 2^"; 
     encoding += buf;
    }
   }
  }


  wchar_t* whex = ( wchar_t* ) malloc( ( encoding.length() + 1 ) * sizeof( wchar_t ) );
  memset( whex, 0x00, ( encoding.length() + 1 ) * sizeof( wchar_t ) );

  size_t converted = 0;
  errno_t error_num = mbstowcs_s( &converted, whex, encoding.length() + 1, encoding.c_str(), encoding.length() );
  if( error_num )
  {
   free( whex );
   return result;
  }

  value_hex_xml->appendChild( pDoc->createTextNode( whex ) );
  result->appendChild( value_hex_xml );
  
  free( whex );
 }

 return result;
}
//**************************************************************************
template< typename In, typename Out >
bool REAL_value_block< In, Out >::from_xml( MSXML2::IXMLDOMElementPtr element )
{
 if( !element )
  return false;

 real_inf_block = new REAL_information_block< In, Out >();
 if( !real_inf_block->from_xml( element->selectSingleNode( L"information_block" ) ) )
  return false;

 if( real_inf_block->is_special_value )
 {
  real_exp_block = new REAL_exponent_block< In, Out >();
  real_mantissa_block = new REAL_mantissa_block< In, Out >();

  return true;
 }

 real_exp_block = new REAL_exponent_block< In, Out >();
 if( real_inf_block->base != 10 )
  if( !real_exp_block->from_xml( element->selectSingleNode( L"exponent_block" ) ) )
   return false;

 real_mantissa_block = new REAL_mantissa_block< In, Out >();
 if( !real_mantissa_block->from_xml( element->selectSingleNode( L"mantissa_block" ) ) )
  return false;

 MSXML2::IXMLDOMElementPtr value_dec_xml = element->selectSingleNode( L"value_dec" );
 if( value_dec_xml )
 {
  value_dec = wcstod( ( wchar_t* ) value_dec_xml->text, NULL );
  if( errno )
   return false;
 }
 else
 {
  MSXML2::IXMLDOMElementPtr value_hex_xml = element->selectSingleNode( L"value_hex" );
  if( !value_hex_xml )
   return false;

  // ------------- !!! ADD METHOD !!! ------------------
 }

 return true;
}
//**************************************************************************
template< typename In, typename Out >
In REAL_value_block< In, Out >::decode_BER( unsigned long long max_block_len )
{
 if( !max_block_len ) // REAL +0 value
 {
  real_inf_block = new REAL_information_block< In, Out >();
  if( !real_inf_block )
   return In();

  real_inf_block->base = 10;

  real_mantissa_block = new REAL_mantissa_block< In, Out >();
  if( !real_mantissa_block )
   return In();

  real_mantissa_block->value_hex = ( unsigned char* ) malloc( 3 );
  if( !real_mantissa_block->value_hex )
   return In();

  real_mantissa_block->value_hex[ 0 ] = 0x2B; // +
  real_mantissa_block->value_hex[ 1 ] = 0x30; // 0
  real_mantissa_block->value_hex[ 2 ] = 0x00;

  return stream_in;
 }

 #pragma warning( push )
 #pragma warning( disable : 6011 )

 real_inf_block = new REAL_information_block< In, Out >();
 if( !real_inf_block )
  return In();

 real_inf_block->stream_in = stream_in;

 In end = real_inf_block->decode_BER( max_block_len );
 if( real_inf_block->error[ 0 ] != 0x00 )
 {
  block_length = 0;
  sprintf_s( error, 255, real_inf_block->error );
  return end;
 }

 #pragma warning( pop )

 max_block_len -= real_inf_block->block_length;

 if( real_inf_block->is_special_value )
  return end;

 if( real_inf_block->base != 10 )
 {
  real_exp_block = new REAL_exponent_block< In, Out >( real_inf_block->exponent_format );
  if( !real_exp_block )
   return In();

  real_exp_block->stream_in = end;

  end = real_exp_block->decode_BER( max_block_len );
  if( real_exp_block->error[ 0 ] != 0x00 )
  {
   sprintf_s( error, 255, real_exp_block->error );
   return end;
  }

  max_block_len -= real_exp_block->block_length;
 }

 real_mantissa_block = new REAL_mantissa_block< In, Out >( real_inf_block->base,
                                                           real_inf_block->scaling_factor,
                                                           real_inf_block->NR_form );
 if( !real_mantissa_block )
  return In();

 real_mantissa_block->stream_in = end;

 end = real_mantissa_block->decode_BER( max_block_len );
 if( real_mantissa_block->error[ 0 ] != 0x00 )
 {
  sprintf_s( error, 255, real_mantissa_block->error );
  return end;
 }

 calculate_value();

 block_length = real_mantissa_block->block_length + ( (real_exp_block == NULL) ? 0 : real_exp_block->block_length ) + real_inf_block->block_length;

 return end;
}
//**************************************************************************
template< typename In, typename Out >
unsigned long long REAL_value_block< In, Out >::encode_BER( bool size_only )
{
 unsigned long long result = 0;

 if( ( !real_inf_block ) || ( !real_mantissa_block ) )
  return 0;

 real_inf_block->stream_out = stream_out;
 result += real_inf_block->encode_BER( size_only );

 if( real_inf_block->is_special_value )
  return result;

 if( real_inf_block->base != 10 )
 {
  if( !real_exp_block )
   return 0;

  real_exp_block->stream_out = stream_out;
  result += real_exp_block->encode_BER( size_only );
 }

 real_mantissa_block->stream_out = stream_out;
 result += real_mantissa_block->encode_BER( size_only );

 return result;
}
//**************************************************************************
template< typename In, typename Out >
void REAL_value_block< In, Out >::calculate_value()
{
 if( real_inf_block->is_special_value )
  return; // Nothing to do

 if( real_inf_block->base != 10 )
 {
  //-----------------------------------------
  // special "pow" realization
  unsigned long long mult = real_inf_block->base;

  unsigned char shift = 0;
  switch( real_inf_block->base )
  {
   case 2:
    shift = 1;
    break;
   case 8:
    shift = 3;
    break;
   case 16:
    shift = 4;
    break;
   default:
    ;
  }

  unsigned long long mod_exponent = ( real_exp_block->value_dec > 0 ) ? ( real_exp_block->value_dec ) : ( -real_exp_block->value_dec );

  for( unsigned long long i = 2; i <= mod_exponent; i++ )
   mult <<= shift;
  //-----------------------------------------

  value_dec = ( real_mantissa_block->value_dec << real_inf_block->scaling_factor ) * ( ( real_exp_block->value_dec > 0 ) ? ( mult ) : ( 1 / (double)mult ) );

  if( real_inf_block->is_negative )
   value_dec *= (-1);
 }
 else
 {
  char* temporary = ( char* ) malloc( ( size_t ) ( real_mantissa_block->block_length + 1 ) );
  if( !temporary )
   return;
  memcpy( temporary, real_mantissa_block->value_hex, ( size_t )real_mantissa_block->block_length );
  temporary[ real_mantissa_block->block_length ] = 0x00;

  //----------------------
  // Special case for ",15625" etc
  if( temporary[ 0 ] == 0x2C )
  {
   temporary[ 0 ] = 0x2E;
   value_dec = strtod( (char*)temporary, NULL);
  }
  else
   value_dec = strtod( (char*)temporary, NULL );
   
  if( value_dec == 0 )
  {
   char decimal_error[ 255 ];
   memset( decimal_error, 0x00, 255 );
   
   if( errno ) 
    warnings.push_back( "Wrong encoded REAL in decimal form" );
   else
    warnings.push_back( "REAL values for +0 and -0 can not be encoded in decimal format" );
  }

  free( temporary );
 }
}
//**************************************************************************
template< typename In, typename Out >
struct REAL_type : virtual public BASIC_type< In, Out >
{
 REAL_type();

 static std::string block_name_static();
 virtual std::string block_name();
};
//**************************************************************************
template< typename In, typename Out >
REAL_type< In, Out >::REAL_type()
{
 tag_class = 1;
 tag_number = 9;

 value_block = new REAL_value_block< In, Out >();
}
//**************************************************************************
template< typename In, typename Out >
std::string REAL_type< In, Out >::block_name_static()
{
 return "REAL";
}
//**************************************************************************
template< typename In, typename Out >
std::string REAL_type< In, Out >::block_name()
{
 return block_name_static();
}
//**************************************************************************
#endif