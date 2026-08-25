// F036.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *data;
} PrefixRecord;

int main(void) {
    PrefixRecord *records = NULL;
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
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *prefix = p;

            if (*name == '\0') continue;

            char *colon = strchr(prefix, ':');
            if (!colon) continue;

            *colon = '\0';
            char *len_str = prefix;
            char *data_start = colon + 1;

            char *endptr;
            long d_len = strtol(len_str, &endptr, 10);
            if (endptr == len_str || d_len < 0 || strlen(data_start) < (size_t)d_len) continue;

            if (count >= capacity) {
                size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                PrefixRecord *new_rec = realloc(records, new_cap * sizeof(PrefixRecord));
                if (!new_rec) continue;
                records = new_rec;
                capacity = new_cap;
            }

            char *nm = strdup(name);
            char *dt = malloc(d_len + 1);
            if (nm && dt) {
                memcpy(dt, data_start, d_len);
                dt[d_len] = '\0';
                records[count].name = nm;
                records[count].data = dt;
                count++;
            } else {
                free(nm);
                free(dt);
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
                    free(records[i].data);
                    for (size_t j = i; j < count - 1; j++) {
                        records[j] = records[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*name == '\0') continue;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(records[i].name, name) == 0) {
                    for (size_t j = 0; records[i].data[j] != '\0'; j++) {
                        if (records[i].data[j] == ' ') {
                            putchar('_');
                        } else {
                            putchar(records[i].data[j]);
                        }
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", records[i].name);
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(records[i].name);
        free(records[i].data);
    }
    free(records);
    free(line);
    return 0;
}
