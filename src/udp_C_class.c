
/* ============================================================================

 UDP communication with limited packed size

 Author: Gastone Pietro Rosati Papini

 ============================================================================ */

#include "udp_C_class.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
  #include "udp_C_win.hxx"
  typedef int ssize_t;
#else
  #include "udp_C_unix.hxx"
#endif

void
Socket_new( SocketData * pS ) {
  pS->socket_id       = -1;
  pS->target_addr_len = sizeof(struct sockaddr_in);
  pS->server_run      = FALSE;
  pS->timeout_ms      = APP_TIMEOUT_MS;
}

void
Socket_check( SocketData * pS ) {
  if ( pS->socket_id >= 0 ) {
    printf( "Opened socket id = %d\n", pS->socket_id );
  } else {
    printf( "Socket not opened\n" );
    exit(-1);
  }
}

void
Socket_open_as_client( SocketData * pS,
                       char const   addr[],
                       uint16_t     port ) {
  Socket_open_addr( pS, addr, port );
  Socket_open( pS, FALSE );
}

void
Socket_open_as_server( SocketData * pS, uint16_t port ) {
  Socket_open_addr( pS, nullptr, port );
  Socket_open( pS, TRUE );
}

void
Socket_open_addr(
  SocketData * pS,
  char const   addr[],
  uint16_t     port
) {
  /* Clear the address structures */
  memset( &pS->target_addr, 0, pS->target_addr_len );
  /* Set the address structures */
  pS->target_addr.sin_family = AF_INET;
  pS->target_addr.sin_port   = htons(port);
  if ( addr == nullptr )
    pS->target_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  else
    pS->target_addr.sin_addr.s_addr = inet_addr(addr);
}

/*\
 | Send message function
\*/

int
Socket_send(
  SocketData * pS,
  uint32_t     message_id,
  uint8_t      message[],
  uint32_t     message_size
) {

  uint32_t n_packets;
  packet_t packet;
  uint32_t ipos;

  #if defined(WIN_NONBLOCK)
	uint64_t socket_start_time;
	uint64_t socket_elapsed_time;
  #endif

  n_packets = Packet_Number( message_size );

  /* Send packets */
  for ( ipos = 0 ; ipos < n_packets; ++ipos ) {

    Packet_Build_from_buffer( message,
                              message_size,
                              ipos,
                              message_id,
                              pS->server_run,
                              &packet );

    #if defined(WIN_NONBLOCK)
    socket_start_time = get_time_ms();
    while ( 1 ) {
	  	if ( sendto( pS->socket_id,
                   packet.data_buffer,
                   (size_t) PACKET_BYTES,
                   0,
                   (struct sockaddr *) &pS->target_addr,
                   sizeof(pS->target_addr) ) == SOCKET_ERROR ) {
        socket_elapsed_time = get_time_ms() - socket_start_time;
			  if ( WSAGetLastError() != WSAEWOULDBLOCK ||
             socket_elapsed_time >= RECV_SND_TIMEOUT_MS ) {
			  	printf( "sendto() failed. Error Code: %d\n", WSAGetLastError() );
				  return FALSE;
			  }
		  } else {
			  break;
      }
    }
    #elif defined(_WIN32)
	  if ( sendto( socket_id,
                 packet.data_buffer,
                 (size_t) PACKET_BYTES,
                 0,
                 (struct sockaddr *) &target_addr,
                 sizeof(target_addr) ) == SOCKET_ERROR ) {
  		printf( "sendto() failed. Error Code: %d\n", WSAGetLastError() );
		  return FALSE;
    }
    #elif defined(__MACH__) || defined(__linux__)
    if ( sendto( pS->socket_id,
                 packet.data_buffer,
                 (size_t) PACKET_BYTES,
                 0,
                 (struct sockaddr *) &pS->target_addr,
                 sizeof(pS->target_addr) ) == SOCKET_ERROR ) {
		  perror("error sendto()");
		  return FALSE;
    }
    #endif
  }

  #ifdef DEBUG_UDP
  printf( "Sent message of %d packets to %s:%d\n",
          n_packets, inet_ntoa(pS->target_addr.sin_addr),
          ntohs(pS->target_addr.sin_port) );
  #endif
	return TRUE;
}

/*\
 | Receive message function
\*/

int
Socket_receive(
  SocketData * pS,
  uint32_t   * p_message_id,
  uint8_t      message[],
  uint32_t     message_size,
  uint64_t     start_time_ms
) {

  packet_t packet;
  ssize_t  buffer_bytes    = 0;
  ssize_t  recv_bytes      = 0;
  uint64_t elapsed_time_ms = 0;

  #if defined(WIN_NONBLOCK)
  uint64_t socket_start_time;
  uint64_t socket_elapsed_time;
  #endif

  packet_info_t pi;
  Packet_Init( &pi, start_time_ms );

  /* azzera pacchetto da ricevere */
  memset( packet.data_buffer, '\0', sizeof(packet.data_buffer) );

  /* Receive packets */
  elapsed_time_ms = start_time_ms == 0 ? 0 : get_time_ms() - start_time_ms;
  while ( elapsed_time_ms <= pS->timeout_ms && pS->server_run == TRUE ) {

    #if defined(WIN_NONBLOCK)
    socket_start_time = get_time_ms();
    while ( 1 ) {
      recv_bytes = recvfrom( pS->socket_id,
                             packet.data_buffer,
                             (size_t) PACKET_BYTES,
                             0,
                             (struct sockaddr *) &pS->target_addr,
                             &pS->target_addr_len );
      socket_elapsed_time = get_time_ms() - socket_start_time;

      if ( recv_bytes == SOCKET_ERROR ) {
        if ( WSAGetLastError() != WSAEWOULDBLOCK ||
             socket_elapsed_time >= RECV_SND_TIMEOUT_MS ) break;
      } else {
        break;
      }
    }
    #elif defined(_WIN32)
    recv_bytes = recvfrom( pS->socket_id,
                           packet.data_buffer,
                           (size_t) PACKET_BYTES,
                           0,
                           (struct sockaddr *) &pS->target_addr,
                           &pS->target_addr_len );
    #elif defined(__MACH__) || defined(__linux__)
    recv_bytes = recvfrom( pS->socket_id,
                           packet.data_buffer,
                           (size_t) PACKET_BYTES,
                           0,
                           (struct sockaddr *) &pS->target_addr,
                           &pS->target_addr_len );
    #endif

    #if defined(WIN_NONBLOCK)
    if ( recv_bytes != SOCKET_ERROR )
    #elif defined(_WIN32)
    if ( recv_bytes != SOCKET_ERROR)
    #elif defined(__MACH__) || defined(__linux__)
    if ( recv_bytes > 0 )
    #endif
    {
      buffer_bytes += recv_bytes;
      Packet_Add_to_buffer( &pi, &packet, message, message_size );
      pS->server_run = pi.server_run;
    } else {
      sleep_ms(SLEEP_MS);
    }

    if ( pi.received_packets == pi.n_packets && pi.n_packets > 0 ) break;

    // Calculate elapsed time
    if ( start_time_ms != 0 ) elapsed_time_ms = get_time_ms() - start_time_ms;

  }

  if ( pi.received_packets == pi.n_packets ) {
    *p_message_id = pi.datagram_id;
    #ifdef DEBUG_UDP
    printf( "Received message of %d packets from %s:%d\n",
            pi.n_packets,
            inet_ntoa(pS->target_addr.sin_addr),
            ntohs(pS->target_addr.sin_port) );
    #endif
    return TRUE;
  } else if ( elapsed_time_ms >= pS->timeout_ms ) {
    printf( "Receive Warning: Time-out reached! Timeout is: %llu Time needed: %llu\n",
            pS->timeout_ms, elapsed_time_ms );
    return FALSE;
  } else {
    printf( "Receive Warning: Server not running'n" );
    return FALSE;
  }

}

#ifdef __cplusplus
}
#endif
