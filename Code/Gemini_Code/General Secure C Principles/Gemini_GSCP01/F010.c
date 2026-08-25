// F010.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} IndexPair;

int main(void) {
    IndexPair *list = NULL;
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
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *key = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *val = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            IndexPair *temp = realloc(list, (count + 1) * sizeof(IndexPair));
            if (temp) {
                list = temp;
                list[count].key = strdup(key);
                list[count].value = strdup(val);
                count++;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *key = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *val = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].key, key) == 0 && strcmp(list[i].value, val) == 0) {
                    free(list[i].key);
                    free(list[i].value);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *key = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            size_t i = 0;
            while (i < count) {
                if (strcmp(list[i].key, key) == 0) {
                    free(list[i].key);
                    free(list[i].value);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "VALUES") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *key = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            int first = 1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].key, key) == 0) {
                    if (!first) {
                        printf(" ");
                    }
                    printf("%s", list[i].value);
                    first = 0;
                }
            }
            printf("\n");
        } else if (strcmp(cmd, "KEYS") == 0) {
            char **seen = NULL;
            size_t seen_count = 0;
            for (size_t i = 0; i < count; i++) {
                int already_seen = 0;
                for (size_t j = 0; j < seen_count; j++) {
                    if (strcmp(seen[j], list[i].key) == 0) {
                        already_seen = 1;
                        break;
                    }
                }
                if (!already_seen) {
                    char **temp = realloc(seen, (seen_count + 1) * sizeof(char *));
                    if (temp) {
                        seen = temp;
                        seen[seen_count] = list[i].key;
                        seen_count++;
                        printf("%s\n", list[i].key);
                    }
                }
            }
            free(seen);
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].key);
        free(list[i].value);
    }
    free(list);
    free(line);
    return 0;
}
