// F033.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *stream) {
    size_t capacity = 16;
    size_t len = 0;
    char *str = malloc(capacity);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
        if (len + 1 >= capacity) {
            capacity *= 2;
            char *new_str = realloc(str, capacity);
            if (!new_str) {
                free(str);
                return NULL;
            }
            str = new_str;
        }
        str[len++] = (char)c;
    }
    if (len == 0 && c == EOF) {
        free(str);
        return NULL;
    }
    if (len > 0 && str[len - 1] == '\n') {
        len--;
    }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *id;
    char *name;
    char *department;
    char *title;
} Employee;

int main(void) {
    Employee *registry = NULL;
    int count = 0;
    int capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0 || strncmp(line, "UPDATE ", 7) == 0) {
            int is_add = (strncmp(line, "ADD ", 4) == 0);
            char *p = line + (is_add ? 4 : 7);
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
                        if (is_add) {
                            if (count >= capacity) {
                                capacity = capacity == 0 ? 4 : capacity * 2;
                                Employee *new_r = realloc(registry, capacity * sizeof(Employee));
                                if (new_r) registry = new_r;
                            }
                            if (count < capacity) {
                                registry[count].id = strdup(id);
                                registry[count].name = strdup(name);
                                registry[count].department = strdup(dept);
                                registry[count].title = strdup(title);
                                count++;
                            }
                        } else {
                            for (int i = 0; i < count; i++) {
                                if (strcmp(registry[i].id, id) == 0) {
                                    free(registry[i].name);
                                    free(registry[i].department);
                                    free(registry[i].title);
                                    registry[i].name = strdup(name);
                                    registry[i].department = strdup(dept);
                                    registry[i].title = strdup(title);
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *id = line + 7;
            while (*id == ' ') id++;
            for (int i = 0; i < count; i++) {
                if (strcmp(registry[i].id, id) == 0) {
                    free(registry[i].id);
                    free(registry[i].name);
                    free(registry[i].department);
                    free(registry[i].title);
                    for (int j = i; j < count - 1; j++) {
                        registry[j] = registry[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *id = line + 5;
            while (*id == ' ') id++;
            for (int i = 0; i < count; i++) {
                if (strcmp(registry[i].id, id) == 0) {
                    printf("%s: %s (%s, %s)\n", registry[i].id, registry[i].name, registry[i].department, registry[i].title);
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s: %s (%s, %s)\n", registry[i].id, registry[i].name, registry[i].department, registry[i].title);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(registry[i].id);
        free(registry[i].name);
        free(registry[i].department);
        free(registry[i].title);
    }
    free(registry);
    return 0;
}
