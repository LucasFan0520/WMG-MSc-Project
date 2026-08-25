// F033.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *id;
    char *name;
    char *department;
    char *title;
} Employee;

int main() {
    Employee *registry = NULL;
    int count = 0;
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[16];
        int read_bytes = 0;
        if (sscanf(line, "%15s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *args = line + read_bytes;
        while (*args == ' ') args++;
        if (strcmp(cmd, "ADD") == 0) {
            char id[256], name[256], dept[256], title[256];
            if (sscanf(args, "%255s %255s %255s %255s", id, name, dept, title) == 4) {
                registry = realloc(registry, (count + 1) * sizeof(Employee));
                registry[count].id = strdup(id);
                registry[count].name = strdup(name);
                registry[count].department = strdup(dept);
                registry[count].title = strdup(title);
                count++;
            }
        } else if (strcmp(cmd, "UPDATE") == 0) {
            char id[256], name[256], dept[256], title[256];
            if (sscanf(args, "%255s %255s %255s %255s", id, name, dept, title) == 4) {
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
        } else if (strcmp(cmd, "DELETE") == 0) {
            char id[256];
            if (sscanf(args, "%255s", id) == 1) {
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
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char id[256];
            if (sscanf(args, "%255s", id) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(registry[i].id, id) == 0) {
                        printf("ID: %s Name: %s Dept: %s Title: %s\n", registry[i].id, registry[i].name, registry[i].department, registry[i].title);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("ID: %s Name: %s Dept: %s Title: %s\n", registry[i].id, registry[i].name, registry[i].department, registry[i].title);
            }
        }
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
