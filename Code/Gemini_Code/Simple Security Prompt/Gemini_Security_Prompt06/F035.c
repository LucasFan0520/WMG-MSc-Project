// F035.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *payload;
} Job;

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
    Job *list = NULL;
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
            if (strcmp(line, "LIST") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s: %s\n", list[i].name, list[i].payload);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *cmd = line;
        char *rest = p1 + 1;
        if (strcmp(cmd, "ENQUEUE") == 0) {
            char *p2 = strchr(rest, ' ');
            if (p2) {
                *p2 = '\0';
                char *name = rest;
                char *payload = p2 + 1;
                list = realloc(list, (count + 1) * sizeof(Job));
                list[count].name = strdup(name);
                list[count].payload = strdup(payload);
                count++;
            }
        } else if (strcmp(cmd, "CLONE") == 0) {
            char *p2 = strchr(rest, ' ');
            if (p2) {
                *p2 = '\0';
                char *oldName = rest;
                char *newName = p2 + 1;
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(list[i].name, oldName) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    list = realloc(list, (count + 1) * sizeof(Job));
                    list[count].name = strdup(newName);
                    list[count].payload = strdup(list[found].payload);
                    count++;
                }
            }
        } else if (strcmp(cmd, "CANCEL") == 0 || strcmp(cmd, "RUN") == 0) {
            char *name = rest;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                if (strcmp(cmd, "RUN") == 0) {
                    printf("%s\n", list[found].payload);
                }
                free(list[found].name);
                free(list[found].payload);
                for (int i = found; i < count - 1; i++) {
                    list[i] = list[i + 1];
                }
                count--;
                if (count == 0) {
                    free(list);
                    list = NULL;
                } else {
                    list = realloc(list, count * sizeof(Job));
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(list[i].name);
        free(list[i].payload);
    }
    free(list);
    return 0;
}