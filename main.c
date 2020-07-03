// @TODO support tags like TODO(jack) and allow filtering of messages by contents of brackets
// @TODO allow filtering of messages by tagname
// @TODO show some lines around the tag message for context
// @XXX multi-line comments which include tags aren't printed in full

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ansicodes.h"

#define ARR_LEN(arr) (sizeof(arr) / sizeof(*arr))
#define FMT_ERRMSG(msg) (BOLD(RED("error")) ": " msg "\n")

// possible exit codes
enum {
        SUCCESS,

        ERR_TOO_FEW_ARGS,
        ERR_FOPEN_FAIL,
        ERR_MALLOC_FAIL,
        ERR_FREAD_FAIL
};

enum {TAG_DELIMITER = '@'};

static const char *tags[] = {
        "FIXME",
        "ALERT",
        "TODOC",
        "TODO",
        "HACK",
        "NOTE",
        "XXX",
        "!!!",
        "BUG"
};

//
// print_msgs - searches for and prints messages tagged with tags from the tags array
//
static int print_msgs(const char *tag, long taglen, char *src, long srclen) {
        int i, j, line_count = 1;
        _Bool tag_found = 0;

        for (i = 0; i < srclen; ++i) {
                if (src[i] == TAG_DELIMITER) {
                        for (++i, j = 0; j < taglen && i + j < srclen; ++j)
                                if (tag[j] != src[i + j])
                                        break;

                        if (j == taglen) {
                                printf("    " YELLOW("%s ") "on line " RED("%d") ":\n        ", tag, line_count);

                                fputs("\x1b[1m", stdout);      // ANSI BOLD
                                for (j = taglen; src[i + j] != '\n' && i + j < srclen; ++j)
                                        putchar(src[i + j]);
                                fputs("\x1b[0m" "\n", stdout); // ANSI RESET

                                tag_found = 1;
                        }
                } else if (src[i] == '\n') {
                        ++line_count;
                }
        }

        return tag_found;
}

int main(int argc, char **argv) {
        FILE *infile;
        char *filebuffer = NULL;
        int current_filesize, largest_filesize = 0;

        if (argc < 2) {
                fprintf(stderr, FMT_ERRMSG("too few arguments\nusage: %s <source code file(s)>"), argv[0]);
                return ERR_TOO_FEW_ARGS;
        }

        for (int i = 1; i < argc; ++i) {
                if (!(infile = fopen(argv[i], "rb"))) {
                        fprintf(stderr, FMT_ERRMSG("failed to open file `%s`"), argv[i]);
                        return ERR_FOPEN_FAIL;
                }

                fseek(infile, 0, SEEK_END);
                current_filesize = ftell(infile);
                rewind(infile);

                if (current_filesize > largest_filesize) {
                        if (!(filebuffer = realloc(filebuffer, current_filesize))) {
                                fprintf(stderr, FMT_ERRMSG("failed to resize buffer for input file `%s`"), argv[i]);
                                fclose(infile);
                                if (filebuffer)
                                        free(filebuffer);

                                return ERR_MALLOC_FAIL;                                
                        }

                        largest_filesize = current_filesize;
                }

                if (!fread(filebuffer, 1, current_filesize, infile)) {
                        fprintf(stderr, FMT_ERRMSG("failed to load file `%s`"), argv[i]);
                        free(filebuffer);
                        fclose(infile);

                        return ERR_FREAD_FAIL;
                }
                fclose(infile);

                printf(MAGENTA("%s") ":\n", argv[i]);
                for (int j = 0; j < ARR_LEN(tags); ++j)
                        if (print_msgs(tags[j], strlen(tags[j]), filebuffer, current_filesize))
                                puts(BOLD("    ~~~"));
        }

        free(filebuffer);

        return SUCCESS;
}
