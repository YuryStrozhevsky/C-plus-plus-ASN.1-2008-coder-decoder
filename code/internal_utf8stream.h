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
#ifndef _A5290D64_13FD_48B5_B618_C63F11DC2446
#define _A5290D64_13FD_48B5_B618_C63F11DC2446
//**************************************************************************
// Came from http://www.rsdn.ru/forum/cpp/1948543.1.aspx
//**************************************************************************
/**	Conversion facet that allows to read Unicode files in UTF-8 encoding */
class utf8_conversion
: public std::codecvt<wchar_t, char, std::mbstate_t>
{
protected:

	result do_in(std::mbstate_t& state,
				 const char* from, const char* from_end, const char*& from_next,
				 wchar_t* to, wchar_t* to_limit, wchar_t*& to_next) const;

	result do_out(std::mbstate_t& state,
				  const wchar_t* from, const wchar_t* from_end, const wchar_t*& from_next,
				  char* to, char* to_limit, char*& to_next) const;

    bool do_always_noconv() const throw() { return false; }
    int  do_encoding() const throw() { return 2; }
};
//**************************************************************************
typedef unsigned char uchar;
//**************************************************************************
inline unsigned char 
take_6_bits(unsigned value, size_t right_position)
{
	return uchar((value >> right_position) & 63);
}
//**************************************************************************
inline int
most_signifant_bit_position(unsigned value)
{
	int result(0);

	int half = 16;
	for(; half > 0; half >>= 1) {
		if (1u << (result + half) <= value ) result += half;
	}	
	return result + 1;
}
//**************************************************************************
utf8_conversion::result
utf8_conversion::do_in(mbstate_t&,
					   const char*  from, const char* from_end, const char*& from_next,
					   wchar_t* to, wchar_t* to_limit, wchar_t*& to_next) const
{
	from_next = from;
	to_next = to;

	for(; to_next < to_limit && from_next < from_end; ++to_next) {

		if (uchar(*from_next) < 0x80) *to_next = uchar(*from_next++);
		else {

			// 111zxxxx : z = 0 xxxx are data bits
			int zero_bit_pos = most_signifant_bit_position(~*from_next);
			int extra_bytes  = 7 - zero_bit_pos;

			if (from_end - from_next < extra_bytes + 1) 
				return partial;

			*to_next = uchar(*from_next++) & ( ( ( 1 << zero_bit_pos ) - 1) - 1);

			for (;extra_bytes--; ++from_next) {
				*to_next = *to_next << 6  |  uchar(*from_next) & 63;
			}
		}
	}	
	return ok;
}
//**************************************************************************
utf8_conversion::result
utf8_conversion::do_out(mbstate_t&,
				  const wchar_t* from, const wchar_t* from_end, const wchar_t*& from_next,
				  char* to, char* to_limit, char*& to_next) const
{
		from_next = from;
		to_next = to;

		for (;from_next < from_end; ++from_next) {

			unsigned symbol = *from_next;

			if (symbol < 0x7F) {
				if (to_next < to_limit)
					*to_next++ = (unsigned char)symbol;
				else
					return ok;
			} else {

				int msb_pos = most_signifant_bit_position(symbol);
				int extra_bytes = msb_pos / 6;

				if (to_limit - to_next >= extra_bytes + 1) {

					*to_next = uchar(0xFF80 >> extra_bytes);
					*to_next++ |= take_6_bits(symbol, extra_bytes*6);

					for(;extra_bytes--;)
						*to_next++ = 0x80 | take_6_bits(symbol, extra_bytes*6);
				}
				else
					return ok;
			}
		}
		return ok;
}
//**************************************************************************
namespace std
{
template<class _Elem,
	class _Traits> inline
	basic_ostream<_Elem, _Traits>&
		__CLRCALL_OR_CDECL endlu(basic_ostream<_Elem, _Traits>& _Ostr)
	{	// insert newline and flush stream
	_Ostr.put(_Ostr.widen('\r'));
	_Ostr.put(_Ostr.widen('\n'));
	_Ostr.flush();
	return (_Ostr);
	}
 };
//**************************************************************************
class no_conversion
: public std::codecvt<wchar_t, char, std::mbstate_t>
{
protected:

	result do_in(std::mbstate_t& state,
				 const char* from, const char* from_end, const char*& from_next,
				 wchar_t* to, wchar_t* to_limit, wchar_t*& to_next) const { return noconv; };

	result do_out(std::mbstate_t& state,
				  const wchar_t* from, const wchar_t* from_end, const wchar_t*& from_next,
				  char* to, char* to_limit, char*& to_next) const { return noconv; };

    bool do_always_noconv() const throw() { return true; }
    int  do_encoding() const throw() { return 2; }
};
//**************************************************************************
#endif