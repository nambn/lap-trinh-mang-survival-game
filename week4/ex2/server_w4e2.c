/*
* @Author: nam
* @Date:   2018-10-11 19:16:07
* @Last Modified by:   nam
* @Last Modified time: 2018-10-12 23:35:25
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BACKLOG 5
#define MAX 1024

// Create a new file in foler 'receive/' with name, size & content
// Return:
// "Successful transfering" if file created
// "Error: File is exitent on server" if file existed
// NULL if others
char *create_file(char* file_name, int file_size_in_int, char* file_content)
{
    FILE *f;

    // true path to file
    char file_uri[strlen(file_name) + 9];
    sprintf(file_uri, "receive/%s", file_name);

    // check if file exist
    if (access(file_uri, F_OK) != -1)
        return "Error: File is exitent on server";

    // create file
    if (!(f = fopen(file_uri, "wb")))
        return NULL;

    // copy content to file
    if (fwrite(file_content, 1, file_size_in_int, f) == 0)
        return NULL;

    return "Successful transfering";
    fclose(f);
}

int main(int argc, char const *argv[])
{
    // valid number of argument
    if (argc != 2)
    {
        printf("Usage: ./server PortNumber\n\n");
        return 0;
    }

    int listen_sock, conn_sock;
    char file_name[MAX], file_size[MAX], *reply;
    int bytes_sent, bytes_received, file_size_in_int;
    struct sockaddr_in server;
    struct sockaddr_in client;
    socklen_t sin_size;
    
    // Construct a TCP socket to listen connection request
    if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){
        perror("\nError: ");
        return 0;
    }
    
    // Bind address to socket
    memset(&server, '\0', sizeof server);
    server.sin_family = AF_INET;         
    server.sin_port = htons(atoi(argv[1]));
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(listen_sock, (struct sockaddr*)&server, sizeof(server))==-1){
        perror("\nError: ");
        return 0;
    }     
    
    // Listen request from client
    if(listen(listen_sock, BACKLOG) == -1){
        perror("\nError: ");
        return 0;
    }
    
    puts("Server up and running...\n");

    // Communicate with client
    while(1) 
    {
        //accept request
        sin_size = sizeof(struct sockaddr_in);
        if ((conn_sock = accept(listen_sock,( struct sockaddr *)&client, &sin_size)) == -1) 
            perror("\nError: ");
  
        printf("You got a connection from %s\n", inet_ntoa(client.sin_addr) );
        
        //start conversation
        while(1)
        {
            // receives file name from client
            if (0 >= (bytes_received = recv(conn_sock, file_name, MAX-1, 0))) {
                puts("Client disconnect\n");
                break;
            }
            file_name[bytes_received] = '\0';
            printf("Received file name: %s\n", file_name);

            // receive file size from client
            if (0 >= (bytes_received = recv(conn_sock, file_size, MAX-1, 0))) {
                puts("Client disconnect\n");
                break;
            }
            file_size[bytes_received] = '\0';
            file_size_in_int = atoi(file_size);
            printf("Received file size: %d byte(s)\n", file_size_in_int);

            // receive file content from client
            char file_content[file_size_in_int + 1];
            if (0 >= (bytes_received = recv(conn_sock, file_content, file_size_in_int, 0))) {
                puts("Client disconnect\n");
                break;
            }
            file_content[bytes_received] = '\0';
            puts("Received file content\n");

            // create file with received name in folder 'receive/'
            reply = create_file (file_name, file_size_in_int, file_content);
            if (!reply)
                reply = "Error: File transfering is interupted";

            // reply to client
            bytes_sent = send(conn_sock, reply, strlen(reply), 0);
            if (bytes_sent <= 0)
                puts("Client disconnect\n");

            // end conversation
        } 
        close(conn_sock);
    }
    close(listen_sock);
    return 0;
}