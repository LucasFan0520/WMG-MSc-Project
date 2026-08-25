// F031.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} ImporterRecord;

int main(void) {
    ImporterRecord *records = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') continue;
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "BATCH") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *n_str = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*n_str == '\0') continue;
            long n = strtol(n_str, NULL, 10);
            if (n <= 0) continue;

            ImporterRecord *batch_items = malloc(n * sizeof(ImporterRecord));
            if (!batch_items) continue;

            int batch_valid = 1;
            for (long i = 0; i < n; i++) {
                char *b_line = NULL;
                size_t b_len = 0;
                if (getline(&b_line, &b_len, stdin) == -1) {
                    batch_valid = 0;
                    free(b_line);
                    break;
                }
                b_line[strcspn(b_line, "\r\n")] = '\0';
                char *bp = b_line;
                while (*bp == ' ' || *bp == '\t') bp++;
                char *name = bp;
                while (*bp && *bp != ' ' && *bp != '\t') bp++;
                if (*bp) {
                    *bp = '\0';
                    bp++;
                }
                while (*bp == ' ' || *bp == '\t') bp++;
                char *note = bp;

                if (*name == '\0') {
                    batch_valid = 0;
                    free(b_line);
                    break;
                }
                char *nm = strdup(name);
                char *nt = strdup(note);
                if (nm && nt) {
                    batch_items[i].name = nm;
                    batch_items[i].note = nt;
                } else {
                    free(nm);
                    free(nt);
                    batch_valid = 0;
                    free(b_line);
                    break;
                }
                free(b_line);
            }

            if (batch_valid) {
                for (long i = 0; i < n; i++) {
                    if (count >= capacity) {
                        size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                        ImporterRecord *new_rec = realloc(records, new_cap * sizeof(ImporterRecord));
                        if (new_rec) {
                            records = new_rec;
                            capacity = new_cap;
                        } else {
                            break;
                        }
                    }
                    records[count] = batch_items[i];
                    count++;
                }
                free(batch_items);
            } else {
                for (long i = 0; i < n; i++) {
                    if (i < count) {
                        // dynamic safety check
                    }
                }
                free(batch_items);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*name == '\0') continue;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(records[i].name, name) == 0) {
                    free(records[i].name);
                    free(records[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        records[j] = records[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*name == '\0') continue;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(records[i].name, name) == 0) {
                    printf("%s: %s\n", records[i].name, records[i].note);
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s: %s\n", records[i].name, records[i].note);
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(records[i].name);
        free(records[i].note);
    }
    free(records);
    free(line);
    return 0;
}
