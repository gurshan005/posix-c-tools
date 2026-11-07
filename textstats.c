#include <ctype.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int summarize(FILE *input, const char *name)
{
    unsigned char buffer[8192];
    uintmax_t lines = 0, words = 0;
    bool in_word = false;
    bool partial_line = false;
    size_t size;

    while ((size = fread(buffer, 1, sizeof(buffer), input)) > 0) {
        for (size_t i = 0; i < size; i++) {
            unsigned char c = buffer[i];
            partial_line = c != '\n';
            if (!partial_line)
                lines++;
            if (isspace(c)) {
                in_word = false;
            } else if (!in_word) {
                words++;
                in_word = true;
            }
        }
    }
    if (ferror(input)) {
        perror(name);
        return EXIT_FAILURE;
    }

    if (partial_line)
        lines++;
    double average = lines ? (double)words / (double)lines : 0;
    printf("lines=%" PRIuMAX " words=%" PRIuMAX " words_per_line=%.2f\n",
           lines, words, average);
    return EXIT_SUCCESS;
}

int main(int argc, char **argv)
{
    if (argc > 2) {
        fputs("usage: textstats [FILE|-]\n", stderr);
        return 2;
    }
    if (argc == 1 || strcmp(argv[1], "-") == 0)
        return summarize(stdin, "stdin");

    FILE *file = fopen(argv[1], "rb");
    if (!file) {
        perror(argv[1]);
        return EXIT_FAILURE;
    }
    int result = summarize(file, argv[1]);
    if (fclose(file) != 0) {
        perror(argv[1]);
        return EXIT_FAILURE;
    }
    return result;
}
