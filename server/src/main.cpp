#include "server.h"
#include "user_db_handle.h"

int main(int argc, char* argv[]) {
    configure_databases();

    if (argc <= 1) {
        Server("127.0.0.1", 42069).run();
    } else {
        const char* ip = argv[1];
        int port = atoi(argv[2]);
        
        Server(ip, port).run();
    }
}