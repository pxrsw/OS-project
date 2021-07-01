#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <ctype.h>

#define BUFFER_SIZE 10240

void print_error(char *e);

int main(int argc, char **argv) {
    if (argc != 5) {
        print_error("4 arguements must be provided");
        return 1;    
    }
    argv +=1;

    char *listen_address = "\0";
    char *port_number = "\0";

    if (strcmp(argv[0], "-h") == 0) {
        listen_address = argv[1];
        if (strcmp(argv[2], "-p") == 0)
            port_number = argv[3];

        else{
            print_error("port number not provided");
            return 1;
        }
        
    } else if (strcmp(argv[0], "-p") == 0) {
        port_number = argv[1];
        if (strcmp(argv[2], "-h") == 0)
            listen_address = argv[3];

        else{
            print_error("listen address not provided");
            return 1;
        }
    } else {
        print_error("uknown arguements!");
        return 1;
    }

    int listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    
    struct sockaddr_in server_socket_address;
    memset(&server_socket_address, 0, sizeof(server_socket_address));
    server_socket_address.sin_family = AF_INET;
    server_socket_address.sin_addr.s_addr = inet_addr(listen_address);
    server_socket_address.sin_port = htons(atoi(port_number));


    bind(listen_socket, (struct sockaddr *)&server_socket_address, sizeof(server_socket_address));

    listen(listen_socket, 5);
    int requests_number = 0;

    int response_socket;
    
    struct sockaddr_in response_socket_address;
    memset(&response_socket_address, 0, sizeof(response_socket_address));
    socklen_t addr_size;
    char buffer[BUFFER_SIZE] = {0};

    while (true) {
        response_socket = accept(listen_socket, 
            (struct sockaddr *)&response_socket_address, 
            (socklen_t*)&addr_size);
        pid_t pid = fork();
        if (pid == 0) { //child process: respond to request
            read(response_socket, buffer, BUFFER_SIZE);

            for (int i = 0; buffer[i] != '\0'; i++)
                buffer[i] = toupper(buffer[i]);
            
            send(response_socket, buffer, strlen(buffer), 0);
            
            close(response_socket);
            return 0;
        } else if (pid < 0){
            print_error("could not fork a new process");
        } else {
            printf("%d client(s) connected\n", ++requests_number);
        }
        
    }
    wait(NULL);
    return 0;
}

void print_error(char *e) {
    printf("%s\n", e);
}