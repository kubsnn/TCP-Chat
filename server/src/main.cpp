#include "server.h"
#include "user_db_handle.h"

int main(int, char*[]) {
    configure_databases();

    Server("127.0.0.1", 42069).run();
}