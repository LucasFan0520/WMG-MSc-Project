// F033.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 128;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
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
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *id;
    char *name;
    char *department;
    char *title;
} Employee;

int main() {
    Employee *registry = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char id[128], name[128], dept[128], title[128];
            if (sscanf(line + 4, "%127s %127s %127s %127s", id, name, dept, title) == 4) {
                registry = realloc(registry, (count + 1) * sizeof(Employee));
                registry[count].id = strdup(id);
                registry[count].name = strdup(name);
                registry[count].department = strdup(dept);
                registry[count].title = strdup(title);
                count++;
            }
        } else if (strncmp(line, "UPDATE ", 7) == 0) {
            char id[128], name[128], dept[128], title[128];
            if (sscanf(line + 7, "%127s %127s %127s %127s", id, name, dept, title) == 4) {
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
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *id = line + 7;
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
                    registry = realloc(registry, count * sizeof(Employee));
                    break;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *id = line + 5;
            for (int i = 0; i < count; i++) {
                if (strcmp(registry[i].id, id) == 0) {
                    printf("%s %s %s %s\n", registry[i].id, registry[i].name, registry[i].department, registry[i].title);
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s %s %s\n", registry[i].id, registry[i].name, registry[i].department, registry[i].title);
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
