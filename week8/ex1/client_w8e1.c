#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#define BUFF_SIZE 1024

int main(int argc, char const *argv[])
{
	struct sockaddr_in server_addr;
	socklen_t sin_size;
	int sockfd;
	int bytes_sent;
	int bytes_received;
	char buff[BUFF_SIZE];

	// valid number of argument
	if (argc != 3)
	{
		printf("Usage: ./client IPAddress PortNumber\n\n");
		return 0;
	}

	// Construct a TCP socket
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("\nsocket()");
		exit(0);
	}

	// Define the address of the server
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[2]));
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);

	// Request to connect server
	if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
	{
		puts("\nCan not connect to sever!");
		exit(0);
	}

	while (1)
	{
		// Communicate with server
		printf("\nInsert string to send (ENTER to quit): ");
		fgets(buff, BUFF_SIZE, stdin);

		// exit if user only input ENTER
		if (buff[0] == '\n')
			break;

		// replace \n with \0
		buff[strcspn(buff, "\n")] = '\0';

		// send to server
		sin_size = sizeof(struct sockaddr);
		if (send(sockfd, buff, strlen(buff), 0) == -1)
		{
			perror("\nsend()");
			break;
		}

		// receive reply
		bytes_received = recv(sockfd, buff, sizeof(buff), 0);
		if (bytes_received <= 0)
		{
			puts("\nCannot receive data from sever!\n");
			break;
		}

		// print reply
		buff[bytes_received] = '\0';
		printf("Reply from server:\n%s\n", buff);
	}

	close(sockfd);
	return 0;
}