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
#ifndef _721C770E_3343_41E6_BEBF_F4D465EEFFF9
#define _721C770E_3343_41E6_BEBF_F4D465EEFFF9
//**************************************************************************
template< typename In, typename Out >
struct BITSTRING_value_block : virtual public CONSTRUCTED_value_block< In, Out >,
                               virtual public hex_block_type< In, Out >
{
 unsigned char unused_bits;
 bool is_constructed;

 BITSTRING_value_block();

 virtual MSXML2::IXMLDOMElementPtr to_xml( MSXML2::IXMLDOMDocumentPtr pDoc );
 virtual bool from_xml( MSXML2::IXMLDOMElementPtr element );

 virtual In decode_BER( unsigned long long max_block_len );
 virtual unsigned long long encode_BER( bool size_only = false );
};
//**************************************************************************
template< typename In, typename Out >
BITSTRING_value_block< In, Out >::BITSTRING_value_block() : unused_bits( 0 ),
                                                            is_constructed( false )
{
}
//**************************************************************************
template< typename In, typename Out >
MSXML2::IXMLDOMElementPtr BITSTRING_value_block< In, Out >::to_xml( MSXML2::IXMLDOMDocumentPtr pDoc )
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

  wchar_t unused_bits_buf[ 10 ];
  memset( unused_bits_buf, 0x00, 10 * sizeof( wchar_t ) );

  errno_t error_num = _itow_s( unused_bits, unused_bits_buf, 10, 10 );
  if( error_num )
   return result;

  MSXML2::IXMLDOMElementPtr unused_bits_xml = pDoc->createElement( L"unused_bits" );
  unused_bits_xml->appendChild( pDoc->createTextNode( unused_bits_buf ) );

  result->appendChild( unused_bits_xml );

  std::string hex = bit_codes( value_hex, block_length, 0 ); // let's put all bits, together with "unused"
  //std::string hex = bit_codes( value_hex, block_length, unused_bits );

  wchar_t* whex = ( wchar_t* ) malloc( ( hex.length() + 1 ) * sizeof( wchar_t ) );
  memset( whex, 0x00, ( hex.length() + 1 ) * sizeof( wchar_t ) );

  size_t converted = 0;
  error_num = mbstowcs_s( &converted, whex, hex.length() + 1, hex.c_str(), hex.length() );
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
bool BITSTRING_value_block< In, Out >::from_xml( MSXML2::IXMLDOMElementPtr element )
{
 if( !element )
  return false;

 if( !is_constructed )   // primitive form
 {
  MSXML2::IXMLDOMElementPtr unused_bits_xml = element->selectSingleNode( L"unused_bits" );
  if( !unused_bits_xml )
   return false;

  unused_bits = _wtoi( ( wchar_t* )unused_bits_xml->text );
  if( errno && !unused_bits )
   return false;

  MSXML2::IXMLDOMElementPtr value_hex_xml = element->selectSingleNode( L"value_hex" );
  if( !value_hex_xml )
   return false;

  std::wstring str_hex = ( wchar_t* ) value_hex_xml->text;
  str_hex = wreplace( str_hex.c_str(), L" ", L"" );

  block_length = from_wbit_codes( str_hex.c_str(), str_hex.length(), &value_hex );
  if( !block_length )
   return false;

  block_length++; // 1 octet for "unused bits"
 }
 else   // constructed form
 {
  if( !CONSTRUCTED_value_block< In, Out >::from_xml( element ) )
   return false;

  for( std::vector< BASIC_type< In, Out >* >::iterator i = value.begin(); i != value.end(); i++ )
  {
   if( (*i)->block_name() != BITSTRING_type< In, Out >::block_name_static() )
   {
    value.erase( value.begin(), value.end() );
    sprintf_s( error, 255, "BIT STRING may consists of BIT STRINGs only" );
    return false;
   }

   BITSTRING_value_block< In, Out >* constr_value_block = dynamic_cast< BITSTRING_value_block< In, Out >* >( (*i)->value_block );

   if( unused_bits && constr_value_block->unused_bits )
   {
    sprintf_s( error, 255, "Usign of \"unused bits\" inside constructive BIT STRING allowed for least one only" );
    return false;
   }
   else
    unused_bits = constr_value_block->unused_bits;
  }
 }

 return true;
}
//**************************************************************************
template< typename In, typename Out >
In BITSTRING_value_block< In, Out >::decode_BER( unsigned long long max_block_len )
{
 if( !max_block_len )
  return stream_in;

 if( stream_in == In() )
 {
  sprintf_s( error, 255, "End of input reached before message was fully decoded" );
  return stream_in;
 }

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
     sprintf_s( error, 255, "EOC is unexpected, BIT STRING may consists of BIT STRINGs only" );
     return In();
    }
   }
   
   if( (*i)->block_name() != BITSTRING_type< In, Out >::block_name_static() )
   {
    value.erase( value.begin(), value.end() );
    sprintf_s( error, 255, "BIT STRING may consists of BIT STRINGs only" );
    return In();
   }

   BITSTRING_value_block< In, Out >* constr_value_block = dynamic_cast< BITSTRING_value_block< In, Out >* >( (*i)->value_block );

   if( unused_bits && constr_value_block->unused_bits )
   {
    value.erase( value.begin(), value.end() );
    sprintf_s( error, 255, "Usign of \"unused bits\" inside constructive BIT STRING allowed for least one only" );
    return In();
   }
   else
   {
    unused_bits = constr_value_block->unused_bits;
    if( unused_bits > 7 )
    {
     sprintf_s( error, 255, "Unused bits for BITSTRING must be in range 0-7" );
     return In();
    }
   }
  }

  return end;
 }
 else
 {
  In end = stream_in;

  unused_bits = (unsigned char)*end;
  if( unused_bits > 7 )
  {
   sprintf_s( error, 255, "Unused bits for BITSTRING must be in range 0-7" );
   return In();
  }

  end++;
  if( end == In() )
  {
   sprintf_s( error, 255, "End of input reached before message was fully decoded" );
   return end;
  }

  value_hex = ( unsigned char* ) malloc( ( size_t ) ( max_block_len - 1 ) );
  if( !value_hex )
   return In();

  unsigned char* data = value_hex;

  for( unsigned long long i = 0; i < ( max_block_len - 1 ); i++ )
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
 
 return In();
}
//**************************************************************************
template< typename In, typename Out >
unsigned long long BITSTRING_value_block< In, Out >::encode_BER( bool size_only )
{
 if( !is_constructed )
 {
  if( size_only )
   return block_length;
   
  if( !block_length )
   return 0;

  stream_out = unused_bits;

  for( unsigned long long i = 0; i < ( block_length - 1 ); i++ )
   stream_out = value_hex[ i ];
   
  return block_length;
 }
 else
  return CONSTRUCTED_value_block< In, Out >::encode_BER( size_only );

 return 0;
}
//**************************************************************************
template< typename In, typename Out >
struct BITSTRING_type : virtual public BASIC_type< In, Out >
{
 static std::string block_name_static();
 virtual std::string block_name();

 BITSTRING_type();

 virtual bool from_xml( MSXML2::IXMLDOMElementPtr element );
 virtual In decode_BER( unsigned long long max_block_len );
};
//**************************************************************************
template< typename In, typename Out >
BITSTRING_type< In, Out >::BITSTRING_type()
{
 tag_class = 1;
 tag_number = 3;

 value_block = new BITSTRING_value_block< In, Out >();
}
//**************************************************************************
template< typename In, typename Out >
std::string BITSTRING_type< In, Out >::block_name_static()
{
 return "BIT STRING";
}
//**************************************************************************
template< typename In, typename Out >
std::string BITSTRING_type< In, Out >::block_name()
{
 return block_name_static();
}
//**************************************************************************
template< typename In, typename Out >
bool BITSTRING_type< In, Out >::from_xml( MSXML2::IXMLDOMElementPtr element )
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

 BITSTRING_value_block< In, Out >* vb = dynamic_cast< BITSTRING_value_block< In, Out >* >( value_block );
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
In BITSTRING_type< In, Out >::decode_BER( unsigned long long max_block_len )
{
 if( !max_block_len ) // Ability to decode empty BIT STRING
  return stream_in;

 BITSTRING_value_block< In, Out >* vb = dynamic_cast< BITSTRING_value_block< In, Out >* >( value_block );
 if( vb == NULL )
  return false;
 
 vb->is_constructed = id_block->is_constructed;
 vb->is_indefinite_form = len_block->is_indefinite_form;

 return BASIC_type< In, Out >::decode_BER( max_block_len );
}
//**************************************************************************
#endif