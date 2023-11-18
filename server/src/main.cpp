#define LOG_TO_FILE 0
#include "logger.h"
#include "server.h"
#include "arg_parser.h"
#include "core/database/user_db_handle.h"
#include "core/utils/crypto.h"
#include <openssl/ssl.h>


int main(int argc, char* argv[]) {
    configure_databases();
    OpenSSL_add_all_algorithms();

    ArgParser args(argc, argv);
    globals::debug = args.debug();

    Server(args.ip(), args.port()).run();
}