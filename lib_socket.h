//-----------------------------------------------------------------------------
/**
 * @file lib_socket.h
 * @author charles-park (charles.park@hardkernel.com)
 * @brief socket server/client control library
 * @version 0.1
 * @date 2024-12-13
 *
 * @copyright Copyright (c) 2022
 *
 */
//-----------------------------------------------------------------------------
//------------------------------------------------------------------------------
#ifndef __LIB_SOCKET_H__
#define __LIB_SOCKET_H__

//------------------------------------------------------------------------------
//
// define socket number
//
//------------------------------------------------------------------------------
enum eSERVER_PORT {
    eSPORT_C4 = 8888,
    eSPORT_M1 = 9000,
    eSPORT_M1S,
    eSPORT_M2,
    sSPORT_END
};

#define ARRARY_SIZE(x)  (sizeof(x)/sizeof(x[0]))

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Function prototype
//------------------------------------------------------------------------------
extern int socket_server_port (enum eSERVER_PORT set_port);
extern int socket_server_init (enum eSERVER_PORT def_port, int (*pcallback_func)(char *, int));

//------------------------------------------------------------------------------
#endif  //  #define __LIB_SOCKET_H__

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
