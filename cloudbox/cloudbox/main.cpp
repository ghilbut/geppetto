#include "cloudbox_lib/cloudbox.h"

int main(int argc, char* argv[]) {

    CloudBox box;
    box.Start("", 80);
    box.RunShell();
    box.Stop();

    return 0;
}
