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
    char *line = NULL;
    size_t line_len = 0;
    ssize_t read;
    while ((read = getline(&line, &line_len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (read > 1 && line[read - 2] == '\r') {
            line[read - 2] = '\0';
        }
        char *p = line;
        while (*p == ' ' || *p == '\t') {
            p++;
        }
        if (*p == '\0') {
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') {
            p++;
        }
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *id = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *dept = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *title = p;
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(registry[i].id, id) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                Employee *temp = realloc(registry, (count + 1) * sizeof(Employee));
                if (temp) {
                    registry = temp;
                    registry[count].id = strdup(id);
                    registry[count].name = strdup(name);
                    registry[count].department = strdup(dept);
                    registry[count].title = strdup(title);
                    count++;
                }
            }
        } else if (strcmp(cmd, "UPDATE") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *id = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *dept = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *title = p;
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
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *id = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
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
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *id = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(registry[i].id, id) == 0) {
                    printf("ID: %s Name: %s Dept: %s Title: %s\n", registry[i].id, registry[i].name, registry[i].department, registry[i].title);
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("ID: %s Name: %s Dept: %s Title: %s\n", registry[i].id, registry[i].name, registry[i].department, registry[i].title);
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
