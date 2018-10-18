/*
* @Author: nam
* @Date:   2018-09-30 17:16:36
* @Last Modified by:   nam
* @Last Modified time: 2018-10-05 23:21:33
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
#include <arpa/inet.h>
#include <netdb.h>

#define MAX 1024

// output when resolve unsuccessful
char* print_error_and_out()
{
	return "Not found infomation.\n\n" ;
}

// handle input of ipv4 address
// ipaddr: pointer of struct in_addr
char *ip_handle(struct in_addr *ipaddr)
{
	// try to get host by ipaddr
	struct hostent *host = gethostbyaddr(ipaddr, 4, AF_INET);
	int i;
	char *buff = malloc(MAX);

	if (host)
	{
		// offical name
		snprintf(buff, MAX, "Official name: \n");
		if (host->h_name)
			snprintf(buff + strlen(buff), MAX, "\t%s\n", host->h_name);
		else
			snprintf(buff + strlen(buff), MAX, "\t(none)\n");

		// alias name
		snprintf(buff + strlen(buff), MAX, "Alias name: \n");

		// if alias name not exist, print (none)
		if (!*host->h_aliases)
			snprintf(buff + strlen(buff), MAX, "\t(none)\n");
		else
			// if alias name exist, print all of them
			for (i = 0; host->h_aliases[i]; i++)
				snprintf(buff + strlen(buff), MAX, "\t%s\n", host->h_aliases[i]);
		return buff;
	}
	else
		// if host for ip cannot be found
		return print_error_and_out();
}

// handle case when input not of valid ipv4
// hostname: string
char *domain_handle(char *hostname)
{
	// try to get host by server name
	struct hostent *host = gethostbyname(hostname);
	struct in_addr **addr_list;
	int i;
	char *buff = malloc(MAX);

	if (host)
	{
		// get ip list from hostname
		snprintf(buff, MAX, "IP list:\n");
		// if no ip found
		if (!*host->h_addr_list)
			snprintf(buff + strlen(buff), MAX, "\t(none)\n");
		else
		{
			// print all ip if found
			addr_list = (struct in_addr **)host->h_addr_list;
			for (i = 0; addr_list[i]; i++)
				snprintf(buff + strlen(buff), MAX, "\t%s\n", inet_ntoa(*addr_list[i]));
		}
		return buff;
	}
	else
		// if domain name is invalid
		return print_error_and_out();
}

char *analyze(char* buff)
{
	struct sockaddr_in sa;
	char *output;

	// validation user's input
	if (inet_pton(AF_INET, buff, &sa.sin_addr))
		// if valid ipv4
		output = ip_handle(&sa.sin_addr);
	else
		// if not valid ipv4
		output = domain_handle(buff);
	return output;
}

int main(int argc, char const *argv[])
{
	int server_sock;
	socklen_t sin_size;
	char buff[MAX+1];
	int bytes_sent, bytes_received;
	struct sockaddr_in server;
	struct sockaddr_in client;

	// valid number of argument
	if (argc != 2)
	{
		printf("Usage: ./server PortNumber\n\n");
		return 0;
	}

	// construct UDP socket
	if ((server_sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		perror("\nError: ");
		exit(0);
	}

	// bind address to socket
	server.sin_family = AF_INET;
	server.sin_port = htons(atoi(argv[1]));
	server.sin_addr.s_addr = INADDR_ANY;
	memset(server.sin_zero, '\0', sizeof server.sin_zero);

	if (bind(server_sock, (struct sockaddr*)&server, sizeof(struct sockaddr)) == -1)
	{
		perror("\nError: ");
		exit(0);
	}

	// communicate with client
	while(1)
	{
		// get string from client
		sin_size = sizeof(struct sockaddr_in);
		bytes_received = recvfrom(server_sock, buff, MAX-1, 0, (struct sockaddr*)&client, &sin_size);

		if (bytes_received < 0)
			perror("\nError: ");
		else
		{
			// handle string from client
			buff[bytes_received] = '\0';

			char *reply = analyze(buff);

			// return Error if reply return NULL
			if (reply == NULL)
				reply = "Error";

			// return result to client
			bytes_sent = sendto(server_sock, reply, strlen(reply), 0, (struct sockaddr*)&client, sin_size);
			if (bytes_sent < 0)
				perror("\nError: ");
		}
	}

	close(server_sock);
	return 0;
}
