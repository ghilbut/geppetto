#include "http/http_server.h"


int main(void) {

    Http::Server server;
    server.Start("", 80);
    for (;;) {
    }
    server.Stop();

    return 0;
}