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
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/select.h>

//-----------------------------------------------------------------------------
#include "lib_socket.h"

//-----------------------------------------------------------------------------
// socket test
// https://uutopia.tistory.com/41
//
// telnet {ip} {port}
// curl -v telnet://{ip}:{port}
//
//-----------------------------------------------------------------------------
pthread_mutex_t mutex_socket = PTHREAD_MUTEX_INITIALIZER;

volatile enum eBOARD_PORT eControlPortID = eBOARD_P_C4;

//-----------------------------------------------------------------------------
struct nlp_socket_info socket_info [] = {
    { eBOARD_P_C4,  8888, "ODROID-C4"},
    { eBOARD_P_M1,  9000, "ODROID-M1"},
    { eBOARD_P_M1S, 9001, "ODROID-M1S"},
    { eBOARD_P_M2,  9002, "ODROID-M2"},
    { eBOARD_P_C5,  9003, "ODROID-C5"},
};

static int (*callback_func)(int c_fd, char *buf, int len);

//-----------------------------------------------------------------------------
// https://stackoverflow.com/questions/2917881/how-to-implement-a-timeout-in-read-function-call
//-----------------------------------------------------------------------------
static int read_timeout (int fd, char *r_buf, int len, int timeout_ms)
{
    // non-block read (if timeout_ms > 0)
    if (timeout_ms > 0) {
        struct timeval timeout;
        fd_set r_fds;
        int status;

        timeout.tv_sec  = 0;
        timeout.tv_usec = timeout_ms * 1000;

        FD_ZERO(&r_fds);
        FD_SET (fd, &r_fds);
        status = select (fd +1, &r_fds, NULL, NULL, &timeout);

        if (status == -1)
            printf ("%s : select function error!\n", __func__);
        if (status ==  0)
            printf ("%s : timeout (%dms)\n", __func__, timeout_ms);
        if (status)
            return read (fd, r_buf, len);
        return 0;
    }

    // block read
    return read (fd, r_buf, len);
}

//-----------------------------------------------------------------------------
// TCP socket use SOCK_STREAM
// UDP socket use SOCK_DGRAM
//-----------------------------------------------------------------------------
void *thread_func_server (void *arg)
{
#define SOCKET_BUF_SIZE 1024
    // tcp socket use
    int s_fd = socket(PF_INET, SOCK_STREAM, 0), c_fd = -1, option = 1, len;
    char msg [SOCKET_BUF_SIZE];
    struct nlp_socket_info *s_info = (struct nlp_socket_info *)arg;
    struct sockaddr_in sa, ca;
    socklen_t ca_sz;

    sa.sin_family       = AF_INET;
    sa.sin_port         = htons(s_info->port);
    sa.sin_addr.s_addr  = htonl(INADDR_ANY);

    // https://blog.naver.com/cache798/130033780509 (socket bind error fix)
    setsockopt (s_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    if (bind (s_fd, (struct sockaddr *)&sa, sizeof(sa)) == -1)
        printf ("%s : bind error!\r\n", __func__);

#define MAX_CLIENT  5
    if (listen (s_fd, MAX_CLIENT) == -1)
        printf ("%s : listen error!\r\n", __func__);

    printf ("%s server thread create : port = %d\n", s_info->name, s_info->port);

    ca_sz = sizeof(ca);

    while (1) {

        if (c_fd < 0) {
            if ((c_fd = accept (s_fd, (void *)&ca, &ca_sz)) == -1) {
                printf ("%s : accept error!\r\n", __func__);
                break;
            }
        }
        // connect info display
        printf ("Accept : name = %s, port = %d, ip addr %s, fd = %d\n",
                    s_info->name, s_info->port, inet_ntoa(ca.sin_addr), c_fd);

        memset  (msg, 0, sizeof(msg));

        // timeout read (timeout val == 0, block mode)
        if ((len = read_timeout (c_fd, msg, sizeof(msg), 0)) != 0) {
            if (socket_info->id == eControlPortID) {
                pthread_mutex_lock (&mutex_socket);
                if (!callback_func(c_fd, msg, len)) {
                    close (c_fd);   c_fd = -1;
                }
                pthread_mutex_unlock (&mutex_socket);
            }
        } else {
            close (c_fd);   c_fd = -1;
        }
    }
    close (s_fd);
    return arg;
}

//-----------------------------------------------------------------------------
struct nlp_socket_info *get_server_port (void)
{
    return &socket_info [eControlPortID];
}

//-----------------------------------------------------------------------------
int set_server_port (enum eBOARD_PORT set_port)
{
    switch (set_port) {
        case eBOARD_P_C4: case eBOARD_P_M1: case eBOARD_P_M1S: case eBOARD_P_M2:
        case eBOARD_P_C5:
            eControlPortID = set_port;
            printf ("%s : set port = %d\n", __func__, socket_info[eControlPortID].port);
            return 1;
        default :
            printf ("%s : unknown port value = %d, set default port = 8888(ODROID-C4)\r\n",
                __func__, set_port);
            eControlPortID = eBOARD_P_C4;
            return 0;
    }
}

//-----------------------------------------------------------------------------
void set_server_callback (int (*pcallback_func)(int, char *, int))
{
    callback_func = pcallback_func;
}

//-----------------------------------------------------------------------------
int socket_server_init (enum eBOARD_PORT def_port, int (*pcallback_func)(int, char *, int))
{
    pthread_t pthread_server[ARRARY_SIZE(socket_info)];
    int i;

    printf ("\n\n*** %s (PORT : %d) ***\n\n", __func__, socket_info[def_port].port);

    set_server_port     (def_port);
    set_server_callback (pcallback_func);

    for (i = 0; i < (int)(ARRARY_SIZE(socket_info)); i++)
        pthread_create (&pthread_server[i], NULL, thread_func_server, (void *)&socket_info[i]);

    return 1;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int socket_client_init ();

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
