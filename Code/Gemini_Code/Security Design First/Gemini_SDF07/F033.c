/* F033.c */
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
    int count = 0;
    int capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0 || strncmp(line, "UPDATE ", 7) == 0) {
            int is_update = (line[0] == 'U');
            char *args = is_update ? line + 7 : line + 4;
            char *p1 = strchr(args, ' ');
            if (!p1) continue;
            *p1 = '\0';
            char *id = args;
            char *rest1 = p1 + 1;
            char *p2 = strchr(rest1, ' ');
            if (!p2) { *p1 = ' '; continue; }
            *p2 = '\0';
            char *name = rest1;
            char *rest2 = p2 + 1;
            char *p3 = strchr(rest2, ' ');
            if (!p3) { *p1 = ' '; *p2 = ' '; continue; }
            *p3 = '\0';
            char *dept = rest2;
            char *title = p3 + 1;

            if (is_update) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(registry[i].id, id) == 0) {
                        char *n_name = strdup(name);
                        char *n_dept = strdup(dept);
                        char *n_title = strdup(title);
                        if (n_name && n_dept && n_title) {
                            free(registry[i].name);
                            free(registry[i].department);
                            free(registry[i].title);
                            registry[i].name = n_name;
                            registry[i].department = n_dept;
                            registry[i].title = n_title;
                        } else {
                            free(n_name); free(n_dept); free(n_title);
                        }
                        break;
                    }
                }
            } else {
                char *n_id = strdup(id);
                char *n_name = strdup(name);
                char *n_dept = strdup(dept);
                char *n_title = strdup(title);
                if (n_id && n_name && n_dept && n_title) {
                    if (count >= capacity) {
                        int new_cap = capacity == 0 ? 4 : capacity * 2;
                        Employee *tmp = realloc(registry, new_cap * sizeof(Employee));
                        if (tmp) {
                            registry = tmp;
                            capacity = new_cap;
                        } else {
                            free(n_id); free(n_name); free(n_dept); free(n_title);
                            *p1 = ' '; *p2 = ' '; *p3 = ' ';
                            continue;
                        }
                    }
                    registry[count].id = n_id;
                    registry[count].name = n_name;
                    registry[count].department = n_dept;
                    registry[count].title = n_title;
                    count++;
                } else {
                    free(n_id); free(n_name); free(n_dept); free(n_title);
                }
            }
            *p1 = ' '; *p2 = ' '; *p3 = ' ';
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *id = line + 7;
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
                    break;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *id = line + 5;
            for (int i = 0; i < count; i++) {
                if (strcmp(registry[i].id, id) == 0) {
                    printf("%s %s %s %s\n", registry[i].id, registry[i].name, registry[i].department, registry[i].title);
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s %s %s\n", registry[i].id, registry[i].name, registry[i].department, registry[i].title);
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(registry[i].id);
        free(registry[i].name);
        free(registry[i].department);
        free(registry[i].title);
    }
    free(registry);
    return 0;
}
