#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>

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
 * 1. create the necessary structures for the server address - getaddrinfo() - DONE
 * 2. get a socket file descriptor using the address struct contents - DONE
 * 3. used setsocketopt to configure the socket - DONE
 * 4. bind - associate an address and port with the socket - DONE
 * 5. listen - DONE
 * 6. accept - accept a client connection - DONE
 */

int main(void) {
    int status, sfd, setsockoptval, client_sfd, backlog;
    struct addrinfo hints;
    struct addrinfo *serverRes, *addrp;//pointer to an addrinfo, s/b first in a linked list
                                       //and a tracking pointer to walk the linked list
    struct sockaddr_storage client_addr;
    socklen_t client_addr_len;

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
    char ip4[INET_ADDRSTRLEN]; // space to hold the IPv4 string
    
    if (status = getaddrinfo(host, port, &hints, &serverRes) != 0) {//**serverRes
        fprintf(stderr, "Getting address details failed with %s\n", gai_strerror(status));
        exit(1);
    }

    /* getaddrinfo() returns a linked list of address structures.
     * Try each address until we successfully bind.
     * If either socket or bind fails, close the socket and
     * try the next address. */

    for (addrp = serverRes; addrp != NULL; addrp = addrp->ai_next) {//walk the linked list of addrinfo structs
                                                                    //to the end -- next pointer is NULL
        sfd = socket(addrp->ai_family, addrp->ai_socktype, addrp->ai_protocol);
        if(sfd == -1) {//socket errored out, try the next address in the linked list
            continue;//drop out of this iteration of the loop, allow the next one to start
        }   

        printf("got a socket with sfg: %d\n", sfd);
        setsockoptval = 1; 
        if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &setsockoptval, sizeof setsockoptval) == -1) {
            perror("server: setsockopt");
            exit(1);
        }
        
        //have a socket descriptor, can we bind to it?
        if(bind(sfd, addrp->ai_addr, addrp->ai_addrlen) == -1) {
            perror("server: bind");
            close(sfd);
            continue;
        }
        inet_ntop(AF_INET, &(((struct sockaddr_in *)addrp->ai_addr)->sin_addr), ip4, INET_ADDRSTRLEN);
        unsigned short aport = ntohs((((struct sockaddr_in *)addrp->ai_addr)->sin_port));
        printf("Was able to bind the socket to server address: %s, port: %u\n", ip4, aport);

        break;//if we got to here, we have bound and don't need to search any more addresses
    }
    
    freeaddrinfo(serverRes);

    if (addrp == NULL) {//walked off the end of the address list
                     //never bound the socket
        fprintf(stderr, "Could not bind\n");
        exit(1);
    }

    if (listen(sfd, backlog) == -1) {
        perror("server: listen");
        exit(1);
    }

    printf("listening now\n");

    client_addr_len = sizeof client_addr;
    if ((client_sfd = accept(sfd, (struct sockaddr *)&client_addr, &client_addr_len)) == -1) {
        perror("server: accept");
        exit(1);
    }
    printf("accepted connection to client: %d\n", client_sfd);
    close(client_sfd);
}


