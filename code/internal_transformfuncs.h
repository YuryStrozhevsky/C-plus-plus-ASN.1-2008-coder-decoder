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
#ifndef _352E4664_A7EA_4B86_8370_C9C9257A9FBC
#define _352E4664_A7EA_4B86_8370_C9C9257A9FBC
//**************************************************************************
struct encode_result
{
 encode_result();

 unsigned long long encoded;

 char error[ 255 ];
 std::vector< std::string > warnings;
};
//**************************************************************************
encode_result::encode_result() : encoded( 0 )
{
 memset( error, 0x00, 255 );
}
//**************************************************************************
template< class In, class V, class Out >
In value_init( In stream, 
               unsigned long long max_block_len,
               BASIC_type< In, Out >** basic_type,
               identification_block_type< In, Out >* id_block,
               length_block_type< In, Out >* len_block )
{
 (*basic_type)->id_block = NULL;
 (*basic_type)->len_block = NULL;
 delete (*basic_type); // delete previously created "BASIC_type" object instance

 V* type = new V();

 type->id_block = id_block;
 type->len_block = len_block;
 type->stream_in = stream;

 In end = type->decode_BER( ( len_block->is_indefinite_form ) ? max_block_len : len_block->length );

 (*basic_type) = type;

 return end;
}
//**************************************************************************
template< typename In, typename Out >
In decode_BER_raw( In stream, unsigned long long max_block_len, BASIC_type< In, Out >** basic_type )
{
 (*basic_type) = new BASIC_type< In, Out >();
 (*basic_type)->stream_in = stream;

 if( stream == In() )
 {
  sprintf_s( (*basic_type)->error, 255, "NULL stream is unaccepted" );
  return In();
 }

 if( !max_block_len ) // No error, just return empty type
  return In();

 //--------------------------------
 identification_block_type< In, Out  >* id_block = new identification_block_type< In, Out  >();
 id_block->stream_in = stream;

 (*basic_type)->id_block = id_block;
 
 In end = id_block->decode_BER( max_block_len );
 if( id_block->error[ 0 ] != 0x00 )
 {
  sprintf_s( (*basic_type)->error, 255, id_block->error );
  return end;
 }

 max_block_len -= id_block->block_length;

 length_block_type< In, Out  >* len_block = new length_block_type< In, Out  >();
 len_block->stream_in = end;

 (*basic_type)->len_block = len_block;
 
 end = len_block->decode_BER( max_block_len );
 if( len_block->error[ 0 ] != 0x00 ) // Give a chance to process in case of zero-length value block
 {
  sprintf_s( (*basic_type)->error, 255, len_block->error );
  return end;
 }
 
 max_block_len -= len_block->block_length;

 // Check for usign indefinite length form in encoding for primitive types
 if( ( !id_block->is_constructed ) &&
     ( len_block->is_indefinite_form ) )
 {
  sprintf_s( (*basic_type)->error, 255, "Indefinite length form used for primitive encoding form" );
  return In();
 }

 //--------------------------------

 switch( id_block->tag_class )
 {
  case 1: // UNIVERSAL
   if( ( id_block->tag_number >= 37 ) && ( !id_block->is_hex_only ) )
   {
    sprintf_s( (*basic_type)->error, 255, "UNIVERSAL 37 and upper tags are reserved by ASN.1 standard" );
    return In();
   }

   switch( id_block->tag_number )
   {
    case 0: // Hopefuly EOC_type (end-of-content)
     if( id_block->is_constructed || len_block->length )
     {
      sprintf_s( (*basic_type)->error, 255, "Type [UNIVERSAL 0] is reserved" );
      return In();
     }

     return value_init< In, EOC_type< In, Out > >( end, 
                                                   max_block_len,
                                                   basic_type, 
                                                   id_block, 
                                                   len_block );
     break;
    case 1: // BOOLEAN
     return value_init< In, BOOLEAN_type< In, Out > >( end, 
                                                       max_block_len,
                                                       basic_type, 
                                                       id_block, 
                                                       len_block );
     break;
    case 2: // INTEGER
     return value_init< In, INTEGER_type< In, Out  > >( end, 
                                                        max_block_len,
                                                        basic_type, 
                                                        id_block, 
                                                        len_block );
     break;
    case 3: // BITSTRING
     return value_init< In, BITSTRING_type< In, Out > >( end, 
                                                         max_block_len,
                                                         basic_type, 
                                                         id_block, 
                                                         len_block );
     break;
    case 4: // OCTETSTRING
     return value_init< In, OCTETSTRING_type< In, Out > >( end, 
                                                           max_block_len,
                                                           basic_type, 
                                                           id_block, 
                                                           len_block );
     break;
    case 5: // NULL
     return value_init< In, NULL_type< In, Out > >( end, 
                                                    max_block_len,
                                                    basic_type, 
                                                    id_block, 
                                                    len_block );
     break;
    case 6: // OBJECT IDENTIFIER
     return value_init< In, OID_type< In, Out > >( end, 
                                                   max_block_len,
                                                   basic_type, 
                                                   id_block, 
                                                   len_block );
     break;
    case 7: // OBJECT DESCRIPTOR
     return value_init< In, OBJECTDESCRIPTOR_type< In, Out > >( end, 
                                                                max_block_len,
                                                                basic_type, 
                                                                id_block, 
                                                                len_block );
     break;
    case 8: // EXTERNAL TYPE
     return value_init< In, EXTERNAL_type< In, Out > >( end, 
                                                        max_block_len,
                                                        basic_type, 
                                                        id_block, 
                                                        len_block );
     break;
    case 9: // REAL
     return value_init< In, REAL_type< In, Out > >( end, 
                                                    max_block_len,
                                                    basic_type, 
                                                    id_block, 
                                                    len_block );
     break;
    case 10: // ENUMERATED
     return value_init< In, ENUMERATED_type< In, Out > >( end, 
                                                          max_block_len,
                                                          basic_type, 
                                                          id_block, 
                                                          len_block );
     break;
    case 11: // EMBEDDED PDV
     return value_init< In, EMBEDDEDPDV_type< In, Out > >( end, 
                                                           max_block_len,
                                                           basic_type, 
                                                           id_block, 
                                                           len_block );
     break;
    case 12: // UTF8STRING
     return value_init< In, UTF8STRING_type< In, Out > >( end, 
                                                          max_block_len,
                                                          basic_type, 
                                                          id_block, 
                                                          len_block );
     break;
    case 13: // RELATIVE OBJECT IDENTIFIER
     return value_init< In, ROID_type< In, Out > >( end, 
                                                    max_block_len,
                                                    basic_type, 
                                                    id_block, 
                                                    len_block );
     break;
    case 14: // TIME
     return value_init< In, TIME_type< In, Out > >( end, 
                                                    max_block_len,
                                                    basic_type, 
                                                    id_block, 
                                                    len_block );
     break;
    case 15:
     sprintf_s( (*basic_type)->error, 255, "[UNIVERSAL 15] is reserved by ASN.1 standard" );
     return In();

     break;
    case 16: // SEQUENCE
     return value_init< In, SEQUENCE_type< In, Out > >( end, 
                                                        max_block_len,
                                                        basic_type, 
                                                        id_block, 
                                                        len_block );
     break;
    case 17: // SET
     return value_init< In, SET_type< In, Out > >( end, 
                                                   max_block_len,
                                                   basic_type, 
                                                   id_block, 
                                                   len_block );
     break;
    case 18: // NUMERICSTRING
     return value_init< In, NUMERICSTRING_type< In, Out > >( end, 
                                                             max_block_len,
                                                             basic_type, 
                                                             id_block, 
                                                             len_block );
     break;
    case 19: // PRINTABLESTRING
     return value_init< In, PRINTABLESTRING_type< In, Out > >( end, 
                                                               max_block_len,
                                                               basic_type, 
                                                               id_block, 
                                                               len_block );
     break;
    case 20: // TELETEXSTRING
     return value_init< In, TELETEXSTRING_type< In, Out > >( end, 
                                                             max_block_len,
                                                             basic_type, 
                                                             id_block, 
                                                             len_block );
     break;
    case 21: // VIDEOTEXSTRING
     return value_init< In, VIDEOTEXSTRING_type< In, Out > >( end, 
                                                              max_block_len,
                                                              basic_type, 
                                                              id_block, 
                                                              len_block );
     break;
    case 22: // IA5STRING
     return value_init< In, IA5STRING_type< In, Out > >( end, 
                                                         max_block_len,
                                                         basic_type, 
                                                         id_block, 
                                                         len_block );
     break;
    case 23: // UTCTIME
     return value_init< In, UTCTIME_type< In, Out > >( end, 
                                                       max_block_len,
                                                       basic_type, 
                                                       id_block, 
                                                       len_block );
     break;
    case 24: // GENERALIZEDTIME
     return value_init< In, GENERALIZEDTIME_type< In, Out > >( end, 
                                                               max_block_len,
                                                               basic_type, 
                                                               id_block, 
                                                               len_block );
     break;
    case 25: // GRAPHICSTRING
     return value_init< In, GRAPHICSTRING_type< In, Out > >( end, 
                                                             max_block_len,
                                                             basic_type, 
                                                             id_block, 
                                                             len_block );
     break;
    case 26: // VISIBLESTRING
     return value_init< In, VISIBLESTRING_type< In, Out > >( end, 
                                                             max_block_len,
                                                             basic_type, 
                                                             id_block, 
                                                             len_block );
     break;
    case 27: // GENERALSTRING
     return value_init< In, GENERALSTRING_type< In, Out > >( end, 
                                                             max_block_len,
                                                             basic_type, 
                                                             id_block, 
                                                             len_block );
     break;
    case 28: // UNIVERSALSTRING
     return value_init< In, UNIVERSALSTRING_type< In, Out > >( end, 
                                                               max_block_len,
                                                               basic_type, 
                                                               id_block, 
                                                               len_block );
     break;
    case 29: // CHARACTERSTRING
     return value_init< In, CHARACTERSTRING_type< In, Out > >( end, 
                                                               max_block_len,
                                                               basic_type, 
                                                               id_block, 
                                                               len_block );
     break;
    case 30: // BMPSTRING
     return value_init< In, BMPSTRING_type< In, Out > >( end, 
                                                         max_block_len,
                                                         basic_type, 
                                                         id_block, 
                                                         len_block );
     break;
    case 31: // DATE
     return value_init< In, DATE_type< In, Out > >( end, 
                                                    max_block_len,
                                                    basic_type, 
                                                    id_block, 
                                                    len_block );
     break;
    case 32: // TIME-OF-DAY
     return value_init< In, TIMEOFDAY_type< In, Out > >( end, 
                                                         max_block_len,
                                                         basic_type, 
                                                         id_block, 
                                                         len_block );
     break;
    case 33: // DATE-TIME
     return value_init< In, DATETIME_type< In, Out > >( end, 
                                                        max_block_len,
                                                        basic_type, 
                                                        id_block, 
                                                        len_block );
     break;
    case 34: // DURATION
     return value_init< In, DURATION_type< In, Out > >( end, 
                                                        max_block_len,
                                                        basic_type, 
                                                        id_block, 
                                                        len_block );
     break;
    case 35: // OID INTERNATIONALIZED
     return value_init< In, OIDINTER_type< In, Out > >( end, 
                                                        max_block_len,
                                                        basic_type, 
                                                        id_block, 
                                                        len_block );
     break;
    case 36: // RELATIVE OID INTERNATIONALIZED
     return value_init< In, ROIDINTER_type< In, Out > >( end, 
                                                         max_block_len,
                                                         basic_type, 
                                                         id_block, 
                                                         len_block );
     break;
    default: // Decoding for unknown types
     if( id_block->is_constructed )
      return value_init< In, CONSTRUCTED_type< In, Out  > >( end, 
                                                             max_block_len,
                                                             basic_type, 
                                                             id_block, 
                                                             len_block );
     else
      return value_init< In, PRIMITIVE_type< In, Out  > >( end, 
                                                           max_block_len,
                                                           basic_type, 
                                                           id_block, 
                                                           len_block );
   }
   break;
  case 2: // APPLICATION
  case 3: // CONTEXT-SPECIFIC
  case 4: // PRIVATE
  default: // Decoding for unknown types
   if( id_block->is_constructed )
    return value_init< In, CONSTRUCTED_type< In, Out  > >( end, 
                                                           max_block_len,
                                                           basic_type, 
                                                           id_block, 
                                                           len_block );
   else
    return value_init< In, PRIMITIVE_type< In, Out  > >( end, 
                                                         max_block_len,
                                                         basic_type, 
                                                         id_block, 
                                                         len_block );
 }

 return end;
}
//**************************************************************************
template< typename T >
T decode_BER( T stream, unsigned long long max_block_length, MSXML2::IXMLDOMDocumentPtr pDoc )
{
 BASIC_type< T, unsigned char >* basic_type;
 T end = decode_BER_raw( stream, max_block_length, &basic_type );

 pDoc->documentElement = basic_type->to_xml( pDoc );

 delete basic_type;
 
 return end;
}
//**************************************************************************
template< typename V, typename In, typename Out >
bool init_from_xml( MSXML2::IXMLDOMElementPtr element, BASIC_type< In, Out >** basic_type, unsigned char tag_class = -1, unsigned long long tag_number = -1 )
{
 (*basic_type) = new V();
 (*basic_type)->from_xml( element );

 if( tag_class != ( unsigned char )(-1) )
  (*basic_type)->id_block->tag_class = tag_class;

 if( tag_number != ( unsigned long long )(-1) )
  (*basic_type)->id_block->tag_number = tag_number;

 return true;
}
//**************************************************************************
template< typename In, typename Out >
bool from_xml( MSXML2::IXMLDOMElementPtr element, BASIC_type< In, Out >** basic_type )
{
 if( !element )
  return false;

 wchar_t* wname = ( wchar_t* )element->nodeName;

 char name[255];
 memset( name, 0x00, 255 );

 size_t converted = 0;
 errno_t errnum = wcstombs_s( &converted, name, 255, wname, 255 );
 if( errnum && !converted )
  return false;

 std::string strname = name;

 if( strname == clear_xml_tag( EOC_type< In, Out >::block_name_static() ) )
  return init_from_xml< EOC_type< In, Out > >( element, basic_type );

 if( strname == clear_xml_tag( BOOLEAN_type< In, Out >::block_name_static() ) )
  return init_from_xml< BOOLEAN_type< In, Out > >( element, basic_type, 1, 1 );

 if( strname == clear_xml_tag( INTEGER_type< In, Out  >::block_name_static() ) )
  return init_from_xml< INTEGER_type< In, Out > >( element, basic_type, 1, 2 );

 if( strname == clear_xml_tag( BITSTRING_type< In, Out >::block_name_static() ) )
  return init_from_xml< BITSTRING_type< In, Out > >( element, basic_type, 1, 3 );

 if( strname == clear_xml_tag( OCTETSTRING_type< In, Out >::block_name_static() ) )
  return init_from_xml< OCTETSTRING_type< In, Out > >( element, basic_type, 1, 4);

 if( strname == clear_xml_tag( NULL_type< In, Out >::block_name_static() ) )
  return init_from_xml< NULL_type< In, Out > >( element, basic_type, 1, 5 );

 if( strname == clear_xml_tag( OID_type< In, Out >::block_name_static() ) )
  return init_from_xml< OID_type< In, Out > >( element, basic_type, 1, 6 );

 if( strname == clear_xml_tag( OBJECTDESCRIPTOR_type< In, Out >::block_name_static() ) )
  return init_from_xml< OBJECTDESCRIPTOR_type< In, Out > >( element, basic_type, 1, 7 );

 if( strname == clear_xml_tag( EXTERNAL_type< In, Out >::block_name_static() ) )
  return init_from_xml< EXTERNAL_type< In, Out > >( element, basic_type, 1, 8 );

 if( strname == clear_xml_tag( REAL_type< In, Out >::block_name_static() ) )
  return init_from_xml< REAL_type< In, Out > >( element, basic_type, 1, 9 );

 if( strname == clear_xml_tag( ENUMERATED_type< In, Out >::block_name_static() ) )
  return init_from_xml< ENUMERATED_type< In, Out > >( element, basic_type, 1, 10 );

 if( strname == clear_xml_tag( EMBEDDEDPDV_type< In, Out >::block_name_static() ) )
  return init_from_xml< EMBEDDEDPDV_type< In, Out > >( element, basic_type, 1, 11 );

 if( strname == clear_xml_tag( UTF8STRING_type< In, Out >::block_name_static() ) )
  return init_from_xml< UTF8STRING_type< In, Out > >( element, basic_type, 1, 12 );

 if( strname == clear_xml_tag( ROID_type< In, Out >::block_name_static() ) )
  return init_from_xml< ROID_type< In, Out > >( element, basic_type, 1, 13 );

 if( strname == clear_xml_tag( TIME_type< In, Out >::block_name_static() ) )
  return init_from_xml< TIME_type< In, Out > >( element, basic_type, 1, 14 );

 if( strname == clear_xml_tag( SEQUENCE_type< In, Out >::block_name_static() ) )
  return init_from_xml< SEQUENCE_type< In, Out > >( element, basic_type, 1, 16 );

 if( strname == clear_xml_tag( SET_type< In, Out >::block_name_static() ) )
  return init_from_xml< SET_type< In, Out > >( element, basic_type, 1, 17 );

 if( strname == clear_xml_tag( NUMERICSTRING_type< In, Out >::block_name_static() ) )
  return init_from_xml< NUMERICSTRING_type< In, Out > >( element, basic_type, 1, 18 );

 if( strname == clear_xml_tag( PRINTABLESTRING_type< In, Out >::block_name_static() ) )
  return init_from_xml< PRINTABLESTRING_type< In, Out > >( element, basic_type, 1, 19 );

 if( strname == clear_xml_tag( TELETEXSTRING_type< In, Out >::block_name_static() ) )
  return init_from_xml< TELETEXSTRING_type< In, Out > >( element, basic_type, 1, 20 );

 if( strname == clear_xml_tag( VIDEOTEXSTRING_type< In, Out >::block_name_static() ) )
  return init_from_xml< VIDEOTEXSTRING_type< In, Out > >( element, basic_type, 1, 21 );

 if( strname == clear_xml_tag( IA5STRING_type< In, Out >::block_name_static() ) )
  return init_from_xml< IA5STRING_type< In, Out > >( element, basic_type, 1, 22 );

 if( strname == clear_xml_tag( UTCTIME_type< In, Out >::block_name_static() ) )
  return init_from_xml< UTCTIME_type< In, Out > >( element, basic_type, 1, 23 );

 if( strname == clear_xml_tag( GENERALIZEDTIME_type< In, Out >::block_name_static() ) )
  return init_from_xml< GENERALIZEDTIME_type< In, Out > >( element, basic_type, 1, 24 );

 if( strname == clear_xml_tag( GRAPHICSTRING_type< In, Out >::block_name_static() ) )
  return init_from_xml< GRAPHICSTRING_type< In, Out > >( element, basic_type, 1, 25 );

 if( strname == clear_xml_tag( VISIBLESTRING_type< In, Out >::block_name_static() ) )
  return init_from_xml< VISIBLESTRING_type< In, Out > >( element, basic_type, 1, 26 );

 if( strname == clear_xml_tag( GENERALSTRING_type< In, Out >::block_name_static() ) )
  return init_from_xml< GENERALSTRING_type< In, Out > >( element, basic_type, 1, 27 );

 if( strname == clear_xml_tag( UNIVERSALSTRING_type< In, Out >::block_name_static() ) )
  return init_from_xml< UNIVERSALSTRING_type< In, Out > >( element, basic_type, 1, 28 );

 if( strname == clear_xml_tag( CHARACTERSTRING_type< In, Out >::block_name_static() ) )
  return init_from_xml< CHARACTERSTRING_type< In, Out > >( element, basic_type, 1, 29 );

 if( strname == clear_xml_tag( BMPSTRING_type< In, Out >::block_name_static() ) )
  return init_from_xml< BMPSTRING_type< In, Out > >( element, basic_type, 1, 30 );

 if( strname == clear_xml_tag( DATE_type< In, Out >::block_name_static() ) )
  return init_from_xml< DATE_type< In, Out > >( element, basic_type, 1, 31 );

 if( strname == clear_xml_tag( TIMEOFDAY_type< In, Out >::block_name_static() ) )
  return init_from_xml< TIMEOFDAY_type< In, Out > >( element, basic_type, 1, 32 );

 if( strname == clear_xml_tag( DATETIME_type< In, Out >::block_name_static() ) )
  return init_from_xml< DATETIME_type< In, Out > >( element, basic_type, 1, 33 );

 if( strname == clear_xml_tag( DURATION_type< In, Out >::block_name_static() ) )
  return init_from_xml< DURATION_type< In, Out > >( element, basic_type, 1, 34 );

 if( strname == clear_xml_tag( OIDINTER_type< In, Out >::block_name_static() ) )
  return init_from_xml< OIDINTER_type< In, Out > >( element, basic_type, 1, 35 );

 if( strname == clear_xml_tag( ROIDINTER_type< In, Out >::block_name_static() ) )
  return init_from_xml< ROIDINTER_type< In, Out > >( element, basic_type, 1, 36 );

 //---
 MSXML2::IXMLDOMNodeListPtr list = element->selectNodes( L"./descendant::value_block" );

 if( list->length == 1 ) // PRIMITIVE
  return init_from_xml< PRIMITIVE_type< In, Out > >( element, basic_type );

 if( list->length > 1 ) // CONSTRUCTIVE
  return init_from_xml< CONSTRUCTED_type< In, Out > >( element, basic_type );

 return false;
}
//**************************************************************************
// Function that would be able to encode arbitrary types (allowed ones, of course)
// given an XML description. It should be able to encode both primitive and
// constructed types.
//
template< typename T >
encode_result* encode_BER( T stream, MSXML2::IXMLDOMElementPtr element )
{
 BASIC_type< unsigned char*, T >* basic_type = NULL;
 if( from_xml( element, &basic_type ) )
 {
  encode_result* result = new encode_result();

  basic_type->stream_out = stream;
  result->encoded = basic_type->encode_BER();

  delete basic_type;
  
  return result;
 }
 else
 {
  if( basic_type != NULL )
   delete basic_type;
   
  return NULL;
 }
}
//**************************************************************************
template< typename T >
encode_result* XML_BER( T stream, MSXML2::IXMLDOMElementPtr element )
{
 encode_result* result = new encode_result();

 BASIC_type< unsigned char*, T >* basic_type = NULL;
 if( from_xml( element, &basic_type ) )
 {
  basic_type->stream_out = stream;
  result->encoded = basic_type->encode_BER();

  delete basic_type;
  
  return result;
 }
 else
 {
  if( basic_type != NULL )
   delete basic_type;
   
  return result;
 }

 return result;
}
//**************************************************************************
template< typename T >
encode_result* XML_BER64( T stream, MSXML2::IXMLDOMElementPtr element )
{
 encode_result* result = new encode_result();

 wchar_t szTempFileName[MAX_PATH]; 
 wchar_t lpTempPathBuffer[MAX_PATH];

 DWORD dwRetVal = GetTempPathW( MAX_PATH, lpTempPathBuffer );
 if (dwRetVal > MAX_PATH || (dwRetVal == 0))
  return NULL;

 UINT uRetVal = GetTempFileNameW( lpTempPathBuffer, 
                                  TEXT( "COMPLI" ),     
                                  0,                
                                  szTempFileName );  
 if (uRetVal == 0)
  return NULL;

 std::wstring wtemp_name = szTempFileName;

 char* name = ( char* ) malloc( wtemp_name.length() + 1 );
 if( !name )
  return NULL;
 name[ wtemp_name.length() ] = 0x00; 

 size_t converted = 0;
 errno_t errnum = wcstombs_s( &converted, name, ( size_t ) ( wtemp_name.length() + 1 ), wtemp_name.c_str(), ( size_t )wtemp_name.length() );
 if( errnum && !converted )
 {
  free( name );
  return NULL;
 }

 std::string temp_name = name;
 free( name );

 std::ofstream ostream( temp_name.c_str(), std::ios::binary );
 ostream.unsetf( std::ios_base::skipws );
 
 std::ostreambuf_iterator< char > out( ostream );

 BASIC_type< unsigned char*, std::ostreambuf_iterator< char > >* basic_type = NULL;
 if( from_xml( element, &basic_type ) )
 {
  basic_type->stream_out = out;
  result->encoded = basic_type->encode_BER();

  delete basic_type;
  
  ostream.flush();
  ostream.close();

  std::ifstream base64_file( temp_name.c_str(), std::ios_base::binary );
  base64_file.unsetf( std::ios_base::skipws );

  std::istreambuf_iterator< char > begin(base64_file);
  std::istreambuf_iterator< char > end;

  base64_encode( begin, end, stream );

  base64_file.close();

  DeleteFile( wtemp_name.c_str() );

  return result;
 }
 else
 {
  if( basic_type != NULL )
   delete basic_type;
   
  ostream.flush();
  ostream.close();

  DeleteFile( wtemp_name.c_str() );

  return result;
 }

 return result;
}
//**************************************************************************
template< typename T >
T BER_XML( T stream, unsigned long long max_block_length, MSXML2::IXMLDOMDocumentPtr pDoc )
{
 BASIC_type< T, unsigned char >* basic_type;
 T end = decode_BER_raw( stream, max_block_length, &basic_type );

 pDoc->documentElement = basic_type->to_xml( pDoc );

 delete basic_type;
 
 return end;
}
//**************************************************************************
template< typename T >
T BER64_XML( T stream, unsigned long long max_block_length, MSXML2::IXMLDOMDocumentPtr pDoc )
{
 DWORD dwTempPathLen = GetTempPath( 0 ,NULL );
 if( !dwTempPathLen )
  return T();

 wchar_t szTempFileName[MAX_PATH]; 
 wchar_t lpTempPathBuffer[MAX_PATH];

 DWORD dwRetVal = GetTempPathW( MAX_PATH, lpTempPathBuffer );
 if (dwRetVal > MAX_PATH || (dwRetVal == 0))
  return T();

 UINT uRetVal = GetTempFileNameW( lpTempPathBuffer, 
                                  TEXT( "COMPLI" ),     
                                  0,                
                                  szTempFileName );  
 if (uRetVal == 0)
  return T();

 std::wstring wtemp_name = szTempFileName;

 char* name = ( char* ) malloc( wtemp_name.length() + 1 );
 if( !name )
  return T();

 name[ wtemp_name.length() ] = 0x00; 

 size_t converted = 0;
 errno_t errnum = wcstombs_s( &converted, name, ( size_t ) ( wtemp_name.length() + 1 ), wtemp_name.c_str(), ( size_t )wtemp_name.length() );
 if( errnum && !converted )
 {
  free( name );
  return T();
 }

 std::string temp_name = name;
 free( name );

 std::ofstream ostream( temp_name.c_str(), std::ios::binary );
 ostream.unsetf( std::ios_base::skipws );
 
 std::ostreambuf_iterator< char > out( ostream );

 base64_decode( stream, T(), out );

 ostream.flush();
 ostream.close();

 std::ifstream base64_file( temp_name.c_str(), std::ios_base::binary );
 base64_file.unsetf( std::ios_base::skipws );

 base64_file.seekg( 0, std::ios::end );
 max_block_length = base64_file.tellg();
 base64_file.seekg( 0, std::ios::beg );

 BASIC_type< std::istreambuf_iterator< char >, unsigned char >* basic_type;
 std::istreambuf_iterator< char > end = decode_BER_raw( std::istreambuf_iterator< char >( base64_file ), max_block_length, &basic_type );
 base64_file.close();

 pDoc->documentElement = basic_type->to_xml( pDoc );

 delete basic_type;
 
 DeleteFile( wtemp_name.c_str() );

 return end;
}
//**************************************************************************
#endif