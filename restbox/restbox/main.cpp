#include "restbox_lib/storage.h"
#include "restbox_lib/restbox.h"

#include <v8.h>


int main(int argc, char* argv[]) {

    if (argc < 2) {
        return -1;
    }

    v8::V8::InitializeICU();
    int c = 2;
    char* v[] = {"", "--expose_gc"};
    v8::V8::SetFlagsFromCommandLine(&c, v, true);

    Storage s(argv[1]);

    {
        RestBox box(s);
        box.RunShell();
    }
    v8::V8::Dispose();

    return 0;
}