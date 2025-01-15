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
enum eBOARD_PORT {
    eBOARD_P_C4 = 0,
    eBOARD_P_M1,
    eBOARD_P_M1S,
    eBOARD_P_M2,
    eBOARD_P_C5,
    eBOARD_P_END
};

struct nlp_socket_info {
    const enum eBOARD_PORT    id;
    const int                 port;
    const char                *name;
};

#define ARRARY_SIZE(x)  (sizeof(x)/sizeof(x[0]))

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Function prototype
//------------------------------------------------------------------------------
extern struct nlp_socket_info *get_server_port     (void);

extern int              set_server_port     (enum eBOARD_PORT set_port);
extern void             set_server_callback (int (*pcallback_func)(char *, int));
extern int              socket_server_init  (enum eBOARD_PORT def_port, int (*pcallback_func)(char *, int));

//------------------------------------------------------------------------------
#endif  //  #define __LIB_SOCKET_H__

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
