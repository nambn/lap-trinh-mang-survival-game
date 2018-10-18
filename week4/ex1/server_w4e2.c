/*
* @Author: nam
* @Date:   2018-10-11 19:16:07
* @Last Modified by:   nam
* @Last Modified time: 2018-10-18 22:56:18
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

// seperate a string to 2 line: numbers and letters
char *seperate(char* buff)
{
	// return NULL if input string is empty
	if (strlen(buff) == 0)
		return NULL;
	
	char numbers[MAX], letters[MAX], *result; 
	int i, i_number = 0, i_letter = 0;
	memset(numbers, '\0', MAX);
	memset(letters, '\0', MAX);
	result = malloc(MAX+1);

	// seperate numbers and letters
	for (i = 0; buff[i] != '\0'; i++)
	{
		if (isdigit(buff[i]))
			numbers[i_number++] = buff[i];
		else if (isalpha(buff[i]))
			letters[i_letter++] = buff[i];
		else
			return NULL;
	}
	sprintf(result, "Numbers: %s\nLetters: %s", numbers, letters);
	return result;
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
	char recv_data[MAX];
	int bytes_sent, bytes_received;
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
  
		printf("You got a connection from %s\n", inet_ntoa(client.sin_addr) ); /* prints client's IP */
		
		//start conversation
		while(1)
		{
			//receives message from client
			bytes_received = recv(conn_sock, recv_data, MAX-1, 0); //blocking
			if (bytes_received <= 0)
			{
				printf("\nConnection closed");
				break;
			}

			// handle received data
			recv_data[bytes_received] = '\0';
			char *reply = seperate(recv_data);
		
			// if string contain symbol return Error
			if (reply == NULL)
				reply = "Error";

			//echo to client
			bytes_sent = send(conn_sock, reply, strlen(reply), 0);
			if (bytes_sent <= 0)
			{
				printf("\nConnection closed");
				break;
			}
			//end conversation
		} 
		close(conn_sock);	
	}
	close(listen_sock);
	return 0;
}
