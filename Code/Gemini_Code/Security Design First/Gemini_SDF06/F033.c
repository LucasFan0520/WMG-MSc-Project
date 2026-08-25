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

int main(void) {
    Employee *registry = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') continue;
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "ADD") == 0 || strcmp(cmd, "UPDATE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *id = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *dept = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *title = p;

            if (*id == '\0') continue;

            int found_idx = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(registry[i].id, id) == 0) {
                    found_idx = (int)i;
                    break;
                }
            }

            if (strcmp(cmd, "ADD") == 0) {
                if (count >= capacity) {
                    size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                    Employee *new_reg = realloc(registry, new_cap * sizeof(Employee));
                    if (!new_reg) continue;
                    registry = new_reg;
                    capacity = new_cap;
                }
                char *i_d = strdup(id);
                char *nm = strdup(name);
                char *dp = strdup(dept);
                char *tl = strdup(title);
                if (i_d && nm && dp && tl) {
                    registry[count].id = i_d;
                    registry[count].name = nm;
                    registry[count].dept = dp;
                    registry[count].title = tl;
                    count++;
                } else {
                    free(i_d);
                    free(nm);
                    free(dp);
                    free(tl);
                }
            } else {
                if (found_idx != -1) {
                    char *nm = strdup(name);
                    char *dp = strdup(dept);
                    char *tl = strdup(title);
                    if (nm && dp && tl) {
                        free(registry[found_idx].name);
                        free(registry[found_idx].dept);
                        free(registry[found_idx].title);
                        registry[found_idx].name = nm;
                        registry[found_idx].dept = dp;
                        registry[found_idx].title = tl;
                    } else {
                        free(nm);
                        free(dp);
                        free(tl);
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *id = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*id == '\0') continue;

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
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *id = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*id == '\0') continue;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(registry[i].id, id) == 0) {
                    printf("ID: %s Name: %s Dept: %s Title: %s\n", registry[i].id, registry[i].name, registry[i].dept, registry[i].title);
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("ID: %s Name: %s Dept: %s Title: %s\n", registry[i].id, registry[i].name, registry[i].dept, registry[i].title);
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(registry[i].id);
        free(registry[i].name);
        free(registry[i].dept);
        free(registry[i].title);
    }
    free(registry);
    free(line);
    return 0;
}
