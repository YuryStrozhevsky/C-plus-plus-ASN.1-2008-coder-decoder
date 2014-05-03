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
#ifndef _50508D69_192F_4B5B_8FD8_69157D7008D2
#define _50508D69_192F_4B5B_8FD8_69157D7008D2
//**************************************************************************
template< typename In, typename Out >
struct UTCTIME_value_block : virtual public value_block_type< In, Out >,
                             virtual public hex_block_type< In, Out >
{
 unsigned int year;
 unsigned int month;
 unsigned int day;
 unsigned int hours;
 unsigned int minutes;

 bool format_with_seconds;

 unsigned int seconds;

 bool format_with_dif;

 unsigned char utc_dif_sign;
 unsigned int utc_dif_hours;
 unsigned int utc_dif_minutes;

 UTCTIME_value_block();
 
 virtual In decode_BER( unsigned long long max_block_len );
 virtual unsigned long long encode_BER( bool size_only = false );

 virtual MSXML2::IXMLDOMElementPtr to_xml( MSXML2::IXMLDOMDocumentPtr pDoc );
 virtual bool from_xml( MSXML2::IXMLDOMElementPtr element );
};
//**************************************************************************
template< typename In, typename Out >
UTCTIME_value_block< In, Out >::UTCTIME_value_block() : year( 0 ),
                                                        month( 0 ),
                                                        day( 0 ),
                                                        hours( 0 ),
                                                        minutes( 0 ),
                                                        format_with_seconds( false ),
                                                        seconds( 0 ),
                                                        format_with_dif( false ),
                                                        utc_dif_sign( 0 ),
                                                        utc_dif_hours( 0 ),
                                                        utc_dif_minutes( 0 )
{
}
//**************************************************************************
template< typename In, typename Out >
In UTCTIME_value_block< In, Out >::decode_BER( unsigned long long max_block_len )
{
 // !!!!!!!!!!!!!
 T end = VISIBLESTRING_type< In, Out >::decode_BER( max_block_len );

 //unsigned long long value_len = block_length - inf_block->block_length - len_block->block_length;
 //// !!!!!!!!!!!!!
 //std::string result;

 //if( value_len == 0 )
 //{
 // sprintf_s( error, 255, "Bad encoded UTCTime" );
 // return T();
 //}

 //char year_buf[ 5 ];
 //char month_buf[ 4 ];
 //char day_buf[ 5 ];
 //char hours_buf[ 4 ];
 //char minutes_buf[ 4 ];

 //if( value_len >= 2 )
 //{
 // if( value[ 0 ] > 0x35 ) // 19-th century
 // {
 //  year_buf[ 0 ] = 0x31;
 //  year_buf[ 1 ] = 0x39;
 // }
 // else // 20-th century
 // {
 //  year_buf[ 0 ] = 0x32;
 //  year_buf[ 1 ] = 0x30;
 // }
 // year_buf[ 2 ] = value[ 0 ];
 // year_buf[ 3 ] = value[ 1 ];
 // year_buf[ 4 ] = 0x00;

 // if( !isdigit( year_buf[ 0 ] ) || !isdigit( year_buf[ 1 ] ) )
 // {
 //  sprintf_s( error, 255, "Bad encoded UTCTime" );
 //  return T();
 // }

 // year = atoi( year_buf );
 //}
 //else
 //{
 // sprintf_s( error, 255, "Bad encoded UTCTime" );
 // return T();
 //}

 //result += year_buf;

 //if( value_len >= 4 )
 //{
 // month_buf[ 0 ] = 0x20;
 // month_buf[ 1 ] = value[ 2 ];
 // month_buf[ 2 ] = value[ 3 ];
 // month_buf[ 3 ] = 0x00;
 // 
 // if( !isdigit( month_buf[ 1 ] ) || !isdigit( month_buf[ 2 ] ) )
 // {
 //  sprintf_s( error, 255, "Bad encoded UTCTime" );
 //  return T();
 // }

 // month = atoi( month_buf );

 // month_buf[ 0 ] = 0x2F; // ":/" sign
 //}
 //else
 //{
 // sprintf_s( error, 255, "Bad encoded UTCTime" );
 // return T();
 //}

 //result += month_buf;

 //if( value_len >= 6 )
 //{
 // day_buf[ 0 ] = 0x20;
 // day_buf[ 1 ] = value[ 4 ];
 // day_buf[ 2 ] = value[ 5 ];
 // day_buf[ 3 ] = 0x00;

 // if( !isdigit( day_buf[ 1 ] ) || !isdigit( day_buf[ 2 ] ) )
 // {
 //  sprintf_s( error, 255, "Bad encoded UTCTime" );
 //  return T();
 // }

 // day = atoi( day_buf );

 // day_buf[ 0 ] = 0x2F; // "/" sign
 //}
 //else
 //{
 // sprintf_s( error, 255, "Bad encoded UTCTime" );
 // return T();
 //}

 //result += day_buf;

 //if( value_len >= 8 )
 //{
 // hours_buf[ 0 ] = 0x20; // " " sign
 // hours_buf[ 1 ] = value[ 6 ];
 // hours_buf[ 2 ] = value[ 7 ];
 // hours_buf[ 3 ] = 0x00;

 // if( !isdigit( hours_buf[ 1 ] ) || !isdigit( hours_buf[ 2 ] ) )
 // {
 //  sprintf_s( error, 255, "Bad encoded UTCTime" );
 //  return T();
 // }

 // hours = atoi( hours_buf );
 //}
 //else
 //{
 // sprintf_s( error, 255, "Bad encoded UTCTime" );
 // return T();
 //}

 //result += hours_buf;

 //if( value_len >= 10 )
 //{
 // minutes_buf[ 0 ] = 0x20;
 // minutes_buf[ 1 ] = value[ 8 ];
 // minutes_buf[ 2 ] = value[ 9 ];
 // minutes_buf[ 3 ] = 0x00;

 // if( !isdigit( minutes_buf[ 1 ] ) || !isdigit( minutes_buf[ 2 ] ) )
 // {
 //  sprintf_s( error, 255, "Bad encoded UTCTime" );
 //  return T();
 // }

 // minutes = atoi( minutes_buf );

 // minutes_buf[ 0 ] = 0x3A; // ":" sign
 //}
 //else
 //{
 // sprintf_s( error, 255, "Bad encoded UTCTime" );
 // return T();
 //}

 //result += minutes_buf;

 //char seconds_buf[ 4 ];
 //seconds_buf[ 0 ] = 0x00;
 //char difference_buf[ 6 ];
 //difference_buf[ 0 ] = 0x00;

 //if( value_len >= 11 )
 //{
 // if( ( value[ 10 ] != 0x2D ) && ( value[ 10 ] != 0x2B ) && ( value[ 10 ] != 0x5A ) )
 // {
 //  if( value_len >= 12 )
 //  {
 //   seconds_buf[ 0 ] = 0x20;
 //   seconds_buf[ 1 ] = value[ 10 ];
 //   seconds_buf[ 2 ] = value[ 11 ];
 //   seconds_buf[ 3 ] = 0x00;

 //   if( !isdigit( seconds_buf[ 1 ] ) || !isdigit( seconds_buf[ 2 ] ) )
 //   {
 //    sprintf_s( error, 255, "Bad encoded UTCTime" );
 //    return T();
 //   }

 //   format_with_seconds = true;
 //   seconds = atoi( seconds_buf );

 //   seconds_buf[ 0 ] = 0x3A; // ":" sign
 //  }
 //  else
 //  {
 //   sprintf_s( error, 255, "Bad encoded UTCTime" );
 //   return T();
 //  }

 //  if( (  value[ 12 ] == 0x2D  ) || ( value[ 12 ] == 0x2B ) )
 //  {
 //   if( value_len >= 17)
 //   {
 //    difference_buf[ 0 ] = 0x20;
 //    difference_buf[ 1 ] = value[ 13 ];
 //    difference_buf[ 2 ] = value[ 14 ];
 //    difference_buf[ 3 ] = value[ 15 ];
 //    difference_buf[ 4 ] = value[ 16 ];
 //    difference_buf[ 5 ] = 0x00;

 //    if( !isdigit( difference_buf[ 1 ] ) || 
 //        !isdigit( difference_buf[ 2 ] ) ||
 //        !isdigit( difference_buf[ 3 ] ) ||
 //        !isdigit( difference_buf[ 4 ] )
 //        )
 //    {
 //     sprintf_s( error, 255, "Bad encoded UTCTime" );
 //     return T();
 //    }

 //    difference_buf[ 0 ] = value[ 12 ];
 //   }
 //   else
 //   {
 //    sprintf_s( error, 255, "Bad encoded UTCTime" );
 //    return T();
 //   }
 //  }
 // }
 // else
 // {
 //  if( (  value[ 10 ] == 0x2D  ) || ( value[ 10 ] == 0x2B ) )
 //  {
 //   if( value_len >= 15 )
 //   {
 //    difference_buf[ 0 ] = 0x20;
 //    difference_buf[ 1 ] = value[ 11 ];
 //    difference_buf[ 2 ] = value[ 12 ];
 //    difference_buf[ 3 ] = value[ 13 ];
 //    difference_buf[ 4 ] = value[ 14 ];
 //    difference_buf[ 5 ] = 0x00;

 //    if( !isdigit( difference_buf[ 1 ] ) || 
 //        !isdigit( difference_buf[ 2 ] ) ||
 //        !isdigit( difference_buf[ 3 ] ) ||
 //        !isdigit( difference_buf[ 4 ] )
 //        )
 //    {
 //     sprintf_s( error, 255, "Bad encoded UTCTime" );
 //     return T();
 //    }

 //    difference_buf[ 0 ] = value[ 10 ];
 //   }
 //   else
 //   {
 //    sprintf_s( error, 255, "Bad encoded UTCTime" );
 //    return T();
 //   }
 //  }
 // }
 //}
 //else
 //{
 // sprintf_s( error, 255, "Bad encoded UTCTime" );
 // return T();
 //}

 return end;
}
//**************************************************************************
template< typename In, typename Out >
unsigned long long UTCTIME_value_block< In, Out >::encode_BER( bool size_only )
{
 return 0;
}
//**************************************************************************
template< typename In, typename Out >
MSXML2::IXMLDOMElementPtr UTCTIME_value_block< In, Out >::to_xml( MSXML2::IXMLDOMDocumentPtr pDoc )
{
 MSXML2::IXMLDOMElementPtr result = block_type::to_xml( pDoc );
 if( error[ 0 ] != 0x00 )
  return result;

 if( !is_hex_only )
 {
  std::wstring format = "%d/%d/%d %d:%d";

  if( format_with_seconds )
   format += ":%d";

  if( format_with_dif )
   format += "%c%d%d";

  format += " UTC time";

  wchar_t time_buf[ 255 ];
  memset( time_buf, 0x00, 255 * sizeof( wchar_t ) );

  swprintf_s( time_buf, 255, format.c_str(), year, month, day, hours, minutes, seconds, utc_dif_sign, utc_dif_hours, utc_dif_minutes );

  MSXML2::IXMLDOMElementPtr value_dec_xml = pDoc->createElement( L"value_dec" );
  value_dec_xml->appendChild( pDoc->createTextNode( time_buf ) );
  result->appendChild( value_dec_xml );
 }
 else
 {
  wchar_t* time_buf = ( wchar_t* ) malloc( ( block_length + 1 ) * sizeof( wchar_t ) );
  memset( time_buf, 0x00, ( block_length + 1 ) * sizeof( wchar_t ) );

  size_t converted = 0;
  errno_t error_num = mbstowcs_s( &converted, time_buf, block_length + 1, value_hex, block_length );
  if( error_num )
   return result;

  MSXML2::IXMLDOMElementPtr value_hex_xml = pDoc->createElement( L"value_hex" );
  value_hex_xml->appendChild( pDoc->createTextNode( time_buf ) );
  result->appendChild( value_hex_xml );
 }

 return result;
}
//**************************************************************************
template< typename In, typename Out >
bool UTCTIME_value_block< In, Out >::from_xml( MSXML2::IXMLDOMElementPtr element )
{
 return false;
}
//**************************************************************************
template< typename In, typename Out >
struct UTCTIME_type : virtual public VISIBLESTRING_type< In, Out >
{
 UTCTIME_type();

 static std::string block_name_static();
 virtual std::string block_name();
};
//**************************************************************************
template< typename In, typename Out >
UTCTIME_type< In, Out >::UTCTIME_type()
{
 tag_class = 1;
 tag_number = 23;
}
//**************************************************************************
template< typename In, typename Out >
std::string UTCTIME_type< In, Out >::block_name_static()
{
 return "UTCTIME";
}
//**************************************************************************
template< typename In, typename Out >
std::string UTCTIME_type< In, Out >::block_name()
{
 return block_name_static();
}
//**************************************************************************
template< typename In, typename Out >
struct GENERALIZEDTIME_type : virtual public VISIBLESTRING_type< In, Out >
{
 GENERALIZEDTIME_type();

 static std::string block_name_static();
 virtual std::string block_name();
};
//**************************************************************************
template< typename In, typename Out >
GENERALIZEDTIME_type< In, Out >::GENERALIZEDTIME_type()
{
 tag_class = 1;
 tag_number = 24;
}
//**************************************************************************
template< typename In, typename Out >
std::string GENERALIZEDTIME_type< In, Out >::block_name_static()
{
 return "GENERALIZEDTIME";
}
//**************************************************************************
template< typename In, typename Out >
std::string GENERALIZEDTIME_type< In, Out >::block_name()
{
 return block_name_static();
}
//**************************************************************************
template< typename In, typename Out >
struct DATE_type : virtual public UTF8STRING_type< In, Out >
{
 DATE_type();

 static std::string block_name_static();
 virtual std::string block_name();
};
//**************************************************************************
template< typename In, typename Out >
DATE_type< In, Out >::DATE_type()
{
 tag_class = 1;
 tag_number = 31;
}
//**************************************************************************
template< typename In, typename Out >
std::string DATE_type< In, Out >::block_name_static()
{
 return "DATE";
}
//**************************************************************************
template< typename In, typename Out >
std::string DATE_type< In, Out >::block_name()
{
 return block_name_static();
}
//**************************************************************************
template< typename In, typename Out >
struct TIMEOFDAY_type : virtual public UTF8STRING_type< In, Out >
{
 TIMEOFDAY_type();

 static std::string block_name_static();
 virtual std::string block_name();
};
//**************************************************************************
template< typename In, typename Out >
TIMEOFDAY_type< In, Out >::TIMEOFDAY_type()
{
 tag_class = 1;
 tag_number = 32;
}
//**************************************************************************
template< typename In, typename Out >
std::string TIMEOFDAY_type< In, Out >::block_name_static()
{
 return "TIME OF DAY";
}
//**************************************************************************
template< typename In, typename Out >
std::string TIMEOFDAY_type< In, Out >::block_name()
{
 return block_name_static();
}
//**************************************************************************
template< typename In, typename Out >
struct DATETIME_type : virtual public UTF8STRING_type< In, Out >
{
 DATETIME_type();

 static std::string block_name_static();
 virtual std::string block_name();
};
//**************************************************************************
template< typename In, typename Out >
DATETIME_type< In, Out >::DATETIME_type()
{
 tag_class = 1;
 tag_number = 33;
}
//**************************************************************************
template< typename In, typename Out >
std::string DATETIME_type< In, Out >::block_name_static()
{
 return "DATETIME";
}
//**************************************************************************
template< typename In, typename Out >
std::string DATETIME_type< In, Out >::block_name()
{
 return block_name_static();
}
//**************************************************************************
template< typename In, typename Out >
struct DURATION_type : virtual public UTF8STRING_type< In, Out >
{
 DURATION_type();

 static std::string block_name_static();
 virtual std::string block_name();
};
//**************************************************************************
template< typename In, typename Out >
DURATION_type< In, Out >::DURATION_type()
{
 tag_class = 1;
 tag_number = 34;
}
//**************************************************************************
template< typename In, typename Out >
std::string DURATION_type< In, Out >::block_name_static()
{
 return "DURATION";
}
//**************************************************************************
template< typename In, typename Out >
std::string DURATION_type< In, Out >::block_name()
{
 return block_name_static();
}
//**************************************************************************
template< typename In, typename Out >
struct TIME_type : virtual public UTF8STRING_type< In, Out >
{
 TIME_type();

 static std::string block_name_static();
 virtual std::string block_name();
};
//**************************************************************************
template< typename In, typename Out >
TIME_type< In, Out >::TIME_type()
{
 tag_class = 1;
 tag_number = 3;
}
//**************************************************************************
template< typename In, typename Out >
std::string TIME_type< In, Out >::block_name_static()
{
 return "TIME";
}
//**************************************************************************
template< typename In, typename Out >
std::string TIME_type< In, Out >::block_name()
{
 return block_name_static();
}
//**************************************************************************
#endif