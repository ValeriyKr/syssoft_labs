#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "globdef.h"
#include "variable.h"

#define VARIABLES_CAPACITY 1024

static struct variable vars[VARIABLES_CAPACITY];
static size_t vars_count;

const char * get_var(const char *name) {
        size_t i;

        assert(NULL != name);

        for (i = 0; i < vars_count; ++i) {
                if (!strcmp(vars[i].name, name))
                        return vars[i].value;
        }
        return getenv(name);
}


const struct variable * get_vars() {
        return vars;
}


size_t get_vars_count() {
        return vars_count;
}


void set_var(const char *name, const char *value) {
        TRY_ALLOC(vars[vars_count].name = (char *) malloc(strlen(name) + 1));
        TRY_ALLOC(vars[vars_count].value = (char *) malloc(strlen(value) + 1));
        strcpy(vars[vars_count].name, name);
        strcpy(vars[vars_count].value, value);
        ++vars_count;
}
