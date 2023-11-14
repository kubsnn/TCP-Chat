#define LOG_TO_FILE 0
#include "logger.h"
#include "server.h"
#include "core/database/user_db_handle.h"
#include "core/utils/crypto.h"

#include <openssl/ssl.h>

int main(int argc, char* argv[]) {
    configure_databases();
    OpenSSL_add_all_algorithms();

    if (argc <= 1) {
        Server("0.0.0.0", 42069).run();
    } else {
        const char* ip = argv[1];
        int port = atoi(argv[2]);

        Server(ip, port).run();
    }
}