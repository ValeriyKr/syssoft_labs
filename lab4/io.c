#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "globdef.h"
#include "io.h"

/**
 * \file io.c
 * \author kk
 *
 * Implementation of I/O functions.
 */


/**
 * \def ERROR_MSG(x)
 * Creates error message
 */
#define ERR_MSG(x) "kcsh: " x


static bool_t term_initialized;
static struct termios savetty;

void init_term() {
        struct termios tty;

        if (term_initialized) return;
        if (!isatty(IN)) {
                error(E_NOTERM, 1);
        }
        tcgetattr(IN, &savetty);
        tty = savetty;
        tty.c_lflag &= ~(ICANON|ECHO|ISIG);
        tty.c_cc[VMIN] = 1;
        tty.c_cc[VTIME] = 0;
        tcsetattr(IN, TCSAFLUSH, &tty);
        term_initialized = true;
}


void reset_term() {
        if (!term_initialized) return;
        tcsetattr(IN, TCSAFLUSH, &savetty);
        term_initialized = false;
}


void say(const char *message) {
        assert(NULL != message);
        if (-1 == write(OUT, message, strlen(message))) {
                error(E_WRITE, 1);
        }
}


void sayln(const char *message) {
        say(message);
        say("\n");
}


void sayi(int n) {
        char buf[sizeof(int) * 8 + 1];
        size_t i = 0;

        /* itoa(n, buf, 10); */
        /* C89 has no itoa. Sad, but true. */
        if (0 == n) {
                say("0");
                return;
        }
        if (n < 0) {
                buf[0] = '0';
                ++i;
        }
        for (; n; ++i, n /= 10) {
                buf[i] = '0' + (n % 10);
        }
        buf[i] = '\0';
        say(buf);
}


void sayul(unsigned long n) {
        char buf[sizeof(unsigned long) * 8 + 1];
        size_t i;

        if (0 == n) {
                say("0");
                return;
        }
        for (i = 0; n; ++i, n /= 10) {
                buf[i] = '0' + (n % 10);
        }
        buf[i] = '\0';
        say(buf);
}


char * errc_to_a(int n) {
        char *buf;
        size_t i, j;

        TRY_ALLOC(buf = (char *) malloc(sizeof(char) * 8 + 1));
        if (0 == n) {
                buf[0] = '0';
                buf[1] = '\0';
                return buf;
        }
        for (i = 0; n; ++i, n /= 10) {
                buf[i] = '0' + (n % 10);
        }
        buf[i] = '\0';
        for (j = 0; j < i/2; ++j) {
                char c = buf[j];
                buf[j] = buf[i - j - 1];
                buf[i - j - 1] = c;
        }
        return buf;
}


void error(enum error_in subj, int do_exit) {
        switch (subj) {
        case E_NOTERM:
        {
                char err[] = ERR_MSG("stdin is not terminal");
                write(ERR, err, sizeof(err));
                break;
        }
        case E_NOMEM:
        {
                char err[] = ERR_MSG("cannot allocate memory. It's kind of \
                                pizdec, buy new computer, student!!1");
                write(ERR, err, sizeof(err));
                break;
        }
        case E_BREG:
        {
                char err[] = ERR_MSG("cannot register built-in function");
                write(ERR, err, sizeof(err));
                break;
        }
        case E_WRITE:
        {
                char err[] = ERR_MSG("cannot write to stdout");
                write(ERR, err, sizeof(err));
                break;
        }
        case E_READ:
        {
                char err[] = ERR_MSG("cannot read stdin");
                write(ERR, err, sizeof(err));
                break;
        }
        case E_MANYARGS:
        {
                char err[] = ERR_MSG("too many arguments");
                write(ERR, err, sizeof(err));
                break;
        }
        case E_SYNTAX:
        {
                char err[] = ERR_MSG("syntax error");
                write(ERR, err, sizeof(err));
                break;
        }
        case E_INVARGS:
        {
                char err[] = ERR_MSG("invalid command arguments");
                write(ERR, err, sizeof(err));
                break;
        }
        case E_FORK:
        {
                char err[] = ERR_MSG("cannot do fork");
                write(ERR, err, sizeof(err));
                break;
        }
        case E_EXEC:
        {
                char err[] = ERR_MSG("cannot do exec");
                write(ERR, err, sizeof(err));
                break;
        }
        case E_UNDEF:
        {
                char err[] = ERR_MSG("unknown error");
                write(ERR, err, sizeof(err));
        }
        }
        if (do_exit) goodnight(subj + 1);
}


static void strshl(char *str, size_t step) {
        if (!step) return;
        for (; *str; ++str) {
                *(str-step) = *str;
        }
        *(str-step) = *str;
}


static void strshr(char *str, size_t step) {
        size_t i;

        if (!step) return;
        for (i = strlen(str); i >= step; --i) {
                str[i] = str[i-step];
        }
}


char* get_line() {
        char c[] = {'\0', '\0'};
        char *line;
        size_t pos = 0;
        size_t length = 0;
        size_t start;

        TRY_ALLOC(line = (char *) calloc(1, 1024));
        say(getenv("prompt"));
        start = strlen(getenv("prompt"));
        while (true) {
                if (-1 == read(IN, c, 1)) error(E_READ, 1);
                switch (c[0]) {
                case 0x7f:
                        /* Backspace */
                        if (pos > 0) {
                                say("\b \b");
                                pos--;
                        }
                        continue;

                case 0x1b:
                {
                        /* Arrow key */
                        read(IN, c, 1);
                        if (c[0] != 0x5b) continue;
                        read(IN, c, 1);

                        switch (c[0]) {
                        case 0x41:
                                /* Up */
                                break;
                        case 0x42:
                                /* Down */
                                break;
                        case 0x43:
                                /* Right */
                                if (pos == length) continue;
                                say("\x1b\x5b\x33\x7e");
                                write(OUT, line + pos, 1);
                                pos++;
                                break;
                        case 0x44:
                                /* Left */
                                if (pos == 0) continue;
                                say("\b");
                                pos--;
                                break;
                        default:
                                continue;
                        }
                        break;
                }

                case 0x03:
                        /* ^C */
                        sigint_handler(0);
                        /* TODO: reset line buffer, read again. */
                        break;

                case 0x04:
                        /* ^D */
                        goodnight(0);
                        break;

                case 0x09:
                        /* Tab */
                        /* TODO: completion */
                        break;

                case 0x0a:
                        /* Return */
                        say("\n");
                        line[length] = '\0';
                        {
                                /* Clean line */
                                /* TODO: make it good */
                                size_t i;
                                for (i = 0; i < length; ++i) {
                                        if (line[i] == '\n' 
                                            || line[i] == '\r') {
                                                line[i] = ' ';
                                        }
                                }
                        }
                        return line;

                default:
                        say(c);
                        line[pos] = c[0];
                        pos++;
                        length++;
                }
        }
}
