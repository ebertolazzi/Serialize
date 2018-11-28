
/* ============================================================================

 UDP communication with limited packed size

 Author: Enrico Bertolazzi, Francesco Biral, Gastone Pietro Rosati Papini

 ============================================================================ */

#ifndef __BUFFER_DEFINES_HH
#define __BUFFER_DEFINES_HH

#ifdef __cplusplus
  #include <cstdint>
  using std::int32_t;
  using std::int64_t;
  using std::uint32_t;
  using std::uint64_t;
  extern "C" {
#else
  #include <stdint.h>
#endif

extern void uint16_to_buffer( uint16_t in, uint8_t buffer[2] );
extern void int16_to_buffer( int16_t in, uint8_t buffer[2] );
extern void int32_to_buffer( int32_t in, uint8_t buffer[4] );
extern void uint32_to_buffer( uint32_t in, uint8_t buffer[4] );
extern void int64_to_buffer( int64_t in, uint8_t buffer[8] );
extern void uint64_to_buffer( uint64_t in, uint8_t buffer[8] );
extern void float_to_buffer( float in, uint8_t buffer[4] );
extern void double_to_buffer( double in, uint8_t buffer[8] );

extern void buffer_to_uint16( uint8_t const buffer[2], uint16_t * out );
extern void buffer_to_int16( uint8_t const buffer[2], int16_t * out );
extern void buffer_to_uint32( uint8_t const buffer[4], uint32_t * out );
extern void buffer_to_int32( uint8_t const buffer[4], int32_t * out );
extern void buffer_to_uint64( uint8_t const buffer[8], uint64_t * out );
extern void buffer_to_int64( uint8_t const buffer[8], int64_t * out );
extern void buffer_to_float( uint8_t const buffer[8], float *out);
extern void buffer_to_double( uint8_t const buffer[8], double *out );

#ifdef __cplusplus
}
#endif

#endif
