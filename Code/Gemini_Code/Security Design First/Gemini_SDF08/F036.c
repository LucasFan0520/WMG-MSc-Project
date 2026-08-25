// F036.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *data;
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
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            char *space = strchr(p, ' ');
            if (space) {
                *space = '\0';
                char *name = p;
                char *prefix = space + 1;
                char *colon = strchr(prefix, ':');
                if (colon) {
                    *colon = '\0';
                    long d_len = strtol(prefix, NULL, 10);
                    char *data_start = colon + 1;
                    if (d_len >= 0 && (long)strlen(data_start) >= d_len) {
                        data_start[d_len] = '\0';
                        Record *tmp = realloc(records, sizeof(Record) * (count + 1));
                        if (tmp) {
                            records = tmp;
                            records[count].name = strdup(name);
                            records[count].data = strdup(data_start);
                            if (records[count].name && records[count].data) {
                                count++;
                            }
                        }
                    }
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            int idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(records[i].name, name) == 0) {
                    idx = i;
                    break;
                }
            }
            if (idx != -1) {
                free(records[idx].name);
                free(records[idx].data);
                for (int i = idx; i < count - 1; i++) {
                    records[i] = records[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "SHOW ", 5) == 0) {
            char *name = line + 5;
            for (int i = 0; i < count; i++) {
                if (strcmp(records[i].name, name) == 0) {
                    char *t = records[i].data;
                    while (*t) {
                        if (*t == ' ') putchar('_');
                        else putchar(*t);
                        t++;
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", records[i].name);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(records[i].name);
        free(records[i].data);
    }
    free(records);
    free(line);
    return 0;
}
