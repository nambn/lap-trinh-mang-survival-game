/*
 * @Author: nam 
 * @Date: 2018-11-21 10:30:31 
 * @Last Modified by: nam
 * @Last Modified time: 2018-11-21 11:31:02
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
#include <fcntl.h>

#define MAX 1024
#define BACKLOG 10

int seperate(char *buff, char *letters, char *numbers)
{
	// return NULL if input string is empty
	if (strlen(buff) == 0)
		return 0;

	int i, i_number = 0, i_letter = 0;
	numbers = malloc(MAX + 1);
	letters = malloc(MAX + 1);

	// seperate numbers and letters
	for (i = 0; buff[i] != '\0'; i++)
		if (isdigit(buff[i]))
			numbers[i_number++] = buff[i];
		else if (isalpha(buff[i]))
			letters[i_letter++] = buff[i];
		else
			return 0;
	return 1;
}

int main(int argc, char const *argv[])
{
	struct sockaddr_in server;
	struct sockaddr_in client;
	socklen_t sin_size;
	char buff[MAX + 1];
	char *letters;
	char *numbers;
	char *reply = malloc(MAX + 1);
	int sockfd;
	int newfd;
	int lastfd;
	int bytes_sent;
	int bytes_received;
	int i;

	// valid number of argument
	if (argc != 2)
	{
		printf("Usage: ./server PortNumber\n\n");
		return 0;
	}

	// construct TCP socket
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("\nsocket()");
		exit(0);
	}

	lastfd = sockfd;

	// bind address to socket
	server.sin_family = AF_INET;
	server.sin_port = htons(atoi(argv[1]));
	server.sin_addr.s_addr = INADDR_ANY;
	memset(server.sin_zero, '\0', sizeof server.sin_zero);

	if (bind(sockfd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
	{
		perror("\nbind() -  ");
		exit(0);
	}

	if (listen(sockfd, BACKLOG) == -1)
	{
		perror("\nlisten()");
		exit(0);
	}

	puts("\nServer up and running...\n");

	if ((newfd = accept(sockfd, (struct sockaddr *)&server, &sin_size)) == -1)
		perror("\naccept()");

	// Change the socket into non-blocking state
	fcntl(lastfd, F_SETFL, O_NONBLOCK);
	fcntl(newfd, F_SETFL, O_NONBLOCK);

	// communicate with client
	while (1)
	{
		for (i = sockfd; i <= lastfd; i++)
		{
			printf("Round number %d\n", i);
			if (i = sockfd)
			{
				sin_size = sizeof(struct sockaddr_in);
				if ((newfd = accept(sockfd, (struct sockaddr *)&server, &sin_size)) == -1)
					perror("\naccept()");
				printf("server: got connection from %s\n", inet_ntoa(client.sin_addr));
				fcntl(newfd, F_SETFL, O_NONBLOCK);
				lastfd = newfd;
			}
			else
			{
				bytes_received = recv(newfd, buff, sizeof(buff), 0);

				if (bytes_received <= 0)
				{
					perror("\nrecv()");
					printf("Round %d - bytes_received: %d \n", i, bytes_received);
				}
				else
				{
					buff[bytes_received] = '\0';

					// try to seperate numbers and letters,
					// send "Error" if string empty or contain special character
					if (!seperate(buff, letters, numbers))
					{
						if (send(newfd, "Error", 5, 0) == -1)
							perror("\nsend()");
					}
					else
					{
						puts(buff);
						// return result to client
						if (send(newfd, buff, strlen(reply), 0) == -1)
							perror("\nsend()");
					}
				}
			}
		}
	}

	return 0;
}