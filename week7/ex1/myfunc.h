#ifndef MYFUNC_H_
#define MYFUNC_H_

#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX 1024

// message struct opcode + length + payload
typedef struct
{
    int opcode;
    int length;
    char payload[MAX];
} message_t;

// custom sending function, provide a socket and a message_t pointer
// return 0 if error, 1 if succesfully
int mysend(int sock, message_t *mess)
{
    char buff[MAX + 3];
    int bytes_sent;
    sprintf(buff, "%1d%02d%s", mess->opcode, mess->length, mess->payload);
    if (0 >= (bytes_sent = send(sock, buff, strlen(buff), 0)))
        return 0;
    printf(">>>>>send %s\n", buff); // uncomment for debug
    return 1;
}

// custom recv function, provide a socket and a message_t pointer
// return 0 if error, 1 if succesfully
int myrecv(int sock, message_t *mess)
{
    memset(mess, '\0', sizeof(message_t));
    char buff[MAX + 3];
    int bytes_received;
    if (0 >= (bytes_received = recv(sock, buff, MAX - 1, 0)))
        return 0;
    buff[bytes_received] = '\0';
    sscanf(buff, "%1d%02d%*s", &mess->opcode, &mess->length);
    strcpy(mess->payload, &buff[3]);
    printf(">>>>>recv %s\n", buff); // uncomment for debug
    return 1;
}

// for create new message_t
message_t new_message(int opcode, int length, char *payload)
{
    message_t mess;
    mess.opcode = opcode;
    mess.length = length;
    strcpy(mess.payload, payload);
    return mess;
}

// print out content for a message, for debuggin'
void printout(message_t message)
{
    printf("\n>>>>>opcode %d\n"
           ">>>>>length %d\n"
           ">>>>>payload %s\n\n",
           message.opcode, message.length, message.payload);
}

// endcode file "temp_recv" with key(int), save result to "temp_send"
int ceasar_encode_the_temp_file(int key)
{
    FILE *f_recv, *f_send;
    char c;
    if (NULL == (f_recv = fopen("temp_recv", "rb")))
    {
        printf("Can not open file temp_recv");
        return 0;
    }

    if (NULL == (f_send = fopen("temp_send", "wb")))
    {
        printf("Can not open file temp_send");
        return 0;
    }

    while (EOF != (c = fgetc(f_recv)))
    {
        if (c >= 'A' && c <= 'Z')
        {
            c = c - key;
            if (c < 'A')
                c += 26;
        }
        else if (c >= 'a' && c <= 'z')
        {
            c = c - key;
            if (c < 'a')
                c += 26;
        }
        fprintf(f_send, "%c", c);
    }
    fclose(f_send);
    return 1;
}

// decode file "temp_recv" with key(int), save result to "temp_send"
int ceasar_decode_the_temp_file(int key)
{
    FILE *f_recv, *f_send;
    char c;
    if (NULL == (f_recv = fopen("temp_recv", "rb")))
    {
        printf("Can not open file temp_recv");
        return 0;
    }

    if (NULL == (f_send = fopen("temp_send", "wb")))
    {
        printf("Can not open file temp_send");
        return 0;
    }

    while (EOF != (c = fgetc(f_recv)))
    {
        if (c >= 'A' && c <= 'Z')
        {
            c = c + key;
            if (c > 'Z')
                c -= 26;
        }
        else if (c >= 'a' && c <= 'z')
        {
            c = c + key;
            if (c > 'z')
                c -= 26;
        }
        fprintf(f_send, "%c", c);
    }
    fclose(f_send);
    return 1;
}

// to get file extension of a string
char *get_file_ext(char *filename)
{
    char *dot = strrchr(filename, '.');
    if (!dot || dot == filename)
        return "";
    return dot + 1;
}

#endif