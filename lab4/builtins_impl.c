#include <unistd.h>

#include "globdef.h"
#include "io.h"
#include "builtins_impl.h"

extern char **environ;


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


int k_setenv(size_t argc, char **argv) {
        char **env;

        if (argc > 3) {
                error(E_INVARGS, 0);
                return 1;
        }
        if (argc == 1) {
                for (env = environ; *env; ++env) {
                        say(*env);
                }
                return 0;
        }
        return setenv(argv[1], (argc == 3 ? argv[2] : ""), 1);
}


int k_help(size_t argc, char **argv) {
        sayln("It's kcsh, you don't wanna use it\n");

        sayln("If you're still here:");

        sayln("\t?");
        sayln("Help (this)\n");

        sayln("\tcd [NEW_DIRECTORY]");
        sayln("Change working directory to NEW_DIRECTORY if it is specified,");
        sayln("to $home otherwise\n");

        sayln("\tsetenv [NAME [VALUE]]");
        sayln("With no arguments -- prints all environment variables, with");
        sayln("NAME sets variable NAME to empty string, with NAME and VALUE");
        sayln("sets variable NAME to value VALUE\n");

        sayln("\texit");
        sayln("exit");

}
