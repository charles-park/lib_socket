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
static void print_usage(const char *prog)
{
    printf("\nUsage: %s [-xymctsr]\n\n", prog);
    puts("  -x --lcd_x         lcd x (COL) position. (default 0).\n"
         "  -y --lcd_y         lcd y (ROW) position. (default 0).\n"
         "  -m --msg           lcd display msg\n"
         "  -c --lcd_clear     lcd clear line.(default -1, all clear)\n"
         "  -t --show_time     current time display (offset)\n"
         "  -s --led_set       led 1 ~ 7 on (D1 ~ D7)\n"
         "  -r --led_clear     led 1 ~ 7 off(D1 ~ D7)\n"
    );
    exit(1);
}

//------------------------------------------------------------------------------
int OPT_X_POS = 0, OPT_Y_POS = 0, OPT_CLEAR = -1, OPT_TIME_OFFSET = 0;
int OPT_LED_SET = 0, OPT_LED_CLEAR = 0;
char *OPT_MSG = NULL;

static void parse_opts (int argc, char *argv[])
{
    while (1) {
        static const struct option lopts[] = {
            { "lcd_x_pos",  1, 0, 'x' },
            { "lcd_y_pos",  1, 0, 'y' },
            { "lcd_msg",    1, 0, 'm' },
            { "lcd_clear",  1, 0, 'c' },
            { "show_time",  1, 0, 't' },
            { "led_set",    1, 0, 's' },
            { "led_clear",  1, 0, 'r' },
            { NULL, 0, 0, 0 },
        };
        int c;

        c = getopt_long(argc, argv, "x:y:m:c:t:s:r:", lopts, NULL);

        if (c == -1)
            break;

        switch (c) {
        case 'x':
            OPT_X_POS = atoi(optarg);
            break;
        case 'y':
            OPT_Y_POS = atoi(optarg);
            break;
        case 'm':
            OPT_MSG = optarg;
            break;
        case 'c':
            OPT_CLEAR = atoi(optarg);
            break;
        case 't':
            OPT_TIME_OFFSET = atoi(optarg);
            break;
        case 's':
            OPT_LED_SET = atoi(optarg);
            break;
        case 'r':
            OPT_LED_CLEAR = atoi(optarg);
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
    return 1;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
int main (int argc, char **argv)
{
    unsigned char i = 0;

    socket_server_init (eSPORT_M1, parse_packet);

    while (1)   sleep (1);
    return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#endif  // __LIB_IOSHELD_APP__
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
