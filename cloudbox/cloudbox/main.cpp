#include "cloudbox_lib/storage.h"
#include "cloudbox_lib/cloudbox.h"

int main(int argc, char* argv[]) {

    if (argc < 2) {
        return -1;
    }

    Storage s(argv[1]);

    CloudBox box(s);
    box.RunShell();

    return 0;
}