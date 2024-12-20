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
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

volatile enum eSERVER_PORT eControlPort = eSPORT_C4;

//-----------------------------------------------------------------------------
struct server_info {
    enum eSERVER_PORT   sport;
    char                *name;
};

const struct server_info s_info [] = {
    { eSPORT_C4,  "ODROID-C4"},
    { eSPORT_M1,  "ODROID-M1"},
    { eSPORT_M1S, "ODROID-M1S"},
    { eSPORT_M2,  "ODROID-M2"},
};

static int (*callback_func)(char *buf, int len);

//-----------------------------------------------------------------------------
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
void *thread_func_server (void *arg)
{
    int s_fd = socket(PF_INET, SOCK_STREAM, 0), c_fd = -1, option = 1;
    struct server_info *s_info = (struct server_info *)arg;
    struct sockaddr_in sa, ca;
    socklen_t ca_sz;

    sa.sin_family       = AF_INET;
    sa.sin_port         = htons(s_info->sport);
    sa.sin_addr.s_addr  = htonl(INADDR_ANY);

    setsockopt (s_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    if (bind (s_fd, (struct sockaddr *)&sa, sizeof(sa)) == -1)
        printf ("%s : bind error!\r\n", __func__);

#define MAX_CLIENT  5
    if (listen (s_fd, MAX_CLIENT) == -1)
        printf ("%s : listen error!\r\n", __func__);

    printf ("%s server thread create : port = %d\n", s_info->name, s_info->sport);

    ca_sz = sizeof(ca);

    while (1) {
        char msg [1024];
        int len;

        if (c_fd < 0) {
            if ((c_fd = accept (s_fd, (void *)&ca, &ca_sz)) == -1) {
                printf ("%s : accept error!\r\n", __func__);
                break;
            }
        }
        // connect info display
        printf ("Accept : name = %s, port = %d, ip addr %s, fd = %d\n",
                    s_info->name, s_info->sport, inet_ntoa(ca.sin_addr), c_fd);

        memset  (msg, 0, sizeof(msg));

        // timeout read
        if ((len = read_timeout (c_fd, msg, sizeof(msg), -1)) != 0) {

            if (s_info->sport == eControlPort) {
                pthread_mutex_lock (&mutex);
                if (!callback_func(msg, len))    printf ("error!\n");
                pthread_mutex_unlock (&mutex);
            }
            if (strstr (msg, "quit") != NULL) {
                close (c_fd);   c_fd = -1;
            }
        }
    }
    close (s_fd);
    return arg;
}

//-----------------------------------------------------------------------------
int socket_server_port (enum eSERVER_PORT set_port)
{
    switch (set_port) {
        case eSPORT_M1: case eSPORT_M1S: case eSPORT_M2: case eSPORT_C4:
            eControlPort = set_port;
            return 1;
        default :
            printf ("%s : unknown port value = %d, default port = 8888(ODROID-C4)\r\n", __func__, set_port);
            eControlPort = eSPORT_C4;
            return 0;
    }
}

//-----------------------------------------------------------------------------
int socket_server_init (enum eSERVER_PORT def_port, int (*pcallback_func)(char *, int))
{
    pthread_t pthread_server[ARRARY_SIZE(s_info)];
    int i;

    eControlPort  = def_port;
    callback_func = pcallback_func;

    for (i = 0; i < (int)(ARRARY_SIZE(s_info)); i++)
        pthread_create (&pthread_server[i], NULL, thread_func_server, (void *)&s_info[i]);

    return 1;
}

//-----------------------------------------------------------------------------
int socket_client_init ();
int socket_client_port ();

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
