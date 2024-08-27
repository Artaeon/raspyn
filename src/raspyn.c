#include "raspyn.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#define close closesocket
#else
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#endif

void raspyn_init_endpoint(raspyn_endpoint_t *endpoint, const char *ip, uint16_t port) {
    #ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        perror("WSAStartup failed");
        exit(EXIT_FAILURE);
    }
    #endif

    endpoint->socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (endpoint->socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    endpoint->address.sin_family = AF_INET;
    endpoint->address.sin_addr.s_addr = inet_addr(ip);
    endpoint->address.sin_port = htons(port);
}

void raspyn_send_message(raspyn_endpoint_t *endpoint, raspyn_message_t *message) {
    char buffer[2048];
    size_t length;
    raspyn_serialize_message(message, buffer, &length);
    sendto(endpoint->socket, buffer, length, 0, (struct sockaddr *)&endpoint->address, sizeof(endpoint->address));
}

void raspyn_receive_message(raspyn_endpoint_t *endpoint, raspyn_message_t *message) {
    char buffer[2048];
    socklen_t addr_len = sizeof(endpoint->address);
    int len = recvfrom(endpoint->socket, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&endpoint->address, &addr_len);
    if (len > 0) {
        raspyn_deserialize_message(buffer, len, message);
    }
}

void raspyn_serialize_message(raspyn_message_t *message, char *buffer, size_t *length) {
    uint16_t net_length = htons(message->length);
    uint32_t net_sequence = htonl(message->sequence);
    uint32_t net_timestamp = htonl(message->timestamp);
    uint32_t net_checksum = htonl(message->checksum);

    memcpy(buffer, &message->version, sizeof(message->version));
    memcpy(buffer + sizeof(message->version), &message->type, sizeof(message->type));
    memcpy(buffer + sizeof(message->version) + sizeof(message->type), &net_length, sizeof(net_length));
    memcpy(buffer + sizeof(message->version) + sizeof(message->type) + sizeof(net_length), &net_sequence, sizeof(net_sequence));
    memcpy(buffer + sizeof(message->version) + sizeof(message->type) + sizeof(net_length) + sizeof(net_sequence), &net_timestamp, sizeof(net_timestamp));
    memcpy(buffer + sizeof(message->version) + sizeof(message->type) + sizeof(net_length) + sizeof(net_sequence) + sizeof(net_timestamp), &net_checksum, sizeof(net_checksum));
    memcpy(buffer + sizeof(message->version) + sizeof(message->type) + sizeof(net_length) + sizeof(net_sequence) + sizeof(net_timestamp) + sizeof(net_checksum), &message->reserved, sizeof(message->reserved));
    memcpy(buffer + sizeof(message->version) + sizeof(message->type) + sizeof(net_length) + sizeof(net_sequence) + sizeof(net_timestamp) + sizeof(net_checksum) + sizeof(message->reserved), message->payload, message->length);

    *length = sizeof(message->version) + sizeof(message->type) + sizeof(net_length) + sizeof(net_sequence) + sizeof(net_timestamp) + sizeof(net_checksum) + sizeof(message->reserved) + message->length;
}

void raspyn_deserialize_message(char *buffer, size_t length, raspyn_message_t *message) {
    uint16_t net_length;
    uint32_t net_sequence;
    uint32_t net_timestamp;
    uint32_t net_checksum;

    memcpy(&message->version, buffer, sizeof(message->version));
    memcpy(&message->type, buffer + sizeof(message->version), sizeof(message->type));
    memcpy(&net_length, buffer + sizeof(message->version) + sizeof(message->type), sizeof(net_length));
    memcpy(&net_sequence, buffer + sizeof(message->version) + sizeof(message->type) + sizeof(net_length), sizeof(net_sequence));
    memcpy(&net_timestamp, buffer + sizeof(message->version) + sizeof(message->type) + sizeof(net_length) + sizeof(net_sequence), sizeof(net_timestamp));
    memcpy(&net_checksum, buffer + sizeof(message->version) + sizeof(message->type) + sizeof(net_length) + sizeof(net_sequence) + sizeof(net_timestamp), sizeof(net_checksum));
    memcpy(&message->reserved, buffer + sizeof(message->version) + sizeof(message->type) + sizeof(net_length) + sizeof(net_sequence) + sizeof(net_timestamp) + sizeof(net_checksum), sizeof(message->reserved));

    message->length = ntohs(net_length);
    message->sequence = ntohl(net_sequence);
    message->timestamp = ntohl(net_timestamp);
    message->checksum = ntohl(net_checksum);
    memcpy(message->payload, buffer + sizeof(message->version) + sizeof(message->type) + sizeof(net_length) + sizeof(net_sequence) + sizeof(net_timestamp) + sizeof(net_checksum) + sizeof(message->reserved), message->length);
    message->payload[message->length] = '\0';
}

uint32_t raspyn_calculate_checksum(raspyn_message_t *message) {
    uint32_t checksum = 0;
    for (size_t i = 0; i < sizeof(message->version) + sizeof(message->type) + sizeof(message->length) + sizeof(message->sequence) + sizeof(message->timestamp) + sizeof(message->reserved) + message->length; i++) {
        checksum += ((unsigned char *)message)[i];
    }
    return checksum;
}

int raspyn_verify_checksum(raspyn_message_t *message) {
    uint32_t calculated_checksum = raspyn_calculate_checksum(message);
    return calculated_checksum == message->checksum;
}