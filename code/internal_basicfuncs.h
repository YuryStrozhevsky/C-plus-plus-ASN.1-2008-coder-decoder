#ifndef _870E69C1_1A78_42FD_93E5_5BA828575D99
#define _870E69C1_1A78_42FD_93E5_5BA828575D99
//**************************************************************************
template< class T, class V >
T value_init( T stream, 
               unsigned long long max_block_len,
               BASIC_type** ret_info,
               inform_block* inf_block,
               length_block* len_block )
{
 delete (*ret_info); // delete previously created "BASIC_type" object instance

 V* type = new V();

 type->inf_block = inf_block;
 type->len_block = len_block;

 type->add_warnings( inf_block );
 type->add_warnings( len_block );

 T end = type->decode_value( stream, 
                             ( len_block->is_indefinite_form ) ? max_block_len : len_block->length );

 (*ret_info) = type;

 return end;
}
//**************************************************************************
template< typename T >
T type_init( T stream,
              unsigned long long max_block_len,
              BASIC_type** ret_info )
{
 (*ret_info) = new BASIC_type(); // in case we will not find any specific value decoding

 if( stream == T() )
 {
  sprintf_s( (*ret_info)->error, 255, "NULL stream is unaccepted" );
  return T();
 }

 if( !max_block_len ) // No error, just return empty type
  return T();

 //--------------------------------
 inform_block* inf_block = new inform_block();

 T end = inf_block->decode_value( stream, max_block_len );
 if( inf_block->error[ 0 ] != 0x00 )
 {
  sprintf_s( (*ret_info)->error, 255, inf_block->error );
  
  delete inf_block;
  
  return end;
 }

 max_block_len -= inf_block->block_length;

 length_block* len_block = new length_block();

 end = len_block->decode_value( end, max_block_len );
 if( len_block->error[ 0 ] != 0x00 ) // Give a chance to process in case of zero-length value block
 {
  sprintf_s( (*ret_info)->error, 255, len_block->error );
  
  delete inf_block;
  delete len_block;
  
  return end;
 }
 
 max_block_len -= len_block->block_length;

 // Check for usign indefinite length form in encoding for primitive types
 if( ( inf_block->tag_class == 1 ) &&
     ( len_block->is_indefinite_form ) )
 {
  switch( inf_block->tag_number )
  {
   case 1:  // BOOLEAN
   case 2:  // INTEGER
   case 5:  // NULL
   case 6:  // OBJECT IDENTIFIER
   case 9:  // REAL
   case 14: // TIME
   case 23:
   case 24:
   case 31:
   case 32:
   case 33:
   case 34:
    sprintf_s( (*ret_info)->error, 255, "Indefinite length form used for primitive type" );
    
    delete inf_block;
    delete len_block;
    
    return T();
   default:
    ;
  }
 }

 //--------------------------------

 switch( inf_block->tag_class )
 {
  case 1: // UNIVERSAL
   if( ( inf_block->tag_number >= 37 ) && ( !inf_block->is_too_long_tag ) )
   {
    sprintf_s( (*ret_info)->error, 255, "UNIVERSAL 37 and upper tags are reserved by ASN.1 standard" );
    return T();
   }

   switch( inf_block->tag_number )
   {
    case 0: // Hopefuly EOC_type (end-of-content)
     if( inf_block->is_constructed || len_block->length )
     {
      sprintf_s( (*ret_info)->error, 255, "Type [UNIVERSAL 0] is reserved" );
      return T();
     }

     return value_init< T, EOC_type >( end, 
                                       max_block_len,
                                       ret_info, 
                                       inf_block, 
                                       len_block );
     break;
    case 1: // BOOLEAN
     return value_init< T, BOOLEAN_type >( end, 
                                           max_block_len,
                                           ret_info, 
                                           inf_block, 
                                           len_block );
     break;
    case 2: // INTEGER
     return value_init< T, INTEGER_type >( end, 
                                           max_block_len,
                                           ret_info, 
                                           inf_block, 
                                           len_block );
     break;
    case 3: // BITSTRING
     return value_init< T, BITSTRING_type >( end, 
                                             max_block_len,
                                             ret_info, 
                                             inf_block, 
                                             len_block );
     break;
    case 4: // OCTETSTRING
     return value_init< T, OCTETSTRING_type >( end, 
                                               max_block_len,
                                               ret_info, 
                                               inf_block, 
                                               len_block );
     break;
    case 5: // NULL
     return value_init< T, NULL_type >( end, 
                                        max_block_len,
                                        ret_info, 
                                        inf_block, 
                                        len_block );
     break;
    case 6: // OBJECT IDENTIFIER
     return value_init< T, OID_type >( end, 
                                       max_block_len,
                                       ret_info, 
                                       inf_block, 
                                       len_block );
     break;
    case 7: // OBJECT DESCRIPTOR
     return value_init< T, OBJECTDESCRIPTOR_type >( end, 
                                                    max_block_len,
                                                    ret_info, 
                                                    inf_block, 
                                                    len_block );
     break;
    case 8: // EXTERNAL TYPE
     return value_init< T, EXTERNAL_type >( end, 
                                            max_block_len,
                                            ret_info, 
                                            inf_block, 
                                            len_block );
     break;
    case 9: // REAL
     return value_init< T, REAL_type >( end, 
                                        max_block_len,
                                        ret_info, 
                                        inf_block, 
                                        len_block );
     break;
    case 10: // ENUMERATED
     return value_init< T, ENUMERATED_type >( end, 
                                              max_block_len,
                                              ret_info, 
                                              inf_block, 
                                              len_block );
     break;
    case 11: // EMBEDDED PDV
     return value_init< T, EMBEDDEDPDV_type >( end, 
                                               max_block_len,
                                               ret_info, 
                                               inf_block, 
                                               len_block );
     break;
    case 12: // UTF8STRING
     return value_init< T, UTF8STRING_type >( end, 
                                              max_block_len,
                                              ret_info, 
                                              inf_block, 
                                              len_block );
     break;
    case 13: // RELATIVE OBJECT IDENTIFIER
     return value_init< T, ROID_type >( end, 
                                        max_block_len,
                                        ret_info, 
                                        inf_block, 
                                        len_block );
     break;
    case 14: // TIME
     return value_init< T, TIME_type >( end, 
                                        max_block_len,
                                        ret_info, 
                                        inf_block, 
                                        len_block );
     break;
    case 15:
     sprintf_s( (*ret_info)->error, 255, "[UNIVERSAL 15] is reserved by ASN.1 standard" );
     return T();

     break;
    case 16: // SEQUENCE
     return value_init< T, SEQUENCE_type >( end, 
                                            max_block_len,
                                            ret_info, 
                                            inf_block, 
                                            len_block );
     break;
    case 17: // SET
     return value_init< T, SET_type >( end, 
                                       max_block_len,
                                       ret_info, 
                                       inf_block, 
                                       len_block );
     break;
    case 18: // NUMERICSTRING
     return value_init< T, NUMERICSTRING_type >( end, 
                                                 max_block_len,
                                                 ret_info, 
                                                 inf_block, 
                                                 len_block );
     break;
    case 19: // PRINTABLESTRING
     return value_init< T, PRINTABLESTRING_type >( end, 
                                                   max_block_len,
                                                   ret_info, 
                                                   inf_block, 
                                                   len_block );
     break;
    case 20: // TELETEXSTRING
     return value_init< T, TELETEXSTRING_type >( end, 
                                                 max_block_len,
                                                 ret_info, 
                                                 inf_block, 
                                                 len_block );
     break;
    case 21: // VIDEOTEXSTRING
     return value_init< T, VIDEOTEXSTRING_type >( end, 
                                                  max_block_len,
                                                  ret_info, 
                                                  inf_block, 
                                                  len_block );
     break;
    case 22: // IA5STRING
     return value_init< T, IA5STRING_type >( end, 
                                             max_block_len,
                                             ret_info, 
                                             inf_block, 
                                             len_block );
     break;
    case 23: // UTCTIME
     return value_init< T, UTCTIME_type >( end, 
                                           max_block_len,
                                           ret_info, 
                                           inf_block, 
                                           len_block );
     break;
    case 24: // GENERALIZEDTIME
     return value_init< T, GENERALIZEDTIME_type >( end, 
                                                   max_block_len,
                                                   ret_info, 
                                                   inf_block, 
                                                   len_block );
     break;
    case 25: // GRAPHICSTRING
     return value_init< T, GRAPHICSTRING_type >( end, 
                                                 max_block_len,
                                                 ret_info, 
                                                 inf_block, 
                                                 len_block );
     break;
    case 26: // VISIBLESTRING
     return value_init< T, VISIBLESTRING_type >( end, 
                                                 max_block_len,
                                                 ret_info, 
                                                 inf_block, 
                                                 len_block );
     break;
    case 27: // GENERALSTRING
     return value_init< T, GENERALSTRING_type >( end, 
                                                 max_block_len,
                                                 ret_info, 
                                                 inf_block, 
                                                 len_block );
     break;
    case 28: // UNIVERSALSTRING
     return value_init< T, UNIVERSALSTRING_type >( end, 
                                                   max_block_len,
                                                   ret_info, 
                                                   inf_block, 
                                                   len_block );
     break;
    case 29: // CHARACTERSTRING
     return value_init< T, CHARACTERSTRING_type >( end, 
                                                   max_block_len,
                                                   ret_info, 
                                                   inf_block, 
                                                   len_block );
     break;
    case 30: // BMPSTRING
     return value_init< T, BMPSTRING_type >( end, 
                                             max_block_len,
                                             ret_info, 
                                             inf_block, 
                                             len_block );
     break;
    case 31: // DATE
     return value_init< T, DATE_type >( end, 
                                        max_block_len,
                                        ret_info, 
                                        inf_block, 
                                        len_block );
     break;
    case 32: // TIME-OF-DAY
     return value_init< T, TIMEOFDAY_type >( end, 
                                             max_block_len,
                                             ret_info, 
                                             inf_block, 
                                             len_block );
     break;
    case 33: // DATE-TIME
     return value_init< T, DATETIME_type >( end, 
                                            max_block_len,
                                            ret_info, 
                                            inf_block, 
                                            len_block );
     break;
    case 34: // DURATION
     return value_init< T, DURATION_type >( end, 
                                            max_block_len,
                                            ret_info, 
                                            inf_block, 
                                            len_block );
     break;
    case 35: // OID INTERNATIONALIZED
     return value_init< T, OIDINTER_type >( end, 
                                            max_block_len,
                                            ret_info, 
                                            inf_block, 
                                            len_block );
     break;
    case 36: // RELATIVE OID INTERNATIONALIZED
     return value_init< T, ROIDINTER_type >( end, 
                                             max_block_len,
                                             ret_info, 
                                             inf_block, 
                                             len_block );
     break;
    default: // Decoding for unknown types
     if( inf_block->is_constructed )
      return value_init< T, CONSTRUCTED_type >( end, 
                                                max_block_len,
                                                ret_info, 
                                                inf_block, 
                                                len_block );
     else
      return value_init< T, PRIMITIVE_type< unsigned char* > >( end, 
                                                                max_block_len,
                                                                ret_info, 
                                                                inf_block, 
                                                                len_block );
   }
   break;
  case 2: // APPLICATION
  case 3: // CONTEXT-SPECIFIC
  case 4: // PRIVATE
  default: // Decoding for unknown types
   if( inf_block->is_constructed )
    return value_init< T, CONSTRUCTED_type >( end, 
                                              max_block_len,
                                              ret_info, 
                                              inf_block, 
                                              len_block );
   else
    return value_init< T, PRIMITIVE_type< unsigned char* > >( end, 
                                                              max_block_len,
                                                              ret_info, 
                                                              inf_block, 
                                                              len_block );
 }

 return end;
}
//**************************************************************************
#endif