#define NOB_IMPLEMENTATION

#define BF_TESTS_PATH "../../src_bf/"
#define INTERPRETER_PATH "../../../build/"

#include "../../../src_build/nob.h"

int main() {
    Nob_Cmd cmd = {0};

    nob_cmd_append(&cmd, INTERPRETER_PATH"bfi.exe", BF_TESTS_PATH"HelloWorld.bf");

    if(!nob_cmd_run_sync_and_reset(&cmd)){
        return 1;
    }

    return 0;
}