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
#ifndef _37D8F15A_05F2_404A_A762_3B882EB9FE3C
#define _37D8F15A_05F2_404A_A762_3B882EB9FE3C
//**************************************************************************
#pragma region block_type
//********************************************************************************************
template< typename In, typename Out >
struct block_type
{
 In stream_in;
 Out stream_out;

 block_type();
 block_type( In _stream_in, Out _stream_out );
 virtual ~block_type();

 unsigned long long block_length;

 char error[ 255 ];
 std::vector< std::string > warnings;

 virtual std::string block_name();
 virtual MSXML2::IXMLDOMElementPtr to_xml( MSXML2::IXMLDOMDocumentPtr pDoc );
 virtual bool from_xml( MSXML2::IXMLDOMElementPtr element ) = 0;

 virtual In decode_BER( unsigned long long max_block_len ) = 0;
 virtual unsigned long long encode_BER( bool size_only = false ) = 0;
};
//**************************************************************************
template< typename In, typename Out >
block_type< In, Out >::block_type( In _stream_in, Out _stream_out ) : block_length( 0 )
{
 stream_in = _stream_in;
 stream_out = _stream_out;

 memset( error, 0x00, 255 );
}
//**************************************************************************
template< typename In, typename Out >
block_type< In, Out >::~block_type()
{
}
//**************************************************************************
template< typename In, typename Out >
block_type< In, Out >::block_type() : block_length( 0 ),
                                      stream_out( NULL )
{
 memset( error, 0x00, 255 );
}
//**************************************************************************
template< typename In, typename Out >
std::string block_type< In, Out >::block_name()
{
 return "basic_block";
}
//**************************************************************************
template< typename In, typename Out >
MSXML2::IXMLDOMElementPtr block_type< In, Out >::to_xml( MSXML2::IXMLDOMDocumentPtr pDoc )
{
 std::string name = block_name();
 name = clear_xml_tag( name );

 wchar_t name_buf[255];
 size_t converted = 0;
 errno_t error_num = mbstowcs_s( &converted, name_buf, 255, name.c_str(), 255 );
 if( error_num )
 {
  sprintf_s( error, 255, "Wrong name convertion" );
  return NULL;
 }

 MSXML2::IXMLDOMElementPtr result = pDoc->createElement( name_buf );

 if( warnings.size() )
 {
  MSXML2::IXMLDOMElementPtr warnings_xml = pDoc->createElement( L"warnings" );

  for( std::vector< std::string >::iterator i = warnings.begin(); i != warnings.end(); i++ )
  {
   MSXML2::IXMLDOMElementPtr warning = pDoc->createElement( L"warning" );

   wchar_t warning_buf[255];
   converted = 0;
   error_num = mbstowcs_s( &converted, warning_buf, 255, (*i).c_str(), 255 );
   if( error_num )
    return NULL;

   warning->appendChild( pDoc->createTextNode( warning_buf) );
   warnings_xml->appendChild( warning );
  }

  result->appendChild( warnings_xml );
 }

 if( error[ 0 ] != 0x00 )
 {
  MSXML2::IXMLDOMElementPtr error_xml = pDoc->createElement( L"error" );

  wchar_t error_buf[255];
  converted = 0;
  error_num = mbstowcs_s( &converted, error_buf, 255, error, 255 );
  if( error_num )
   return NULL;

  error_xml->appendChild( pDoc->createTextNode( error_buf ) );

  result->appendChild( error_xml );
 }

 return result;
}
//**************************************************************************
#pragma endregion
//**************************************************************************
#pragma region hex_block_type
//********************************************************************************************
template< typename In, typename Out >
struct hex_block_type : virtual public block_type< In, Out >
{
 bool is_hex_only;
 unsigned char* value_hex;
 unsigned long long value_hex_len;

 hex_block_type();
 virtual ~hex_block_type();

 virtual MSXML2::IXMLDOMElementPtr to_xml( MSXML2::IXMLDOMDocumentPtr pDoc );
 virtual bool from_xml( MSXML2::IXMLDOMElementPtr element );

 virtual In decode_BER( unsigned long long max_block_len );
 virtual unsigned long long encode_BER( bool size_only = false );
};
//**************************************************************************
template< typename In, typename Out >
hex_block_type< In, Out >::hex_block_type() : value_hex( NULL ),
                                              value_hex_len( 0 ),
                                              is_hex_only( false )

{
}
//**************************************************************************
template< typename In, typename Out >
hex_block_type< In, Out >::~hex_block_type()
{
 if( value_hex != NULL )
  free( value_hex );
}
//**************************************************************************
template< typename In, typename Out >
MSXML2::IXMLDOMElementPtr hex_block_type< In, Out >::to_xml( MSXML2::IXMLDOMDocumentPtr pDoc )
{
 return NULL;
}
//**************************************************************************
template< typename In, typename Out >
bool hex_block_type< In, Out >::from_xml( MSXML2::IXMLDOMElementPtr element )
{
 return false;
}
//**************************************************************************
template< typename In, typename Out >
In hex_block_type< In, Out >::decode_BER( unsigned long long max_block_len )
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
  return stream_in;

 In end = stream_in;
 unsigned char* data = value_hex;

 while( block_length < max_block_len )
 {
  if( end == In() )
  {
   sprintf_s( error, 255, "End of input reached before message was fully decoded" );
   return end;
  }

  *data = (unsigned char)(*end);

  data++;
  end++;
  block_length++;
 }

 return end;
}
//**************************************************************************
template< typename In, typename Out >
unsigned long long hex_block_type< In, Out >::encode_BER( bool size_only )
{
 if( !is_hex_only )
  return 0;

 if( size_only )
  return block_length;

 for( unsigned long long i = 0; i < block_length; i++ )
  stream_out = value_hex[ i ];

 return block_length;
}
//**************************************************************************
#pragma endregion
//**************************************************************************
#pragma region identification_block_type
//********************************************************************************************
template< typename In, typename Out >
struct identification_block_type : virtual public hex_block_type< In, Out >
{
 unsigned char tag_class;
 unsigned long long tag_number;
 bool is_constructed;

 identification_block_type();

 virtual std::string block_name();

 virtual MSXML2::IXMLDOMElementPtr to_xml( MSXML2::IXMLDOMDocumentPtr pDoc );
 virtual bool from_xml( MSXML2::IXMLDOMElementPtr element );

 virtual In decode_BER( unsigned long long max_block_len );
 virtual unsigned long long encode_BER( bool size_only = false );
};
//**************************************************************************
template< typename In, typename Out >
identification_block_type< In, Out >::identification_block_type() : tag_class( 0 ),
                                                                    tag_number( 0 ),
                                                                    is_constructed( false )
{
}
//**************************************************************************
template< typename In, typename Out >
std::string identification_block_type< In, Out >::block_name()
{
 return "identification_block";
}
//**************************************************************************
template< typename In, typename Out >
MSXML2::IXMLDOMElementPtr identification_block_type< In, Out >::to_xml( MSXML2::IXMLDOMDocumentPtr pDoc )
{
 MSXML2::IXMLDOMElementPtr result = block_type< In, Out >::to_xml( pDoc );
 if( error[ 0 ] != 0x00 )
  return result;

 wchar_t* tag_class_buf = NULL;

 switch( tag_class )
 {
  case 1:
   tag_class_buf = L"UNIVERSAL";
   break;
  case 2:
   tag_class_buf = L"APPLICATION";
   break;
  case 3:
   tag_class_buf = L"CONTEXT_SPECIFIC";
   break;
  case 4:
   tag_class_buf = L"PRIVATE";
   break;
  default:
   return result;
 }

 MSXML2::IXMLDOMElementPtr tag_class_xml = pDoc->createElement( L"tag_class" );
 tag_class_xml->appendChild( pDoc->createTextNode( tag_class_buf ) );

 result->appendChild( tag_class_xml );

 MSXML2::IXMLDOMElementPtr is_constructed_xml = pDoc->createElement( L"is_constructed" );
 is_constructed_xml->appendChild( pDoc->createTextNode( ( is_constructed ) ? L"true" : L"false" ) );

 result->appendChild( is_constructed_xml );

 MSXML2::IXMLDOMElementPtr tag_number_xml;
 if( !is_hex_only )
 {
  tag_number_xml = pDoc->createElement( L"value_dec" );

  wchar_t tag_number_buf[ 255 ];
  memset( tag_number_buf, 0x00, 255 * sizeof( wchar_t ) );

  errno_t error_num = _ui64tow_s( tag_number, tag_number_buf, 255, 10 );
  if( error_num )
   return result;
   
  tag_number_xml->appendChild( pDoc->createTextNode( tag_number_buf ) );
 }
 else
 {
  tag_number_xml = pDoc->createElement( L"value_hex" );

  std::string hex = hex_codes( value_hex, value_hex_len );

  wchar_t* whex = ( wchar_t* ) malloc( ( hex.length() + 1 ) * sizeof( wchar_t ) );
  memset( whex, 0x00, ( hex.length() + 1 ) * sizeof( wchar_t ) );

  size_t converted = 0;
  errno_t error_num = mbstowcs_s( &converted, whex, hex.length() + 1, hex.c_str(), hex.length() );
  if( error_num )
  {
   free( whex );
   return result;
  }
   
  tag_number_xml->appendChild( pDoc->createTextNode( whex ) );

  free( whex );  
 }


 result->appendChild( tag_number_xml );

 return result;
}
//**************************************************************************
template< typename In, typename Out >
bool identification_block_type< In, Out >::from_xml( MSXML2::IXMLDOMElementPtr element )
{
 if( !element )
  return false;

 MSXML2::IXMLDOMElementPtr tag_class_xml = element->selectSingleNode( L"tag_class" );
 if( !tag_class_xml )
  return false;

 if( tag_class_xml->text == ( _bstr_t )L"UNIVERSAL" )
  tag_class = 1;
 if( tag_class_xml->text == ( _bstr_t )L"APPLICATION" )
  tag_class = 2;
 if( tag_class_xml->text == ( _bstr_t )L"CONTEXT_SPECIFIC" )
  tag_class = 3;
 if( tag_class_xml->text == ( _bstr_t )L"PRIVATE" )
  tag_class = 4;
 if( tag_class == 0 )
  return false;

 MSXML2::IXMLDOMElementPtr tag_number_xml = element->selectSingleNode( L"value_dec" );
 if( tag_number_xml )
 {
  tag_number = _wtoi64( ( wchar_t* ) tag_number_xml->text );
  if( errno && !tag_number )
   return false;
 }
 else
 {
  tag_number_xml = element->selectSingleNode( L"value_hex" );
  if( !tag_number_xml )
   return false;

  std::wstring str_text = ( wchar_t* )tag_number_xml->text;
  str_text = wreplace( str_text, L" ", L"" );

  if( !from_whex_codes( str_text.c_str(), str_text.length(), &value_hex ) )
   return false;

  value_hex_len = str_text.length() >> 1;
  is_hex_only = true;
 }

 MSXML2::IXMLDOMElementPtr is_constructed_xml = element->selectSingleNode( L"is_constructed" );
 if( !is_constructed_xml )
  return false;

 if( is_constructed_xml->text == ( _bstr_t )L"true" )
  is_constructed = true;
 else
 {
  if( is_constructed_xml->text == ( _bstr_t )L"false" )
   is_constructed = false;
  else
   return false;
 }

 return true;
}
//**************************************************************************
template< typename In, typename Out >
In identification_block_type< In, Out >::decode_BER( unsigned long long max_block_len )
{
 if( !max_block_len )
 {
  sprintf_s( error, 255, "No information block encoded" );
  return In();
 }

 if( stream_in == In() )
 {
  sprintf_s( error, 255, "End of input reached before message was fully decoded" );
  return In();
 }
 //--------------------------------------
 // Find tag class
 //
 unsigned char tag_class_stream = ( (unsigned char)*stream_in ) & 0xC0;
 
 switch( tag_class_stream )
 {
  case 0x00:
   tag_class = 1; // UNIVERSAL
   break;
  case 0x40:
   tag_class = 2; // APPLICATION
   break;
  case 0x80:
   tag_class = 3; // CONTEXT-SPECIFIC
   break;
  case 0xC0:
   tag_class = 4; // PRIVATE
   break;
  default:
   tag_class = 0xFF;  // unknown (impossible case)
   sprintf_s( error, 255, "Unknown tag class" );
   return In();
 }

 //--------------------------------------
 // Find encoding form
 unsigned char is_constructed_stream = (unsigned char)*stream_in & 0x20;

 if( is_constructed_stream == 0x20 )
  is_constructed = true;
 else
  is_constructed = false;

 //--------------------------------------
 unsigned char number = ( (unsigned char)*stream_in ) & 0x1F;

 stream_in++;

 // Simple case (tag number < 31)
 if( number != 0x1F )
 {
  tag_number = number;
  block_length = 1;
 }
 else // tag number bigger or equal to 31
 {
  unsigned long long count = 1;  // add the latest octet

  value_hex = ( unsigned char* ) malloc( 255 );
  if( !value_hex )
   return In();

  value_hex_len = 255;
  unsigned char* hexcodes_temp = value_hex;

  while( ( (unsigned char)*stream_in ) & 0x80 )
  {
   *hexcodes_temp = *stream_in & 0x7F;

   hexcodes_temp++;
   stream_in++;
   count++;

   // Check for end of stream_in
   if( stream_in == In() )
   {
    sprintf_s( error, 255, "End of input reached before message was fully decoded" );
    return stream_in;
   }

   if( count == value_hex_len )
   {
    unsigned char* hex_buf = ( unsigned char* ) malloc( ( size_t )value_hex_len );
    memcpy( hex_buf, value_hex, ( size_t )value_hex_len );

    free( value_hex );

    value_hex = ( unsigned char* ) malloc( ( size_t )value_hex_len + 255 );
    if( !value_hex )
     return In();

    memcpy( value_hex, hex_buf, ( size_t )value_hex_len );

    hexcodes_temp = value_hex + value_hex_len;

    value_hex_len += 255;

    free( hex_buf );
   }
  }

  value_hex[ count - 1 ] = *stream_in & 0x7F;
  stream_in++;
  value_hex_len = count;

  if( value_hex_len > 8 )
  {
   warnings.push_back( "Tag too long, represented as hex-coded" );
   is_hex_only = true;

   tag_number = (-1);  // Too big unsigned long long number because "tag_number = 0" is an invalid according to standard
  }

  from_base( value_hex, &tag_number, 7, ( unsigned char )value_hex_len );

  block_length = ( count + 1 );
 }
 //--------------------------------------

 // Check usign constructive form of encoding for primitive types
 if( ( ( tag_class == 1 ) ) && 
     ( is_constructed ) )
 {
  switch( tag_number )
  {
   case 1:  // BOOLEAN
   case 2:  // REAL
   case 5:  // NULL
   case 6:  // OBJECT IDENTIFIER
   case 9:  // REAL
   case 14: // TIME
   case 23:
   case 24:
   case 31:
   case 32:
   case 33:
   case 34:
    sprintf_s( error, 255, "Constructed encoding used for primitive type" );
    return In();
   default:
    ;
  }
 }

 return stream_in;
}
//**************************************************************************
template< typename In, typename Out >
unsigned long long identification_block_type< In, Out >::encode_BER( bool size_only )
{
 unsigned char first_octet = 0;

 switch( tag_class )
 {
  case 1:
   first_octet |= 0x00; // UNIVERSAL
   break;
  case 2:
   first_octet |= 0x40; // APPLICATION
   break;
  case 3:
   first_octet |= 0x80; // CONTEXT-SPECIFIC
   break;
  case 4:
   first_octet |= 0xC0; // PRIVATE
   break;
  default:
   sprintf_s( error, 255, "Unknown tag class" );
   return 0;
 }

 if( is_constructed ) 
  first_octet |= 0x20;

 if( ( tag_number < 31 ) && ( !is_hex_only ) )
 {
  if( !size_only )
  {
   unsigned char number  = ( unsigned char )tag_number;
   number &= 0x1F;
   first_octet |= number;

   stream_out = first_octet;
  }

  return 1;
 }
 else
 {
  first_octet |= 0x1F;
  stream_out = first_octet;

  if( !is_hex_only )
  {
   unsigned char* encoded_number;
   int size = to_base( tag_number, &encoded_number, 7 );

   if( !size_only )
   {
    for( int i = 0; i < ( size - 1 ); i++ )
     stream_out = encoded_number[ i ] | 0x80;

    stream_out = encoded_number[ size - 1 ];
   }

   free( encoded_number );
   
   return ( size + 1 );
  }
  else
  {
   if( !size_only )
   {
    for( unsigned long long i = 0; i < ( value_hex_len - 1 ); i++ )
     stream_out = value_hex[ i ] | 0x80;
    
    stream_out = value_hex[ value_hex_len - 1 ];
   }

   return ( value_hex_len + 1 );
  }
 }

 return 0;
}
//**************************************************************************
#pragma endregion
//**************************************************************************
#pragma region length_block_type
//********************************************************************************************
template< typename In, typename Out >
struct length_block_type : virtual public block_type< In, Out >
{
 bool is_indefinite_form;
 bool long_form_used;
 unsigned long long length;

 length_block_type();

 virtual std::string block_name();

 virtual MSXML2::IXMLDOMElementPtr to_xml( MSXML2::IXMLDOMDocumentPtr pDoc );
 virtual bool from_xml( MSXML2::IXMLDOMElementPtr element );

 virtual In decode_BER( unsigned long long max_block_len );
 virtual unsigned long long encode_BER( bool size_only = false );
};
//**************************************************************************
template< typename In, typename Out >
length_block_type< In, Out >::length_block_type() : length( 0 ),
                                                    is_indefinite_form( false ),
                                                    long_form_used( false )
{
}
//**************************************************************************
template< typename In, typename Out >
std::string length_block_type< In, Out >::block_name()
{
 return "length_block";
}
//**************************************************************************
template< typename In, typename Out >
MSXML2::IXMLDOMElementPtr length_block_type< In, Out >::to_xml( MSXML2::IXMLDOMDocumentPtr pDoc )
{
 MSXML2::IXMLDOMElementPtr result = block_type< In, Out >::to_xml( pDoc );
 if( error[ 0 ] != 0x00 )
  return result;

 MSXML2::IXMLDOMElementPtr is_indefinite_form_xml = pDoc->createElement( L"is_indefinite_form" );
 is_indefinite_form_xml->appendChild( pDoc->createTextNode( ( is_indefinite_form ) ? L"true" : L"false" ) );

 result->appendChild( is_indefinite_form_xml );

 MSXML2::IXMLDOMElementPtr long_form_used_xml = pDoc->createElement( L"long_form_used" );
 long_form_used_xml->appendChild( pDoc->createTextNode( ( long_form_used ) ? L"true" : L"false" ) );

 result->appendChild( long_form_used_xml );

 MSXML2::IXMLDOMElementPtr length_xml = pDoc->createElement( L"length" );

 wchar_t length_buf[ 255 ];
 memset( length_buf, 0x00, 255 * sizeof( wchar_t ) );

 errno_t err = _ui64tow_s( length, length_buf, 255, 10 );
 if( err )
  return NULL;
  
 length_xml->appendChild( pDoc->createTextNode( length_buf ) );

 result->appendChild( length_xml );

 return result;
}
//**************************************************************************
template< typename In, typename Out >
bool length_block_type< In, Out >::from_xml( MSXML2::IXMLDOMElementPtr element )
{
 if( !element )
  return false;

 MSXML2::IXMLDOMElementPtr is_indefinite_form_xml = element->selectSingleNode( L"is_indefinite_form" );
 if( !is_indefinite_form_xml )
  return false;

 if( is_indefinite_form_xml->text == ( _bstr_t )L"true" )
  is_indefinite_form = true;
 else
 {
  if( is_indefinite_form_xml->text == ( _bstr_t )L"false" )
   is_indefinite_form = false;
  else
   return false;
 }

 MSXML2::IXMLDOMElementPtr long_form_used_xml = element->selectSingleNode( L"long_form_used" );
 if( !long_form_used_xml )
  return false;

 if( long_form_used_xml->text == ( _bstr_t )L"true" )
  long_form_used = true;
 else
 {
  if( long_form_used_xml->text == ( _bstr_t )L"false" )
   long_form_used = false;
  else
   return false;
 }

 MSXML2::IXMLDOMElementPtr length_xml = element->selectSingleNode( L"length" );
 if( !length_xml )
  return false;

 length = _wtoi64( ( wchar_t* ) length_xml->text );
 if( errno && !length )
  return false;

 return true;
}
//**************************************************************************
template< typename In, typename Out >
In length_block_type< In, Out >::decode_BER( unsigned long long max_block_len )
{
 if( !max_block_len )
 {
  sprintf_s( error, 255, "No length block encoded" );
  return In();
 }

 if( stream_in == In() )
 {
  sprintf_s( error, 255, "End of input reached before message was fully decoded" );
  return In();
 }

 #pragma warning( push )
 #pragma warning( disable : 6011 )

 if( (unsigned char)*stream_in == 0xFF )
 {
  sprintf_s( error, 255, "Length block 0xFF is reserved by standard" );
  return In();
 }

 if( (unsigned char)*stream_in == 0x80 )
  is_indefinite_form = true;
 else
  is_indefinite_form = false;

 // Nothing to do with this
 if( is_indefinite_form )
 {
  block_length = 1;

  stream_in++;
  return stream_in;
 }

 if( (unsigned char)*stream_in & 0x80 )
  long_form_used = true;
 else
  long_form_used = false;

 if( long_form_used == false )
 {
  length = (unsigned long long)*stream_in;
  block_length = 1;

  stream_in++;
  return stream_in;
 }

 unsigned char count = (unsigned char)*stream_in & 0x7F;

 if( count > 8 ) // unable to work with such big integers
 {
  sprintf_s( error, 255, "Too big integer" );
  return In();
 }

 stream_in++;
 unsigned char* buf = (unsigned char*) malloc( count );
 if( !buf )
  return In();

 for( unsigned char i = 0; i < count; i++ )
 {
  if( stream_in == In() )
  {
   sprintf_s( error, 255, "End of input reached before message was fully decoded" );
   return stream_in;
  }

  buf[ i ] = (unsigned char)*stream_in;
  stream_in++;
 }

 if( *buf == 0x00 )
  warnings.push_back( "Needlessly long encoded length" );

 from_base( buf, &(length), 8, count );

 if( ( long_form_used ) && ( length <= 127 ) )
  warnings.push_back( "Unneccesary usage of long length form" );

 block_length = count + 1; // +1 for initial length octet

 free( buf );
 
 return stream_in;

 #pragma warning( pop )
}
//**************************************************************************
template< typename In, typename Out >
unsigned long long length_block_type< In, Out >::encode_BER( bool size_only )
{
 if( length > 127 )
  long_form_used = true;

 if( is_indefinite_form )
 {
  if( !size_only)
   stream_out = ( unsigned char )0x80;
   
  return 1;
 }

 if( long_form_used )
 {
  unsigned char* encoded_length;
  int size = to_base( length, &encoded_length, 8 );

  if( size > 127 )
  {
   free( encoded_length );
   sprintf_s( error, 255, "Too big length" );
   return 0;
  }

  if( size_only )
  {
   free( encoded_length );
   return ( size + 1 );
  }
   
  unsigned char ch = size;
  ch |= 0x80;

  stream_out = ch;

  for( int i = 0; i < size; i++ )
   stream_out = encoded_length[ i ];

  free( encoded_length );
  
  return ( size + 1 );
 }
 else
 {
  if( !size_only )
  {
   unsigned char ch = ( unsigned char )length;
   stream_out = ch;
  }

  return 1;
 }


 return 0;
}
//**************************************************************************
#pragma endregion
//**************************************************************************
#pragma region value_block_type
//**************************************************************************
template< typename In, typename Out >
struct value_block_type : virtual public block_type< In, Out >
{
 virtual std::string block_name();
};
//**************************************************************************
template< typename In, typename Out >
std::string value_block_type< In, Out >::block_name()
{
 return "value_block";
}
//**************************************************************************
#pragma endregion
//**************************************************************************
#pragma region BASIC_type
//**************************************************************************
template< typename In, typename Out >
struct BASIC_type : virtual public block_type< In, Out >
{
 unsigned char tag_class;
 unsigned long long tag_number;

 identification_block_type< In, Out >* id_block; // init in "internal_transformfuncs.h"
 length_block_type< In, Out >* len_block;        // init in BASIC_type (common type for all ASN.1 type)
 value_block_type< In, Out >* value_block;       // init in each specific type (specific type for each ASN.1 type)

 BASIC_type();
 virtual ~BASIC_type();

 static std::string block_name_static();
 virtual std::string block_name();

 virtual MSXML2::IXMLDOMElementPtr to_xml( MSXML2::IXMLDOMDocumentPtr pDoc );
 virtual bool from_xml( MSXML2::IXMLDOMElementPtr element );

 virtual In decode_BER( unsigned long long max_block_len );
 virtual unsigned long long encode_BER( bool size_only = false );
};
//**************************************************************************
template< typename In, typename Out >
BASIC_type< In, Out >::BASIC_type() : id_block( NULL ),
                                      len_block( NULL ),
                                      value_block( NULL ),
                                      tag_class( ( unsigned char ) ( -1 ) ),
                                      tag_number( ( unsigned long long ) ( -1 ) )
{
}
//**************************************************************************
template< typename In, typename Out >
BASIC_type< In, Out >::~BASIC_type()
{
 if( id_block != NULL )
  delete id_block;

 if( len_block != NULL )
  delete len_block;

 if( value_block != NULL )
  delete value_block;
}
//**************************************************************************
template< typename In, typename Out >
std::string BASIC_type< In, Out >::block_name_static()
{
 return "ASN1 TYPE";
}
//**************************************************************************
template< typename In, typename Out >
std::string BASIC_type< In, Out >::block_name()
{
 return block_name_static();
}
//**************************************************************************
template< typename In, typename Out >
MSXML2::IXMLDOMElementPtr BASIC_type< In, Out >::to_xml( MSXML2::IXMLDOMDocumentPtr pDoc )
{
 MSXML2::IXMLDOMElementPtr result = block_type< In, Out >::to_xml( pDoc );
 
 if( id_block != NULL )
 {
  MSXML2::IXMLDOMElementPtr element = id_block->to_xml( pDoc );
  if( element != NULL )
   result->appendChild( element );
 }

 if( len_block != NULL )
 {
  MSXML2::IXMLDOMElementPtr element = len_block->to_xml( pDoc );
  if( element != NULL )
   result->appendChild( element );
 }

 if( value_block != NULL )
 {
  MSXML2::IXMLDOMElementPtr element = value_block->to_xml( pDoc );
  if( element != NULL)
   result->appendChild( element );
 }
 
 return result;
}
//**************************************************************************
template< typename In, typename Out >
bool BASIC_type< In, Out >::from_xml( MSXML2::IXMLDOMElementPtr element )
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

 if( value_block != NULL )
  if( !value_block->from_xml( element->selectSingleNode( L"./value_block" ) ) )
   return false;

 return true;
}
//**************************************************************************
template< typename In, typename Out >
In BASIC_type< In, Out >::decode_BER( unsigned long long max_block_len )
{
 if( ( id_block == NULL ) ||
     ( len_block == NULL ) ||
     ( value_block == NULL ))
 {
  if( !error ) // was a strange warning here from VS2010 compliler, DO NOT TOUCH!
   return In();

  sprintf_s( error, 255, "Internal blocks not initialized" );
  return In();
 }

 value_block->stream_in = stream_in;

 In end = value_block->decode_BER( max_block_len );
 if( value_block->error[ 0 ] != 0x00 )
 {
  sprintf_s( error, 255, value_block->error );
  return In();
 }

 block_length = 0;
 
 if( id_block != NULL )
 {
  if( id_block->error[ 0 ] == 0x00)
   block_length += id_block->block_length;
 }

 if( len_block != NULL )
 {
  if( len_block->error[ 0 ] == 0x00)
   block_length += len_block->block_length;
 }

 if( value_block != NULL )
 {
  if( value_block->error[ 0 ] == 0x00)
   block_length += value_block->block_length;
 }

 return stream_in;
}
//**************************************************************************
template< typename In, typename Out >
unsigned long long BASIC_type< In, Out >::encode_BER( bool size_only )
{
 unsigned long long result = 0;

 if( !id_block )
  return 0;

 id_block->stream_out = stream_out;
 result += id_block->encode_BER( size_only );

 if( !len_block )
  return 0;

 len_block->stream_out = stream_out;

 if( !value_block )
  return 0;

 value_block->stream_out = stream_out;
 len_block->length = value_block->encode_BER( true );

 result += len_block->encode_BER( size_only );

 if( !size_only )
 {
  value_block->stream_out = stream_out;
  result += value_block->encode_BER( false);
 }
 else
  result += len_block->length;

 if( len_block->is_indefinite_form )
 {
  if( !size_only )
  {
   stream_out = 0x00;
   stream_out = 0x00;
  }
  else
   result += 2;
 }

 return result;
}
//**************************************************************************
#pragma endregion
//**************************************************************************
#pragma region PRIMITIVE_value_block
//**************************************************************************
template< typename In, typename Out >
struct PRIMITIVE_value_block : virtual public value_block_type< In, Out >,
                               virtual public hex_block_type< In, Out >
{
 virtual MSXML2::IXMLDOMElementPtr to_xml( MSXML2::IXMLDOMDocumentPtr pDoc );
 virtual bool from_xml( MSXML2::IXMLDOMElementPtr element );

 virtual In decode_BER( unsigned long long max_block_len );
 virtual unsigned long long encode_BER( bool size_only = false );
};
//**************************************************************************
template< typename In, typename Out >
MSXML2::IXMLDOMElementPtr PRIMITIVE_value_block< In, Out >::to_xml( MSXML2::IXMLDOMDocumentPtr pDoc )
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
bool PRIMITIVE_value_block< In, Out >::from_xml( MSXML2::IXMLDOMElementPtr element )
{
 if( !element )
  return false;

 MSXML2::IXMLDOMElementPtr value_hex_xml = element->selectSingleNode( L"value_hex" );
 if( !value_hex_xml )
  return false;

 std::wstring str_hex = ( wchar_t* ) value_hex_xml->text;
 str_hex = wreplace( str_hex.c_str(), L" ", L"" );

 if( !from_whex_codes( str_hex.c_str(), str_hex.length(), &value_hex ) )
  return false;

 block_length = str_hex.length() >> 1;

 return true;
}
//**************************************************************************
template< typename In, typename Out >
In PRIMITIVE_value_block< In, Out >::decode_BER( unsigned long long max_block_len )
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

 for( unsigned long long i = 0; i < ( max_block_len ); i++ )
 {
  if( end == In() )
  {
   sprintf_s( error, 255, "End of input reached before message was fully decoded" );
   return end;
  }

  *data = *end;

  data++;
  end++;
 }

 block_length = max_block_len;
 
 return end;
}
//**************************************************************************
template< typename In, typename Out >
unsigned long long PRIMITIVE_value_block< In, Out >::encode_BER( bool size_only )
{
 if( size_only )
  return block_length;

 for( unsigned long long i = 0; i < block_length; i++ )
  stream_out = value_hex[ i ];

 return block_length;
}
//**************************************************************************
#pragma endregion
//**************************************************************************
#pragma region PRIMITIVE_type
//**************************************************************************
template< typename In, typename Out >
struct PRIMITIVE_type : virtual public BASIC_type< In, Out >
{
 PRIMITIVE_type();
};
//**************************************************************************
template< typename In, typename Out >
PRIMITIVE_type< In, Out >::PRIMITIVE_type()
{
 value_block = new PRIMITIVE_value_block< In, Out >();
}
//**************************************************************************
#pragma endregion
//**************************************************************************
#pragma region CONSTRUCTED_value_block
//**************************************************************************
template< typename In, typename Out >
struct CONSTRUCTED_value_block : virtual public value_block_type< In, Out >
{
 bool is_indefinite_form;
 std::vector< BASIC_type< In, Out >* > value;

 CONSTRUCTED_value_block();
 CONSTRUCTED_value_block( bool indefinite_form );
 ~CONSTRUCTED_value_block();

 virtual MSXML2::IXMLDOMElementPtr to_xml( MSXML2::IXMLDOMDocumentPtr pDoc );
 virtual bool from_xml( MSXML2::IXMLDOMElementPtr element );

 virtual In decode_BER( unsigned long long max_block_len );
 virtual unsigned long long encode_BER( bool size_only = false );

protected:
  unsigned long long check_len( unsigned long long max_block_len );
};
//**************************************************************************
template< typename In, typename Out >
CONSTRUCTED_value_block< In, Out >::CONSTRUCTED_value_block() : is_indefinite_form( false )
{
}
//**************************************************************************
template< typename In, typename Out >
CONSTRUCTED_value_block< In, Out >::CONSTRUCTED_value_block( bool indefinite_form ) : is_indefinite_form( indefinite_form )
{
}
//**************************************************************************
template< typename In, typename Out >
CONSTRUCTED_value_block< In, Out >::~CONSTRUCTED_value_block()
{
 for( std::vector< BASIC_type< In, Out >* >::iterator i = value.begin(); i != value.end(); i++ )
  delete (*i);
}
//**************************************************************************
template< typename In, typename Out >
MSXML2::IXMLDOMElementPtr CONSTRUCTED_value_block< In, Out >::to_xml( MSXML2::IXMLDOMDocumentPtr pDoc )
{
 MSXML2::IXMLDOMElementPtr result = block_type< In, Out >::to_xml( pDoc );
 if( error[ 0 ] != 0x00 )
  return result;

 for( std::vector< BASIC_type< In, Out >* >::iterator i = value.begin(); i != value.end(); i++ )
 {
  if( (*i)->block_name() != EOC_type< In, Out >::block_name_static() )
   result->appendChild( (*i)->to_xml( pDoc ) );
 }

 return result;
}
//**************************************************************************
template< typename In, typename Out >
bool CONSTRUCTED_value_block< In, Out >::from_xml( MSXML2::IXMLDOMElementPtr element )
{
 if( !element )
  return false;

 MSXML2::IXMLDOMNodeListPtr list = element->selectNodes( L"./node()" );

 for( int i = 0; i < list->length; i++ )
 {
  std::wstring node_name = ( wchar_t* )list->item[ i ]->nodeName;
  if( ( node_name == L"error" ) ||
      ( node_name == L"warnings" ) )
   continue;

  BASIC_type< In, Out >* basic_type;

  if( ::from_xml( list->item[ i ], &basic_type ) )
   value.push_back( basic_type );
  else
   return false;
 }

 return true;
}
//**************************************************************************
template< typename In, typename Out >
unsigned long long CONSTRUCTED_value_block< In, Out >::check_len( unsigned long long max_block_len )
{
 if( is_indefinite_form )
  return 1;

 return max_block_len;
}
//**************************************************************************
template< typename In, typename Out >
In CONSTRUCTED_value_block< In, Out >::decode_BER( unsigned long long max_block_len )
{
 In end = stream_in;

 while( check_len( max_block_len ) )
 {
  BASIC_type< In, Out >* basic_type; 

  if( end == In() )
  {
   sprintf_s( error, 255, "End of input reached before message was fully decoded" );
   return end;
  }

  end = decode_BER_raw( end, max_block_len, &basic_type );
  if( basic_type->error[ 0 ] != 0x00 )
  {
   value.push_back( basic_type );
   sprintf_s( error, 255, basic_type->error );
   return In();
  }

  block_length += basic_type->block_length;
  max_block_len -= basic_type->block_length;

  value.push_back( basic_type );

  if( ( is_indefinite_form ) && ( basic_type->block_name() == EOC_type< In, Out >::block_name_static() ) )
   break;

  if( basic_type->error[ 0 ] != 0x00 )
  {
   sprintf_s( error, 255, basic_type->error, 255 );
   break;
  }
   
  if( end == In() )
   break;
 }

 if( is_indefinite_form == true )
 {
  if( value[ value.size() - 1 ]->block_name() == EOC_type< In, Out >::block_name_static() )
  {
   delete value[ value.size() - 1 ];
   value.pop_back();
  }
  else
  {
   warnings.push_back( "No EOC block encoded" );
  }
 }

 return end;
}
//**************************************************************************
template< typename In, typename Out >
unsigned long long CONSTRUCTED_value_block< In, Out >::encode_BER( bool size_only )
{
 unsigned long long result = 0;
 
 for( std::vector< BASIC_type< In, Out >* >::iterator i = value.begin(); i != value.end(); i++ )
 {
  (*i)->stream_out = stream_out;
  result += (*i)->encode_BER( size_only );
 }
  
 return result;
}
//**************************************************************************
#pragma endregion
//**************************************************************************
#pragma region CONSTRUCTED_type
//**************************************************************************
template< typename In, typename Out >
struct CONSTRUCTED_type : virtual public BASIC_type< In, Out >
{
 CONSTRUCTED_type();

 virtual In decode_BER( unsigned long long max_block_len );
};
//**************************************************************************
template< typename In, typename Out >
CONSTRUCTED_type< In, Out >::CONSTRUCTED_type()
{
 value_block = new CONSTRUCTED_value_block< In, Out >();
}
//**************************************************************************
template< typename In, typename Out >
In CONSTRUCTED_type< In, Out >::decode_BER( unsigned long long max_block_len )
{
 if( ( id_block == NULL ) ||
     ( len_block == NULL ) ||
     ( value_block == NULL ))
 {
  if( !error ) // was a strange warning here from VS2010 compliler, DO NOT TOUCH!
   return In();

  sprintf_s( error, 255, "Internal blocks not initialized" );
  return In();
 }

 CONSTRUCTED_value_block< In, Out >* vb = dynamic_cast< CONSTRUCTED_value_block< In, Out >* >( value_block );
 if( vb == NULL )
  return In();

 vb->is_indefinite_form = len_block->is_indefinite_form;
 vb->stream_in = stream_in;

 In end = value_block->decode_BER( max_block_len );
 if( value_block->error[ 0 ] != 0x00 )
 {
  sprintf_s( error, 255, value_block->error );
  return In();
 }

 block_length = 0;
 
 if( id_block != NULL )
 {
  if( id_block->error[ 0 ] == 0x00)
   block_length += id_block->block_length;
 }

 if( len_block != NULL )
 {
  if( len_block->error[ 0 ] == 0x00)
   block_length += len_block->block_length;
 }

 if( value_block != NULL )
 {
  if( value_block->error[ 0 ] == 0x00)
   block_length += value_block->block_length;
 }

 return stream_in;
}
//**************************************************************************
#pragma endregion
//**************************************************************************
#endif