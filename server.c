#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAXDATASIZE  128

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
 *
 * REFACTORING - started on 1/21/25
 *  a) make server stay live and receive/handle multiple client connections for simple messages - DONE
 *  b) working on creating a GitHub workflow that will run a test upon push to a branch - IN PROGRESS on 1/23/25. Aaaaand, it's not working. :(
 *  	Moved text.c to repository root. What happens?
 */

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {   
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void) {
    int status, sfd, setsockoptval, client_sfd, backlog, numbytes;
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
    char ip[INET6_ADDRSTRLEN];  // space to hold the IP address tring
    char buf[MAXDATASIZE];      // space to hold string received
    
    if ((status = getaddrinfo(host, port, &hints, &serverRes)) != 0) {//**serverRes
        fprintf(stderr, "SERVER: Getting address details failed with %s\n", gai_strerror(status));
        exit(1);
    }

    /* getaddrinfo() returns a linked list of address structures.
     * Try each address until we successfully bind.
     * If either socket or bind fails, close the socket and
     * try the next address. */

    for (addrp = serverRes; addrp != NULL; addrp = addrp->ai_next) {//walk the linked list of addrinfo structs
                                                                    //to the end, where the next pointer is NULL
        sfd = socket(addrp->ai_family, addrp->ai_socktype, addrp->ai_protocol);
        if(sfd == -1) {//socket errored out, try the next address in the linked list
            continue;//drop out of this iteration of the loop, allow the next one to start
        }   

        printf("SERVER: got a socket with sfd: %d\n", sfd);
        setsockoptval = 1; 
        if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &setsockoptval, sizeof setsockoptval) == -1) {
            perror("SERVER: setsockopt");
            exit(1);
        }
        
        //have a socket descriptor, can we bind to it?
        if(bind(sfd, addrp->ai_addr, addrp->ai_addrlen) == -1) {
            perror("SERVER: bind");
            close(sfd);
            continue;
        }
        inet_ntop(AF_INET, &(((struct sockaddr_in *)addrp->ai_addr)->sin_addr), ip, INET6_ADDRSTRLEN);
        unsigned short aport = ntohs((((struct sockaddr_in *)addrp->ai_addr)->sin_port));
        printf("SERVER: Was able to bind the socket to server address: %s, port: %u\n", ip, aport);

        break;//if we got to here, we have bound and don't need to search any more addresses
    }
    
    freeaddrinfo(serverRes);

    if (addrp == NULL) {//walked off the end of the address list
                     //never bound the socket
        fprintf(stderr, "SERVER: Could not bind\n");
        exit(1);
    }

    if (listen(sfd, backlog) == -1) {
        perror("SERVER: listen");
        exit(1);
    }

    printf("SERVER: listening now\n");

    //REFACTOR - use fork() to handle multiple client connections
    while(1) {  // main accept() loop
        client_addr_len = sizeof client_addr;
        client_sfd = accept(sfd, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_sfd == -1) {
            perror("SERVER: accept");
            continue;
        }

        inet_ntop(client_addr.ss_family,
        get_in_addr((struct sockaddr *)&client_addr), ip, sizeof ip);
        printf("SERVER: got connection from %s\n", ip);

        if (!fork()) { // this is the child process, because fork() returns 0 to the parent and a pid to the child (not 0)
            char client_msg[MAXDATASIZE];
            sprintf(client_msg, "Hello client at %s\n", ip); 
            close(sfd); // child doesn't need the listener
            //the server sends some text to the client 
            if (send(client_sfd, client_msg, strlen(client_msg), 0) == -1)
                perror("SERVER: send");
            
            //and then the server listens for for some reply from the client
            if ((numbytes = recv(client_sfd, buf, MAXDATASIZE-1, 0)) == -1) {
                perror("SERVER: recv");
                exit(1);
            }
            
            printf("SERVER: Received %s from a client.\n", buf);
            close(client_sfd);//done communicating with 'this' instance of a client
            exit(0);
        }
        close(client_sfd);  // parent doesn't need this, child has this file descriptor to use
    }

    return 0;
}


