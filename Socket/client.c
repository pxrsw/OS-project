#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/time.h>

#define BUFFER_SIZE 10240

void print_error(char *e);

int main(int argc, char **argv) {

    char *server_address = "\0";
    char *port_number = "\0";
    char *text = "\0";

    argv = argv + 1;
    if (argc != 6) {
        print_error("not enough arguements");
        return 1;
    }

    if (strcmp(argv[0], "-h") == 0) {
        server_address = argv[1];
        if (strcmp(argv[2], "-p") == 0)
            port_number = argv[3];

        else{
            print_error("port number not provided");
            return 1;
        }
        
    } else if (strcmp(argv[0], "-p") == 0) {
        port_number = argv[1];
        if (strcmp(argv[2], "-h") == 0)
            server_address = argv[3];

        else{
            print_error("server address not provided");
            return 1;
        }
    } else {
        print_error("uknown arguements!");
        return 1;
    }
    text = argv[4];

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    
    struct sockaddr_in server_socket_address;
    memset(&server_socket_address, 0, sizeof(server_address));

    server_socket_address.sin_family = AF_INET;
    server_socket_address.sin_addr.s_addr = inet_addr(server_address);
    server_socket_address.sin_port = htons(atoi(port_number));

    struct timeval stop, start;
    gettimeofday(&start, NULL);
    connect(client_socket, (struct sockaddr*)&server_socket_address, sizeof(server_socket_address));
    
    send(client_socket, text, strlen(text), 0);

    char buffer[BUFFER_SIZE] = {0};
    recv(client_socket, buffer, BUFFER_SIZE, 0);
    gettimeofday(&stop, NULL);


    printf("%s\n", buffer);
    printf("%lu micro seconds\n", (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec); 
    return 0;
}

void print_error(char *e) {
    printf("%s\n", e);
}