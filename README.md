# lib_socket
Network label printer server socket communication for ODROID boards.

* ODROID BOARD TCP PORT  
    eBOARD_P_C4 = 8888,  
    eBOARD_P_M1 = 9000,  
    eBOARD_P_M1S = 9001,  
    eBOARD_P_M2 = 9002,  
    eBOARD_P_C5 = 9003,  


```
    Usage: ./lib_socket [-scbm]
    puts(
         -s --server        server mode init.\n"
         -c --client        client moode init.\n"
         -b --board         board name (c4, m1, m1s, m2, c5). default c4.\n"
         -m --msg           client to server message.\n"

         // server mode test : https://uutopia.tistory.com/41
         e.g)lib_socket -s -b m1 \n"    // server mode, port 9000 enable
             lib_socket -s -b m2 \n"    // server mode, port 9002 enable
         // server find : nmap {ipaddr}.* -p T:{port} --open
             lib_socket -c -b m1 -m '1234567' \n"   // client mode, send msg to open tcp port 9000
```
