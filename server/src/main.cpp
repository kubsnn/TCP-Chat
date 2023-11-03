#define LOG_TO_FILE 0
#include "logger.h"
#include "server.h"
#include "core/database/user_db_handle.h"

int main(int argc, char* argv[]) {
    configure_databases();

    if (argc != 3) {
        Server("0.0.0.0", 42069).run();
    } else {
        const char* ip = argv[1];
        int port = atoi(argv[2]);

        Server(ip, port).run();
    }
}