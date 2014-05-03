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
#ifndef _368F4CD8_AD32_449C_BEB2_57F336F11103
#define _368F4CD8_AD32_449C_BEB2_57F336F11103
//**************************************************************************
template< typename In, typename Out >
struct EOC_value_block : virtual public value_block_type< In, Out >
{
 virtual MSXML2::IXMLDOMElementPtr to_xml( MSXML2::IXMLDOMDocumentPtr pDoc );
 virtual bool from_xml( MSXML2::IXMLDOMElementPtr element );

 virtual In decode_BER( unsigned long long max_block_len );
 virtual unsigned long long encode_BER( bool size_only = false );
};
//**************************************************************************
template< typename In, typename Out >
MSXML2::IXMLDOMElementPtr EOC_value_block< In, Out >::to_xml( MSXML2::IXMLDOMDocumentPtr pDoc )
{
 return block_type< In, Out >::to_xml( pDoc );
}
//**************************************************************************
template< typename In, typename Out >
bool EOC_value_block< In, Out >::from_xml( MSXML2::IXMLDOMElementPtr element )
{
 return true;
}
//**************************************************************************
template< typename In, typename Out >
In EOC_value_block< In, Out >::decode_BER( unsigned long long max_block_len )
{
 if( max_block_len != 0 )
 {
  sprintf_s( error, 255, "Wrong encoding for EOC block");
  return In();
 }

 block_length = 0;

 return stream_in; // should not move "stream" because no value in the type and "inf_block" and "len_block" were already decoded
}
//**************************************************************************
template< typename In, typename Out >
unsigned long long EOC_value_block< In, Out >::encode_BER( bool size_only )
{
 return 0;
}
//**************************************************************************
template< typename In, typename Out >
struct EOC_type : virtual public BASIC_type< In, Out >
{
 EOC_type();

 static std::string block_name_static();
 virtual std::string block_name();
};
//**************************************************************************
template< typename In, typename Out >
EOC_type< In, Out >::EOC_type()
{
 value_block = new EOC_value_block< In, Out >();
}
//**************************************************************************
template< typename In, typename Out >
std::string EOC_type< In, Out >::block_name_static()
{
 return "END OF CONTENT";
}
//**************************************************************************
template< typename In, typename Out >
std::string EOC_type< In, Out >::block_name()
{
 return block_name_static();
}
//**************************************************************************
#endif