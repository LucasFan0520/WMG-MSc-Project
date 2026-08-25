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
    ImporterRecord *list = NULL;
    size_t count = 0;
    char *line = NULL;
    size_t line_len = 0;
    ssize_t read;
    while ((read = getline(&line, &line_len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (read > 1 && line[read - 2] == '\r') {
            line[read - 2] = '\0';
        }
        char *p = line;
        while (*p == ' ' || *p == '\t') {
            p++;
        }
        if (*p == '\0') {
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') {
            p++;
        }
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "BATCH") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *n_s = p;
            int n = atoi(n_s);
            if (n > 0) {
                ImporterRecord *batch_items = malloc(n * sizeof(ImporterRecord));
                int all_valid = 1;
                for (int i = 0; i < n; i++) {
                    batch_items[i].name = NULL;
                    batch_items[i].note = NULL;
                }
                for (int i = 0; i < n; i++) {
                    char *b_line = NULL;
                    size_t b_len = 0;
                    if (getline(&b_line, &b_len, stdin) != -1) {
                        size_t blen = strlen(b_line);
                        if (blen > 0 && b_line[blen - 1] == '\n') b_line[blen - 1] = '\0';
                        if (blen > 1 && b_line[blen - 2] == '\r') b_line[blen - 2] = '\0';
                        char *bp = b_line;
                        while (*bp == ' ' || *bp == '\t') bp++;
                        char *bname = bp;
                        while (*bp && *bp != ' ' && *bp != '\t') bp++;
                        if (*bp) {
                            *bp = '\0';
                            bp++;
                        }
                        while (*bp == ' ' || *bp == '\t') bp++;
                        char *bnote = bp;
                        if (strlen(bname) > 0 && strlen(bnote) > 0) {
                            batch_items[i].name = strdup(bname);
                            batch_items[i].note = strdup(bnote);
                        } else {
                            all_valid = 0;
                        }
                    } else {
                        all_valid = 0;
                    }
                    free(b_line);
                }
                if (all_valid) {
                    ImporterRecord *temp = realloc(list, (count + n) * sizeof(ImporterRecord));
                    if (temp) {
                        list = temp;
                        for (int i = 0; i < n; i++) {
                            list[count++] = batch_items[i];
                        }
                    } else {
                        for (int i = 0; i < n; i++) {
                            free(batch_items[i].name);
                            free(batch_items[i].note);
                        }
                    }
                } else {
                    for (int i = 0; i < n; i++) {
                        free(batch_items[i].name);
                        free(batch_items[i].note);
                    }
                }
                free(batch_items);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            size_t i = 0;
            while (i < count) {
                if (strcmp(list[i].name, name) == 0) {
                    free(list[i].name);
                    free(list[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
                    printf("%s: %s\n", list[i].name, list[i].note);
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s: %s\n", list[i].name, list[i].note);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].name);
        free(list[i].note);
    }
    free(list);
    free(line);
    return 0;
}
