// F031.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} ImporterRecord;

int main(void) {
    ImporterRecord *items = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    long read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }

        if (strcmp(line, "END") == 0) {
            break;
        }

        char *p = line;
        while (*p == ' ' || *p == '	') p++;
        if (*p == '\0') continue;

        char *cmd = p;
        while (*p && *p != ' ' && *p != '	') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        while (*p == ' ' || *p == '	') p++;

        if (strcmp(cmd, "BATCH") == 0) {
            char *n_str = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            long n = strtol(n_str, NULL, 10);
            if (n < 0) n = 0;

            ImporterRecord *batch_items = malloc(n * sizeof(ImporterRecord));
            if (n > 0 && !batch_items) exit(1);

            int all_valid = 1;
            for (long i = 0; i < n; i++) {
                char *bline = NULL;
                size_t blen = 0;
                long bread = getline(&bline, &blen, stdin);
                if (bread != -1) {
                    if (bread > 0 && bline[bread - 1] == '\n') {
                        bline[bread - 1] = '\0';
                        bread--;
                    }
                    if (bread > 0 && bline[bread - 1] == '\n') {
                        bline[bread - 1] = '\0';
                        bread--;
                    }

                    char *bp = bline;
                    while (*bp == ' ' || *bp == '	') bp++;
                    char *name = bp;
                    while (*bp && *bp != ' ' && *bp != '	') bp++;
                    if (*bp) {
                        *bp = '\0';
                        bp++;
                    }
                    while (*bp == ' ' || *bp == '	') bp++;
                    char *note = bp;

                    if (*name != '\0' && *note != '\0') {
                        batch_items[i].name = strdup(name);
                        batch_items[i].note = strdup(note);
                        if (!batch_items[i].name || !batch_items[i].note) exit(1);
                    } else {
                        all_valid = 0;
                        batch_items[i].name = NULL;
                        batch_items[i].note = NULL;
                    }
                } else {
                    all_valid = 0;
                    batch_items[i].name = NULL;
                    batch_items[i].note = NULL;
                }
                free(bline);
            }

            if (all_valid) {
                for (long i = 0; i < n; i++) {
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        ImporterRecord *new_items = realloc(items, capacity * sizeof(ImporterRecord));
                        if (!new_items) exit(1);
                        items = new_items;
                    }
                    items[count] = batch_items[i];
                    count++;
                }
            } else {
                for (long i = 0; i < n; i++) {
                    free(batch_items[i].name);
                    free(batch_items[i].note);
                }
            }
            free(batch_items);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; ) {
                if (strcmp(items[i].name, name) == 0) {
                    free(items[i].name);
                    free(items[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    printf("%s\n", items[i].note);
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", items[i].name, items[i].note);
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(items[i].name);
        free(items[i].note);
    }
    free(items);
    free(line);
    return 0;
}
