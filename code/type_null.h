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
#ifndef _8E5F4C88_E280_4F7C_BE77_E66F48D0C083
#define _8E5F4C88_E280_4F7C_BE77_E66F48D0C083
//**************************************************************************
template< typename In, typename Out >
struct NULL_value_block : virtual public value_block_type< In, Out >,
                          virtual public hex_block_type< In, Out >
{
 virtual MSXML2::IXMLDOMElementPtr to_xml( MSXML2::IXMLDOMDocumentPtr pDoc );
 virtual bool from_xml( MSXML2::IXMLDOMElementPtr element );

 virtual In decode_BER( unsigned long long max_block_len );
 virtual unsigned long long encode_BER( bool size_only = false );
};
//**************************************************************************
template< typename In, typename Out >
MSXML2::IXMLDOMElementPtr NULL_value_block< In, Out >::to_xml( MSXML2::IXMLDOMDocumentPtr pDoc )
{
 MSXML2::IXMLDOMElementPtr result = block_type< In, Out >::to_xml( pDoc );
 return result;
}
//**************************************************************************
template< typename In, typename Out >
bool NULL_value_block< In, Out >::from_xml( MSXML2::IXMLDOMElementPtr element )
{
 if( !element)
  return false;

 return true;
}
//**************************************************************************
template< typename In, typename Out >
In NULL_value_block< In, Out >::decode_BER( unsigned long long max_block_len )
{
 if( max_block_len )
  warnings.push_back( "Non-zero length of value block for NULL type" );

 block_length = max_block_len;

 while( max_block_len )
 {
  if( stream_in == In() )
  {
   sprintf_s( error, 255, "End of input reached before message was fully decoded" );
   return stream_in;
  }

  stream_in++;
  max_block_len--;
 }

 return stream_in;
}
//**************************************************************************
template< typename In, typename Out >
unsigned long long NULL_value_block< In, Out >::encode_BER( bool size_only )
{
 return 0;
}
//**************************************************************************
template< typename In, typename Out >
struct NULL_type : virtual public BASIC_type< In, Out >
{
 NULL_type();

 static std::string block_name_static();
 virtual std::string block_name();
};
//**************************************************************************
template< typename In, typename Out >
NULL_type< In, Out >::NULL_type()
{
 tag_class = 1;
 tag_number = 5;

 value_block = new NULL_value_block< In, Out >();
}
//**************************************************************************
template< typename In, typename Out >
std::string NULL_type< In, Out >::block_name_static()
{
 return "NULL";
}
//**************************************************************************
template< typename In, typename Out >
std::string NULL_type< In, Out >::block_name()
{
 return block_name_static();
}
//**************************************************************************
#endif