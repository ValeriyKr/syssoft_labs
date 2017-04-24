#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "globdef.h"
#include "builtin.h"

#define BUILTINS_CAPACITY 128

static struct builtin *builtins;
static size_t builtins_count;


void init_builtins() {
        TRY_ALLOC(builtins = (struct builtin *)
                        malloc(sizeof(struct builtin) * BUILTINS_CAPACITY));
}


int register_builtin(const char *name, builtin_func_t f) {
        assert(NULL != name);
        assert(NULL != f);
        if (builtins_count == BUILTINS_CAPACITY) return 1;

        TRY_ALLOC(builtins[builtins_count].name = malloc(strlen(name) + 1));
        strcpy(builtins[builtins_count].name, name);
        builtins[builtins_count++].command = f;
        return 0;
}


builtin_func_t builtin(const char *name) {
        size_t i;
        assert(NULL != name);
        
        for (i = 0; i < builtins_count; ++i) {
                if (!strcmp(builtins[i].name, name))
                        return builtins[i].command;
        }
        return NULL;
}
