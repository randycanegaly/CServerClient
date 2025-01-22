#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAXDATASIZE 128

//http://localhost
//port is 4221

void *get_in_addr(struct sockaddr *sa);

int main(int argc, char *argv[]) {
    int status, sfd, setsockoptval, numbytes;
    /*
     * struct addrinfo holds information about the 
     * type of connection and communication you want
     * as well as the address you want to connect to
     * you first create one with what you know
     * as a hint and pass that. getaddrinfo() fills
     * out what is missing for the address specified
     */

    struct addrinfo hints;
    struct addrinfo *serverinfo, *addrp;
    char ip4[INET_ADDRSTRLEN]; //allocate storage for a string for an IPv4 address
    char buf[MAXDATASIZE];
    char addrstr[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;//do the right IPv4 vs. IPv6 thing
    hints.ai_socktype = SOCK_STREAM;//using sockets to communicate
    hints.ai_flags = AI_CANONNAME;//show the real name of the host in results

    if (argc != 3) {
        fprintf(stderr, "Usage: You need to provide two arguments: host and port.\n");
        exit(1);
    }

    //getaddrinfo()
    if ((status = getaddrinfo(argv[1], argv[2], &hints, &serverinfo)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));//gai_strerror() 
                                                                         //returns a string describing the 
                                                                         //status error code
        exit(1);
    }
    //should have some address info in serverinfo at this point
    //get the first element in the serverinfo linked list and see what's in it

    for (addrp = serverinfo; addrp != NULL; addrp = addrp->ai_next) {//walk the linked list of addrinfo structs 
                                                                    //to the end -- next pointer is NULL
        //socket();
        if ((sfd = socket(addrp->ai_family, addrp->ai_socktype, addrp->ai_protocol)) == -1) {
            perror("server: socket\n");
            continue;
        }
        printf("got a socket with sfg: %d\n", sfd);
        
        //connect();
        if( connect(sfd, addrp->ai_addr, addrp->ai_addrlen) == -1 ) {
            perror("client: connect\n");
            continue;//couldn't connect, try the next address
        }

        break;//if we got to here, we have connected and don't need to search any more addresses
    }
    
    inet_ntop(addrp->ai_family, get_in_addr((struct sockaddr *)addrp->ai_addr), addrstr, sizeof addrstr);
    printf("client: connected to %s\n", addrstr);
    
    //client listens for some message from the server after connecting
    if ((numbytes = recv(sfd, buf, MAXDATASIZE-1, 0)) == -1) {
        perror("client: recv\n");
        exit(1);
    }

    buf[numbytes] = '\0';//terminate the buffer string
    printf("received %d bytes -- %s\n", numbytes, buf);

    char *reply = "Hi back!"; 
    int replen = strlen(reply);

    //client sends some reply to the server
    if (send(sfd, reply, replen, 0) == -1) {
        perror("client: send\n");
        exit(1);
    }

    close(sfd);
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
