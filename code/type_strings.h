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
#ifndef _55A67A0D_B3AE_4635_8274_BF209D7E88A4
#define _55A67A0D_B3AE_4635_8274_BF209D7E88A4
//**************************************************************************
template< typename In, typename Out >
struct UTF8STRING_value_block : virtual public value_block_type< In, Out >,
                                virtual public hex_block_type< In, Out >
{
 UTF8STRING_value_block();

 virtual MSXML2::IXMLDOMElementPtr to_xml( MSXML2::IXMLDOMDocumentPtr pDoc );
 virtual bool from_xml( MSXML2::IXMLDOMElementPtr element );
};
//**************************************************************************
template< typename In, typename Out >
UTF8STRING_value_block< In, Out >::UTF8STRING_value_block()
{
}
//**************************************************************************
template< typename In, typename Out >
MSXML2::IXMLDOMElementPtr UTF8STRING_value_block< In, Out >::to_xml( MSXML2::IXMLDOMDocumentPtr pDoc )
{
 MSXML2::IXMLDOMElementPtr result = block_type< In, Out >::to_xml( pDoc );
 if( error[ 0 ] != 0x00 )
  return result;

 MSXML2::IXMLDOMElementPtr value_hex_xml = pDoc->createElement( L"value_hex" );

 unsigned long long value_len = block_length;

 int wide_string_len = MultiByteToWideChar( CP_UTF8, MB_ERR_INVALID_CHARS, (LPCSTR)value_hex, ( int )block_length, NULL, 0 );
 if( !wide_string_len )
 {
  sprintf_s( error, 255, "MultiByteToWideChar error" );
  return result;
 }

 wchar_t* wide_buf = (wchar_t*) malloc( ( wide_string_len + 1 ) * sizeof( wchar_t ) ); // "wide_string_len" is in "chars", hence multiply it by 2
 memset( wide_buf, 0x00, ( wide_string_len + 1 ) * sizeof( wchar_t ) );

 wide_string_len = MultiByteToWideChar( CP_UTF8, MB_ERR_INVALID_CHARS, (LPCSTR)value_hex, ( int )block_length, (LPWSTR)wide_buf, wide_string_len );
 if( !wide_string_len )
 {
  sprintf_s( error, 255, "MultiByteToWideChar error" );
  free( wide_buf );
  return result;
 }
 
 value_hex_xml->appendChild( pDoc->createTextNode( wide_buf ) );
 
 free( wide_buf );
 
 result->appendChild( value_hex_xml );

 return result;
}
//**************************************************************************
template< typename In, typename Out >
bool UTF8STRING_value_block< In, Out >::from_xml( MSXML2::IXMLDOMElementPtr element )
{
 if( !element )
  return false;

 if( value_hex )
  free( value_hex );

 is_hex_only = true;

 MSXML2::IXMLDOMElementPtr value_hex_xml = element->selectSingleNode( L"value_hex" );
 if( !value_hex_xml )
  return false;

 std::wstring text = ( wchar_t* ) value_hex_xml->text;

 int string_len = WideCharToMultiByte( CP_UTF8, 0, (LPCWSTR)value_hex_xml->text, -1, NULL, 0, NULL, NULL );
 if( !string_len )
  return false;

 value_hex = ( unsigned char* ) malloc( string_len );

 string_len = WideCharToMultiByte( CP_UTF8, 0, (LPCWSTR)value_hex_xml->text, -1, ( char* ) value_hex, string_len, NULL, NULL );
 if( !string_len )
  return false;

 block_length = string_len - 1; // minus least NULL symbols

 return true;
}
//**************************************************************************
template< typename In, typename Out >
struct UTF8STRING_type : virtual public BASIC_type< In, Out >
{
 UTF8STRING_type();

 static std::string block_name_static();
 virtual std::string block_name();
};
//**************************************************************************
template< typename In, typename Out >
UTF8STRING_type< In, Out >::UTF8STRING_type()
{
 tag_class = 1;
 tag_number = 12;

 value_block = new UTF8STRING_value_block< In, Out >();
}
//**************************************************************************
template< typename In, typename Out >
std::string UTF8STRING_type< In, Out >::block_name_static()
{
 return "UTF8STRING";
}
//**************************************************************************
template< typename In, typename Out >
std::string UTF8STRING_type< In, Out >::block_name()
{
 return block_name_static();
}
//**************************************************************************
template< typename In, typename Out >
struct BMPSTRING_value_block : virtual public value_block_type< In, Out >,
                               virtual public hex_block_type< In, Out >
{
 BMPSTRING_value_block();

 virtual MSXML2::IXMLDOMElementPtr to_xml( MSXML2::IXMLDOMDocumentPtr pDoc );
 virtual bool from_xml( MSXML2::IXMLDOMElementPtr element );
};
//**************************************************************************
template< typename In, typename Out >
BMPSTRING_value_block< In, Out >::BMPSTRING_value_block()
{
}
//**************************************************************************
template< typename In, typename Out >
MSXML2::IXMLDOMElementPtr BMPSTRING_value_block< In, Out >::to_xml( MSXML2::IXMLDOMDocumentPtr pDoc )
{
 MSXML2::IXMLDOMElementPtr result = block_type< In, Out >::to_xml( pDoc );
 if( error[ 0 ] != 0x00 )
  return result;

 MSXML2::IXMLDOMElementPtr value_hex_xml = pDoc->createElement( L"value_hex" );

 std::string codes = xcodes( value_hex, block_length, 2 );

 wchar_t* wcodes = ( wchar_t* ) malloc( ( codes.length() + 1 ) * sizeof( wchar_t ) );
 size_t converted = 0;
 errno_t error_num = mbstowcs_s( &converted, wcodes, ( codes.length() + 1 ), codes.c_str(), ( codes.length() + 1 ) );
 if( error_num )
 {
  sprintf_s( error, 255, "Wrong name convertion" );
  return NULL;
 }

 value_hex_xml->appendChild( pDoc->createTextNode( wcodes ) );
 result->appendChild( value_hex_xml );

 free( wcodes );

 return result;
}
//**************************************************************************
template< typename In, typename Out >
bool BMPSTRING_value_block< In, Out >::from_xml( MSXML2::IXMLDOMElementPtr element )
{
 if( !element )
  return false;

 if( value_hex )
  free( value_hex );

 is_hex_only = true;

 MSXML2::IXMLDOMElementPtr value_hex_xml = element->selectSingleNode( L"value_hex" );
 if( !value_hex_xml )
  return false;

 block_length = value_hex_xml->text.length();

 value_hex = ( unsigned char* ) malloc( ( size_t )( block_length * 2 ) );
 if( !value_hex )
  return false;

 wchar_t* wstring = ( wchar_t* )value_hex_xml->text;

 for( size_t i = 0; i < block_length; i++ )
 {
  value_hex[ i * 2 + 0 ] = ( ( char* )wstring )[ i * 2 + 1 ];
  value_hex[ i * 2 + 1 ] = ( ( char* )wstring )[ i * 2 + 0 ];
 }

 block_length *= 2;

 return true;
}
//**************************************************************************
template< typename In, typename Out >
struct BMPSTRING_type : virtual public BASIC_type< In, Out >
{
 BMPSTRING_type();

 static std::string block_name_static();
 virtual std::string block_name();
};
//**************************************************************************
template< typename In, typename Out >
BMPSTRING_type< In, Out >::BMPSTRING_type()
{
 tag_class = 1;
 tag_number = 30;

 value_block = new BMPSTRING_value_block< In, Out >();
}
//**************************************************************************
template< typename In, typename Out >
std::string BMPSTRING_type< In, Out >::block_name_static()
{
 return "BMPSTRING";
}
//**************************************************************************
template< typename In, typename Out >
std::string BMPSTRING_type< In, Out >::block_name()
{
 return block_name_static();
}
//**************************************************************************
template< typename In, typename Out >
struct UNIVERSALSTRING_value_block : virtual public value_block_type< In, Out >,
                                     virtual public hex_block_type< In, Out >
{
 UNIVERSALSTRING_value_block();

 virtual MSXML2::IXMLDOMElementPtr to_xml( MSXML2::IXMLDOMDocumentPtr pDoc );
 virtual bool from_xml( MSXML2::IXMLDOMElementPtr element );
};
//**************************************************************************
template< typename In, typename Out >
UNIVERSALSTRING_value_block< In, Out >::UNIVERSALSTRING_value_block()
{
}
//**************************************************************************
template< typename In, typename Out >
MSXML2::IXMLDOMElementPtr UNIVERSALSTRING_value_block< In, Out >::to_xml( MSXML2::IXMLDOMDocumentPtr pDoc )
{
 MSXML2::IXMLDOMElementPtr result = block_type< In, Out >::to_xml( pDoc );
 if( error[ 0 ] != 0x00 )
  return result;

 MSXML2::IXMLDOMElementPtr value_hex_xml = pDoc->createElement( L"value_hex" );

 std::string codes = xcodes( value_hex, block_length, 4 );

 wchar_t* wcodes = ( wchar_t* ) malloc( ( codes.length() + 1 ) * sizeof( wchar_t ) );
 size_t converted = 0;
 errno_t error_num = mbstowcs_s( &converted, wcodes, ( codes.length() + 1 ), codes.c_str(), ( codes.length() + 1 ) );
 if( error_num )
 {
  sprintf_s( error, 255, "Wrong name convertion" );
  return NULL;
 }

 value_hex_xml->appendChild( pDoc->createTextNode( wcodes ) );
 result->appendChild( value_hex_xml );

 free( wcodes );

 return result;
}
//**************************************************************************
template< typename In, typename Out >
bool UNIVERSALSTRING_value_block< In, Out >::from_xml( MSXML2::IXMLDOMElementPtr element )
{
 if( !element )
  return false;

 if( value_hex )
  free( value_hex );

 is_hex_only = true;

 MSXML2::IXMLDOMElementPtr value_hex_xml = element->selectSingleNode( L"value_hex" );
 if( !value_hex_xml )
  return false;

 block_length = value_hex_xml->text.length();

 value_hex = ( unsigned char* ) malloc( ( size_t )( block_length * 4 ) );
 if( !value_hex )
  return false;

 wchar_t* wstring = ( wchar_t* )value_hex_xml->text;

 for( size_t i = 0; i < block_length; i++ )
 {
  value_hex[ i * 4 + 0 ] = 0x00;
  value_hex[ i * 4 + 1 ] = 0x00;
  value_hex[ i * 4 + 2 ] = ( ( char* )wstring )[ i * 2 + 1 ];
  value_hex[ i * 4 + 3 ] = ( ( char* )wstring )[ i * 2 + 0 ];
 }

 block_length *= 4;

 return true;
}
//**************************************************************************
template< typename In, typename Out >
struct UNIVERSALSTRING_type : virtual public BASIC_type< In, Out >
{
 UNIVERSALSTRING_type();

 static std::string block_name_static();
 virtual std::string block_name();
};
//**************************************************************************
template< typename In, typename Out >
UNIVERSALSTRING_type< In, Out >::UNIVERSALSTRING_type()
{
 tag_class = 1;
 tag_number = 28;

 value_block = new UNIVERSALSTRING_value_block< In, Out >();
}
//**************************************************************************
template< typename In, typename Out >
std::string UNIVERSALSTRING_type< In, Out >::block_name_static()
{
 return "UNIVERSALSTRING";
}
//**************************************************************************
template< typename In, typename Out >
std::string UNIVERSALSTRING_type< In, Out >::block_name()
{
 return block_name_static();
}
//**************************************************************************
template< typename In, typename Out >
struct SIMPLESTRING_value_block : virtual public value_block_type< In, Out >,
                                  virtual public hex_block_type< In, Out >
{
 SIMPLESTRING_value_block();

 virtual MSXML2::IXMLDOMElementPtr to_xml( MSXML2::IXMLDOMDocumentPtr pDoc );
 virtual bool from_xml( MSXML2::IXMLDOMElementPtr element );
};
//**************************************************************************
template< typename In, typename Out >
SIMPLESTRING_value_block< In, Out >::SIMPLESTRING_value_block()
{
}
//**************************************************************************
template< typename In, typename Out >
MSXML2::IXMLDOMElementPtr SIMPLESTRING_value_block< In, Out >::to_xml( MSXML2::IXMLDOMDocumentPtr pDoc )
{
 MSXML2::IXMLDOMElementPtr result = block_type< In, Out >::to_xml( pDoc );
 if( error[ 0 ] != 0x00 )
  return result;

 if( !block_length )
  return result;

 MSXML2::IXMLDOMElementPtr value_hex_xml = pDoc->createElement( L"value_hex" );

 wchar_t* value_buf = NULL;

 try // try to store as a string
 {
  unsigned long long value_len = block_length;
  value_buf = ( wchar_t* ) malloc( ( size_t ) ( ( value_len + 1 ) * sizeof( wchar_t ) ) );

  size_t converted = 0;
  errno_t error_num = mbstowcs_s( &converted, value_buf, ( size_t ) ( value_len + 1 ), (char*)value_hex, ( size_t )value_len );
  if( error_num || ( converted == 1 ) ) // if error or converted only NULL character
   throw "";

  value_hex_xml->appendChild( pDoc->createTextNode( value_buf ) );
  result->appendChild( value_hex_xml );
  
  free( value_buf );
 }
 catch(...) // a problem, store as a "hex-encoded" value
 {
  free( value_buf );
  
  value_hex_xml->put_dataType( L"bin.hex" );

  SAFEARRAY* psa = SafeArrayCreateVector( VT_UI1, 0L, ( ULONG )block_length );
  if( psa == NULL )
   return result;

  memcpy( psa->pvData, value_hex, ( size_t )block_length );

  VARIANT var;
  VariantInit( &var );
  var.parray = psa;
  var.vt = ( VT_ARRAY | VT_UI1 );

  value_hex_xml->nodeTypedValue = var; 
  
  HRESULT hr = SafeArrayDestroy( psa );
  if( FAILED( hr ))
   return result;

  result->appendChild( value_hex_xml );
 }

 return result;
}
//**************************************************************************
template< typename In, typename Out >
bool SIMPLESTRING_value_block< In, Out >::from_xml( MSXML2::IXMLDOMElementPtr element )
{
 if( !element )
  return false;

 is_hex_only = true;

 MSXML2::IXMLDOMElementPtr value_hex_xml = element->selectSingleNode( L"value_hex" );
 if( !value_hex_xml )
  return false;

 std::wstring text = ( wchar_t* ) value_hex_xml->text;

 value_hex = ( unsigned char* ) malloc( text.length() + 1 );

 size_t converted = 0;
 errno_t errnum = wcstombs_s( &converted, (char*) value_hex, text.length() + 1, text.c_str(), text.length() );
 if( errnum )
  return false;

 block_length = text.length();

 return true;
}
//**************************************************************************
template< typename In, typename Out >
struct SIMPLESTRING_type : virtual public BASIC_type< In, Out >
{
 SIMPLESTRING_type();

 static std::string block_name_static();
 virtual std::string block_name();
};
//**************************************************************************
template< typename In, typename Out >
SIMPLESTRING_type< In, Out >::SIMPLESTRING_type()
{
 value_block = new SIMPLESTRING_value_block< In, Out >();
}
//**************************************************************************
template< typename In, typename Out >
std::string SIMPLESTRING_type< In, Out >::block_name_static()
{
 return "SIMPLESTRING";
}
//**************************************************************************
template< typename In, typename Out >
std::string SIMPLESTRING_type< In, Out >::block_name()
{
 return block_name_static();
}
//**************************************************************************
template< typename In, typename Out >
struct NUMERICSTRING_type : virtual public SIMPLESTRING_type< In, Out >
{
 NUMERICSTRING_type();

 static std::string block_name_static();
 virtual std::string block_name();
};
//**************************************************************************
template< typename In, typename Out >
NUMERICSTRING_type< In, Out >::NUMERICSTRING_type()
{
 tag_class = 1;
 tag_number = 18;
}
//**************************************************************************
template< typename In, typename Out >
std::string NUMERICSTRING_type< In, Out >::block_name_static()
{
 return "NUMERICSTRING";
}
//**************************************************************************
template< typename In, typename Out >
std::string NUMERICSTRING_type< In, Out >::block_name()
{
 return block_name_static();
}
//**************************************************************************
template< typename In, typename Out >
struct PRINTABLESTRING_type : virtual public SIMPLESTRING_type< In, Out >
{
 PRINTABLESTRING_type();

 static std::string block_name_static();
 virtual std::string block_name();
};
//**************************************************************************
template< typename In, typename Out >
PRINTABLESTRING_type< In, Out >::PRINTABLESTRING_type()
{
 tag_class = 1;
 tag_number = 19;
}
//**************************************************************************
template< typename In, typename Out >
std::string PRINTABLESTRING_type< In, Out >::block_name_static()
{
 return "PRINTABLESTRING";
}
//**************************************************************************
template< typename In, typename Out >
std::string PRINTABLESTRING_type< In, Out >::block_name()
{
 return block_name_static();
}
//**************************************************************************
template< typename In, typename Out >
struct TELETEXSTRING_type : virtual public SIMPLESTRING_type< In, Out >
{
 TELETEXSTRING_type();

 static std::string block_name_static();
 virtual std::string block_name();
};
//**************************************************************************
template< typename In, typename Out >
TELETEXSTRING_type< In, Out >::TELETEXSTRING_type()
{
 tag_class = 1;
 tag_number = 20;
}
//**************************************************************************
template< typename In, typename Out >
std::string TELETEXSTRING_type< In, Out >::block_name_static()
{
 return "TELETEXSTRING";
}
//**************************************************************************
template< typename In, typename Out >
std::string TELETEXSTRING_type< In, Out >::block_name()
{
 return block_name_static();
}
//**************************************************************************
template< typename In, typename Out >
struct VIDEOTEXSTRING_type : virtual public SIMPLESTRING_type< In, Out >
{
 VIDEOTEXSTRING_type();

 static std::string block_name_static();
 virtual std::string block_name();
};
//**************************************************************************
template< typename In, typename Out >
VIDEOTEXSTRING_type< In, Out >::VIDEOTEXSTRING_type()
{
 tag_class = 1;
 tag_number = 21;
}
//**************************************************************************
template< typename In, typename Out >
std::string VIDEOTEXSTRING_type< In, Out >::block_name_static()
{
 return "VIDEOTEXSTRING";
}
//**************************************************************************
template< typename In, typename Out >
std::string VIDEOTEXSTRING_type< In, Out >::block_name()
{
 return block_name_static();
}
//**************************************************************************
template< typename In, typename Out >
struct IA5STRING_type : virtual public SIMPLESTRING_type< In, Out >
{
 IA5STRING_type();

 static std::string block_name_static();
 virtual std::string block_name();
};
//**************************************************************************
template< typename In, typename Out >
IA5STRING_type< In, Out >::IA5STRING_type()
{
 tag_class = 1;
 tag_number = 22;
}
//**************************************************************************
template< typename In, typename Out >
std::string IA5STRING_type< In, Out >::block_name_static()
{
 return "IA5STRING";
}
//**************************************************************************
template< typename In, typename Out >
std::string IA5STRING_type< In, Out >::block_name()
{
 return block_name_static();
}
//**************************************************************************
template< typename In, typename Out >
struct GRAPHICSTRING_type : virtual public SIMPLESTRING_type< In, Out >
{
 GRAPHICSTRING_type();

 static std::string block_name_static();
 virtual std::string block_name();
};
//**************************************************************************
template< typename In, typename Out >
GRAPHICSTRING_type< In, Out >::GRAPHICSTRING_type()
{
 tag_class = 1;
 tag_number = 25;
}
//**************************************************************************
template< typename In, typename Out >
std::string GRAPHICSTRING_type< In, Out >::block_name_static()
{
 return "GRAPHICSTRING";
}
//**************************************************************************
template< typename In, typename Out >
std::string GRAPHICSTRING_type< In, Out >::block_name()
{
 return block_name_static();
}
//**************************************************************************
template< typename In, typename Out >
struct VISIBLESTRING_type : virtual public SIMPLESTRING_type< In, Out >
{
 VISIBLESTRING_type();

 static std::string block_name_static();
 virtual std::string block_name();
};
//**************************************************************************
template< typename In, typename Out >
VISIBLESTRING_type< In, Out >::VISIBLESTRING_type()
{
 tag_class = 1;
 tag_number = 26;
}
//**************************************************************************
template< typename In, typename Out >
std::string VISIBLESTRING_type< In, Out >::block_name_static()
{
 return "VISIBLESTRING";
}
//**************************************************************************
template< typename In, typename Out >
std::string VISIBLESTRING_type< In, Out >::block_name()
{
 return block_name_static();
}
//**************************************************************************
template< typename In, typename Out >
struct GENERALSTRING_type : virtual public SIMPLESTRING_type< In, Out >
{
 GENERALSTRING_type();

 static std::string block_name_static();
 virtual std::string block_name();
};
//**************************************************************************
template< typename In, typename Out >
GENERALSTRING_type< In, Out >::GENERALSTRING_type()
{
 tag_class = 1;
 tag_number = 27;
}
//**************************************************************************
template< typename In, typename Out >
std::string GENERALSTRING_type< In, Out >::block_name_static()
{
 return "GENERALSTRING";
}
//**************************************************************************
template< typename In, typename Out >
std::string GENERALSTRING_type< In, Out >::block_name()
{
 return block_name_static();
}
//**************************************************************************
template< typename In, typename Out >
struct CHARACTERSTRING_type : virtual public SIMPLESTRING_type< In, Out >
{
 CHARACTERSTRING_type();

 static std::string block_name_static();
 virtual std::string block_name();
};
//**************************************************************************
template< typename In, typename Out >
CHARACTERSTRING_type< In, Out >::CHARACTERSTRING_type()
{
 tag_class = 1;
 tag_number = 29;
}
//**************************************************************************
template< typename In, typename Out >
std::string CHARACTERSTRING_type< In, Out >::block_name_static()
{
 return "CHARACTERSTRING";
}
//**************************************************************************
template< typename In, typename Out >
std::string CHARACTERSTRING_type< In, Out >::block_name()
{
 return block_name_static();
}
//**************************************************************************
template< typename In, typename Out >
struct OBJECTDESCRIPTOR_type : virtual public SIMPLESTRING_type< In, Out >
{
 OBJECTDESCRIPTOR_type();

 static std::string block_name_static();
 virtual std::string block_name();
};
//**************************************************************************
template< typename In, typename Out >
OBJECTDESCRIPTOR_type< In, Out >::OBJECTDESCRIPTOR_type()
{
 tag_class = 1;
 tag_number = 7;
}
//**************************************************************************
template< typename In, typename Out >
std::string OBJECTDESCRIPTOR_type< In, Out >::block_name_static()
{
 return "OBJECT DESCRIPTOR";
}
//**************************************************************************
template< typename In, typename Out >
std::string OBJECTDESCRIPTOR_type< In, Out >::block_name()
{
 return block_name_static();
}
//**************************************************************************
template< typename In, typename Out >
struct OIDINTER_type : virtual public UTF8STRING_type< In, Out >
{
 OIDINTER_type();

 static std::string block_name_static();
 virtual std::string block_name();
};
//**************************************************************************
template< typename In, typename Out >
OIDINTER_type< In, Out >::OIDINTER_type()
{
 tag_class = 1;
 tag_number = 35;
}
//**************************************************************************
template< typename In, typename Out >
std::string OIDINTER_type< In, Out >::block_name_static()
{
 return "OID INTERNATIONALIZED";
}
//**************************************************************************
template< typename In, typename Out >
std::string OIDINTER_type< In, Out >::block_name()
{
 return block_name_static();
}
//**************************************************************************
template< typename In, typename Out >
struct ROIDINTER_type : virtual public UTF8STRING_type< In, Out >
{
 ROIDINTER_type();

 static std::string block_name_static();
 virtual std::string block_name();
};
//**************************************************************************
template< typename In, typename Out >
ROIDINTER_type< In, Out >::ROIDINTER_type()
{
 tag_class = 1;
 tag_number = 36;
}
//**************************************************************************
template< typename In, typename Out >
std::string ROIDINTER_type< In, Out >::block_name_static()
{
 return "RELATIVE OID INTERNATIONALIZED";
}
//**************************************************************************
template< typename In, typename Out >
std::string ROIDINTER_type< In, Out >::block_name()
{
 return block_name_static();
}
//**************************************************************************
#endif