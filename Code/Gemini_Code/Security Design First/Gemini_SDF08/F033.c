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
        if (strncmp(line, "ADD ", 4) == 0 || strncmp(line, "UPDATE ", 7) == 0) {
            int is_add = (line[0] == 'A');
            char *p = is_add ? (line + 4) : (line + 7);
            char *s1 = strchr(p, ' ');
            if (s1) {
                *s1 = '\0';
                char *id = p;
                char *p2 = s1 + 1;
                char *s2 = strchr(p2, ' ');
                if (s2) {
                    *s2 = '\0';
                    char *name = p2;
                    char *p3 = s2 + 1;
                    char *s3 = strchr(p3, ' ');
                    if (s3) {
                        *s3 = '\0';
                        char *dept = p3;
                        char *title = s3 + 1;
                        if (is_add) {
                            Employee *tmp = realloc(registry, sizeof(Employee) * (count + 1));
                            if (tmp) {
                                registry = tmp;
                                registry[count].id = strdup(id);
                                registry[count].name = strdup(name);
                                registry[count].dept = strdup(dept);
                                registry[count].title = strdup(title);
                                if (registry[count].id && registry[count].name && registry[count].dept && registry[count].title) {
                                    count++;
                                }
                            }
                        } else {
                            for (int i = 0; i < count; i++) {
                                if (strcmp(registry[i].id, id) == 0) {
                                    char *nn = strdup(name);
                                    char *nd = strdup(dept);
                                    char *nt = strdup(title);
                                    if (nn && nd && nt) {
                                        free(registry[i].name);
                                        free(registry[i].dept);
                                        free(registry[i].title);
                                        registry[i].name = nn;
                                        registry[i].dept = nd;
                                        registry[i].title = nt;
                                    } else {
                                        free(nn);
                                        free(nd);
                                        free(nt);
                                    }
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *id = line + 7;
            int idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(registry[i].id, id) == 0) {
                    idx = i;
                    break;
                }
            }
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
            for (int i = 0; i < count; i++) {
                if (strcmp(registry[i].id, id) == 0) {
                    printf("%s: %s (%s, %s)\n", registry[i].id, registry[i].name, registry[i].dept, registry[i].title);
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s: %s (%s, %s)\n", registry[i].id, registry[i].name, registry[i].dept, registry[i].title);
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
    free(line);
    return 0;
}
