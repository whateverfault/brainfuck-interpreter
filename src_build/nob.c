#define NOB_IMPLEMENTATION

#include "nob.h"
#include "folders.h"

int main(int argc, char **argv) {
    NOB_GO_REBUILD_URSELF(argc, argv);

    if (!nob_mkdir_if_not_exists(BUILD_FOLDER)) return 1;

    Nob_Cmd cmd = {0};

    nob_cmd_append(&cmd, "gcc",
         "-Wall",
         "-Wextra",
         "-o",
          BUILD_FOLDER"bfi", SRC_FOLDER"bfi.c");

    if (!nob_cmd_run_sync_and_reset(&cmd)) {
        getchar();
        return 1;
    }

    return 0;
}