#include <unistd.h>

#include "globdef.h"
#include "io.h"
#include "builtins_impl.h"

int k_cd(size_t argc, char **argv) {
        if (argc != 2) {
                error(E_INVARGS, 0);
                return 1;
        }

        if (chdir(argv[1])) {
                error(E_UNDEF, 0);
                return 2;
        }
        return 0;
}


int k_exit(size_t argc, char **argv) {
        goodnight(0);
}
