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
#ifndef _41D7494D_DAAD_41E1_8FA0_B2BD3CD405AE
#define _41D7494D_DAAD_41E1_8FA0_B2BD3CD405AE
//**************************************************************************
template< typename In, typename Out >
struct SEQUENCE_type : virtual public CONSTRUCTED_type< In, Out >
{
 SEQUENCE_type();

 static std::string block_name_static();
 virtual std::string block_name();
};
//**************************************************************************
template< typename In, typename Out >
SEQUENCE_type< In, Out >::SEQUENCE_type()
{
 tag_class = 1;
 tag_number = 16;
}
//**************************************************************************
template< typename In, typename Out >
std::string SEQUENCE_type< In, Out >::block_name_static()
{
 return "SEQUENCE";
}
//**************************************************************************
template< typename In, typename Out >
std::string SEQUENCE_type< In, Out >::block_name()
{
 return block_name_static();
}
//**************************************************************************
template< typename In, typename Out >
struct SET_type : virtual public CONSTRUCTED_type< In, Out >
{
 SET_type();

 static std::string block_name_static();
 virtual std::string block_name();
};
//**************************************************************************
template< typename In, typename Out >
SET_type< In, Out >::SET_type()
{
 tag_class = 1;
 tag_number = 17;
}
//**************************************************************************
template< typename In, typename Out >
std::string SET_type< In, Out >::block_name_static()
{
 return "SET";
}
//**************************************************************************
template< typename In, typename Out >
std::string SET_type< In, Out >::block_name()
{
 return block_name_static();
}
//**************************************************************************
template< typename In, typename Out >
struct EXTERNAL_type : virtual public SEQUENCE_type< In, Out >
{
 EXTERNAL_type();

 static std::string block_name_static();
 virtual std::string block_name();
};
//**************************************************************************
template< typename In, typename Out >
EXTERNAL_type< In, Out >::EXTERNAL_type()
{
 tag_class = 1;
 tag_number = 8;
}
//**************************************************************************
template< typename In, typename Out >
std::string EXTERNAL_type< In, Out >::block_name_static()
{
 return "EXTERNAL";
}
//**************************************************************************
template< typename In, typename Out >
std::string EXTERNAL_type< In, Out >::block_name()
{
 return block_name_static();
}
//**************************************************************************
template< typename In, typename Out >
struct EMBEDDEDPDV_type : virtual public SEQUENCE_type< In, Out >
{
 EMBEDDEDPDV_type();

 static std::string block_name_static();
 virtual std::string block_name();
};
//**************************************************************************
template< typename In, typename Out >
EMBEDDEDPDV_type< In, Out >::EMBEDDEDPDV_type()
{
 tag_class = 1;
 tag_number = 11;
}
//**************************************************************************
template< typename In, typename Out >
std::string EMBEDDEDPDV_type< In, Out >::block_name_static()
{
 return "EMBEDDEDPDV";
}
//**************************************************************************
template< typename In, typename Out >
std::string EMBEDDEDPDV_type< In, Out >::block_name()
{
 return block_name_static();
}
//**************************************************************************
#endif