// F036.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *data;
} RecordPrefix;

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
    RecordPrefix *list = NULL;
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
            if (strcmp(line, "REPORT") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s\n", list[i].name);
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
                char *name = rest;
                char *length_data = p2 + 1;
                char *colon = strchr(length_data, ':');
                if (colon) {
                    *colon = '\0';
                    int length = atoi(length_data);
                    char *actual_data = colon + 1;
                    if (length >= 0 && (int)strlen(actual_data) >= length) {
                        list = realloc(list, (count + 1) * sizeof(RecordPrefix));
                        list[count].name = strdup(name);
                        list[count].data = malloc(length + 1);
                        strncpy(list[count].data, actual_data, length);
                        list[count].data[length] = '\0';
                        count++;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = rest;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(list[found].name);
                free(list[found].data);
                for (int i = found; i < count - 1; i++) {
                    list[i] = list[i + 1];
                }
                count--;
                if (count == 0) {
                    free(list);
                    list = NULL;
                } else {
                    list = realloc(list, count * sizeof(RecordPrefix));
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            char *name = rest;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
                    for (int j = 0; list[i].data[j] != '\0'; j++) {
                        if (list[i].data[j] == ' ') {
                            putchar('_');
                        } else {
                            putchar(list[i].data[j]);
                        }
                    }
                    putchar('\n');
                    break;
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(list[i].name);
        free(list[i].data);
    }
    free(list);
    return 0;
}