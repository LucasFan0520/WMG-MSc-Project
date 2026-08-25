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
        line[strcspn(line, "\r\n")] = 0;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "ADD") == 0) {
            char *id = strtok(NULL, " ");
            char *name = strtok(NULL, " ");
            char *dept = strtok(NULL, " ");
            char *title = strtok(NULL, " ");
            if (id && name && dept && title) {
                registry = realloc(registry, (count + 1) * sizeof(Employee));
                registry[count].id = strdup(id);
                registry[count].name = strdup(name);
                registry[count].dept = strdup(dept);
                registry[count].title = strdup(title);
                count++;
            }
        } else if (strcmp(cmd, "UPDATE") == 0) {
            char *id = strtok(NULL, " ");
            char *name = strtok(NULL, " ");
            char *dept = strtok(NULL, " ");
            char *title = strtok(NULL, " ");
            if (id && name && dept && title) {
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
            char *id = strtok(NULL, " ");
            if (id) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(registry[i].id, id) == 0) {
                        free(registry[i].id);
                        free(registry[i].name);
                        free(registry[i].dept);
                        free(registry[i].title);
                        for (int j = i; j < count - 1; j++) {
                            registry[j] = registry[j + 1];
                        }
                        count--;
                        i--;
                    }
                }
                if (count == 0) {
                    free(registry);
                    registry = NULL;
                } else {
                    registry = realloc(registry, count * sizeof(Employee));
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *id = strtok(NULL, " ");
            if (id) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(registry[i].id, id) == 0) {
                        printf("%s %s %s %s\n", registry[i].id, registry[i].name, registry[i].dept, registry[i].title);
                        break;
                    }
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
