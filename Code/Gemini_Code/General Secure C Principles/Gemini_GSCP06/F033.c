// F033.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *id;
    char *name;
    char *department;
    char *title;
} Employee;

int main(void) {
    Employee *registry = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *args = line + 4;
            char *sp1 = strchr(args, ' ');
            if (sp1) {
                *sp1 = '\0';
                char *id = args;
                char *rest1 = sp1 + 1;
                char *sp2 = strchr(rest1, ' ');
                if (sp2) {
                    *sp2 = '\0';
                    char *name = rest1;
                    char *rest2 = sp2 + 1;
                    char *sp3 = strchr(rest2, ' ');
                    if (sp3) {
                        *sp3 = '\0';
                        char *dept = rest2;
                        char *title = sp3 + 1;
                        if (count >= capacity) {
                            capacity = capacity == 0 ? 4 : capacity * 2;
                            Employee *next = realloc(registry, capacity * sizeof(Employee));
                            if (!next) break;
                            registry = next;
                        }
                        registry[count].id = strdup(id);
                        registry[count].name = strdup(name);
                        registry[count].department = strdup(dept);
                        registry[count].title = strdup(title);
                        count++;
                    }
                }
            }
        } else if (strncmp(line, "UPDATE ", 7) == 0) {
            char *args = line + 7;
            char *sp1 = strchr(args, ' ');
            if (sp1) {
                *sp1 = '\0';
                char *id = args;
                char *rest1 = sp1 + 1;
                char *sp2 = strchr(rest1, ' ');
                if (sp2) {
                    *sp2 = '\0';
                    char *name = rest1;
                    char *rest2 = sp2 + 1;
                    char *sp3 = strchr(rest2, ' ');
                    if (sp3) {
                        *sp3 = '\0';
                        char *dept = rest2;
                        char *title = sp3 + 1;
                        for (size_t i = 0; i < count; i++) {
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
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *id = line + 7;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(registry[i].id, id) == 0) {
                    free(registry[i].id);
                    free(registry[i].name);
                    free(registry[i].department);
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
                    printf("%s %s %s %s\n", registry[i].id, registry[i].name, registry[i].department, registry[i].title);
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s %s %s\n", registry[i].id, registry[i].name, registry[i].department, registry[i].title);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(registry[i].id);
        free(registry[i].name);
        free(registry[i].department);
        free(registry[i].title);
    }
    free(registry);
    free(line);
    return 0;
}
