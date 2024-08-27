#include "raspyn.h"
#include <assert.h>

void test_example() {
    // Example test case
    raspyn_client_t client;
    raspyn_init_client(&client, "127.0.0.1", 12345);
    raspyn_send_message(&client, "Test message");
    raspyn_receive_message(&client);
    // Add assertions to validate behavior
}

int main() {
    test_example();
    return 0;
}