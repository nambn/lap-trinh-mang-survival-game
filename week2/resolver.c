/*
* @Author: nam
* @Date:   2018-09-24 20:53:13
* @Last Modified by:   nam
* @Last Modified time: 2018-09-26 23:18:03
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>

// output when resolve unsuccessful
void print_error_and_out()
{
	printf("Not found infomation.\n\n");
	exit(0);
}

// handle input of ipv4 address
// ipaddr: pointer of struct in_addr
void ip_handle(struct in_addr *ipaddr)
{
	// try to get host by ipaddr
	struct hostent *host = gethostbyaddr(ipaddr, 4, AF_INET);
	int i;

	if (host)
	{
		// offical name
		printf("Official name: \n");
		if (host->h_name)
			printf("	%s\n", host->h_name);
		else
			printf("	(none)\n");

		printf("Alias name: \n");
		// if alias name not exist, print (none)
		if (!*host->h_aliases)
			printf("	(none)\n");
		else
			// if alias name exist, print all of them
			for (i = 0; host->h_aliases[i]; i++)
				printf("	%s\n", host->h_aliases[i]);
	}
	else
		// if host for ip cannot be found
		print_error_and_out();
}

// handle case when input not of valid ipv4
// hostname: string
void domain_handle(char *hostname)
{
	// try to get host by server name
	struct hostent *host = gethostbyname(hostname);
	struct in_addr **addr_list;
	int i;


	if (host)
	{
		// get ip list from hostname
		printf("IP list:\n");
		if (!*host->h_addr_list)
			printf("	(none)\n");
		else
		{
			addr_list = (struct in_addr **)host->h_addr_list;
			for (i = 0; addr_list[i]; i++)
				printf("	%s\n", inet_ntoa(*addr_list[i]));
		}
	}
	else
		// if domain name is invalid
		print_error_and_out();
}

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		printf("Wrong number of arguments.\n");
		printf("Usage: ./resolver (IPv4 or domain name)\n\n");
		return 0;
	}

	struct sockaddr_in sa;
	// validation user's input
	if (inet_pton(AF_INET, argv[1], &sa.sin_addr))
		// if valid ipv4
		ip_handle(&sa.sin_addr);
	else
		// if not valid ipv4
		domain_handle(argv[1]);
	
	return 0;
}