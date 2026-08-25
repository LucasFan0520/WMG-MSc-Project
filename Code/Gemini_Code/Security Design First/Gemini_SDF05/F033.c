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
    int count = 0;
    int capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *id = p;
            char *space1 = strchr(id, ' ');
            if (!space1) continue;
            *space1 = '\0';
            char *name = space1 + 1;
            while (*name == ' ') name++;
            char *space2 = strchr(name, ' ');
            if (!space2) continue;
            *space2 = '\0';
            char *dept = space2 + 1;
            while (*dept == ' ') dept++;
            char *space3 = strchr(dept, ' ');
            char *title = "";
            if (space3) {
                *space3 = '\0';
                title = space3 + 1;
                while (*title == ' ') title++;
            }
            if (count >= capacity) {
                int new_cap = capacity == 0 ? 4 : capacity * 2;
                Employee *tmp = realloc(registry, new_cap * sizeof(Employee));
                if (!tmp) return 1;
                registry = tmp;
                capacity = new_cap;
            }
            registry[count].id = strdup(id);
            registry[count].name = strdup(name);
            registry[count].dept = strdup(dept);
            registry[count].title = strdup(title);
            count++;
        } else if (strncmp(line, "UPDATE ", 7) == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *id = p;
            char *space1 = strchr(id, ' ');
            if (!space1) continue;
            *space1 = '\0';
            char *name = space1 + 1;
            while (*name == ' ') name++;
            char *space2 = strchr(name, ' ');
            if (!space2) continue;
            *space2 = '\0';
            char *dept = space2 + 1;
            while (*dept == ' ') dept++;
            char *space3 = strchr(dept, ' ');
            char *title = "";
            if (space3) {
                *space3 = '\0';
                title = space3 + 1;
                while (*title == ' ') title++;
            }
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(registry[i].id, id) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(registry[found].name);
                free(registry[found].dept);
                free(registry[found].title);
                registry[found].name = strdup(name);
                registry[found].dept = strdup(dept);
                registry[found].title = strdup(title);
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *id = line + 7;
            while (*id == ' ') id++;
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
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *id = line + 5;
            while (*id == ' ') id++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(registry[i].id, id) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                printf("ID: %s Name: %s Dept: %s Title: %s\n", registry[found].id, registry[found].name, registry[found].dept, registry[found].title);
            } else {
                printf("NOT_FOUND\n");
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("ID: %s Name: %s Dept: %s Title: %s\n", registry[i].id, registry[i].name, registry[i].dept, registry[i].title);
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
