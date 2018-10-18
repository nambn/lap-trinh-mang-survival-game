/*
* @Author: nam
* @Date:   2018-10-18 12:50:55
 * @Last Modified by: nam
 * @Last Modified time: 2018-11-02 19:45:27
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <arpa/inet.h>
#include <errno.h>
#include "myfunc.h"

#define BACKLOG 5
#define MAX 1024
#define TIMEOUT 5 * 60 * 1000 // 5 min

int main(int argc, char const *argv[])
{
    // valid number of argument
    if (argc != 2)
    {
        printf("Usage: ./server PortNumber\n\n");
        return 0;
    }

    message_t message;
    FILE *f_recv, *f_send;
    int listen_sock, conn_sock, request, key;
    char file_buff[100], buff[MAX];
    int bytes_sent, bytes_received;
    struct sockaddr_in server;
    struct sockaddr_in client;
    socklen_t sin_size;
    int pid, file_read_byte, working;
    struct pollfd fds[200];
    int nfds = 1, new_sd, i, one = 1;

    // Construct a TCP socket to listen connection request
    if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("\nsocket() error: ");
        return 0;
    }

    // reuse listen_sock
    if (0 > setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, (char *)&one, sizeof(1)))
    {
        perror("\nsetsockopt() error: ");
        return 0;
    }

    // nonblocking listen_sock
    if (0 > ioctl(listen_sock, FIONBIO, (char *)&one))
    {
        perror("\nError: ");
        return 0;
    }

    // Bind address to socket
    memset(&server, '\0', sizeof server);
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[1]));
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(listen_sock, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        perror("\nbind error: ");
        return 0;
    }

    // Listen request from client
    if (listen(listen_sock, BACKLOG) == -1)
    {
        perror("\nlisten error: ");
        return 0;
    }

    // initialize poll
    memset(fds, 0, sizeof(fds));
    fds[0].fd = listen_sock;
    fds[0].events = POLLIN;

    puts("Server up and running...\n");

    while (1)
    {
        // call poll in 5 min
        if (0 >= poll(fds, nfds, TIMEOUT))
        {
            perror("\nError/Timeout: ");
            break;
        }

        int i, current_size = nfds;
        for (i = 0; i < current_size; i++)
        {
            // loop to find which fds return polling
            if (fds[i].revents == 0)
                continue;

            // found but not polling, end programs
            if (fds[i].revents != POLLIN)
            {
                perror("\nError: ");
                break;
            }

            // if polling, listen to each connection
            if (fds[i].fd == listen_sock)
            {
                while (1)
                {
                    if (0 > (new_sd = accept(listen_sock, NULL, NULL)))
                    {
                        perror("\nError: ");
                        break;
                    }

                    printf("New connection from %d\n", new_sd);
                    fds[nfds].fd = new_sd;
                    fds[nfds].events = POLLIN;
                    nfds++;
                }
            }

            // Communicate with client
            // receive request from client
            if (0 == myrecv(fds[i].fd, &message))
            {
                printf("Connection close!\n");
                break;
            }

            // request = 0 => encode
            // request = 1 => decode
            request = message.opcode;
            key = atoi(message.payload);

            // create temp file for client file
            if (NULL == (f_recv = fopen("temp_recv", "wb")))
            {
                printf("Can not open file temp_recv");
                exit(0);
            }

            // get file content from client /////////////////////////
            while (1)
            {
                if (0 == myrecv(fds[i].fd, &message))
                {
                    printf("Connection close!\n");
                    break;
                }

                // check file sending status
                if (message.opcode == 2 && message.length > 0)
                    fwrite(message.payload, message.length, 1, f_recv);
                else if (message.opcode == 2 && message.length == 0)
                {
                    puts("File received successfully");
                    break;
                }
                else
                {
                    puts("Connection close!\n");
                    return 0;
                }

                // delay between rapid recv
                if (0 >= (bytes_sent = send(fds[i].fd, "1", 1, 0)))
                {
                    puts("Connection close!\n");
                    return 0;
                }
            }
            fclose(f_recv);

            // encode/decode as client request
            if (request == 0)
                working = ceasar_encode_the_temp_file(key);
            else if (request == 1)
                working = ceasar_decode_the_temp_file(key);
            else
            {
                printf("%d", request);
                exit(0);
            }

            // send opcode = 3 to client if errors
            if (!working)
            {
                message = new_message(3, 0, "");
                mysend(fds[i].fd, &message);
            }

            // send file to client
            if (NULL == (f_send = fopen("temp_send", "rb")))
            {
                printf("Can not open file temp_send");
                exit(0);
            }

            // send file to client ///////////////////////////////////
            while (0 < (file_read_byte = fread(file_buff, 1, 99, f_send)))
            {
                message = new_message(2, file_read_byte, file_buff);
                if (0 == mysend(fds[i].fd, &message))
                {
                    printf("Connection close!\n");
                    return 0;
                }
                memset(&file_buff, '\0', 100);

                // delay between rapid send
                if (0 >= (bytes_received = recv(fds[i].fd, buff, 1, 0)))
                {
                    printf("Connection close!\n");
                    return 0;
                }
            }
            fclose(f_send);

            // send read file ending
            message = new_message(2, 0, "");
            mysend(fds[i].fd, &message);

            remove("temp_send");
            remove("temp_recv");
        }
    }

    // clean up
    for (i = 0; i < nfds; i++)
    {
        if (fds[i].fd >= 0)
            close(fds[i].fd);
    }

    return 0;
}
