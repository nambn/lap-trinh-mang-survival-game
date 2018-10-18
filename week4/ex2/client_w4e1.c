/*
* @Author: nam
* @Date:   2018-10-11 19:16:27
* @Last Modified by:   nam
* @Last Modified time: 2018-10-12 23:34:05
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX 1024

int main (int argc, char const *argv[])
{
    // valid number of argument
    if (argc != 3)
    {
        printf("Usage: ./client IPAddress PortNumber\n\n");
        return 0;
    }

    int client_sock, file_size;
    char file_name[MAX], *buff, file_size_in_string[MAX];
    struct sockaddr_in server_addr;
    int bytes_sent, bytes_received;
    FILE *f;

    // Construct socket
    client_sock = socket(AF_INET,SOCK_STREAM,0);
    
    // Specify server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    
    // Request to connect server
    if(connect(client_sock, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) < 0){
        printf("\nError!Can not connect to sever! Client exit imediately! ");
        return 0;
    }
        
    // Communicate with server          
    while(1){

        // get file from user
        printf("\nInsert file path to send (ENTER to quit): ");
        fgets(file_name, MAX, stdin);

        // exit if user only input ENTER
        if (file_name[0] == '\n') break;

        // replace \n with \0 in user input
        file_name[strcspn(file_name, "\n")] = '\0';

        // open file
        if (!(f = fopen(file_name, "rb"))) {
            puts("Error: file not found");
            continue;
        }

        // get file size
        fseek (f, 0, SEEK_END);
        file_size = ftell(f);
        rewind(f);

        // copy file content to buffer
        if (!(buff = (char*) malloc(file_size))) {
            perror("File buffer malloc failed");
            return 0;
        }
        fread(buff, file_size, 1, f);
        fclose(f);

        // send file name
        bytes_sent = send(client_sock, file_name, strlen(file_name), 0);
        if(bytes_sent <= 0){
            perror("Error: Send file name failed");
            break;
        }
        sleep(1);

        // send file size
        sprintf(file_size_in_string, "%d", file_size);

        bytes_sent = send(client_sock, file_size_in_string, strlen(file_size_in_string), 0);
        if(bytes_sent <= 0){
            perror("Error: Send file name failed");
            break;
        }
        sleep(1);

        // send file content
        bytes_sent = send(client_sock, buff, strlen(buff), 0);
        if(bytes_sent <= 0){
            puts("Error: File transfering is interupted");
            break;
        }
        sleep(1);

        // receive echo reply
        bytes_received = recv(client_sock, buff, MAX-1, 0);
        if(bytes_received <= 0){
            printf("\nError!Cannot receive data from sever!\n");
            break;
        }
        
        // print reply
        buff[bytes_received] = '\0';
        printf("Reply from server: $s\n", buff);
    }
    
    // Close socket
    close(client_sock);
    return 0;
}
