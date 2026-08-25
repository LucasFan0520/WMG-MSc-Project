// F033.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 1024;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) { free(buf); return NULL; }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *id;
    char *name;
    char *dept;
    char *title;
} Employee;

int main() {
    Employee *registry = NULL;
    int count = 0;
    int capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char cmd[64];
        int n = 0;
        if (sscanf(line, "%63s%n", cmd, &n) < 1) {
            free(line);
            continue;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s %s %s\n", registry[i].id, registry[i].name, registry[i].dept, registry[i].title);
            }
            free(line);
            continue;
        }
        char *p = line + n;
        while (*p == ' ' || *p == '\t') p++;
        if (strcmp(cmd, "ADD") == 0) {
            char id[256], name[256], dept[256], title[256];
            if (sscanf(p, "%255s %255s %255s %255s", id, name, dept, title) == 4) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    registry = realloc(registry, capacity * sizeof(Employee));
                }
                registry[count].id = strdup(id);
                registry[count].name = strdup(name);
                registry[count].dept = strdup(dept);
                registry[count].title = strdup(title);
                count++;
            }
        } else if (strcmp(cmd, "UPDATE") == 0) {
            char id[256], name[256], dept[256], title[256];
            if (sscanf(p, "%255s %255s %255s %255s", id, name, dept, title) == 4) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(registry[i].id, id) == 0) {
                        free(registry[i].name);
                        free(registry[i].dept);
                        free(registry[i].title);
                        registry[i].name = strdup(name);
                        registry[i].dept = strdup(dept);
                        registry[i].title = strdup(title);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char id[256];
            if (sscanf(p, "%255s", id) == 1) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(registry[i].id, id) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(registry[found].id);
                    free(registry[found].name);
                    free(registry[found].dept);
                    free(registry[found].title);
                    for (int i = found; i < count - 1; i++) {
                        registry[i] = registry[i + 1];
                    }
                    count--;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char id[256];
            if (sscanf(p, "%255s", id) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(registry[i].id, id) == 0) {
                        printf("%s %s %s\n", registry[i].name, registry[i].dept, registry[i].title);
                        break;
                    }
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(registry[i].id);
        free(registry[i].name);
        free(registry[i].dept);
        free(registry[i].title);
    }
    free(registry);
    return 0;
}
