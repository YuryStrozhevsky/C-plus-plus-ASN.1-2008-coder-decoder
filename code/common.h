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
#ifndef _17DEA975_9CB5_41BB_ACE6_98FED698FBC3
#define _17DEA975_9CB5_41BB_ACE6_98FED698FBC3
//**************************************************************************
// Функция перевода числа из основания "2^base" в число с основанием 10.
//
// (значение "enc_size" позволяет задать количество элементов в массиве "enc")
//
void from_base( const unsigned char* enc, unsigned long long* value, const unsigned char base, const unsigned char enc_size )
{
 for( char i = ( enc_size - 1 ); i >= 0; i-- )
  *value += (unsigned long long)enc[ ( enc_size - 1 ) - i ] << ( base * i );
}
//**************************************************************************
int to_base( unsigned long long value, unsigned char** encoded, const unsigned char base, int reserved  = ( -1 ) )
{
 int result = 0;
 unsigned long long biggest = ( ( unsigned long long )1 << base );

 for( int i = 1; i < 8; i++ )
 {
  if( value < biggest )
  {
   if( reserved < 0 )
   {
    (*encoded) = ( unsigned char* ) malloc( i );
    if( !(*encoded) )
     return 0;

    result = i;
   }
   else
   {
    if( reserved < i )
     return 0;

    (*encoded) = ( unsigned char* ) malloc( reserved );
    if( !(*encoded) )
     return 0;

    memset( (*encoded), 0x00, reserved );

    result = reserved;
   }

   for( int j = ( i - 1 ); j >= 0; j-- )
   {
    (*encoded)[ result - j - 1 ] = ( unsigned char )( value >> ( j * base ) );
    value -= ( unsigned long long )( (*encoded)[ result - j - 1 ] ) << ( j * base );
   }

   return result;
  }

  biggest <<= base;
 }

 return result;
}
//**************************************************************************
// Decoding of "two complement" values
//
long long decode_tc( unsigned char* buf, unsigned char buf_len, std::vector< std::string >* warning )
{
 //-------------------------------------------------
 // Needlessly long test

 if( buf_len >= 2 )
 {
  bool condition_1 = ( buf[ 0 ] == 0xFF ) && ( buf[ 1 ] & 0x80 );
  bool condition_2 = ( buf[ 0 ] == 0x00 ) && ( ( buf[ 1 ] & 0x80 ) == 0x00 );

  if( ( condition_1 ) || 
      ( condition_2 ) )
   warning->push_back( "Needlessly long format" );
 }
 //-------------------------------------------------

 unsigned long long big_int = 0;
 unsigned char* big_int_buf = ( unsigned char* ) malloc( buf_len );
 if( !big_int_buf )
  return 0;

 memset( big_int_buf, 0x00, buf_len );
 big_int_buf[ 0 ] = ( buf[ 0 ] & 0x80 ); // mask only the biggest bit

 from_base( big_int_buf, &big_int, 8, buf_len );

 free( big_int_buf );
 
 unsigned long long small_int = 0;
 unsigned char* small_int_buf = ( unsigned char* ) malloc( buf_len );
 if( !small_int_buf )
  return 0;

 memcpy( small_int_buf, buf, buf_len );
 small_int_buf[ 0 ] &= 0x7F; // mask biggest bit

 from_base( small_int_buf, &small_int, 8, buf_len );
 
 free( small_int_buf );

 return ( small_int - big_int );
}
//**************************************************************************
int encode_tc( long long value, unsigned char** output_buf )
{
 unsigned long long mod_value = ( value < 0 ) ? ( value * ( -1 ) ) : value;
 unsigned long long big_int = 128;

 for( unsigned char i = 1; i < 8; i++ )
 {
  if( mod_value <= big_int )
  {
   unsigned char j = 0;

   if( value < 0 )
   {
    unsigned long long small_int = big_int - mod_value;

    int size = to_base( small_int, output_buf, 8, i );

    (*output_buf)[ 0 ] |= 0x80;

    return size;
   }
   else
   {
    int size = to_base( mod_value, output_buf, 8, i );

    if( (*output_buf)[ 0 ] & 0x80 )
    {
     unsigned char* temp = ( unsigned char* ) malloc( size );
     if( !temp )
      return 0;

     memcpy( temp, (*output_buf), size );
     free( (*output_buf) );
     (*output_buf) = ( unsigned char* ) malloc( size + 1 );
     if( !(*output_buf) )
      return 0;

     memcpy( (*output_buf) + 1, temp, size );
     (*output_buf)[ 0 ] = 0x00;
     free( temp );

     return ( size + 1 );
    }

    return size;
   }
  }

  big_int <<= 8;
 }

 return 0;
}
//**************************************************************************
std::string unicode_points( unsigned char* value, unsigned long long value_len, unsigned char block_size )
{
 std::string result = "{";

 // Only two variants are admitted:
 // 1) BMPString - block_size = 2;
 // 2) UniversalString - block_size = 4
 if( !( ( block_size == 2) || ( block_size == 4 ) ) )
  return result;

 char buf[ 2 ];
 buf[ 1 ] = 0x00;

 unsigned char block_buf[ 4 ];
 memset( block_buf, 0x00, 4 );

 char char_buf[ 4 ];

 std::string simple_chars = "\"";

 for( unsigned long long i = 0; i < value_len; i++ )
 {
  if( i == ( value_len - 1 ) )
  {
   block_buf[ i % block_size + ( 4 - block_size ) ] = value[ i ];

   // In case if it was not a latest octet in "block_buf",
   // but latest in "value"
   if( ( i % block_size + ( 4 - block_size ) ) != 3 )
   {
    for( int j = i % block_size + ( 4 - block_size ); j < 4; j++ )
     block_buf[ j ] = 0x00;
   }
  }

  if( ( !( i % block_size ) && i ) || ( i == ( value_len - 1 ) ) )
  {
   if( ( block_buf[ 0 ] == 0x00 ) && 
       ( block_buf[ 1 ] == 0x00 ) && 
       ( block_buf[ 2 ] == 0x00 ) && 
       ( block_buf[ 3 ] <= 0x7F )) // simple char
   {
    char simple_char_buf[ 2 ];
    simple_char_buf[ 0 ] = block_buf[ 3 ];
    simple_char_buf[ 1 ] = 0x00;

    simple_chars += simple_char_buf;

    if( i == ( value_len - 1 ) )
    {
     simple_chars += "\"";

     if( result.length() != 1 )
      result += ",";

     result += simple_chars;
     simple_chars = "\"";
    }
   }
   else // native unicode code-point
   {
    if( simple_chars.length() != 1 )
    {
     simple_chars += "\"";

     if( result.length() != 1 )
      result += ",";

     result += simple_chars;
     simple_chars = "\"";
    }

    if( i != block_size)
     result += ",";

    result += "{";

    for( unsigned char j = 0; j < 4; j++ )
    {
     sprintf_s( char_buf, 4, "%i", block_buf[ j ] );
     result += char_buf;

     if( j != 3 )
      result += ",";
    }

    result += "}";
   }

   memset( block_buf, 0x00, 4 );
  }

  block_buf[ i % block_size + ( 4 - block_size ) ] = value[ i ];
 }

 return result + "}";
}
//**************************************************************************
std::string xcodes( unsigned char* value, unsigned long long value_len, unsigned char block_size )
{
 std::string result = "";

 // Only two variants are admitted:
 // 1) BMPString - block_size = 2;
 // 2) UniversalString - block_size = 4
 if( !( ( block_size == 2) || ( block_size == 4 ) ) )
  return result;

 char buf[ 2 ];
 buf[ 1 ] = 0x00;

 unsigned char block_buf[ 4 ];
 memset( block_buf, 0x00, 4 );

 char char_buf[ 3 ];

 std::string simple_chars = "";

 for( unsigned long long i = 0; i < value_len; i++ )
 {
  if( i == ( value_len - 1 ) )
  {
   block_buf[ i % block_size + ( 4 - block_size ) ] = value[ i ];

   // In case if it was not a latest octet in "block_buf",
   // but latest in "value"
   if( ( i % block_size + ( 4 - block_size ) ) != 3 )
   {
    for( int j = i % block_size + ( 4 - block_size ); j < 4; j++ )
     block_buf[ j ] = 0x00;
   }
  }

  if( ( !( i % block_size ) && i ) || ( i == ( value_len - 1 ) ) )
  {
   if( ( block_buf[ 0 ] == 0x00 ) && 
       ( block_buf[ 1 ] == 0x00 ) && 
       ( block_buf[ 2 ] == 0x00 ) && 
       ( block_buf[ 3 ] <= 0x7F )) // simple char
   {
    char simple_char_buf[ 2 ];
    simple_char_buf[ 0 ] = block_buf[ 3 ];
    simple_char_buf[ 1 ] = 0x00;

    simple_chars += simple_char_buf;

    if( i == ( value_len - 1 ) )
    {
     result += simple_chars;
     simple_chars = "";
    }
   }
   else // native unicode code-point
   {
    if( simple_chars.length() != 0 )
    {
     result += simple_chars;
     simple_chars = "";
    }

    result += "&#x";

    for( unsigned char j = 0; j < 4; j++ )
    {
     sprintf_s( char_buf, 3, "%02X", block_buf[ j ] );
     result += char_buf;
    }

    result += ";";
   }

   memset( block_buf, 0x00, 4 );
  }

  block_buf[ i % block_size + ( 4 - block_size ) ] = value[ i ];
 }

 return result;
}
//**************************************************************************
std::string hex_codes( unsigned char* value, unsigned long long value_len )
{
 std::string result = "";

 char buf[3];

 for( unsigned long long i = 0; i < value_len; i++ )
 {
  memset( buf, 0x00, 3 );

  if( result.length() )
   result += " ";

  sprintf_s( buf, 3, "%02X", value[ i ] );

  result += buf;
 }

 return result;
}
//**************************************************************************
char* to_console_cp( char* to_convert )
{
 char* converted = NULL;

 size_t value_len = strlen( to_convert );

 size_t wide_string_len = MultiByteToWideChar( CP_ACP, MB_ERR_INVALID_CHARS, (LPCSTR)to_convert, value_len, NULL, 0 );
 if( !wide_string_len )
  return NULL;

 wchar_t* wide_buf = ( wchar_t* ) malloc( ( wide_string_len + 1 ) * sizeof( wchar_t ) );
 if( !wide_buf )
  return NULL;
 memset( wide_buf, 0x00, ( wide_string_len + 1 ) * sizeof( wchar_t ) );

 wide_string_len = MultiByteToWideChar( CP_ACP, MB_ERR_INVALID_CHARS, (LPCSTR)to_convert, value_len, (LPWSTR)wide_buf, wide_string_len );
 if( !wide_string_len )
 {
  free( wide_buf );
  return NULL;
 }

 int string_len = WideCharToMultiByte( GetConsoleOutputCP(), 0, (LPCWSTR)wide_buf, -1, NULL, 0, NULL, NULL );
 if( !string_len )
 {
  free( wide_buf );
  return NULL;
 }

 converted = (char*) malloc( string_len );
 if( !converted )
  return NULL;
 memset( converted, 0x00, string_len );

 string_len = WideCharToMultiByte( GetConsoleOutputCP(), 0, (LPCWSTR)wide_buf, -1, converted, string_len, NULL, NULL );
 if( !string_len )
 {
  free( wide_buf );
  free( converted );
  return NULL;
 }

 free( wide_buf);

 return converted;
}
//**************************************************************************
char* to_acp( const wchar_t* name )
{
 char* result = NULL;

 int string_len = WideCharToMultiByte( CP_ACP, 0, (LPCWSTR)name, -1, NULL, 0, NULL, NULL );
 if( !string_len )
  return NULL;

 result = (char*) malloc( string_len );
 if( !result )
  return NULL;

 memset( result, 0x00, string_len );

 string_len = WideCharToMultiByte( CP_ACP, 0, (LPCWSTR)name, -1, result, string_len, NULL, NULL );
 if( !string_len )
 {
  free( result );
  return NULL;
 }

 return result;
}
//**************************************************************************
bool from_hex_codes( const char* value, unsigned long long value_len, unsigned char** ret_value )
{
 std::map< char, unsigned char > hex_map;

 hex_map[ '0' ] = 0x00;
 hex_map[ '1' ] = 0x01;
 hex_map[ '2' ] = 0x02;
 hex_map[ '3' ] = 0x03;
 hex_map[ '4' ] = 0x04;
 hex_map[ '5' ] = 0x05;
 hex_map[ '6' ] = 0x06;
 hex_map[ '7' ] = 0x07;
 hex_map[ '8' ] = 0x08;
 hex_map[ '9' ] = 0x09;
 hex_map[ 'A' ] = 0x0A;
 hex_map[ 'a' ] = 0x0A;
 hex_map[ 'B' ] = 0x0B;
 hex_map[ 'b' ] = 0x0B;
 hex_map[ 'C' ] = 0x0C;
 hex_map[ 'c' ] = 0x0C;
 hex_map[ 'D' ] = 0x0D;
 hex_map[ 'd' ] = 0x0D;
 hex_map[ 'E' ] = 0x0E;
 hex_map[ 'e' ] = 0x0E;
 hex_map[ 'F' ] = 0x0F;
 hex_map[ 'f' ] = 0x0F;

 (*ret_value) = ( unsigned char* ) malloc( ( size_t ) ( value_len >> 1 ) );
 memset( *ret_value, 0x00, ( size_t ) ( value_len >> 1 ) );

 unsigned long long j = 0;
 unsigned char temp = 0x00;

 for( unsigned long long i = 0; i < value_len; i++ )
 {
  if( !( i % 2 ) )
   temp = hex_map[ value[ i ] ] << 4;
  else
  {
   temp |= hex_map[ value[ i ] ];

   (*ret_value)[ j ] = temp;
   j++;
  }
 }

 return false;
}
//**************************************************************************
bool from_whex_codes( const wchar_t* value, unsigned long long value_len, unsigned char** ret_value )
{
 std::map< wchar_t, unsigned char > whex_map;

 whex_map[ L'0' ] = 0x00;
 whex_map[ L'1' ] = 0x01;
 whex_map[ L'2' ] = 0x02;
 whex_map[ L'3' ] = 0x03;
 whex_map[ L'4' ] = 0x04;
 whex_map[ L'5' ] = 0x05;
 whex_map[ L'6' ] = 0x06;
 whex_map[ L'7' ] = 0x07;
 whex_map[ L'8' ] = 0x08;
 whex_map[ L'9' ] = 0x09;
 whex_map[ L'A' ] = 0x0A;
 whex_map[ L'a' ] = 0x0A;
 whex_map[ L'B' ] = 0x0B;
 whex_map[ L'b' ] = 0x0B;
 whex_map[ L'C' ] = 0x0C;
 whex_map[ L'c' ] = 0x0C;
 whex_map[ L'D' ] = 0x0D;
 whex_map[ L'd' ] = 0x0D;
 whex_map[ L'E' ] = 0x0E;
 whex_map[ L'e' ] = 0x0E;
 whex_map[ L'F' ] = 0x0F;
 whex_map[ L'f' ] = 0x0F;

 (*ret_value) = ( unsigned char* ) malloc( ( size_t ) ( value_len >> 1 ) );
 memset( *ret_value, 0x00, ( size_t ) ( value_len >> 1 ) );

 unsigned long long j = 0;
 unsigned char temp = 0x00;

 for( unsigned long long i = 0; i < value_len; i++ )
 {
  if( !( i % 2 ) )
   temp = whex_map[ value[ i ] ] << 4;
  else
  {
   temp |= whex_map[ value[ i ] ];

   (*ret_value)[ j ] = temp;
   j++;
  }
 }

 return true;
}
//**************************************************************************
std::string bit_codes( unsigned char* value, unsigned long long value_len, unsigned char unused_bits = 0 )
{
 std::string result = "";

 if( !value_len )
  return result;

 for( unsigned long long i = 0; i < ( value_len - 1 ); i++ )
 {
  unsigned char ch = value[ i ];

  if( result.length() )
   result += " ";

  for( unsigned char j = 0; j < 8; j++ )
  {
   // Do not show "unused bits"
   if( ( i == ( value_len - 2 ) ) && 
       ( j > ( 7 - unused_bits ) ) )
    break;

   if( ch & ( 1 << (7 - j ) ) )
    result += "1";
   else
    result += "0";

   if( ( j == 3 ) &&
       !( ( i == ( value_len - 2 ) ) &&
          ( ( j + 1 ) > ( 7 - unused_bits ) ) ) )
     result += " ";
  }
 }

 return result;
}
//**************************************************************************
unsigned long long from_bit_codes( const char* value, unsigned long long value_len, unsigned char** result )
{
 unsigned long long result_len = ( value_len >> 3 ) + ( ( value_len % 8 ) ? 1 : 0 );

 *result = ( unsigned char* ) malloc( ( size_t )result_len );
 if( !(*result) )
  return 0;

 memset( *result, 0x00, ( size_t )result_len );

 int result_index = 0;
 for( int i = 0; i < value_len; i++ )
 {
  if( value[ i ] == '1' )
   (*result)[ result_index ] |= 1 << ( 7 - ( i % 8 ) );

  if( i && ( ( ( i + 1 ) % 8 ) == 0 ) )
   result_index++;
 }

 return result_len;
}
//**************************************************************************
unsigned long long from_wbit_codes( const wchar_t* value, unsigned long long value_len, unsigned char** result )
{
 unsigned long long result_len = ( value_len >> 3 ) + ( ( value_len % 8 ) ? 1 : 0 );

 *result = ( unsigned char* ) malloc( ( size_t )result_len );
 if( !(*result) )
  return 0;

 memset( *result, 0x00, ( size_t )result_len );

 int result_index = 0;
 for( int i = 0; i < value_len; i++ )
 {
  if( value[ i ] == L'1' )
   (*result)[ result_index ] |= 1 << ( 7 - ( i % 8 ) );

  if( i && ( ( ( i + 1 ) % 8 ) == 0 ) )
   result_index++;
 }

 return result_len;
}
//**************************************************************************
// Came from http://www.rsdn.ru/forum/cpp.applied/885108.all.aspx
//
inline std::string replace(std::string text, std::string s, std::string d)
{
 for( size_t index=0; index=text.find(s, index), index!=std::string::npos;)
 {
  text.replace(index, s.length(), d);
  index += d.length();
 }

 return text;
}
//**************************************************************************
inline std::wstring wreplace(std::wstring text, std::wstring s, std::wstring d)
{
 for( size_t index=0; index=text.find(s, index), index!=std::string::npos;)
 {
  text.replace(index, s.length(), d);
  index += d.length();
 }

 return text;
}
//**************************************************************************
inline std::string clear_xml_tag( std::string name )
{
 name = replace( name, " ", "_" );
 name = replace( name, "[", "" );
 name = replace( name, "]", "" );

 return name;
}
//**************************************************************************
void trim( std::string& str, std::string delimiter = " ", int both_sides = 1 )
{
 if( str.empty() )
  return;

 std::string::size_type pos = str.find_first_not_of( delimiter );
 if( pos != std::string::npos )
 {
  str.erase( 0, pos );
  
  if( both_sides )
  {
   pos = str.find_last_not_of( delimiter );
   if( pos != std::string::npos )
    str.erase( pos + 1 );
  }
 }
}
//**************************************************************************
#endif