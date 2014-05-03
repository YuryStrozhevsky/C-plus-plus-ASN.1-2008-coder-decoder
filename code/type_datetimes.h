#ifndef _0C2A870B_A6F6_45F0_92CE_EE98EAE24F0A
#define _0C2A870B_A6F6_45F0_92CE_EE98EAE24F0A
//**************************************************************************
struct UTCTIME_type : virtual public VISIBLESTRING_type
{
 virtual ~UTCTIME_type();
 virtual std::string type_name();

 template< typename T > T decode_value( T stream, unsigned long long max_block_len );

 virtual std::string encode_type( int detailed_level );
 virtual MSXML2::IXMLDOMElementPtr encode_type( int detailed_level, MSXML2::IXMLDOMDocumentPtr pDoc );
};
//**************************************************************************
UTCTIME_type::~UTCTIME_type()
{
}
//**************************************************************************
std::string UTCTIME_type::type_name()
{
 return "UTCTIME";
}
//**************************************************************************
std::string UTCTIME_type::encode_type( int detailed_level )
{
 unsigned long long value_len = block_length - inf_block->block_length - len_block->block_length;
 std::string result;

 if( value_len == 0 )
  return "";

 char year_buf[ 5 ];
 char month_buf[ 4 ];
 char day_buf[ 5 ];
 char hours_buf[ 4 ];
 char minutes_buf[ 4 ];

 if( value_len >= 2 )
 {
  if( value_block[ 0 ] > 0x35 ) // 19-th century
  {
   year_buf[ 0 ] = 0x31;
   year_buf[ 1 ] = 0x39;
  }
  else // 20-th century
  {
   year_buf[ 0 ] = 0x32;
   year_buf[ 1 ] = 0x30;
  }
  year_buf[ 2 ] = value_block[ 0 ];
  year_buf[ 3 ] = value_block[ 1 ];
  year_buf[ 4 ] = 0x00;
 }
 else
  return "";

 result += year_buf;

 if( value_len >= 4 )
 {
  month_buf[ 0 ] = 0x2F; // ":/" sign
  month_buf[ 1 ] = value_block[ 2 ];
  month_buf[ 2 ] = value_block[ 3 ];
  month_buf[ 3 ] = 0x00;
 }
 else
  return result;

 result += month_buf;

 if( value_len >= 6 )
 {
  day_buf[ 0 ] = 0x2F; // "/" sign
  day_buf[ 1 ] = value_block[ 4 ];
  day_buf[ 2 ] = value_block[ 5 ];
  day_buf[ 3 ] = 0x00;
 }
 else
  return result;

 result += day_buf;

 if( value_len >= 8 )
 {
  hours_buf[ 0 ] = 0x20; // " " sign
  hours_buf[ 1 ] = value_block[ 6 ];
  hours_buf[ 2 ] = value_block[ 7 ];
  hours_buf[ 3 ] = 0x00;
 }
 else
  return result;

 result += hours_buf;

 if( value_len >= 10 )
 {
  minutes_buf[ 0 ] = 0x3A; // ":" sign
  minutes_buf[ 1 ] = value_block[ 8 ];
  minutes_buf[ 2 ] = value_block[ 9 ];
  minutes_buf[ 3 ] = 0x00;
 }
 else
  return result;

 result += minutes_buf;

 char seconds_buf[ 4 ];
 seconds_buf[ 0 ] = 0x00;
 char difference_buf[ 6 ];
 difference_buf[ 0 ] = 0x00;

 if( value_len >= 11 )
 {
  if( ( value_block[ 10 ] != 0x2D ) && ( value_block[ 10 ] != 0x2B ) && ( value_block[ 10 ] != 0x5A ) )
  {
   if( value_len >= 12 )
   {
   seconds_buf[ 0 ] = 0x3A; // ":" sign
   seconds_buf[ 1 ] = value_block[ 10 ];
   seconds_buf[ 2 ] = value_block[ 11 ];
   seconds_buf[ 3 ] = 0x00;
   }
   else
    return result;

   if( (  value_block[ 12 ] == 0x2D  ) || ( value_block[ 12 ] == 0x2B ) )
   {
    if( value_len >= 17)
    {
     difference_buf[ 0 ] = value_block[ 12 ];
     difference_buf[ 1 ] = value_block[ 13 ];
     difference_buf[ 2 ] = value_block[ 14 ];
     difference_buf[ 3 ] = value_block[ 15 ];
     difference_buf[ 4 ] = value_block[ 16 ];
     difference_buf[ 5 ] = 0x00;
    }
    else
     return result;
   }
  }
  else
  {
   if( (  value_block[ 10 ] == 0x2D  ) || ( value_block[ 10 ] == 0x2B ) )
   {
    if( value_len >= 15 )
    {
     difference_buf[ 0 ] = value_block[ 10 ];
     difference_buf[ 1 ] = value_block[ 11 ];
     difference_buf[ 2 ] = value_block[ 12 ];
     difference_buf[ 3 ] = value_block[ 13 ];
     difference_buf[ 4 ] = value_block[ 14 ];
     difference_buf[ 5 ] = 0x00;
    }
    else
     return result;
   }
  }
 }
 else
  return result;

 result += seconds_buf;
 result += difference_buf;
 result += " UTC";

 return result;
}
//**************************************************************************
MSXML2::IXMLDOMElementPtr UTCTIME_type::encode_type( int detailed_level, MSXML2::IXMLDOMDocumentPtr pDoc )
{
 //-----------------------------------------------------
 // In case of using "successors" of type
 // without their own XML encoding

 std::string name = type_name();
 name = clear_xml_tag( name );

 wchar_t name_buf[255];
 size_t converted = 0;
 errno_t error_num = mbstowcs_s( &converted, name_buf, 255, name.c_str(), 255 );
 if( error_num )
 {
  sprintf_s( error, 255, "Wrong name convertion" );
  return NULL;
 }
 //-----------------------------------------------------

 MSXML2::IXMLDOMElementPtr result = pDoc->createElement( name_buf );

 if( warnings_and_error( pDoc, result) )
 {
  MSXML2::IXMLDOMElementPtr value_xml = pDoc->createElement( L"value" );

  std::string bit_value = encode_type( detailed_level );

  wchar_t* value_buf = ( wchar_t* ) malloc( ( bit_value.length() + 1 ) * sizeof( wchar_t ) );
  memset( value_buf, 0x00, ( bit_value.length() + 1 ) * sizeof( wchar_t ) );

  converted = 0;
  error_num = mbstowcs_s( &converted, value_buf, bit_value.length() + 1, bit_value.c_str(), bit_value.length() + 1 );
  if( error_num )
  {
   sprintf_s( error, 255, "Wrong UTCTIME value convertion" );
   return NULL;
  }

  value_xml->appendChild( pDoc->createTextNode( value_buf ) );
  result->appendChild( value_xml );

  free( value_buf );
 }

 return result;
}
//**************************************************************************
template< typename T >
T UTCTIME_type::decode_value( T stream, 
                              unsigned long long max_block_len )
{
 T end = VISIBLESTRING_type::decode_value( stream, max_block_len );

 unsigned long long value_len = block_length - inf_block->block_length - len_block->block_length;
 std::string result;

 if( value_len == 0 )
 {
  sprintf_s( error, 255, "Bad encoded UTCTime" );
  return T();
 }

 char year_buf[ 5 ];
 char month_buf[ 4 ];
 char day_buf[ 5 ];
 char hours_buf[ 4 ];
 char minutes_buf[ 4 ];

 if( value_len >= 2 )
 {
  if( value_block[ 0 ] > 0x35 ) // 19-th century
  {
   year_buf[ 0 ] = 0x31;
   year_buf[ 1 ] = 0x39;
  }
  else // 20-th century
  {
   year_buf[ 0 ] = 0x32;
   year_buf[ 1 ] = 0x30;
  }
  year_buf[ 2 ] = value_block[ 0 ];
  year_buf[ 3 ] = value_block[ 1 ];
  year_buf[ 4 ] = 0x00;

  if( !isdigit( year_buf[ 0 ] ) || !isdigit( year_buf[ 1 ] ) )
  {
   sprintf_s( error, 255, "Bad encoded UTCTime" );
   return T();
  }
 }
 else
 {
  sprintf_s( error, 255, "Bad encoded UTCTime" );
  return T();
 }

 result += year_buf;

 if( value_len >= 4 )
 {
  month_buf[ 0 ] = 0x20;
  month_buf[ 1 ] = value_block[ 2 ];
  month_buf[ 2 ] = value_block[ 3 ];
  month_buf[ 3 ] = 0x00;
  
  if( !isdigit( month_buf[ 1 ] ) || !isdigit( month_buf[ 2 ] ) )
  {
   sprintf_s( error, 255, "Bad encoded UTCTime" );
   return T();
  }

  month_buf[ 0 ] = 0x2F; // ":/" sign
 }
 else
 {
  sprintf_s( error, 255, "Bad encoded UTCTime" );
  return T();
 }

 result += month_buf;

 if( value_len >= 6 )
 {
  day_buf[ 0 ] = 0x20;
  day_buf[ 1 ] = value_block[ 4 ];
  day_buf[ 2 ] = value_block[ 5 ];
  day_buf[ 3 ] = 0x00;

  if( !isdigit( day_buf[ 1 ] ) || !isdigit( day_buf[ 2 ] ) )
  {
   sprintf_s( error, 255, "Bad encoded UTCTime" );
   return T();
  }

  day_buf[ 0 ] = 0x2F; // "/" sign
 }
 else
 {
  sprintf_s( error, 255, "Bad encoded UTCTime" );
  return T();
 }

 result += day_buf;

 if( value_len >= 8 )
 {
  hours_buf[ 0 ] = 0x20; // " " sign
  hours_buf[ 1 ] = value_block[ 6 ];
  hours_buf[ 2 ] = value_block[ 7 ];
  hours_buf[ 3 ] = 0x00;

  if( !isdigit( hours_buf[ 1 ] ) || !isdigit( hours_buf[ 2 ] ) )
  {
   sprintf_s( error, 255, "Bad encoded UTCTime" );
   return T();
  }
 }
 else
 {
  sprintf_s( error, 255, "Bad encoded UTCTime" );
  return T();
 }

 result += hours_buf;

 if( value_len >= 10 )
 {
  minutes_buf[ 0 ] = 0x20;
  minutes_buf[ 1 ] = value_block[ 8 ];
  minutes_buf[ 2 ] = value_block[ 9 ];
  minutes_buf[ 3 ] = 0x00;

  if( !isdigit( minutes_buf[ 1 ] ) || !isdigit( minutes_buf[ 2 ] ) )
  {
   sprintf_s( error, 255, "Bad encoded UTCTime" );
   return T();
  }

  minutes_buf[ 0 ] = 0x3A; // ":" sign
 }
 else
 {
  sprintf_s( error, 255, "Bad encoded UTCTime" );
  return T();
 }

 result += minutes_buf;

 char seconds_buf[ 4 ];
 seconds_buf[ 0 ] = 0x00;
 char difference_buf[ 6 ];
 difference_buf[ 0 ] = 0x00;

 if( value_len >= 11 )
 {
  if( ( value_block[ 10 ] != 0x2D ) && ( value_block[ 10 ] != 0x2B ) && ( value_block[ 10 ] != 0x5A ) )
  {
   if( value_len >= 12 )
   {
    seconds_buf[ 0 ] = 0x20;
    seconds_buf[ 1 ] = value_block[ 10 ];
    seconds_buf[ 2 ] = value_block[ 11 ];
    seconds_buf[ 3 ] = 0x00;

    if( !isdigit( seconds_buf[ 1 ] ) || !isdigit( seconds_buf[ 2 ] ) )
    {
     sprintf_s( error, 255, "Bad encoded UTCTime" );
     return T();
    }

    seconds_buf[ 0 ] = 0x3A; // ":" sign
   }
   else
   {
    sprintf_s( error, 255, "Bad encoded UTCTime" );
    return T();
   }

   if( (  value_block[ 12 ] == 0x2D  ) || ( value_block[ 12 ] == 0x2B ) )
   {
    if( value_len >= 17)
    {
     difference_buf[ 0 ] = 0x20;
     difference_buf[ 1 ] = value_block[ 13 ];
     difference_buf[ 2 ] = value_block[ 14 ];
     difference_buf[ 3 ] = value_block[ 15 ];
     difference_buf[ 4 ] = value_block[ 16 ];
     difference_buf[ 5 ] = 0x00;

     if( !isdigit( difference_buf[ 1 ] ) || 
         !isdigit( difference_buf[ 2 ] ) ||
         !isdigit( difference_buf[ 3 ] ) ||
         !isdigit( difference_buf[ 4 ] )
         )
     {
      sprintf_s( error, 255, "Bad encoded UTCTime" );
      return T();
     }

     difference_buf[ 0 ] = value_block[ 12 ];
    }
    else
    {
     sprintf_s( error, 255, "Bad encoded UTCTime" );
     return T();
    }
   }
  }
  else
  {
   if( (  value_block[ 10 ] == 0x2D  ) || ( value_block[ 10 ] == 0x2B ) )
   {
    if( value_len >= 15 )
    {
     difference_buf[ 0 ] = 0x20;
     difference_buf[ 1 ] = value_block[ 11 ];
     difference_buf[ 2 ] = value_block[ 12 ];
     difference_buf[ 3 ] = value_block[ 13 ];
     difference_buf[ 4 ] = value_block[ 14 ];
     difference_buf[ 5 ] = 0x00;

     if( !isdigit( difference_buf[ 1 ] ) || 
         !isdigit( difference_buf[ 2 ] ) ||
         !isdigit( difference_buf[ 3 ] ) ||
         !isdigit( difference_buf[ 4 ] )
         )
     {
      sprintf_s( error, 255, "Bad encoded UTCTime" );
      return T();
     }

     difference_buf[ 0 ] = value_block[ 10 ];
    }
    else
    {
     sprintf_s( error, 255, "Bad encoded UTCTime" );
     return T();
    }
   }
  }
 }
 else
 {
  sprintf_s( error, 255, "Bad encoded UTCTime" );
  return T();
 }

 return end;
}
//**************************************************************************
struct GENERALIZEDTIME_type : virtual public VISIBLESTRING_type
{
 virtual ~GENERALIZEDTIME_type();
 virtual std::string type_name();

 virtual std::string encode_type( int detailed_level );
 virtual MSXML2::IXMLDOMElementPtr encode_type( int detailed_level, MSXML2::IXMLDOMDocumentPtr pDoc );
};
//**************************************************************************
GENERALIZEDTIME_type::~GENERALIZEDTIME_type()
{
}
//**************************************************************************
std::string GENERALIZEDTIME_type::type_name()
{
 return "GENERALIZEDTIME";
}
//**************************************************************************
std::string GENERALIZEDTIME_type::encode_type( int detailed_level )
{
 // !!! For now this way ( temporary )
 return VISIBLESTRING_type::encode_type( detailed_level );

 unsigned long long value_len = block_length - inf_block->block_length - len_block->block_length;
 std::string result;

 if( value_len == 0 )
  return "";

 char year_buf[ 5 ];
 char month_buf[ 4 ];
 char day_buf[ 5 ];
 char hours_buf[ 4 ];
 char minutes_buf[ 4 ];

 if( value_len >= 2 )
 {
  if( value_block[ 0 ] > 0x35 ) // 19-th century
  {
   year_buf[ 0 ] = 0x31;
   year_buf[ 1 ] = 0x39;
  }
  else // 20-th century
  {
   year_buf[ 0 ] = 0x32;
   year_buf[ 1 ] = 0x30;
  }
  year_buf[ 2 ] = value_block[ 0 ];
  year_buf[ 3 ] = value_block[ 1 ];
  year_buf[ 4 ] = 0x00;
 }
 else
  return "";

 result += year_buf;

 if( value_len >= 4 )
 {
  month_buf[ 0 ] = 0x2F; // ":/" sign
  month_buf[ 1 ] = value_block[ 2 ];
  month_buf[ 2 ] = value_block[ 3 ];
  month_buf[ 3 ] = 0x00;
 }
 else
  return result;

 result += month_buf;

 if( value_len >= 6 )
 {
  day_buf[ 0 ] = 0x2F; // "/" sign
  day_buf[ 1 ] = value_block[ 4 ];
  day_buf[ 2 ] = value_block[ 5 ];
  day_buf[ 3 ] = 0x00;
 }
 else
  return result;

 result += day_buf;

 if( value_len >= 8 )
 {
  hours_buf[ 0 ] = 0x20; // " " sign
  hours_buf[ 1 ] = value_block[ 6 ];
  hours_buf[ 2 ] = value_block[ 7 ];
  hours_buf[ 3 ] = 0x00;
 }
 else
  return result;

 result += hours_buf;

 if( value_len >= 10 )
 {
  minutes_buf[ 0 ] = 0x3A; // ":" sign
  minutes_buf[ 1 ] = value_block[ 8 ];
  minutes_buf[ 2 ] = value_block[ 9 ];
  minutes_buf[ 3 ] = 0x00;
 }
 else
  return result;

 result += minutes_buf;

 char seconds_buf[ 4 ];
 seconds_buf[ 0 ] = 0x00;
 char difference_buf[ 6 ];
 difference_buf[ 0 ] = 0x00;

 if( value_len >= 11 )
 {
  if( ( value_block[ 10 ] != 0x2D ) && ( value_block[ 10 ] != 0x2B ) && ( value_block[ 10 ] != 0x5A ) )
  {
   if( value_len >= 12 )
   {
   seconds_buf[ 0 ] = 0x3A; // ":" sign
   seconds_buf[ 1 ] = value_block[ 10 ];
   seconds_buf[ 2 ] = value_block[ 11 ];
   seconds_buf[ 3 ] = 0x00;
   }
   else
    return result;

   if( (  value_block[ 12 ] == 0x2D  ) || ( value_block[ 12 ] == 0x2B ) )
   {
    if( value_len >= 17)
    {
     difference_buf[ 0 ] = value_block[ 12 ];
     difference_buf[ 1 ] = value_block[ 13 ];
     difference_buf[ 2 ] = value_block[ 14 ];
     difference_buf[ 3 ] = value_block[ 15 ];
     difference_buf[ 4 ] = value_block[ 16 ];
     difference_buf[ 5 ] = 0x00;
    }
    else
     return result;
   }
  }
  else
  {
   if( (  value_block[ 10 ] == 0x2D  ) || ( value_block[ 10 ] == 0x2B ) )
   {
    if( value_len >= 15 )
    {
     difference_buf[ 0 ] = value_block[ 10 ];
     difference_buf[ 1 ] = value_block[ 11 ];
     difference_buf[ 2 ] = value_block[ 12 ];
     difference_buf[ 3 ] = value_block[ 13 ];
     difference_buf[ 4 ] = value_block[ 14 ];
     difference_buf[ 5 ] = 0x00;
    }
    else
     return result;
   }
  }
 }
 else
  return result;

 result += seconds_buf;
 result += difference_buf;
 result += " UTC";

 return result;
}
//**************************************************************************
MSXML2::IXMLDOMElementPtr GENERALIZEDTIME_type::encode_type( int detailed_level, MSXML2::IXMLDOMDocumentPtr pDoc )
{
 //-----------------------------------------------------
 // In case of using "successors" of type
 // without their own XML encoding

 std::string name = type_name();
 name = clear_xml_tag( name );

 wchar_t name_buf[255];
 size_t converted = 0;
 errno_t error_num = mbstowcs_s( &converted, name_buf, 255, name.c_str(), 255 );
 if( error_num )
 {
  sprintf_s( error, 255, "Wrong name convertion" );
  return NULL;
 }
 //-----------------------------------------------------

 MSXML2::IXMLDOMElementPtr result = pDoc->createElement( name_buf );

 if( warnings_and_error( pDoc, result) )
 {
  MSXML2::IXMLDOMElementPtr value_xml = pDoc->createElement( L"value" );

  std::string bit_value = encode_type( detailed_level );

  wchar_t* value_buf = ( wchar_t* ) malloc( ( bit_value.length() + 1 ) * sizeof( wchar_t ) );
  memset( value_buf, 0x00, ( bit_value.length() + 1 ) * sizeof( wchar_t ) );

  converted = 0;
  error_num = mbstowcs_s( &converted, value_buf, bit_value.length() + 1, bit_value.c_str(), bit_value.length() + 1 );
  if( error_num )
  {
   sprintf_s( error, 255, "Wrong GENERALIZEDTIME value convertion" );
   return NULL;
  }

  value_xml->appendChild( pDoc->createTextNode( value_buf ) );
  result->appendChild( value_xml );

  free( value_buf );
 }

 return result;
}
//**************************************************************************
struct DATE_type : virtual public UTF8STRING_type
{
 virtual ~DATE_type();
 virtual std::string type_name();
};
//**************************************************************************
DATE_type::~DATE_type()
{
}
//**************************************************************************
std::string DATE_type::type_name()
{
 return "DATE";
}
//**************************************************************************
struct TIMEOFDAY_type : virtual public UTF8STRING_type
{
 virtual ~TIMEOFDAY_type();
 virtual std::string type_name();
};
//**************************************************************************
TIMEOFDAY_type::~TIMEOFDAY_type()
{
}
//**************************************************************************
std::string TIMEOFDAY_type::type_name()
{
 return "TIMEOFDAY";
}
//**************************************************************************
struct DATETIME_type : virtual public UTF8STRING_type
{
 virtual ~DATETIME_type();
 virtual std::string type_name();
};
//**************************************************************************
DATETIME_type::~DATETIME_type()
{
}
//**************************************************************************
std::string DATETIME_type::type_name()
{
 return "DATETIME";
}
//**************************************************************************
struct DURATION_type : virtual public UTF8STRING_type
{
 virtual ~DURATION_type();
 virtual std::string type_name();
};
//**************************************************************************
DURATION_type::~DURATION_type()
{
}
//**************************************************************************
std::string DURATION_type::type_name()
{
 return "DURATION";
}
//**************************************************************************
struct TIME_type : virtual public UTF8STRING_type
{
 virtual ~TIME_type();
 virtual std::string type_name();
};
//**************************************************************************
TIME_type::~TIME_type()
{
}
//**************************************************************************
std::string TIME_type::type_name()
{
 return "TIME";
}
//**************************************************************************
#endif