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
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    } else {
        buf[len] = '\0';
    }
    return buf;
}

char *safe_dup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) {
        strcpy(d, s);
    }
    return d;
}

typedef struct {
    char *id;
    char *name;
    char *dept;
    char *title;
} Employee;

int main(void) {
    Employee *registry = NULL;
    size_t count = 0;
    size_t cap = 0;
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
            char *id = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') { *p = '\0'; p++; }
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') { *p = '\0'; p++; }
            char *dept = p;
            while (*p && *p != ' ') p++;
            char *title = "";
            if (*p == ' ') { *p = '\0'; title = p + 1; }
            if (is_add) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Employee *nb = realloc(registry, cap * sizeof(Employee));
                    if (!nb) {
                        free(line);
                        break;
                    }
                    registry = nb;
                }
                registry[count].id = safe_dup(id);
                registry[count].name = safe_dup(name);
                registry[count].dept = safe_dup(dept);
                registry[count].title = safe_dup(title);
                count++;
            } else {
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(registry[i].id, id) == 0) {
                        free(registry[i].name);
                        free(registry[i].dept);
                        free(registry[i].title);
                        registry[i].name = safe_dup(name);
                        registry[i].dept = safe_dup(dept);
                        registry[i].title = safe_dup(title);
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *id = line + 7;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(registry[i].id, id) == 0) {
                    free(registry[i].id);
                    free(registry[i].name);
                    free(registry[i].dept);
                    free(registry[i].title);
                    for (size_t j = i; j < count - 1; j++) {
                        registry[j] = registry[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *id = line + 5;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(registry[i].id, id) == 0) {
                    printf("%s %s %s %s\n", registry[i].id, registry[i].name, registry[i].dept, registry[i].title);
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s %s %s\n", registry[i].id, registry[i].name, registry[i].dept, registry[i].title);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(registry[i].id);
        free(registry[i].name);
        free(registry[i].dept);
        free(registry[i].title);
    }
    free(registry);
    return 0;
}
