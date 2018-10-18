/*
 * @Author: nam 
 * @Date: 2018-11-02 13:02:40 
 * @Last Modified by: nam
 * @Last Modified time: 2018-11-02 19:45:14
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "myfunc.h"

#define MAX 1024

int main(int argc, char const *argv[])
{
	// valid number of argument
	if (argc != 3)
	{
		printf("Usage: ./client IPAddress PortNumber\n\n");
		return 0;
	}

	message_t message;
	int key, client_sock, request;
	char filename[MAX], buff[MAX + 3], payload[MAX];
	struct sockaddr_in server_addr;
	int bytes_sent, bytes_received;
	char file_buff[100], new_file_name[MAX];
	int file_read_byte;
	FILE *f_send, *f_recv;

	// Construct socket
	client_sock = socket(AF_INET, SOCK_STREAM, 0);

	// Specify server address
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[2]));
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);

	// Request to connect server
	if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0)
	{
		printf("\nError!Can not connect to sever! Client exit imediately! ");
		return 0;
	}

	// Communicate with server

	// show menu
	printf(
		"\n---------------------------------------------\n"
		"FILE ENCODE/DECODE\n"
		"---------------------------------------------\n"
		"1. Encode\n"
		"2. Decode\n"
		"Your choice (1-2, other to quit): ");

	// user choice of request
	scanf("%d", &request);
	getchar();
	if (request != 1 && request != 2)
		return 0;
	request--;

	// user input file to open
	printf("File path: ");
	scanf("%[^\n]%*c", filename);
	if ((f_send = fopen(filename, "rb")) == NULL)
	{
		printf("Can not open file \"%s\"!\n", filename);
		return 0;
	}

	// user input key
	while (1)
	{
		printf("Key: ");
		scanf("%d", &key);
		getchar();
		if (key < 0 || key > 25)
		{
			puts("Key must be a number between 0 and 25!");
			continue;
		}
		break;
	}

	// print key to payload
	sprintf(payload, "%d", key);

	// send encode/decode request and key to server
	message = new_message(request, 0, payload);
	if (0 == mysend(client_sock, &message))
	{
		printf("Connection close!\n");
		return 0;
	}

	// send file to server //////////////////////////////////
	while (0 < (file_read_byte = fread(file_buff, 1, 99, f_send)))
	{
		message = new_message(2, file_read_byte, file_buff);
		if (0 == mysend(client_sock, &message))
		{
			printf("Connection close!\n");
			return 0;
		}
		memset(&file_buff, '\0', 100);

		// delay between rapid send
		if (0 >= (bytes_received = recv(client_sock, buff, 1, 0)))
		{
			printf("Connection close!\n");
			return 0;
		}
	}
	fclose(f_send);

	// send read file ending
	message = new_message(2, 0, "");
	mysend(client_sock, &message);

	printf("\nFile sent to server successfully.\n\n");

	// create file for server response
	strcpy(new_file_name, "server_result.");
	strcat(new_file_name, get_file_ext(filename));

	if (NULL == (f_recv = fopen(new_file_name, "wb")))
	{
		printf("Can not open file %s\n", new_file_name);
		exit(0);
	}

	// get file content from server /////////////////////////
	while (1)
	{
		if (0 == myrecv(client_sock, &message))
		{
			printf("Connection close!\n");
			break;
		}

		// check file sending status
		if (message.opcode == 2 && message.length > 0)
			fwrite(message.payload, message.length, 1, f_recv);
		else if (message.opcode == 2 && message.length == 0)
		{
			printf("\nSave result from server as \"%s\"\n\n", new_file_name);
			break;
		}
		else
		{
			puts("Server error!\n");
			return 0;
		}

		// delay between rapid recv
		if (0 >= (bytes_sent = send(client_sock, "1", 1, 0)))
		{
			puts("Connection close!\n");
			return 0;
		}
	}
	fclose(f_recv);

	// Close socket
	close(client_sock);
	return 0;
}
