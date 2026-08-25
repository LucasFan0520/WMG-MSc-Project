// F033.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    if (len > 0 && buf[len-1] == '\r') {
        buf[len-1] = '\0';
    }
    return buf;
}

typedef struct {
    char *id;
    char *name;
    char *dept;
    char *title;
} Employee;

int main(void) {
    Employee *registry = NULL;
    int count = 0;
    int cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0 || strncmp(line, "UPDATE ", 7) == 0) {
            int is_add = strncmp(line, "ADD ", 4) == 0;
            char *p = is_add ? line + 4 : line + 7;
            while (*p == ' ') p++;
            char *id = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                char *name = p;
                while (*p && *p != ' ') p++;
                if (*p == ' ') {
                    *p = '\0';
                    p++;
                    while (*p == ' ') p++;
                    char *dept = p;
                    while (*p && *p != ' ') p++;
                    if (*p == ' ') {
                        *p = '\0';
                        char *title = p + 1;
                        while (*title == ' ') title++;
                        int found = -1;
                        for (int i = 0; i < count; i++) {
                            if (strcmp(registry[i].id, id) == 0) {
                                found = i;
                                break;
                            }
                        }
                        if (found != -1) {
                            free(registry[found].name);
                            free(registry[found].dept);
                            free(registry[found].title);
                            registry[found].name = strdup(name);
                            registry[found].dept = strdup(dept);
                            registry[found].title = strdup(title);
                        } else if (is_add) {
                            if (count >= cap) {
                                cap = cap == 0 ? 16 : cap * 2;
                                registry = realloc(registry, cap * sizeof(Employee));
                            }
                            registry[count].id = strdup(id);
                            registry[count].name = strdup(name);
                            registry[count].dept = strdup(dept);
                            registry[count].title = strdup(title);
                            count++;
                        }
                    }
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *id = line + 7;
            while (*id == ' ') id++;
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
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *id = line + 5;
            while (*id == ' ') id++;
            for (int i = 0; i < count; i++) {
                if (strcmp(registry[i].id, id) == 0) {
                    printf("%s %s %s %s\n", registry[i].id, registry[i].name, registry[i].dept, registry[i].title);
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s %s %s\n", registry[i].id, registry[i].name, registry[i].dept, registry[i].title);
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
