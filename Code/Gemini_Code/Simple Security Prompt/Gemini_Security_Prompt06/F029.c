// F029.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} Record;

char *read_line(FILE *f) {
    size_t size = 32;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

int main() {
    Record *list = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *p1 = strchr(line, ' ');
        if (!p1) {
            if (strcmp(line, "DEDUP") == 0) {
                int i = 0;
                while (i < count) {
                    int duplicate = 0;
                    for (int j = 0; j < i; j++) {
                        if (strcmp(list[j].key, list[i].key) == 0) {
                            duplicate = 1;
                            break;
                        }
                    }
                    if (duplicate) {
                        free(list[i].key);
                        free(list[i].value);
                        for (int j = i; j < count - 1; j++) {
                            list[j] = list[j + 1];
                        }
                        count--;
                    } else {
                        i++;
                    }
                }
                if (count == 0) {
                    free(list);
                    list = NULL;
                } else {
                    list = realloc(list, count * sizeof(Record));
                }
            } else if (strcmp(line, "REPORT") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s %s\n", list[i].key, list[i].value);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *cmd = line;
        char *rest = p1 + 1;
        if (strcmp(cmd, "ADD") == 0) {
            char *p2 = strchr(rest, ' ');
            if (p2) {
                *p2 = '\0';
                char *key = rest;
                char *val = p2 + 1;
                list = realloc(list, (count + 1) * sizeof(Record));
                list[count].key = strdup(key);
                list[count].value = strdup(val);
                count++;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *key = rest;
            int i = 0;
            while (i < count) {
                if (strcmp(list[i].key, key) == 0) {
                    free(list[i].key);
                    free(list[i].value);
                    for (int j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
            if (count == 0) {
                free(list);
                list = NULL;
            } else {
                list = realloc(list, count * sizeof(Record));
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *key = rest;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].key, key) == 0) {
                    printf("%s\n", list[i].value);
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(list[i].key);
        free(list[i].value);
    }
    free(list);
    return 0;
}