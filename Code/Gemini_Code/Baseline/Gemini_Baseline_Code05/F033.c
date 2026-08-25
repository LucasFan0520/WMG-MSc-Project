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

Employee *registry = NULL;
int count = 0;
int capacity = 0;

int find_employee(const char *id) {
    for (int i = 0; i < count; i++) {
        if (strcmp(registry[i].id, id) == 0) {
            return i;
        }
    }
    return -1;
}

void set_employee(int idx, const char *id, const char *name, const char *dept, const char *title) {
    if (idx < 0) {
        if (count >= capacity) {
            capacity = capacity == 0 ? 4 : capacity * 2;
            registry = realloc(registry, capacity * sizeof(Employee));
        }
        registry[count].id = strdup(id);
        registry[count].name = strdup(name);
        registry[count].dept = strdup(dept);
        registry[count].title = strdup(title);
        count++;
    } else {
        free(registry[idx].name);
        free(registry[idx].dept);
        free(registry[idx].title);
        registry[idx].name = strdup(name);
        registry[idx].dept = strdup(dept);
        registry[idx].title = strdup(title);
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0 || strncmp(line, "UPDATE ", 7) == 0) {
            int is_add = (strncmp(line, "ADD ", 4) == 0);
            char *p = is_add ? line + 4 : line + 7;
            char *sp1 = strchr(p, ' ');
            if (sp1) {
                *sp1 = 0;
                char *name = sp1 + 1;
                char *sp2 = strchr(name, ' ');
                if (sp2) {
                    *sp2 = 0;
                    char *dept = sp2 + 1;
                    char *sp3 = strchr(dept, ' ');
                    if (sp3) {
                        *sp3 = 0;
                        char *title = sp3 + 1;
                        int idx = find_employee(p);
                        if (is_add && idx == -1) {
                            set_employee(-1, p, name, dept, title);
                        } else if (!is_add && idx != -1) {
                            set_employee(idx, p, name, dept, title);
                        }
                    }
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *id = line + 7;
            int idx = find_employee(id);
            if (idx != -1) {
                free(registry[idx].id);
                free(registry[idx].name);
                free(registry[idx].dept);
                free(registry[idx].title);
                for (int i = idx; i < count - 1; i++) {
                    registry[i] = registry[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *id = line + 5;
            int idx = find_employee(id);
            if (idx != -1) {
                printf("%s %s %s %s\n", registry[idx].id, registry[idx].name, registry[idx].dept, registry[idx].title);
            }
        } else if (strcmp(line, "LIST") == 0) {
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
