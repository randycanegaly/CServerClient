#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

/*
* network functions are to be implemented in this order, mostly
getaddrinfo() — set up the structs for comm config and adresses/ports 
socket() — Get the File Descriptor
bind() — What port am I on?
connect()—Hey, you! - this is for clients
listen() — waiting for a connection
accept() — 
send() and recv() - server/client talk socket gram
sendto() and recvfrom() — server/clint talk, DGRAM-style
close() and shutdown() — clean up when done
getpeername() — Who are you?
gethostname() — Who am I?
*/

/*
 * Isquared:
 * 1. create the necessary structures for the server address - getaddrinfo()
 * 2. 
 */

int main(void) {
    int status;
    struct addrinfo hints;
    struct addrinfo *serverRes;//pointer to an addrinfo, s/b first in a linked list

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;
    hints.ai_protocol = 0;
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;//these hints member assignments are
                         //belt and suspenders because already memset(), but educational

    char *host = "localhost"; 
    char *port = "4221"; 
    if (status = getaddrinfo(host, port, &hints, &serverRes) != 0) {//**serverRes
        fprintf(stderr, "Getting address details failed with %s\n", gai_strerror(status));
        exit(1);
    }

    printf("getaddr() succeeded\n"); 
    printf("ai_flags: %d\n", serverRes->ai_flags);
    char ip4[INET_ADDRSTRLEN]; // space to hold the IPv4 string
    inet_ntop(AF_INET, &(((struct sockaddr_in *)serverRes->ai_addr)->sin_addr), ip4, INET_ADDRSTRLEN);
    //inet_ntop(AF_INET, &(sa.sin_addr), ip4, INET_ADDRSTRLEN);
    unsigned short aport = ntohs((((struct sockaddr_in *)serverRes->ai_addr)->sin_port));
    printf("server address: %s, port: %u\n", ip4, aport);
}
