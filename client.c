#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>


//http://localhost
//port is 4221


int main(int argc, char *argv[]) {
    int status;
    /*
     * struct addrinfo holds information about the 
     * type of connection and communication you want
     * as well as the address you want to connect to
     * you first create one with what you know
     * as a hint and pass that. getaddrinfo() fills
     * out what is missing for the address specified
     */

    struct addrinfo hints;
    struct addrinfo *serverinfo;//pointer to an array of addrinfo structs - all addresses
                                //for the host
    //clear hints structure to all zeroes and provide what I know at this point as hints
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;//do the right IPv4 vs. IPv6 thing
    hints.ai_socktype = SOCK_STREAM;//using sockets to communicate
    hints.ai_flags = AI_CANONNAME;//show the real name of the host in results

    if (argc != 3) {
        fprintf(stderr, "Usage: You need to provide two arguments: host and port.\n");
        exit(1);
    }

    if ((status = getaddrinfo(argv[1], argv[2], &hints, &serverinfo)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));//gai_strerror() 
                                                                         //returns a string describing the 
                                                                         //status error code
        exit(1);
    }
    //should have some address info in serverinfo at this point
    //get the first element in the serverinfo linked list and see what's in it
    printf("first server info, server canon name: %s\n", serverinfo[0].ai_canonname); 

    //TO DO:
    //walk serverinfo, trying to create sockets and stop on the first successful one
    //



    //socket();
    //bind();
    //connect();




}
