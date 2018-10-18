/*
* @Author: nam
* @Date:   2018-10-11 19:16:27
* @Last Modified by:   nam
* @Last Modified time: 2018-10-11 21:31:10
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

	int client_sock;
	char buff[MAX];
	struct sockaddr_in server_addr;
	int bytes_sent, bytes_received;
	
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

		//send message
		printf("\nInsert string to send (ENTER to quit): ");
		fgets(buff, MAX, stdin);

		// exit if user only input ENTER
		if (buff[0] == '\n') break;

		// replace \n with \0
		buff[strcspn(buff, "\n")] = '\0';

		bytes_sent = send(client_sock, buff, strlen(buff), 0);
		if(bytes_sent <= 0){
			printf("\nConnection closed!\n");
			break;
		}
		
		// receive echo reply
		bytes_received = recv(client_sock, buff, MAX-1, 0);
		if(bytes_received <= 0){
			printf("\nError!Cannot receive data from sever!\n");
			break;
		}
		
		// print reply
		buff[bytes_received] = '\0';
		puts("Reply from server:");
		puts(buff);
	}
	
	// Close socket
	close(client_sock);
	return 0;
}
