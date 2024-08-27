#include "raspyn.h"

int main() {
    raspyn_server_t server;
    raspyn_init_server(&server, "127.0.0.1", 12345);
    raspyn_run_server(&server);
    return 0;
}