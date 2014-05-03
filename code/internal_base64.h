#ifndef _43602737_A607_4BE4_8C34_199C2CA3F292
#define _43602737_A607_4BE4_8C34_199C2CA3F292
//***************************************************************************
template< class _in, class _out >
void base64_encode( _in _in_iterator, _in _end_of_in, _out _out_iterator )
{
 int BLOCK = 192;

 std::vector< char > buf;
 buf.reserve( BLOCK );
 
 int count;
 int string_len=0;
 int len;
 
 while(_in_iterator!=_end_of_in)
 {
  buf.clear();
  count=0;

  for( ; ( ( _in_iterator != _end_of_in ) && ( count < BLOCK ) ); _in_iterator++, count++ )    
   buf.push_back( *_in_iterator );
   
  if( count < BLOCK )
   buf.resize( count );
   
  len = 0;

  len = Base64EncodeGetRequiredLength( buf.size(), 
                                      ( _in_iterator == _end_of_in ) ? ATL_BASE64_FLAG_NOCRLF : ( ATL_BASE64_FLAG_NOPAD | ATL_BASE64_FLAG_NOCRLF ) 
                                     ); 

  LPSTR temp_string = static_cast< LPSTR >( malloc( len + 1 ) );
  if( !temp_string )
   return;
  ZeroMemory( temp_string, len + 1 );

  Base64Encode( reinterpret_cast< const BYTE* >( &buf[ 0 ] ), 
                buf.size(), 
                temp_string,
                &len,
                ( _in_iterator == _end_of_in ) ? ATL_BASE64_FLAG_NOCRLF : ( ATL_BASE64_FLAG_NOPAD | ATL_BASE64_FLAG_NOCRLF ) 
               );

  for( unsigned int i = 0; i < strlen( temp_string ); i++ )
  {
   _out_iterator = temp_string[ i ];
   string_len++;
   if( string_len > 59 )
   {
    string_len = 0;
    _out_iterator = '\r';
    _out_iterator = '\n';
   }
  }
   
  ZeroMemory( temp_string, len + 1 );
  free( temp_string );
 }
 
 _out_iterator = '\r';
 _out_iterator = '\n';
}
//***************************************************************************
template< class _in, class _out >
void base64_decode( _in _in_iterator, _in _end_of_in, _out _out_iterator )
{
 std::vector< char > string_vector;

 int BLOCK = 1024;

 std::vector< char > buf;
 buf.reserve( BLOCK );
 
 int count;
 int string_len = 0;
 
 while( _in_iterator != _end_of_in )
 {
  buf.clear();
  count = 0;

  for(; ( ( _in_iterator != _end_of_in ) && ( count < BLOCK ) ); ++_in_iterator, count++ )    
   buf.push_back( *_in_iterator );
   
  if( count < BLOCK )
   buf.resize( count );
   
  #pragma warning( push )
  #pragma warning( disable: 6309 )
  string_len = 0;
  Base64Decode( ( LPCSTR )&buf[ 0 ], 
                buf.size(),
                NULL,
                &string_len
               );
  string_vector.resize( string_len );
  Base64Decode( ( LPCSTR )&buf[0],
                buf.size(),
                ( BYTE* )&string_vector[0],
                &string_len
               );
  #pragma warning( pop )
  
  std::copy( string_vector.begin(), string_vector.end(), _out_iterator );
 }
}
//***************************************************************************
#endif