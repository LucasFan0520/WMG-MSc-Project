// F033.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *id;
    char *name;
    char *dept;
    char *title;
} Employee;

int main() {
    Employee *registry = NULL;
    int count = 0;
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        int len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[len - 1] = '\0';
            len--;
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[32];
        int read_bytes = 0;
        if (sscanf(line, "%31s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *ptr = line + read_bytes;
        while (*ptr == ' ') ptr++;
        if (strcmp(cmd, "ADD") == 0) {
            char id[256], name[256], dept[256], title[256];
            if (sscanf(ptr, "%255s %255s %255s %255s", id, name, dept, title) == 4) {
                registry = realloc(registry, (count + 1) * sizeof(Employee));
                registry[count].id = strdup(id);
                registry[count].name = strdup(name);
                registry[count].dept = strdup(dept);
                registry[count].title = strdup(title);
                count++;
            }
        } else if (strcmp(cmd, "UPDATE") == 0) {
            char id[256], name[256], dept[256], title[256];
            if (sscanf(ptr, "%255s %255s %255s %255s", id, name, dept, title) == 4) {
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
            if (sscanf(ptr, "%255s", id) > 0) {
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
                    if (count > 0) {
                        registry = realloc(registry, count * sizeof(Employee));
                    } else {
                        free(registry);
                        registry = NULL;
                    }
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char id[256];
            if (sscanf(ptr, "%255s", id) > 0) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(registry[i].id, id) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    printf("%s %s %s %s\n", registry[found].id, registry[found].name, registry[found].dept, registry[found].title);
                } else {
                    printf("NOT_FOUND\n");
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s %s %s\n", registry[i].id, registry[i].name, registry[i].dept, registry[i].title);
            }
        }
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
