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

int main(void) {
    Employee *registry = NULL;
    int count = 0;
    char *line = NULL;
    size_t cap = 0;
    while (getline(&line, &cap, stdin) != -1) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char id[256], name[256], dept[256], title[256];
            if (sscanf(line + 4, "%255s %255s %255s %255s", id, name, dept, title) == 4) {
                int found = 0;
                for (int i = 0; i < count; i++) {
                    if (strcmp(registry[i].id, id) == 0) {
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    Employee *tmp = realloc(registry, (count + 1) * sizeof(Employee));
                    if (tmp) {
                        registry = tmp;
                        registry[count].id = strdup(id);
                        registry[count].name = strdup(name);
                        registry[count].dept = strdup(dept);
                        registry[count].title = strdup(title);
                        count++;
                    }
                }
            }
        } else if (strncmp(line, "UPDATE ", 7) == 0) {
            char id[256], name[256], dept[256], title[256];
            if (sscanf(line + 7, "%255s %255s %255s %255s", id, name, dept, title) == 4) {
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
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *id = line + 7;
            while (*id == ' ') id++;
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
                    break;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *id = line + 5;
            while (*id == ' ') id++;
            for (int i = 0; i < count; i++) {
                if (strcmp(registry[i].id, id) == 0) {
                    printf("%s %s %s %s\n", registry[i].id, registry[i].name, registry[i].dept, registry[i].title);
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
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
