// F033.c
#define _GNU_SOURCE
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
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }

        char *cmd = line;
        char *space1 = strchr(line, ' ');
        char *args = NULL;
        if (space1) {
            *space1 = '\0';
            args = space1 + 1;
            while (*args == ' ') args++;
        }

        if ((strcmp(cmd, "ADD") == 0 || strcmp(cmd, "UPDATE") == 0) && args) {
            char *id = args;
            char *space2 = strchr(args, ' ');
            if (space2) {
                *space2 = '\0';
                char *name = space2 + 1;
                while (*name == ' ') name++;
                char *space3 = strchr(name, ' ');
                if (space3) {
                    *space3 = '\0';
                    char *dept = space3 + 1;
                    while (*dept == ' ') dept++;
                    char *space4 = strchr(dept, ' ');
                    char *title = "";
                    if (space4) {
                        *space4 = '\0';
                        title = space4 + 1;
                        while (*title == ' ') title++;
                    }

                    if (strcmp(cmd, "ADD") == 0) {
                        registry = realloc(registry, (count + 1) * sizeof(Employee));
                        if (!registry) return 1;
                        registry[count].id = strdup(id);
                        registry[count].name = strdup(name);
                        registry[count].dept = strdup(dept);
                        registry[count].title = strdup(title);
                        count++;
                    } else {
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
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0 && args) {
            for (int i = 0; i < count; i++) {
                if (strcmp(registry[i].id, args) == 0) {
                    free(registry[i].id);
                    free(registry[i].name);
                    free(registry[i].dept);
                    free(registry[i].title);
                    for (int j = i; j < count - 1; j++) {
                        registry[j] = registry[j + 1];
                    }
                    count--;
                    registry = realloc(registry, count * sizeof(Employee));
                    if (count > 0 && !registry) return 1;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0 && args) {
            for (int i = 0; i < count; i++) {
                if (strcmp(registry[i].id, args) == 0) {
                    printf("%s %s %s %s\n", registry[i].id, registry[i].name, registry[i].dept, registry[i].title);
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s %s %s\n", registry[i].id, registry[i].name, registry[i].dept, registry[i].title);
            }
        }
    }

    free(line);
    for (int i = 0; i < count; i++) {
        free(registry[i].id);
        free(registry[i].name);
        free(registry[i].dept);
        free(registry[i].title);
    }
    free(registry);
    return 0;
}
