#include "raspyn.h"

int main() {
    raspyn_client_t client;
    raspyn_init_client(&client, "127.0.0.1", 12345);
    raspyn_send_message(&client, "Hello, Raspyn!");
    raspyn_receive_message(&client);
    return 0;
}