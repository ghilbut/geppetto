#include "restbox_lib/storage.h"
#include "restbox_lib/restbox.h"

int main(int argc, char* argv[]) {

    if (argc < 2) {
        return -1;
    }

    Storage s(argv[1]);

    RestBox box(s);
    box.RunShell();

    return 0;
}