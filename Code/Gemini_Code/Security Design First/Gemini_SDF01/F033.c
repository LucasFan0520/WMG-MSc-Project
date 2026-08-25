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

int main(void) {
    Employee *items = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    long read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }

        if (strcmp(line, "END") == 0) {
            break;
        }

        char *p = line;
        while (*p == ' ' || *p == '	') p++;
        if (*p == '\0') continue;

        char *cmd = p;
        while (*p && *p != ' ' && *p != '	') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        while (*p == ' ' || *p == '	') p++;

        if (strcmp(cmd, "ADD") == 0 || strcmp(cmd, "UPDATE") == 0) {
            char *id = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;

            char *name = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;

            char *department = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *title = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].id, id) == 0) {
                    found = (int)i;
                    break;
                }
            }

            if (strcmp(cmd, "ADD") == 0) {
                if (found != -1) {
                    free(items[found].name);
                    free(items[found].department);
                    free(items[found].title);
                    items[found].name = strdup(name);
                    items[found].department = strdup(department);
                    items[found].title = strdup(title);
                    if (!items[found].name || !items[found].department || !items[found].title) exit(1);
                } else {
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        Employee *new_items = realloc(items, capacity * sizeof(Employee));
                        if (!new_items) exit(1);
                        items = new_items;
                    }
                    items[count].id = strdup(id);
                    items[count].name = strdup(name);
                    items[count].department = strdup(department);
                    items[count].title = strdup(title);
                    if (!items[count].id || !items[count].name || !items[count].department || !items[count].title) exit(1);
                    count++;
                }
            } else {
                if (found != -1) {
                    free(items[found].name);
                    free(items[found].department);
                    free(items[found].title);
                    items[found].name = strdup(name);
                    items[found].department = strdup(department);
                    items[found].title = strdup(title);
                    if (!items[found].name || !items[found].department || !items[found].title) exit(1);
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *id = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].id, id) == 0) {
                    free(items[i].id);
                    free(items[i].name);
                    free(items[i].department);
                    free(items[i].title);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *id = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].id, id) == 0) {
                    printf("%s %s %s %s\n", items[i].id, items[i].name, items[i].department, items[i].title);
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s %s %s\n", items[i].id, items[i].name, items[i].department, items[i].title);
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(items[i].id);
        free(items[i].name);
        free(items[i].department);
        free(items[i].title);
    }
    free(items);
    free(line);
    return 0;
}
