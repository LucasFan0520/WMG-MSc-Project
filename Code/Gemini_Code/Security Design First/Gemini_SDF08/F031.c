// F031.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} Record;

int main(void) {
    Record *records = NULL;
    int count = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "BATCH ", 6) == 0) {
            char *n_str = line + 6;
            long n = strtol(n_str, NULL, 10);
            if (n > 0) {
                Record *batch_items = malloc(sizeof(Record) * n);
                int all_valid = 1;
                for (long i = 0; i < n; i++) {
                    batch_items[i].name = NULL;
                    batch_items[i].note = NULL;
                }
                for (long i = 0; i < n; i++) {
                    char *b_line = NULL;
                    size_t b_len = 0;
                    ssize_t b_read = getline(&b_line, &b_len, stdin);
                    if (b_read != -1) {
                        if (b_read > 0 && b_line[b_read - 1] == '\n') {
                            b_line[b_read - 1] = '\0';
                        }
                        char *space = strchr(b_line, ' ');
                        if (space && all_valid) {
                            *space = '\0';
                            batch_items[i].name = strdup(b_line);
                            batch_items[i].note = strdup(space + 1);
                            if (!batch_items[i].name || !batch_items[i].note) {
                                all_valid = 0;
                            }
                        } else {
                            all_valid = 0;
                        }
                    } else {
                        all_valid = 0;
                    }
                    free(b_line);
                }
                if (all_valid) {
                    Record *tmp = realloc(records, sizeof(Record) * (count + n));
                    if (tmp) {
                        records = tmp;
                        for (long i = 0; i < n; i++) {
                            records[count + i] = batch_items[i];
                        }
                        count += n;
                    } else {
                        for (long i = 0; i < n; i++) {
                            free(batch_items[i].name);
                            free(batch_items[i].note);
                        }
                    }
                } else {
                    for (long i = 0; i < n; i++) {
                        free(batch_items[i].name);
                        free(batch_items[i].note);
                    }
                }
                free(batch_items);
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            int i = 0;
            while (i < count) {
                if (strcmp(records[i].name, name) == 0) {
                    free(records[i].name);
                    free(records[i].note);
                    for (int j = i; j < count - 1; j++) {
                        records[j] = records[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *name = line + 5;
            for (int i = 0; i < count; i++) {
                if (strcmp(records[i].name, name) == 0) {
                    printf("%s\n", records[i].note);
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", records[i].name, records[i].note);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(records[i].name);
        free(records[i].note);
    }
    free(records);
    free(line);
    return 0;
}
