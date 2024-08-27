#ifndef RASPYN_H
#define RASPYN_H

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#endif

#include <stdint.h>
#include <stddef.h>

#define MAX_PAYLOAD_SIZE 1024

typedef struct {
    uint8_t version;
    uint8_t type;
    uint16_t length;
    uint32_t sequence;
    uint32_t timestamp;
    uint32_t checksum;
    uint8_t reserved[2];
    char payload[MAX_PAYLOAD_SIZE];
} raspyn_message_t;

typedef struct {
    int socket;
    struct sockaddr_in address;
} raspyn_endpoint_t;

void raspyn_init_endpoint(raspyn_endpoint_t *endpoint, const char *ip, uint16_t port);
void raspyn_send_message(raspyn_endpoint_t *endpoint, raspyn_message_t *message);
void raspyn_receive_message(raspyn_endpoint_t *endpoint, raspyn_message_t *message);

void raspyn_serialize_message(raspyn_message_t *message, char *buffer, size_t *length);
void raspyn_deserialize_message(char *buffer, size_t length, raspyn_message_t *message);

void raspyn_encrypt_message(raspyn_message_t *message, const char *key);
void raspyn_decrypt_message(raspyn_message_t *message, const char *key);

uint32_t raspyn_calculate_checksum(raspyn_message_t *message);
int raspyn_verify_checksum(raspyn_message_t *message);

#endif // RASPYN_H