/**
 * prol.c
 * Summary: Thin CLI wrapper for libprol.
 * Author:  KaisarCode
 * Website: https://kaisarcode.com
 * License: https://www.gnu.org/licenses/gpl-3.0.html
 */

#define _POSIX_C_SOURCE 200809L

#include "prol.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PROL_TEXT_CAP 8192
#define PROL_RESULT_CAP 32

/**
 * Reads text from standard input into the provided buffer.
 * @param buffer Destination buffer.
 * @param size Buffer size in bytes.
 * @return Pointer to buffer on success, or NULL on empty input.
 */
static const char *prol_read_stdin(char *buffer, size_t size) {
    size_t n;

    if (!buffer || size < 2) return NULL;
    n = fread(buffer, 1, size - 1, stdin);
    if (n == 0) return NULL;
    buffer[n] = '\0';
    return buffer;
}

/**
 * Standalone entry point.
 * @param argc Number of command-line arguments.
 * @param argv Command-line argument vector.
 * @return Process exit status.
 */
int main(int argc, char **argv) {
    char buffer[PROL_TEXT_CAP];
    prol_result_t results[PROL_RESULT_CAP];
    const char *text;
    double threshold;
    int limit;
    int i;
    int written;

    text = NULL;
    threshold = 0.001;
    limit = 1;
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-t") == 0 && i + 1 < argc) threshold = atof(argv[++i]);
        else if (strcmp(argv[i], "-l") == 0 && i + 1 < argc) limit = atoi(argv[++i]);
        else if (argv[i][0] != '-') text = argv[i];
    }
    if (!text) text = prol_read_stdin(buffer, sizeof(buffer));
    if (!text || !*text) return 0;
    if (limit < 1) limit = 1;
    if (limit > PROL_RESULT_CAP) limit = PROL_RESULT_CAP;
    if (prol_init() != 0) return 1;
    written = prol_detect_top(text, results, limit, threshold);
    for (i = 0; i < written; i++) {
        if (limit == 1) printf("%s\n", results[i].code);
        else printf("%s: %.4f\n", results[i].code, results[i].score);
    }
    return 0;
}
