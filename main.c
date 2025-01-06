//-----------------------------------------------------------------------------
/**
 * @file main.c
 * @author charles-park (charles.park@hardkernel.com)
 * @brief socket server/client control library
 * @version 0.1
 * @date 2024-12-13
 *
 * @copyright Copyright (c) 2022
 *
 */
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <limits.h>
#include <netdb.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <linux/fb.h>
#include <linux/sockios.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>

#if defined(__LIB_SOCKET_APP__)

//------------------------------------------------------------------------------
#include "lib_socket.h"

//------------------------------------------------------------------------------
static void tolowerstr (char *p)
{
    int i, c = strlen(p);

    for (i = 0; i < c; i++, p++)
        *p = tolower(*p);
}

//------------------------------------------------------------------------------
static void print_usage(const char *prog)
{
    printf("\nUsage: %s [-scbm]\n\n", prog);
    puts(
         "  -s --server        server mode init.\n"
         "  -c --client        client moode init.\n"
         "  -b --board         board name (c4, m1, m1s, m2, c5). default c4.\n"
         "  -m --msg           client to server message.\n"
         "\n"
         // server mode test : https://uutopia.tistory.com/41
         "  e.g) lib_socket -s -b m1 \n"    // server mode, port 9000 enable
         "       lib_socket -s -b m2 \n"    // server mode, port 9002 enable
         // server find : nmap {ipaddr}.* -p T:{port} --open
         "       lib_socket -c -b m1 -m '1234567' \n"   // client mode, send msg to open tcp port 9000
    );
    exit(1);
}

//------------------------------------------------------------------------------
int OPT_BOARD_P = 0, OPT_MODE = 0;
char *OPT_MSG = NULL;

static void parse_opts (int argc, char *argv[])
{
    while (1) {
        static const struct option lopts[] = {
            { "server_mode",  0, 0, 's' },
            { "client_mode",  0, 0, 'c' },
            { "board_name",   1, 0, 'b' },
            { "message",      1, 0, 'm' },
            { NULL, 0, 0, 0 },
        };
        int c;

        c = getopt_long(argc, argv, "scb:m:", lopts, NULL);

        if (c == -1)
            break;

        switch (c) {
        case 's':
            OPT_MODE = 0;
            break;
        case 'c':
            OPT_MODE = 1;
            break;
        case 'b':
            tolowerstr(optarg);

            if      (!strncmp ("m1", optarg, strlen(optarg)))
                OPT_BOARD_P = eBOARD_P_M1;
            else if (!strncmp ("m1s", optarg, strlen(optarg)))
                OPT_BOARD_P = eBOARD_P_M1S;
            else if (!strncmp ("m2", optarg, strlen(optarg)))
                OPT_BOARD_P = eBOARD_P_M2;
            else if (!strncmp ("c5", optarg, strlen(optarg)))
                OPT_BOARD_P = eBOARD_P_C5;
            else
                OPT_BOARD_P = eBOARD_P_C4;
            break;
        case 'm':
            OPT_MSG = optarg;
            break;
        default:
            print_usage(argv[0]);
            break;
        }
    }
}

//------------------------------------------------------------------------------
static int parse_packet (char *msg, int r_size)
{
    printf ("%s : size = %d, msg = %s\n", __func__, r_size, msg);
    if (!strncmp(msg, "exit", strlen("exit")))
        return 0;
    return 1;
}

//------------------------------------------------------------------------------
int main (int argc, char **argv)
{
    parse_opts (argc, argv);

    if (!OPT_MODE)
        socket_server_init (OPT_BOARD_P, parse_packet);

    while (1)   sleep (1);
    return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#endif  // __LIB_IOSHELD_APP__
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
